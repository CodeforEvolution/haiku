/*
 * Copyright 2018, Jacob Secunda, secundaja@gmail.com.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef _FBSD_COMPAT_SYS_VNET_H_
#define _FBSD_COMPAT_SYS_VNET_H_

#include <sys/queue.h>

struct vnet {
	LIST_ENTRY(vnet)	 vnet_le;	/* all vnets list */
	u_int			 vnet_magic_n;
	u_int			 vnet_ifcnt;
	u_int			 vnet_sockcnt;
	u_int			 vnet_state;	/* SI_SUB_* */
	void			*vnet_data_mem;
	uintptr_t		 vnet_data_base;
};

extern struct vnet	*vnet0;

#define CURVNET_SET_QUIET(arg)
#define CURVNET_RESTORE()

#endif // _FBSD_COMPAT_SYS_VNET_H_
