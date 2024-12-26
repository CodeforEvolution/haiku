/*
 * Copyright 2024, Jacob Secunda <secundaja@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "network.h"

#include "efi_platform.h"


static efi_guid sSimpleNetworkProtocolGUID = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;


SNP::SNP()
	:
	fEFISimpleNetwork(nullptr),
	fMACAddress(kNoMACAddress)
{
}


SNP::~SNP()
{
	if (fEFISimpleNetwork != nullptr) {
		fEFISimpleNetwork->Shutdown(fEFISimpleNetwork);
		fEFISimpleNetwork->Stop(fEFISimpleNetwork);
		fEFISimpleNetwork = nullptr;
	}
}


status_t
SNP::Init()
{
	// Check for EFI Simple Network Protocol (SNP)
	efi_status status = kSystemTable->BootServices->LocateProtocol(
		&sSimpleNetworkProtocolGUID, nullptr, reinterpret_cast<void**>(&fEFISimpleNetwork));

	if (status != EFI_SUCCESS) {
		fEFISimpleNetwork = nullptr;
		return B_NOT_SUPPORTED;
	}
	
	status = fEFISimpleNetwork->Start(fEFISimpleNetwork);
	if (status != EFI_SUCCESS && status != EFI_ALREADY_STARTED) {
		fEFISimpleNetwork = nullptr;
		return B_ERROR;
	}
	
	status = fEFISimpleNetwork->Initialize(fEFISimpleNetwork, 0, 0);
	if (status != EFI_SUCCESS) {
		fEFISimpleNetwork = nullptr;
		return B_ERROR;
	}
	
	// Retrieve MAC address
	fMACAddress = reinterpret_cast<uint8*>(&fEFISimpleNetwork->Mode->CurrentAddress.addr);
	
	return B_OK;
}


mac_addr_t
SNP::MACAddress() const
{
	return fMACAddress;
}


void*
SNP::AllocateSendReceiveBuffer(size_t size)
{
	return new(std::nothrow) uint8[size];
}


void
SNP::FreeSendReceiveBuffer(void* buffer)
{
	delete static_cast<uint8*>(buffer);
}


ssize_t
SNP::Send(const void* buffer, size_t size)
{
	if (buffer == nullptr)
		return B_BAD_VALUE;
		
	efi_status status = fEFISimpleNetwork->Transmit(fEFISimpleNetwork, 0, size,
		const_cast<void*>(buffer), nullptr, nullptr, nullptr);
	if (status != EFI_SUCCESS)
		return B_ERROR;
		
	// Block on transmission
	uint32_t interruptStatus = 0;
	void* transmitBuffer = nullptr;
	do {
		status = fEFISimpleNetwork->GetStatus(fEFISimpleNetwork, &interruptStatus,
			&transmitBuffer);
		if (status != EFI_SUCCESS)
			return B_ERROR;
	} while (transmitBuffer == nullptr);
	
	// Guess we did it!
	return size;
}


ssize_t
SNP::Receive(void* buffer, size_t size)
{
	if (buffer == nullptr)
		return B_BAD_VALUE;

	size_t dummyIndex = 0;
	efi_status status = kBootServices->WaitForEvent(1, &fEFISimpleNetwork->WaitForPacket, &dummyIndex);
	if (status != EFI_SUCCESS)
		return B_ERROR;

	status = fEFISimpleNetwork->Receive(fEFISimpleNetwork, nullptr, &size, buffer, nullptr,
		nullptr, nullptr);
	if (status != EFI_SUCCESS) 
		return (status == EFI_BUFFER_TOO_SMALL) ? B_BUFFER_OVERFLOW : B_ERROR;
		
	return size;
}


// #pragma mark -


status_t
platform_net_stack_init()
{
	auto interface = new(std::nothrow) SNP();
	if (interface == nullptr)
		return B_NO_MEMORY;

	status_t error = interface->Init();
	if (error != B_OK) {
		delete interface;
		return error;
	}

	error = NetStack::Default()->AddEthernetInterface(interface);
	if (error != B_OK) {
		delete interface;
		return error;
	}

	return B_OK;
}
