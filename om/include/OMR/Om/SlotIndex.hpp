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

#if !defined(OMR_OM_SLOTINDEX_HPP_)
#define OMR_OM_SLOTINDEX_HPP_

#include <cstdlib>

namespace OMR {
namespace Om {

using Index = std::size_t;

/// An index into an object. A SlotIndex can be used to find the address of a
/// slot inside an object. Turning an index into a useful value also requires
/// that you know the type of the slot.
///
/// TODO: Make SlotIndex only constructible by ObjectMaps and their iterators.
class SlotIndex {
public:
	SlotIndex() noexcept = default;

	constexpr SlotIndex(const SlotIndex&) noexcept = default;

	constexpr SlotIndex(std::size_t offset) noexcept : offset_(offset) {}

	SlotIndex& operator=(const SlotIndex& rhs) {
		offset_ = rhs.offset_;
		return *this;
	}

	constexpr bool operator==(const SlotIndex& rhs) const noexcept {
		return offset_ == rhs.offset_;
	}

	constexpr bool operator!=(const SlotIndex& rhs) const noexcept {
		return offset_ != rhs.offset_;
	}

	/// Convert the SlotIndex into a raw offset into an object.
	std::size_t offset() const noexcept { return offset_; }

private:
	std::size_t offset_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SLOTINDEX_HPP_
