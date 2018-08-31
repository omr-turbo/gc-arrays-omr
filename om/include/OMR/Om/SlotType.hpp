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

#if !defined OMR_OM_SLOTTYPE_HPP_
#define OMR_OM_SLOTTYPE_HPP_

#include <OMR/Om/CoreType.hpp>
#include <OMR/Om/Id.hpp>

#include <cstdlib>

namespace OMR {
namespace Om {

class SlotType {
public:
	constexpr SlotType(const SlotType&) = default;

	constexpr SlotType(CoreType coreType) noexcept : SlotType(Id(0), coreType) {}

	constexpr SlotType(Id id, CoreType coreType) noexcept : id_(id), coreType_(coreType) {}

	constexpr CoreType coreType() const noexcept { return coreType_; }

	constexpr Id id() const noexcept { return id_; }

	SlotType& id(Id id) noexcept {
		id_ = id;
		return *this;
	}

	constexpr std::size_t hash() const noexcept {
		return Hash::mix(id_.hash(), std::size_t(coreType()));
	}

	constexpr bool operator==(const SlotType& rhs) const noexcept { return id_ == rhs.id_; }

	constexpr bool operator!=(const SlotType& rhs) const noexcept { return id_ != rhs.id_; }

	std::size_t width() const noexcept { return coreTypeWidth(coreType()); }

private:
	Id id_;
	CoreType coreType_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SLOTTYPE_HPP_
