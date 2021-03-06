/*
 * \brief  Platform-specific helper functions for the _main() function
 * \author Christian Prochaska
 * \date   2009-08-05
 */

/*
 * Copyright (C) 2009-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _PLATFORM___MAIN_HELPER_H_
#define _PLATFORM___MAIN_HELPER_H_


/* Pistachio-specific definitions */
enum { L4_PAGEMASK = ~0xFFF };
enum { L4_PAGESIZE = 0x1000 };


static void main_thread_bootstrap() { }


#endif /* _PLATFORM___MAIN_HELPER_H_ */
