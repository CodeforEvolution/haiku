#ifndef _FBSD_COMPAT_SYS_MODULE_H_
#define _FBSD_COMPAT_SYS_MODULE_H_


#include <sys/linker_set.h>


typedef struct module* module_t;

typedef enum modeventtype {
	MOD_LOAD,
	MOD_UNLOAD,
	MOD_SHUTDOWN,
	MOD_QUIESCE
} modeventtype_t;

typedef int (*modeventhand_t)(module_t, int /* modeventtype_t */, void *);

/*
 * Struct for registering modules statically via SYSINIT.
 */
typedef struct moduledata {
	const char	*name;		/* module name */
	modeventhand_t  evhand;		/* event handler */
	void		*priv;		/* extra data */
} moduledata_t;

#define DECLARE_MODULE(name, data, sub, order)

#define MODULE_VERSION(name, version)
#define MODULE_DEPEND(module, mdepend, vmin, vpref, vmax)

#endif
