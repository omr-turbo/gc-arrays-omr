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

#if !defined(OMR_OM_SLOTDESCRIPTOR_HPP_)
#define OMR_OM_SLOTDESCRIPTOR_HPP_

#include <OMR/Om/SlotAttr.hpp>
#include <OMR/Om/SlotIndex.hpp>

namespace OMR {
namespace Om {

/// Describes a slot's index and attributes. The SlotAttr is unowned data,
/// typically stored in a managed Shape, so it's not normally safe to hold a
/// SlotDescriptor across a GC safepoint. This is the result of searching for a slot in an object.
/// Users can use a SlotDescriptor or SlotIndex to access a value in an object.
class SlotDescriptor : public SlotIndex {
public:
	SlotDescriptor() = default;

	SlotDescriptor(const SlotDescriptor&) = default;

	SlotDescriptor(SlotIndex index, const SlotAttr* attr) : SlotIndex(index), attr_(attr) {}

	const SlotAttr& attr() const noexcept { return *attr_; }

	bool operator==(const SlotDescriptor& rhs) const noexcept {
		return (offset() == rhs.offset()) && (attr() == rhs.attr());
	}

	bool operator!=(const SlotDescriptor& rhs) const noexcept {
		return (offset() != rhs.offset()) || (attr() != rhs.attr());
	}

private:
	const SlotAttr* attr_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SLOTDESCRIPTOR_HPP_
