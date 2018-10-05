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

#if !defined(OMR_RUNTIME_HPP_)
#define OMR_RUNTIME_HPP_

#include "StartupManagerImpl.hpp"
#include "mminitcore.h"
#include "omr.h"
#include "omrgcstartup.hpp"
#include "omrport.h"
#include "omrprofiler.h"
#include "omrrasinit.h"
#include "omrutil.h"
#include "omrvm.h"
#include "thread_api.h"
#include <StartupManagerImpl.hpp>
#include <omr.h>
#include <omrgcstartup.hpp>
#include <omrport.h>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <thread_api.h>

namespace OMR
{

class PlatformError : public std::exception
{
public:
	PlatformError(int error) : _error(error) {}

	int error() noexcept { return _error; }

private:
	int _error;
};

/// OMR Thread library wrapper
class ThreadInterface
{
public:
	static void init()
	{
		auto e = omrthread_init_library();
		if (e != 0) {
			throw PlatformError(e);
		}
	}

	static void tearDown() noexcept { omrthread_shutdown_library(); }

	ThreadInterface() { init(); }

	~ThreadInterface() noexcept { tearDown(); }

private:
};

/// omrthread wrapper.
class Thread
{
public:
	Thread(const ThreadInterface &ithread)
	{
		auto e = omrthread_attach_ex(&_self, J9THREAD_ATTR_DEFAULT);
		if (e != 0) {
			throw PlatformError(e);
		}
	}

	~Thread() noexcept { omrthread_detach(_self); }

	omrthread_t data() { return _self; }

	const omrthread_t data() const { return _self; }

private:
	omrthread_t _self;
};

/// OMR Thread and Port Library Wrapper
class PlatformInterface
{
public:
	PlatformInterface()
	{
		Thread self(_thread);

		auto e = omrport_init_library(&library(), sizeof(OMRPortLibrary));
		if (e != 0) {
			throw PlatformError(e);
		}
	}

	~PlatformInterface() noexcept
	{
		Thread self(_thread);
		library().port_shutdown_library(&library());
	}

	OMRPortLibrary &library() noexcept { return _library; }

	const OMRPortLibrary &library() const noexcept { return _library; }

	const ThreadInterface &thread() const noexcept { return _thread; }

private:
	ThreadInterface _thread;
	OMRPortLibrary _library;
};

/// Process-wide initialization and tear down.
/// RAII wrapper for OMR_Runtime init and teardown.
class Runtime
{
public:
	/// Initialize the process runtime.
	Runtime()
	{
		memset(&_data, 0, sizeof(OMR_Runtime));
		_data._configuration._maximum_vm_count = 0;
		_data._vmCount = 0;
		_data._vmList = nullptr;
		_data._portLibrary = &platform().library();
		auto e = omr_initialize_runtime(&_data);
		if (e != 0) {
			throw PlatformError(e);
		}
	}

	~Runtime() noexcept { omr_destroy_runtime(&_data); }

	PlatformInterface &platform() { return _platform; }

	const PlatformInterface &platform() const { return _platform; }

	OMR_Runtime &data() { return _data; }

	const OMR_Runtime &data() const { return _data; }

private:
	PlatformInterface _platform;
	OMR_Runtime _data;
};

} // namespace OMR

#endif // OMR_RUNTIME_HPP_
