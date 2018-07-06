/*
 * Copyright 2009 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FBSD_COMPAT_SYS_PRIORITY_H_
#define _FBSD_COMPAT_SYS_PRIORITY_H_

#define	PRI_MIN			(0)		/* Highest priority. */
#define	PRI_MAX			(255)		/* Lowest priority. */

#define	PRI_MIN_ITHD		(PRI_MIN)
#define	PRI_MAX_ITHD		(PRI_MIN_REALTIME - 1)

#define	PI_SOFT			(PRI_MIN_ITHD + 24)
#define	PI_SWI(x)		(PI_SOFT + (x) * RQ_PPQ)

#define PRI_MIN_KERN	(64)
#define PZERO			(PRI_MIN_KERN + 20)

#endif /* _FBSD_COMPAT_SYS_PRIORITY_H_ */
