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

#if !defined(OMR_OM_ID_HPP_)
#define OMR_OM_ID_HPP_

#include <OMR/Om/HashUtilities.hpp>

#include <cstddef>
#include <functional>

namespace OMR {
namespace Om {
using RawId = std::uint32_t;

class Id {
public:
	Id() = default;

	constexpr Id(const Id&) = default;

	constexpr explicit Id(RawId data) : data_(data) {}

	constexpr bool operator==(const Id& rhs) const noexcept { return data_ == rhs.data_; }

	constexpr bool operator!=(const Id& rhs) const noexcept { return data_ != rhs.data_; }

	constexpr bool isObject() const { return (data_ & TAG_MASK) == OBJECT_TAG; }

	constexpr bool isInteger() const { return (data_ & TAG_MASK) == FIXNUM_TAG; }

	constexpr RawId raw() const { return data_; }

	constexpr std::size_t hash() const { return Hash::hash(data_); }

private:
	static constexpr RawId HASH_SHIFT = 0x2;
	static constexpr RawId TAG_MASK = 0x3;
	static constexpr RawId FIXNUM_TAG = 0x0;
	static constexpr RawId OBJECT_TAG = 0x1;

	RawId data_;
};

class IdGenerator {
public:
	Id newId() { return Id(next_++); }

private:
	RawId next_ = 0;
};

} // namespace Om
} // namespace OMR

namespace std {
template<>
struct hash<OMR::Om::Id> {
	constexpr size_t operator()(const OMR::Om::Id& id) const noexcept { return id.raw(); }
};

} // namespace std

#endif // OMR_OM_ID_HPP_
