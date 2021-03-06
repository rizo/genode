/*
 * \brief  POSIX semaphore implementation
 * \author Christian Prochaska
 * \date   2012-03-12
 *
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>
#include <os/timed_semaphore.h>

#include <semaphore.h>

using namespace Genode;

extern "C" {

	/*
	 * This class is named 'struct sem' because the 'sem_t' type is
	 * defined as 'struct sem*' in 'semaphore.h'
	 */
	struct sem : Timed_semaphore
	{
		sem(int value) : Timed_semaphore(value) { }
	};


	int sem_close(sem_t *)
	{
		PDBG("not implemented");
		return -1;
	}


	int sem_destroy(sem_t *sem)
	{
		destroy(env()->heap(), *sem);
		return 0;
	}


	int sem_getvalue(sem_t * __restrict sem, int * __restrict sval)
	{
		*sval = (*sem)->cnt();
		return 0;
	}


	int sem_init(sem_t *sem, int pshared, unsigned int value)
	{
		*sem = new (env()->heap()) struct sem(value);
		return 0;
	}


	sem_t *sem_open(const char *, int, ...)
	{
		PDBG("not implemented");
		return 0;
	}


	int sem_post(sem_t *sem)
	{
		(*sem)->up();
		return 0;
	}


	int sem_timedwait(sem_t * __restrict, const struct timespec * __restrict)
	{
		PDBG("not implemented");
		return -1;
	}


	int sem_trywait(sem_t *)
	{
		PDBG("not implemented");
		return -1;
	}


	int sem_unlink(const char *)
	{
		PDBG("not implemented");
		return -1;
	}


	int sem_wait(sem_t *sem)
	{
		(*sem)->down();
		return 0;
	}

}
