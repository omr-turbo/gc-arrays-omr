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

#include <OMR/GC/MarkingFn.hpp>
#include <OMR/GC/ScavengingFn.hpp>
#include <OMR/GC/StackRootList.hpp>
#include <OMR/GC/LocalHeapCache.hpp>
#include <EnvironmentBase.hpp>
#include <OMR/Runtime.hpp>
#include <OMR_VMThread.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mminitcore.h>
#include <new>
#include <omrutil.h>
#include <set>
#include <type_traits>
#include <unordered_set>

class MM_EnvironmentBase;

namespace OMR
{
namespace GC
{

class Visitor;
class Context;
class RunContext;
class StartupContext;

class StartupError : public ::std::runtime_error
{
	using runtime_error::runtime_error;
};

using ContextSet = ::std::set<Context *>;

void
attachVmContext(OMR_VM &vm, Context &cx);

void
detachVmContext(OMR_VM &vm, Context &cx);

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

	ContextSet &contexts() { return _contexts; }

	const ContextSet &contexts() const { return _contexts; }

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
	MarkingFnVector _userRoots;
	ScavengingFnVector _scavengingFns;
	ContextSet _contexts;
};

/// A GC context. Base class. GC users should create RunContexts.
class Context
{
public:
	static constexpr const char *THREAD_NAME = "OMR::GC::Context";

	explicit inline Context(System &system);

	Context(const Context &other) = delete;

	inline ~Context() noexcept;

	System &system() const noexcept { return *_system; }

	inline MM_EnvironmentBase *env() const noexcept;

	inline OMR_VMThread *vm() const noexcept { return _vmContext; }

	StackRootList &stackRoots() noexcept { return _stackRoots; }

	MarkingFnVector &markingFns() noexcept { return _userMarkingFns; }

	const MarkingFnVector &markingFns() const noexcept { return _userMarkingFns; }

	ScavengingFnVector &scavengingFns() noexcept { return _scavengingFns; }

	const ScavengingFnVector &scavengingFns() const noexcept { return _scavengingFns; }

	LocalHeapCache& heapCache() noexcept { return _heapCache; }

	const LocalHeapCache& heapCache() const noexcept { return _heapCache; }

	NonZeroLocalHeapCache& nonZeroHeapCache() noexcept { return _nonZeroHeapCache; }

	const NonZeroLocalHeapCache& nonZeroHeapCache() const noexcept { return _nonZeroHeapCache; }

private:
	friend class RunContext;
	friend class StartupContext;

	static void attachVmContext(OMR_VM &vm, Context &cx);

	static void detachVmContext(OMR_VM &vm, Context &cx);

	LocalHeapCache _heapCache;
	NonZeroLocalHeapCache _nonZeroHeapCache;
	Thread _thread;
	System *_system;
	OMR_VMThread *_vmContext; //< TODO: Delete this when we can.
	StackRootList _stackRoots;
	MarkingFnVector _userMarkingFns;
	ScavengingFnVector _scavengingFns;

};

static_assert(std::is_standard_layout<Context>::value,
              "The GC context must be a StandardLayoutType for calculating JIT offsets.");

inline Context::Context(System &system)
        : _thread(system.runtime().platform().thread()), _system(&system), _vmContext(nullptr)
{
	_system->contexts().insert(this);
	attachVmContext(system.vm(), *this);
}

inline Context::~Context() noexcept
{
	_system->contexts().erase(this);
	detachVmContext(_system->vm(), *this);
	_vmContext = nullptr;
}

inline MM_EnvironmentBase *
Context::env() const noexcept
{
	return MM_EnvironmentBase::getEnvironment(vm());
}

/// A special limited context that is only used during startup or shutdown.
class StartupContext : public Context
{
private:
	/// Only the GC::System can bring up a startup context.
	friend class System;

	StartupContext(System &system) : Context(system) {}

	StartupContext(const StartupContext &other) = delete;
};

/// A full runtime context.
class RunContext : public Context
{
public:
	RunContext(System &system) : Context(system) {}
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
