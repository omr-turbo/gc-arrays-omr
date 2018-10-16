/*******************************************************************************
 *  Copyright (c) 2018, 2018 IBM and others
 *
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License 2.0 which accompanies this
 *  distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 *  or the Apache License, Version 2.0 which accompanies this distribution and
 *  is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  This Source Code may also be made available under the following
 *  Secondary Licenses when the conditions for such availability set
 *  forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 *  General Public License, version 2 with the GNU Classpath
 *  Exception [1] and GNU General Public License, version 2 with the
 *  OpenJDK Assembly Exception [2].
 *
 *  [1] https://www.gnu.org/software/classpath/license.html
 *  [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include <OMR/GC/System.hpp>

#include "GCExtensionsBase.hpp"
#include "Heap.hpp"
#include "OMR_VMThread.hpp"
#include "StartupManagerImpl.hpp"
#include "mminitcore.h"
#include "omr.h"
#include "omrgcstartup.hpp"
#include "omrport.h"
#include "omrutil.h"
#include "omrvm.h"
#include "thread_api.h"
#include <ParallelDispatcher.hpp>
#include <cassert>
#include <iostream>

namespace OMR
{
namespace GC
{

System::System(Runtime &runtime) : _runtime(&runtime)
{
#if defined(OMR_GC_TRACE)
	std::cerr << __PRETTY_FUNCTION__ << std::endl;
#endif // OMR_GC_TRACE

	Thread self(runtime.platform().thread());

	initVm();
	initCollector();

	StartupContext cx(*this);
	initGcSlaveThreads(cx);
}

System::~System()
{
	Thread self(runtime().platform().thread());
	// TODO: Shut down the heap (requires a thread (boo!!))
	omr_detach_vm_from_runtime(&_vm);
}

void
System::initVm()
{
	memset(&_vm, 0, sizeof(OMR_VM));
	_vm._runtime = &runtime().data();
	_vm._language_vm = this;
	_vm._vmThreadList = nullptr;
	_vm._gcOmrVMExtensions = nullptr;
	_vm._languageThreadCount = 0;

	auto e = omr_attach_vm_to_runtime(&_vm);

	if (e != 0) {
		throw PlatformError(e);
	}
}

void
System::initCollector()
{
	MM_StartupManagerImpl startupManager(&_vm);
	auto e = OMR_GC_IntializeHeapAndCollector(&_vm, &startupManager);
	if (e != 0) {
		throw PlatformError(e);
	}
}

void
System::initGcSlaveThreads(StartupContext &cx)
{
	auto extensions = cx.env()->getExtensions();
	assert(extensions != nullptr);
	auto ok = extensions->dispatcher->startUpThreads();
	if (!ok) {
		extensions->dispatcher->shutDownThreads();
		throw PlatformError(0);
	}
}

/// Note:
void
ContextBase::attachVmContext(OMR_VM &vm, ContextBase &cx)
{
	omrthread_t self = nullptr;

	// assert(nullptr == omr_vmthread_getCurrent(&vm));

	{
		auto error = omrthread_attach_ex(&self, J9THREAD_ATTR_DEFAULT);
		if (error) {
			throw StartupError("failed to attach thread");
		}
	}

	omrthread_monitor_enter(vm._vmThreadListMutex);

	///////// FIRSTATTACH

	{
		omr_error_t error = omr_vmthread_firstAttach(&vm, &cx._vmContext);
		if (error) {
			throw StartupError("failed to attach thread");
		}
	}


	setOMRVMThreadNameWithFlag(nullptr, cx._vmContext, (char *)"OMR::Om::Context", TRUE);
	cx._vmContext->_language_vmthread = &cx;

	{
		intptr_t error = initializeMutatorModel(cx._vmContext);
		if (error) {
			throw StartupError("Failed to initialize mutator model");
		}
	}

	omrthread_monitor_exit(vm._vmThreadListMutex);
	omrthread_detach(self);
}

void
ContextBase::detachVmContext(OMR_VM &vm, ContextBase &cx)
{
	omrthread_t self;

	assert(cx._vmContext->_attachCount == 1);
	// assert(nullptr != omr_vmthread_getCurrent(&vm));

	omrthread_attach_ex(&self, J9THREAD_ATTR_DEFAULT);

	/////// LASTFREE

	omrthread_monitor_enter(vm._vmThreadListMutex);
	cleanupMutatorModel(cx._vmContext, TRUE);
	setOMRVMThreadNameWithFlag(nullptr, cx._vmContext, NULL, FALSE);
	omr_vmthread_lastDetach(cx._vmContext);
	omrthread_monitor_exit(vm._vmThreadListMutex);

	omrthread_detach(self);
}

} // namespace GC
} // namespace OMR
