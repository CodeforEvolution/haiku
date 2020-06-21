/*
 * Copyright 2020 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */


#include <virtio.h>
#include <virtio-info.h>

#include "virtio-gpu.h"


/* Tell the kernel what revision of the driver API we support */
int32 api_version = B_CUR_DRIVER_API_VERSION;


/* Our primary defines and structs for the Virtio gpu */
#define VIRTIO_GPU_DRIVER_MODULE_NAME "drivers/graphics/virtio/driver_v1"
#define VIRTIO_GPU_DEVICE_MODULE_NAME "drivers/graphics/virtio/device_v1"


#define DEVICE_NAME				"virtio"
#define VIRTIO_ACCELERANT_NAME	"virtio.accelerant"


typedef struct {
	device_node*				node;
	::virtio_device				virtio_device;
	virtio_device_interface*	virtio;
	::virtio_queue				virtio_queue;

	struct virtio_gpu_config	config;

	uint32 						features;

	sem_id 						sem_cb;

	virtio_shared_info*			shared_info;
	area_id						shared_area;
} virtio_gpu_driver_info;


typedef struct {
	virtio_gpu_driver_info*		info;
} virtio_gpu_handle;


/* These structures are private to the kernel driver */
typedef struct device_info device_info;


struct device_info {
	uint32			is_open;				/* a count of how many times the devices has been opened */
	area_id			shared_area;			/* the area shared between the driver and all of the accelerants */
	shared_info* 	si;						/* a pointer to the shared area, for convenience */
	vuint32*		regs;					/* kernel's pointer to memory mapped registers */
	pci_info		pcii;					/* a convenience copy of the pci info for this device */
	char			name[B_OS_NAME_LENGTH];	/* where we keep the name of the device for publishing and comparing */
};


static pci_module_info*	pci_bus = NULL;


const char*
get_feature_name(uint32 feature)
{
	switch (feature) {
		case VIRTIO_GPU_F_VIRGL:
			return "virgl 3d mode";
		case VIRTIO_GPU_F_EDID:
			return "edid info";
	}
	return NULL;
}


//	#pragma mark - device module API


static status_t
virtio_gpu_init_device(void* _info, void** _cookie)
{
	CALLED();
	virtio_gpu_driver_info* info = (virtio_gpu_driver_info*)_info;

	device_node* parent = sDeviceManager->get_parent_node(info->node);
	sDeviceManager->get_driver(parent, (driver_module_info **)&info->virtio,
		(void **)&info->virtio_device);
	sDeviceManager->put_node(parent);

	info->virtio->negotiate_features(info->virtio_device,
		VIRTIO_GPU_F_EDID | VIRTIO_FEATURE_RING_INDIRECT_DESC,
		&info->features, &get_feature_name);

	status_t status = info->virtio->read_device_config(
		info->virtio_device, 0, &info->config,
		sizeof(struct virtio_gpu_config));
	if (status != B_OK)
		return status;

	status = info->virtio->alloc_queues(info->virtio_device, 2,
		&info->virtio_queue);
	if (status != B_OK) {
		ERROR("queue allocation failed (%s)\n", strerror(status));
		return status;
	}


	*_cookie = info;
	return status;
}


static void
virtio_gpu_uninit_device(void* _cookie)
{
	CALLED();
}


static status_t
virtio_gpu_open(void* _info, const char* path, int openMode, void** _cookie)
{
	CALLED();
	virtio_gpu_driver_info* info = (virtio_gpu_driver_info*)_info;

	virtio_gpu_handle* handle = (virtio_gpu_handle*)malloc(
		sizeof(virtio_gpu_handle));
	if (handle == NULL)
		return B_NO_MEMORY;

	handle->info = info;

	*_cookie = handle;
	return B_OK;
}


static status_t
virtio_gpu_close(void* cookie)
{
	CALLED();

	return B_OK;
}


static status_t
virtio_gpu_free(void* cookie)
{
	CALLED();
	virtio_gpu_handle* handle = (virtio_gpu_handle*)cookie;

	free(handle);
	return B_OK;
}


static status_t
virtio_gpu_read(void* /*cookie*/, off_t /*pos*/, const void* /*buffer*/,
	size_t* _length)
{
	CALLED();

	*_length = 0;
	return B_NOT_ALLOWED;
}


static status_t
virtio_gpu_write(void* /*cookie*/, off_t /*pos*/, const void* /*buffer*/,
	size_t* _length)
{
	CALLED();

	*_length = 0;
	return B_NOT_ALLOWED;
}


