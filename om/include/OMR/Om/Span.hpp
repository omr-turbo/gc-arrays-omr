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

#if !defined(OMR_OM_SPAN_HPP_)
#define OMR_OM_SPAN_HPP_

#include <array>
#include <cstddef>

namespace OMR {
namespace Om {
/// A span of memory. Pointer and length pair.
template<typename Type>
class Span {
public:
	// uninitialized constructor.
	inline constexpr Span() = default;

	/// Full initialization.
	inline constexpr Span(Type* value, std::size_t length) : value_{value}, length_{length} {}

	template<std::size_t N>
	inline constexpr Span(std::array<Type, N>& array) : value_(array.data()), length_(N) {}

	template<std::size_t N>
	inline constexpr Span(const std::array<Type, N>& array)
	        : value_(array.data()), length_(N) {}

	/// Init from pointer. Span has zero length.
	explicit inline constexpr Span(Type* value) : value_{value}, length_{0} {}

	/// Init from length. Value is nullptr.
	explicit inline constexpr Span(std::size_t length) : value_{nullptr}, length_{length} {}

	/// Assignment from span.
	template<typename U>
	inline constexpr Span(const Span<U>& other)
	        : value_{other.value()}, length_{other.length()} {}

	/// Array-index operator.
	constexpr Type& operator[](std::size_t index) const { return at(index); }

	/// Array-index operator. TODO: Bounds checking
	constexpr Type& at(std::size_t index) const { return value()[index]; }

	/// Get the pointer.
	inline constexpr Type* operator()() const { return value_; }

	/// Get the pointer.
	constexpr Type* value() const noexcept { return value_; }

	/// Set the pointer value.
	inline Span<Type>& value(Type* value) {
		value_ = value;
		return *this;
	}

	/// Number of elements in the span.
	inline constexpr std::size_t length() const { return length_; }

	/// Set the number of elements in the span.
	inline Span<Type>& length(std::size_t length) {
		length_ = length;
		return *this;
	}

	/// The last instance of Type in this Span.
	inline Type* last() { return value_ + (length_ - 1); }

	/// Size of the referred span, in bytes.
	inline std::size_t sizeInBytes() const { return length() * sizeof(Type); }

	inline Span<Type>& operator=(Span<Type>& rhs) {
		value_ = rhs.value();
		length_ = rhs.length();
		return *this;
	}

	/// Return a pointer to the start of the span.
	Type* begin() const noexcept { return value(); }

	/// Return an pointer that is "one past the end".
	Type* end() const noexcept { return value() + length(); }

	const Type* cbegin() const noexcept { return value(); }

	const Type* cend() const noexcept { return value() + length(); }

protected:
	Type* value_;
	std::size_t length_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SPAN_HPP_
