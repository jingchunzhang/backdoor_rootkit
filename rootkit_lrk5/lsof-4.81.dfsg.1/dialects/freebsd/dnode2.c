/*
 * dnode2.c - FreeBSD ZFS node functions for lsof
 *
 * This module must be separate to permit use of the OpenSolaris ZFS header
 * files.
 */


/*
 * Copyright 2008 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Victor A. Abell
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors nor Purdue University are responsible for any
 *    consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Credit to the authors and Purdue
 *    University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

#ifndef lint
static char copyright[] =
"@(#) Copyright 2008 Purdue Research Foundation.\nAll rights reserved.\n";
static char *rcsid = "$Id: dnode2.c,v 1.2 2008/05/09 12:53:13 abe Exp $";
#endif


#if	defined(HAS_ZFS)

#define _KERNEL
#include <sys/zfs_znode.h>
#undef	_KERNEL

#include "dzfs.h"


/*
 * readzfsnode() -- read the ZFS node
 */

char *
readzfsnode(za, zi)
	KA_T za;			/* ZFS node address */
	zfs_info_t *zi;			/* return ZFS info structure pointer */
{
	struct znode zn;		/* ZFS node */
	znode_phys_t zp;		/* ZFS physical node */

	memset((void *)zi, 0, sizeof(zfs_info_t));
	if (!za
	||  kread(za, (char *)&zn, sizeof(zn))
	) {
	    if (!za)
		return("No ZFS node address");
	    return("Can't read znode");
	}
/*
 * Return items contained in the znode.
 */
	zi->ino = (INODETYPE)zn.z_id;
	zi->ino_def = 1;

# if	!defined(HAS_V_LOCKF)
	zi->lockf = (KA_T)zn.z_lockf;
# endif	/* !defined(HAS_V_LOCKF) */

/*
 * Read the physical znode for other items.
 */
	if (!zn.z_phys
	||  kread((KA_T)zn.z_phys, (char *)&zp, sizeof(zp))
	) {
	    if (!zn.z_phys)
		return("No physical znode address");
	    return("Can't read physical znode");
	}
/*
 * Return remaining items of znode information.
 */
	zi->nl = zp.zp_links;
	zi->rdev = zp.zp_rdev;
	zi->sz = (SZOFFTYPE)zp.zp_size;
	zi->nl_def = zi->rdev_def = zi->sz_def = 1;
	return((char *)NULL);
}
#endif	/* defined(HAS_ZFS) */