static status_t
virtio_gpu_ioctl(void* cookie, uint32 op, void* buffer, size_t length)
{
	CALLED();
	virtio_gpu_handle* handle = (virtio_gpu_handle*)cookie;
	virtio_gpu_driver_info* info = handle->info;

	TRACE("ioctl(op = %ld)\n", op);

	switch (op) {
		case B_GET_ACCELERANT_SIGNATURE:
		{
			*(status_t*)buffer = B_OK;
			dprintf(DEVICE_NAME ": acc: %s\n", VIRTIO_ACCELERANT_NAME);
			if (user_strlcpy((char*)buffer, VIRTIO_ACCELERANT_NAME,
					B_FILE_NAME_LENGTH) < B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}

		// needed for cloning
		case VIRTIO_GET_DEVICE_NAME:
		{
			if (user_strlcpy((char*)buffer, "graphics/virtio",
					B_PATH_NAME_LENGTH) < B_OK)
				return B_BAD_ADDRESS;

			return B_OK;
		}
	}

	return B_DEV_INVALID_IOCTL;
}


void
virtio_block_set_capacity(virtio_block_driver_info* info, uint64 capacity,
	uint32 blockSize)
{
	TRACE("set_capacity(device = %p, capacity = %Ld, blockSize = %ld)\n",
		info, capacity, blockSize);

	// get log2, if possible
	uint32 blockShift = log2(blockSize);

	if ((1UL << blockShift) != blockSize)
		blockShift = 0;

	info->capacity = capacity;

	if (info->block_size != blockSize) {
		if (info->block_size != 0) {
			ERROR("old %" B_PRId32 ", new %" B_PRId32 "\n", info->block_size,
				blockSize);
			panic("updating DMAResource not yet implemented...");
		}

		dma_restrictions restrictions;
		memset(&restrictions, 0, sizeof(restrictions));
		if ((info->features & VIRTIO_BLK_F_SIZE_MAX) != 0)
			restrictions.max_segment_size = info->config.size_max;
		if ((info->features & VIRTIO_BLK_F_SEG_MAX) != 0)
			restrictions.max_segment_count = info->config.seg_max;

		// TODO: we need to replace the DMAResource in our IOScheduler
		status_t status = info->dma_resource->Init(restrictions, blockSize,
			1024, 32);
		if (status != B_OK)
			panic("initializing DMAResource failed: %s", strerror(status));

		info->io_scheduler = new(std::nothrow) IOSchedulerSimple(
			info->dma_resource);
		if (info->io_scheduler == NULL)
			panic("allocating IOScheduler failed.");

		// TODO: use whole device name here
		status = info->io_scheduler->Init("virtio");
		if (status != B_OK)
			panic("initializing IOScheduler failed: %s", strerror(status));

		info->io_scheduler->SetCallback(do_io, info);
	}

	info->block_size = blockSize;
}


//	#pragma mark - driver module API


static float
virtio_gpu_supports_device(device_node *parent)
{
	CALLED();
	const char *bus;
	uint16 deviceType;

	// make sure parent is really the Virtio bus manager
	if (sDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false))
		return -1;

	if (strcmp(bus, "virtio"))
		return 0.0;

	// check whether it's really a Direct Access Device and a Virtio gpu device
	if (sDeviceManager->get_attr_uint16(parent, VIRTIO_DEVICE_TYPE_ITEM,
			&deviceType, true) != B_OK || deviceType != VIRTIO_DEVICE_ID_GPU)
		return 0.0;

	TRACE("Virtio block device found!\n");

	return 0.6;
}


static status_t
virtio_gpu_register_device(device_node *node)
{
	CALLED();

	// ready to register
	device_attr attrs[] = {
		{ NULL }
	};

	return sDeviceManager->register_node(node, VIRTIO_GPU_DRIVER_MODULE_NAME,
		attrs, NULL, NULL);
}


static status_t
virtio_gpu_init_driver(device_node *node, void **cookie)
{
	CALLED();

	virtio_gpu_driver_info* info = (virtio_gpu_driver_info*)malloc(
		sizeof(virtio_gpu_driver_info));
	if (info == NULL)
		return B_NO_MEMORY;

	memset(info, 0, sizeof(*info));

	info->media_status = B_OK;
	info->dma_resource = new(std::nothrow) DMAResource;
	if (info->dma_resource == NULL) {
		free(info);
		return B_NO_MEMORY;
	}

	info->sem_cb = create_sem(0, "virtio_block_cb");
	if (info->sem_cb < 0) {
		delete info->dma_resource;
		status_t status = info->sem_cb;
		free(info);
		return status;
	}
	info->node = node;

	*cookie = info;
	return B_OK;
}


static void
virtio_gpu_uninit_driver(void *_cookie)
{
	CALLED();
	virtio_gpu_driver_info* info = (virtio_gpu_driver_info*)_cookie;
	delete_sem(info->sem_cb);
	free(info);
}


static status_t
virtio_gpu_register_child_devices(void* _cookie)
{
	CALLED();
	virtio_gpu_driver_info* info = (virtio_gpu_driver_info*)_cookie;
	status_t status;

	char name[64];
	snprintf(name, sizeof(name), "graphics/virtio");

	status = sDeviceManager->publish_device(info->node, name,
		VIRTIO_GPU_DEVICE_MODULE_NAME);

	return status;
}


//	#pragma mark -


module_dependency module_dependencies[] = {
	{B_DEVICE_MANAGER_MODULE_NAME, (module_info**)&sDeviceManager},
	{}
};


struct device_module_info sVirtioGPUDevice = {
	{
		VIRTIO_GPU_DEVICE_MODULE_NAME,
		0,
		NULL
	},

	virtio_gpu_init_device,
	virtio_gpu_uninit_device,
	NULL, // remove,

	virtio_gpu_open,
	virtio_gpu_close,
	virtio_gpu_free,
	virtio_gpu_read,
	virtio_gpu_write,
	NULL, // io,
	virtio_gpu_ioctl,

	NULL,	// select
	NULL,	// deselect
};


struct driver_module_info sVirtioGPUDriver = {
	{
		VIRTIO_GPU_DRIVER_MODULE_NAME,
		0,
		NULL
	},

	virtio_gpu_supports_device,
	virtio_gpu_register_device,
	virtio_gpu_init_driver,
	virtio_gpu_uninit_driver,
	virtio_gpu_register_child_devices,
	NULL,	// rescan
	NULL,	// removed
};


module_info* modules[] = {
	(module_info*)&sVirtioGPUDriver,
	(module_info*)&sVirtioGPUDevice,
	NULL
};
