/*******************************************************************************
 * Copyright (c) 2017, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/

#include "EnvironmentDelegate.hpp"

#include "AtomicOperations.hpp"
#include "EnvironmentStandard.hpp"
#include "GCExtensionsBase.hpp"
#include "ModronAssertions.h"
#include "SublistFragment.hpp"
#include "omr.h"
#include <iostream>

OMR_VMThread *
MM_EnvironmentDelegate::attachVMThread(OMR_VM *vm, const char *threadName, uintptr_t reason)
{
	OMR_VMThread *vmContext = NULL;
	omr_vmthread_firstAttach(vm, &vmContext);
	return vmContext;
}

void
MM_EnvironmentDelegate::detachVMThread(OMR_VM *vm, OMR_VMThread *vmContext, uintptr_t reason)
{
	omr_vmthread_redetach(vmContext);
}

void
MM_EnvironmentDelegate::acquireVMAccess()
{}

/**
 * Release shared VM acccess.
 */
void
MM_EnvironmentDelegate::releaseVMAccess()
{}

/**
 * Check whether another thread is requesting exclusive VM access. This method
 must be
 * called frequently by all threads that are holding shared VM access. If this
 method
 * returns true, the calling thread should release shared VM access as quickly
 as possible
 * and reacquire it if necessary.

 * @return true if another thread is waiting to acquire exclusive VM access
 */
bool
MM_EnvironmentDelegate::isExclusiveAccessRequestWaiting()
{
	assert(0);
	return false;
}

/**
 * Acquire exclusive VM access. This method should only be called by the OMR
 * runtime to perform stop-the-world operations such as garbage collection.
 * Calling thread will be blocked until all other threads holding shared VM
 * access have release VM access.
 */
void
MM_EnvironmentDelegate::acquireExclusiveVMAccess()
{
	_env->getOmrVMThread()->exclusiveCount += 1;
}

/**
 * Release exclusive VM acccess. If no other thread is waiting for exclusive VM
 * access this method will notify all threads waiting for shared VM access to
 * continue and acquire shared VM access.
 */
void
MM_EnvironmentDelegate::releaseExclusiveVMAccess()
{
	_env->getOmrVMThread()->exclusiveCount -= 1;
}

/**
 * Give up exclusive access in preparation for transferring it to a
 * collaborating thread (i.e. main-to-master or master-to-main). This may
 * involve nothing more than transferring OMR_VMThread::exclusiveCount from the
 * owning thread to the another thread that thereby assumes exclusive access.
 * Implement if this kind of collaboration is required.
 *
 * @return the exclusive count of the current thread before relinquishing
 * @see assumeExclusiveVMAccess(uintptr_t)
 */
uintptr_t
MM_EnvironmentDelegate::relinquishExclusiveVMAccess()
{
	assert(false);
	return 0;
}

/**
 * Assume exclusive access from a collaborating thread (i.e. main-to-master or
 * master-to-main). Implement if this kind of collaboration is required.
 *
 * @param exclusiveCount the exclusive count to be restored
 * @see relinquishExclusiveVMAccess()
 */
void
MM_EnvironmentDelegate::assumeExclusiveVMAccess(uintptr_t exclusiveCount)
{
	assert(false);
}
