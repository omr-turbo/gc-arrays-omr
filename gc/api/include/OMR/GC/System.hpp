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

#if !defined(OMR_GC_SYSTEM_HPP_)
#define OMR_GC_SYSTEM_HPP_

#include <OMR/GC/CompactingFn.hpp>
#include <OMR/GC/MarkingFn.hpp>
#include <OMR/GC/ScavengingFn.hpp>
#include <OMR/GC/StackRootList.hpp>
#include <OMR/GC/LocalHeapCache.hpp>
#include <EnvironmentBase.hpp>
#include <OMR/Runtime.hpp>
#include <OMR/IntrusiveList.hpp>
#include <OMR_VMThread.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mminitcore.h>
#include <new>
#include <omrutil.h>
#include <type_traits>

class MM_EnvironmentBase;

namespace OMR
{
namespace GC
{

class Visitor;
class ContextBase;
class Context;
class StartupContext;

class StartupError : public ::std::runtime_error
{
	using runtime_error::runtime_error;
};

using ContextList = IntrusiveList<Context>;
using ContextListNode = ContextList::Node;
using ContextListIterator = ContextList::Iterator;

/// A GC subsystem.
class System
{
public:
	explicit System(OMR::Runtime &runtime);

	System(const System &) = delete;

	/// TODO: Move ctor for system
	System(System &&) = delete;

	~System();

	OMR_VM &vm() { return _vm; }

	const OMR_VM &vm() const { return _vm; }

	Runtime &runtime() { return *_runtime; }

	MarkingFnVector &markingFns() noexcept { return _userRoots; }

	const MarkingFnVector &markingFns() const noexcept { return _userRoots; }

	ScavengingFnVector &scavengingFns() noexcept { return _scavengingFns; }

	const ScavengingFnVector &scavengingFns() const noexcept { return _scavengingFns; }

	CompactingFnVector &compactingFns() noexcept { return _compactingFns; }

	const CompactingFnVector &compactingFns() const noexcept { return _compactingFns; }

	ContextList &contexts() { return _contexts; }

	const ContextList &contexts() const { return _contexts; }

protected:
	friend class Context;

private:
	/// Initialize the VM, which ties together all the OMR services.
	void initVm();

	/// Initialize the heap and collector.
	void initCollector();

	void initGcSlaveThreads(StartupContext &cx);


	Runtime *_runtime;
	OMR_VM _vm;
	ContextList _contexts;
	MarkingFnVector _userRoots;
	ScavengingFnVector _scavengingFns;
	CompactingFnVector _compactingFns;
};

/// A GC context. Base class. GC users should create Contexts.
class ContextBase
{
public:
	static constexpr const char *THREAD_NAME = "OMR::GC::Context";

	System &system() const noexcept { return *_system; }

	MM_EnvironmentBase *env() const noexcept { return MM_EnvironmentBase::getEnvironment(vmContext()); }

	OMR_VMThread *vmContext() const noexcept { return _vmContext; }

protected:
	ContextBase(System &system)
	    : _thread(system.runtime().platform().thread()), _system(&system), _vmContext(nullptr)
	{
		attachVmContext(system.vm(), *this);
	}

	~ContextBase() noexcept {
		detachVmContext(_system->vm(), *this);
		_system = nullptr;
		_vmContext = nullptr;
	}

private:
	Thread _thread;
	System *_system;
	OMR_VMThread *_vmContext; //< TODO: Delete this when we can.

	static void attachVmContext(OMR_VM &vm, ContextBase &cx);

	static void detachVmContext(OMR_VM &vm, ContextBase &cx);
};

class Context : public ContextBase {
public:
	explicit inline Context(System& system) : ContextBase(system) {
		system.contexts().add(this);
	}

	~Context() noexcept {
		system().contexts().remove(this);
	}

	StackRootList &stackRoots() noexcept { return _stackRoots; }

	MarkingFnVector &markingFns() noexcept { return _userMarkingFns; }

	const MarkingFnVector &markingFns() const noexcept { return _userMarkingFns; }

	CompactingFnVector &compactingFns() noexcept { return _compactingFns; }

	const CompactingFnVector &compactingFns() const noexcept { return _compactingFns; }

	ScavengingFnVector &scavengingFns() noexcept { return _scavengingFns; }

	const ScavengingFnVector &scavengingFns() const noexcept { return _scavengingFns; }

	LocalHeapCache& heapCache() noexcept { return _heapCache; }

	const LocalHeapCache& heapCache() const noexcept { return _heapCache; }

	NonZeroLocalHeapCache& nonZeroHeapCache() noexcept { return _nonZeroHeapCache; }

	const NonZeroLocalHeapCache& nonZeroHeapCache() const noexcept { return _nonZeroHeapCache; }

	ContextListNode& node() noexcept { return _node; }

	const ContextListNode& node() const noexcept { return _node; }

private:
	LocalHeapCache _heapCache;
	NonZeroLocalHeapCache _nonZeroHeapCache;
	ContextListNode _node;
	StackRootList _stackRoots;
	MarkingFnVector _userMarkingFns;
	ScavengingFnVector _scavengingFns;
	CompactingFnVector _compactingFns;
};

// static_assert(std::is_standard_layout<Context>::value,
//               "The GC context must be a StandardLayoutType for calculating JIT offsets.");

/// Backwards compatible alias
using RunContext = Context;

/// A special limited context that is only used during startup or shutdown.
class StartupContext : public Context
{
private:
	/// Only the GC::System can bring up a startup context.
	friend class System;

	StartupContext(System &system) : Context(system) {}

	StartupContext(const StartupContext &other) = delete;
};

/// @group Context conversion routines
/// @{

inline Context &
getContext(OMR_VMThread *vmContext)
{
	return *static_cast<Context *>(vmContext->_language_vmthread);
}

inline Context &
getContext(MM_EnvironmentBase *env)
{
	return getContext(env->getOmrVMThread());
}

/// @}

} // namespace GC
} // namespace OMR

#endif // OMR_GC_SYSTEM_HPP_
