/*
 * \brief  Dummy pager framework
 * \author Norman Feske
 * \date   2009-10-02
 */

/*
 * Copyright (C) 2009-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/pager.h>


using namespace Genode;


/**********************
 ** Pager activation **
 **********************/

void Pager_activation_base::entry()
{
	Ipc_pager pager;
	_cap = pager;
	_cap_valid.unlock();

//	PINF("Ready for page faults");
	pager.wait_for_fault();
	while (1) {

		/* lookup referenced object */
		Pager_object *obj = _ep ? _ep->obj_by_id(pager.badge()) : 0;

		/* handle request */
		if (obj) {
//			PINF("Pagefault request from a common pager object");

			if (!pager.resolved()){
				if (obj->pager(pager)){
					/* something strange occured - leave thread in pagefault */
//					PINF("Leave unresolved, wait for next page fault");
					pager.wait_for_fault();
				}
				else{
//					PINF("Resolved, reply and wait for next page fault");
					pager.reply_and_wait_for_fault();
				}
			}
			else{
				pager.reply_and_wait_for_fault();
			}
		}
		else {
//			PINF("Pagefault request from one of cores region manager sessions");

			/*
			 * We got a request from one of cores region-manager sessions
			 * to answer the pending page fault of a resolved region-manager
			 * client. Hence, we have to send the page-fault reply to the
			 * specified thread and answer the call of the region-manager
			 * session.
			 *
			 * When called from a region-manager session, we receive the
			 * core-local address of the targeted pager object via the
			 * first message word, which corresponds to the 'fault_ip'
			 * argument of normal page-fault messages.
			 */
			obj = reinterpret_cast<Pager_object *>(pager.fault_ip());

			/* send reply to the calling region-manager session */
			pager.acknowledge_wakeup();

			/* answer page fault of resolved pager object */
			pager.set_reply_dst(obj->cap());
			pager.acknowledge_wakeup();

//			PINF("Wait for next page fault");
			pager.wait_for_fault();
		}
	}
}


/**********************
 ** Pager entrypoint **
 **********************/

Pager_entrypoint::Pager_entrypoint(Cap_session *, Pager_activation_base *a)
: _activation(a)
{
	_activation->ep(this);
}


void Pager_entrypoint::dissolve(Pager_object *obj) { remove(obj); }


Pager_capability Pager_entrypoint::manage(Pager_object *obj)
{
	/* return invalid capability if no activation is present */
	if (!_activation) return Pager_capability();

	Native_capability cap = Native_capability(_activation->cap().dst(), obj->badge());

	/* add server object to object pool */
	obj->cap(cap);
	insert(obj);

	/* return capability that uses the object id as badge */
	return reinterpret_cap_cast<Pager_object>(cap);
}
