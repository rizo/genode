/*
 * \brief  Genode C API exit function needed by OKLinux
 * \author Stefan Kalkowski
 * \date   2009-08-14
 */

/*
 * Copyright (C) 2009
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__OKLINUX_SUPPORT__GENODE__EXIT_H_
#define _INCLUDE__OKLINUX_SUPPORT__GENODE__EXIT_H_

/**
 * Tell parent to exit the program
 */
void genode_exit(int value);

#endif //_INCLUDE__OKLINUX_SUPPORT__GENODE__EXIT_H_
