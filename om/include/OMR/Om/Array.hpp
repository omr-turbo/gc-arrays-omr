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

#if !defined(OMR_OM_ARRAY_HPP_)
#define OMR_OM_ARRAY_HPP_

#include <OMR/Om/CellHeader.hpp>
#include <OMR/GC/Handle.hpp>
#include <OMR/Om/HashUtilities.hpp>

#include <cassert>
#include <type_traits>

namespace OMR {
namespace Om {

/// A managed cell of memory providing general-purpose raw storage.
/// Note that the Array is considered a leaf-object, and references stored
/// into the buffer must be explicitly marked. Arrays are of a fixed, limited size.
/// For a more powerful array-type, see Vector and ArrayRef. Arrays are primarily low level storage
/// for other higher level abstractions.
class Array {
public:
	CellHeader& header() noexcept { return header_; }

	const CellHeader& header() const noexcept { return header_; }

	/// The array has a generic method for calculating a hash of the data.
	/// In general, it is preferable that the user provides a less-general hashing
	/// function.
	std::size_t hash() const noexcept {
		std::size_t hash = size_ + 7;
		for (std::size_t i = 0; i < size_; i++) {
			hash = Hash::mix(hash, data_[i]);
		}
		return hash;
	}

	/// The full size of this heap cell, in bytes.
	std::size_t cellSize() const noexcept { return cellSize(size_); }

	/// The raw data pointer
	void* data() noexcept { return data_; }

	const void* data() const noexcept { return data_; }

	/// The size of the data buffer, in bytes.
	std::size_t dataSize() const noexcept { return size_; }

protected:
	friend class ArrayInitializer;
	friend class ArrayOffsets;

	/// Get the allocation size of an array buffer with storage for `dataSize`
	/// bytes.
	static std::size_t cellSize(std::size_t dataSize) { return dataSize + sizeof(Array); }

	inline Array() = default;

	inline Array(Shape* shape, std::size_t size) : header_(shape), size_(size) {}

	CellHeader header_;
	std::size_t size_; //< size in bytes of the data array.
	std::uintptr_t data_[0];
};

static_assert(std::is_standard_layout<Array>::value, "Array must be a StandardLayoutType");

class ArrayOffsets {
public:
	ArrayOffsets() = delete;
	static constexpr std::size_t size = offsetof(Array, size_);
	static constexpr std::size_t data = offsetof(Array, data_);
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAY_HPP_
