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

#if !defined(OMR_OM_BARRIER_HPP_)
#define OMR_OM_BARRIER_HPP_

#include <OMR/Om/Context.hpp>

// #include <StandardWriteBarrier.hpp>

namespace OMR {
namespace Om {

class BaseBarrier {
public:
	static void preWrite(Context& cx, void* cell, void* ref) {
		// Nothing yet.
	}

	static void postWrite(Context& cx, void* cell, void* ref) {
		// standardWriteBarrier(cx.vmContext(), cell, ref);
	}
};

template<typename SlotHandleT>
struct Barrier;

/// General read/write barriers for slots.
template<typename SlotHandleT>
struct Barrier : public BaseBarrier {
public:
	/// Store to slot with pre/post GC barriers.
	template<typename T = void*>
	static void writeReference(Context& cx, void* cell, SlotHandleT slot, T ref) {
		preWrite(cx, cell, slot.readReference());
		slot.writeReference(ref);
		postWrite(cx, cell, slot.readReference());
	}

	/// Store to slot atomically, with pre/post GC writebarriers.
	template<typename T = void*>
	static void atomicWriteReference(Context& cx, void* cell, SlotHandleT slot, T ref) {
		preWrite(cx, cell, slot.readReference());
		slot.atomicWriteReference(ref);
		postWrite(cx, cell, ref);
	}
};

/// Barrier specialization for Value slots. Specialization for value slot handles, where
/// the overwritten value may not be a reference.
template<>
struct Barrier<ValueSlotHandle> : public BaseBarrier {
public:
	/// Store to slot with pre/post GC barriers
	template<typename T = void*>
	static void writeReference(Context& cx, void* cell, ValueSlotHandle slot, T ref) {
		if (slot.isReference()) {
			preWrite(cx, cell, slot.readReference());
		}
		slot.writeReference(ref);
		postWrite(cx, cell, slot.readReference());
	}

	/// Store to slot atomically, with pre/post GC writebarriers.
	template<typename T = void*>
	static void atomicWriteReference(Context& cx, void* cell, ValueSlotHandle slot, T ref) {
		if (slot.isReference()) {
			preWrite(cx, cell, slot.readReference());
		}
		slot.atomicWriteReference(ref);
		postWrite(cx, cell, ref);
	}

	///
	/// Store to slot with pre/post GC barriers
	template<typename T = void*>
	static void write(Context& cx, void* cell, ValueSlotHandle slot, T ref) {
		if (slot.isReference()) {
			preWrite(cx, cell, slot.readReference());
		}
		slot.writeReference(ref);
		postWrite(cx, cell, slot.readReference());
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_BARRIER_HPP_
