/*
 * Copyright 2023, Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <OS.h>
#include <vm_defs.h>

#include "strace.h"
#include "Syscall.h"
#include "TypeHandler.h"


#define FLAG_INFO_ENTRY(name) \
	{ name, #name }

static const FlagsTypeHandler::FlagInfo kAreaProtectionFlagInfos[] = {
	FLAG_INFO_ENTRY(B_READ_AREA),
	FLAG_INFO_ENTRY(B_WRITE_AREA),
	FLAG_INFO_ENTRY(B_EXECUTE_AREA),
	FLAG_INFO_ENTRY(B_STACK_AREA),

	FLAG_INFO_ENTRY(B_CLONEABLE_AREA),
	FLAG_INFO_ENTRY(B_OVERCOMMITTING_AREA),

	{ 0, NULL }
};


static FlagsTypeHandler::FlagsList kAreaProtectionFlags;

void
patch_area()
{
	for (int i = 0; kAreaProtectionFlagInfos[i].name != NULL; i++) {
		kAreaProtectionFlags.push_back(kAreaProtectionFlagInfos[i]);
	}

	Syscall *open = get_syscall("_kern_create_area");
	open->GetParameter("protection")->SetHandler(
		new FlagsTypeHandler(kAreaProtectionFlags));

	Syscall *clone = get_syscall("_kern_clone_area");
	clone->GetParameter("protection")->SetHandler(
		new FlagsTypeHandler(kAreaProtectionFlags));

	Syscall *reserve_address_range = get_syscall("_kern_reserve_address_range");
	reserve_address_range->GetParameter("_address")->SetInOut(true);

	Syscall *set_area_protection = get_syscall("_kern_set_area_protection");
	set_area_protection->GetParameter("newProtection")->SetHandler(
		new FlagsTypeHandler(kAreaProtectionFlags));

	Syscall *map_file = get_syscall("_kern_map_file");
	map_file->GetParameter("protection")->SetHandler(
		new FlagsTypeHandler(kAreaProtectionFlags));
}