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

#if !defined(OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_)
#define OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_

#include "omrcfg.h"

#include "objectdescription.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>

namespace OMR {
namespace GC {

using CompressedRef = std::uint32_t;

/// A handle to a slot containing a compressed reference.
class CompressedRefSlotHandle
{
public:
	CompressedRefSlotHandle(CompressedRef *slot, std::size_t shift) : _slot(slot), _shift(shift) {}

	void *toAddress() const noexcept { return _slot; }

	omrobjectptr_t readReference() const noexcept {
		return omrobjectptr_t(std::uint64_t(*_slot) << _shift);
	}

	void writeReference(omrobjectptr_t value) const noexcept {
		*_slot = CompressedRef(std::uint64_t(value) >> _shift);
	}

	void atomicWriteReference(omrobjectptr_t value) const noexcept { assert(0); }

private:
	CompressedRef *_slot;
	std::size_t _shift;
};


}  // namespace GC
}  // namespace OMR

#endif // OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_
