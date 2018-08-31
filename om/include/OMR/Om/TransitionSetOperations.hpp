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

#if !defined(OMR_OM_TRANSITIONSET_INL_HPP_)
#define OMR_OM_TRANSITIONSET_INL_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/Om/MemArrayOperations.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/TransitionSet.hpp>

namespace OMR {
namespace Om {

/// TODO: Write barrier?
inline bool initializeTransitionSet(Context& cx,
                                    InnerHandle<TransitionSet> set,
                                    std::size_t initialSetSize = 32) {
	assert(!set->initialized());
	return initializeMemArray(
	        cx,
	        InnerHandle<MemArray<TransitionSetEntry>>(set, &TransitionSet::table),
	        initialSetSize);
}

/// Store a transition entry into the transition set. Can fail if the transition set is full.
/// TODO: Make updates to table atomic
/// TODO: Do we want a write barrier on the transition stores
/// TODO: Make transition table resizable
inline bool tryStoreTransition(TransitionSet& set, TransitionSetEntry entry, std::size_t hash) {
	if (!set.table.initialized()) {
		return false;
	}

	std::size_t sz = set.size();

	for (std::size_t i = 0; i < sz; i++) {
		std::size_t idx = (hash + i) % sz;
		if (set.table[idx].shape == nullptr) {
			set.table[idx] = entry;
			return true;
		}
	}
	return false;
}

inline bool storeTransition(Context& cx,
                            InnerHandle<TransitionSet> set,
                            TransitionSetEntry entry,
                            std::size_t hash) {
	if (!set->initialized()) {
		initializeTransitionSet(cx, set);
	}
	return tryStoreTransition(*set, entry, hash);
}

/// Lookup a transition in the set, using a precalculated hash. Returns nullptr on failure.
inline Shape*
lookUpTransition(TransitionSet& set, Span<const SlotAttr> attributes, std::size_t hash) {
	if (!set.initialized()) {
		return nullptr;
	}

	std::size_t size = set.size();

	for (std::size_t i = 0; i < size; i++) {
		std::size_t index = (hash + i) % size;
		Shape* shape = set.table.at(index).shape;

		if (shape == nullptr) {
			return nullptr;
		}
		if (attributes == shape->instanceSlotAttrs()) {
			return shape;
		}
	}
	return nullptr;
}

/// Lookup a transition. Returns nullptr on failure.
inline Shape* lookUpTransition(TransitionSet& set, Span<const SlotAttr> attributes) {
	return lookUpTransition(set, attributes, hash(attributes));
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_TRANSITIONSET_INL_HPP_
