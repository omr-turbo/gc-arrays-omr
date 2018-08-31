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

#ifndef OMR_OM_ALLOCATOR_HPP_
#define OMR_OM_ALLOCATOR_HPP_

#include <OMR/Om/Allocation.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/GC/Handle.hpp>
#include <OMR/Om/Id.hpp>
#include <OMR/GC/StackRoot.hpp>

#include <cstdlib>
#include <omrgc.h>

namespace OMR {
namespace Om {

class Cell;
class Context;

constexpr bool DEBUG_SLOW = true;

struct BaseAllocator {
	template<typename ResultT = Cell, typename InitializerT>
	static ResultT*
	allocate(Context& cx, InitializerT& init, std::size_t size = sizeof(ResultT)) {
		Allocation allocation(cx, init, size);
		ResultT* result = (ResultT*)allocation.allocateAndInitializeObject(cx.gc().vm());

		if (DEBUG_SLOW) {
			GC::StackRoot<ResultT> root(cx.gc(), result);
			OMR_GC_SystemCollect(cx.gc().vm(), 0);
			result = root.get();
		}

		return result;
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ALLOCATOR_HPP_
