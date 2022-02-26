/*
 * Copyright 2022 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _DEVICE_ROSTER_H
#define _DEVICE_ROSTER_H


#include <Device.h>
#include <ObjectList.h>


class BDeviceRoster : private BObjectList<BDevice*> {

};


#endif /* _DEVICE_ROSTER_H */
