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

#if !defined(OMR_OM_MEMARRAY_HPP_)
#define OMR_OM_MEMARRAY_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/BasicSlotHandle.hpp>
#include <OMR/Om/InnerHandle.hpp>

#include <cstdint>

namespace OMR {
namespace Om {
/// A simple array API backed by a heap-allocated Array.
/// The array understands it's size, but has no notion of capacity, or unfilled
/// slots. MemArrays can be resized.
/// @see-also: MemArrayOps
template<typename T>
class MemArray {
public:
	static_assert(std::is_trivial<T>::value, "The MemArray only supports simple type storage.");

	constexpr MemArray() : buffer_(nullptr) {}

	constexpr bool initialized() const noexcept { return buffer_ != nullptr; }

	constexpr bool empty() const noexcept { return buffer_ == nullptr; }

	T* data() noexcept { return reinterpret_cast<T*>(buffer_->data()); }

	const T* data() const noexcept { return reinterpret_cast<T*>(buffer_->data()); }

	/// Number of elements in this array.
	std::size_t size() const noexcept { return buffer_->dataSize() / sizeof(T); }

	T& at(std::size_t index) noexcept { return data()[index]; }

	const T& at(std::size_t index) const noexcept { return data()[index]; }

	T& operator[](std::size_t index) noexcept { return at(index); }

	const T& operator[](std::size_t index) const noexcept { return at(index); }

	template<typename VisitorT>
	void visit(VisitorT visitor) {
		if (buffer_ != nullptr) {
			visitor.edge(this, BasicSlotHandle(&buffer_));
		}
	}

	Array* buffer_;
};

static_assert(std::is_standard_layout<MemArray<int>>::value,
              "MemArray must be a StandardLayoutType.");

} // namespace Om
} // namespace OMR

#endif // OMR_OM_MEMARRAY_HPP_
