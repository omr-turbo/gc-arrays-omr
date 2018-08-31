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

#if !defined(OMR_OM_SLOTATTR_HPP_)
#define OMR_OM_SLOTATTR_HPP_

#include <OMR/Om/HashUtilities.hpp>
#include <OMR/Om/Id.hpp>
#include <OMR/Om/SlotType.hpp>
#include <OMR/Om/Span.hpp>
#include <OMR/Om/Value.hpp>

#include <cstddef>
#include <functional>

namespace OMR {
namespace Om {

class SlotAttr {
public:
	/// Allocate a SlotAttr, where the type is {id: 0, coreType: VALUE}
	constexpr explicit SlotAttr(Id id) noexcept : type_(Id(0), CoreType::VALUE), id_(id) {}

	constexpr SlotAttr(const SlotAttr&) noexcept = default;

	constexpr SlotAttr(const SlotType& type, const Id& id) noexcept : type_(type), id_(id) {}

	constexpr Id id() const noexcept { return id_; }

	SlotAttr& id(Id id) {
		id_ = id;
		return *this;
	}

	SlotType& type() noexcept { return type_; }

	constexpr const SlotType& type() const noexcept { return type_; }

	SlotAttr& type(const SlotType& type) noexcept {
		type_ = type;
		return *this;
	}

	constexpr std::size_t hash() const noexcept {
		return Hash::mix(id().hash(), type().hash());
	}

	constexpr bool operator==(const SlotAttr& rhs) const noexcept {
		return (id_ == rhs.id_) && (type_ == rhs.type_);
	}

	constexpr bool operator!=(const SlotAttr& rhs) const noexcept {
		return (id_ != rhs.id_) || (type_ != rhs.type_);
	}

	/// The fundamental type of the slot.
	constexpr CoreType coreType() const noexcept { return type_.coreType(); }

	/// Width of the slot's value.
	std::size_t width() const noexcept { return coreTypeWidth(coreType()); }

private:
	SlotType type_;
	Id id_;
};

static_assert(sizeof(SlotAttr) == sizeof(SlotType) + sizeof(Id),
              "SlotAttr must have no padding--needed for memcmp.");

static_assert(std::is_standard_layout<SlotAttr>::value,
              "Slot Descriptors are heap allocated, so must be StandardLayoutType.");

/// @group Span operations
/// @{

/// Deep hash of a Span of SlotAttrs.
inline std::size_t hash(const Span<const SlotAttr>& ds) {
	std::size_t h = 7;
	for (const SlotAttr& d : ds) {
		Hash::mix(h, d.hash());
	}
	return h;
}

/// Deep compare of two spans of SlotAttrs.
/// The two spans must have the same length and data.
inline bool operator==(const Span<const SlotAttr>& lhs, const Span<const SlotAttr>& rhs) {
	if (lhs.length() != rhs.length()) {
		return true;
	}

	if (lhs.value() == rhs.value()) {
		return true;
	}

	// TODO: if SlotAttr padding is zero, this could be memcmp'd.
	for (std::size_t i = 0; i < lhs.length(); i++) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

inline bool operator!=(const Span<const SlotAttr>& lhs, const Span<const SlotAttr>& rhs) {
	return !(lhs == rhs);
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SLOTATTR_HPP_
