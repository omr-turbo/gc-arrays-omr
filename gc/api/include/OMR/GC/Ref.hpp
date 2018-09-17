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

#if !defined(OMR_GC_REF_HPP_)
#define OMR_GC_REF_HPP_

#include <OMR/GC/Address.hpp>

#include <OMR/CanDereference.hpp>
#include <cstdint>
#include <type_traits>

namespace OMR
{
namespace GC
{

template<typename T>
class RefBase
{
public:
	/// Obtain the raw pointer this Ref<T> represents.
	constexpr T* get() const noexcept { return _value; }

protected:
	RefBase() = default;

	RefBase(T* value) : _value(value) {}

	T* _value;
};

/// Define pointer-style operators when T is not void.
template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class RefPointerOperators : public RefBase<T>
{
public:
	constexpr T* operator->() const noexcept { return this->get(); }

	constexpr T& operator*() const noexcept { return *this->get(); }

protected:
	using RefBase<T>::RefBase;
};

/// Empty set of pointer operators when T is (possible const or volatile) void.
template<typename T>
class RefPointerOperators<T, false> : public RefBase<T>
{
protected:
	using RefBase<T>::RefBase;
};

/// A pointer to managed memory. It is GC-Unsafe. This is a very simple
/// pointer-wrapper.
template<typename T>
class Ref : public RefPointerOperators<T>
{
public:
	Ref() = default;

	constexpr Ref(std::nullptr_t) : RefPointerOperators<T>(nullptr) {}

	template<typename U>
	constexpr Ref(U* ptr) : RefPointerOperators<T>(ptr)
	{}

	template<typename U>
	constexpr Ref(Ref<U> other) : RefPointerOperators<T>(other.get())
	{
		static_assert(std::is_convertible<U*, T*>::value,
		              "U* must be convertible to T* "
		              "If not, use castRef<U> to explicitly cast.");
	}

	template<typename U>
	Ref<T>& operator=(U* rhs) noexcept
	{
		this->_value = rhs;
	}

	template<typename U>
	Ref<T> operator=(const Ref<U>& rhs) noexcept
	{
		this->_value = rhs.get();
	}

	constexpr T* get() const noexcept { return this->_value; }

	/// The address of the referrent.
	Address toAddress() const { return reinterpret_cast<Address>(this->_value); }

	/// @group Comparisons
	/// @{

	template<typename U>
	constexpr bool operator==(const Ref<U>& rhs) const noexcept
	{
		return this->_value == rhs.get();
	}

	template<typename U>
	constexpr bool operator==(U* rhs) const noexcept
	{
		return this->_value == rhs;
	}

	constexpr bool operator==(std::nullptr_t) const noexcept { return this->_value == nullptr; }

	template<typename U>
	constexpr bool operator!=(const Ref<U>& rhs) const noexcept
	{
		return this->_value != rhs.get();
	}

	template<typename U>
	constexpr bool operator!=(U* rhs) const noexcept
	{
		return this->_value != rhs;
	}

	constexpr bool operator!=(std::nullptr_t) const noexcept { return this->_value != nullptr; }

	/// @todo: need more comparison operators.

	/// @}

	/// @group Type conversion
	/// @{

	/// Ref<T> can be cast to U*.
	explicit constexpr operator T*() const noexcept { return this->_value; }

	/// Ref<T> is convertible to Ref<U>.
	template<typename U>
	explicit operator std::enable_if<std::is_convertible<T*, U*>::value, Ref<U>>() const
	        noexcept
	{
		return Ref<U>(this->_value);
	}

	/// Ref<T> is implicitly convertible to Ref<const T>.
	operator Ref<const T>() const noexcept
	{
		return Ref<const T>(const_cast<const T*>(this->_value));
	}

	/// @}
};

template<typename U, typename T>
constexpr Ref<U>
reinterpretRef(Ref<T> ref)
{
	return Ref<U>(reinterpret_cast<U*>(ref.get()));
}

template<typename T>
constexpr bool
operator==(std::nullptr_t, Ref<T> ref)
{
	return ref == nullptr;
}

template<typename T>
constexpr bool
operator!=(std::nullptr_t, Ref<T> ref)
{
	return ref != nullptr;
}

} // namespace GC
} // namespace OMR

#endif // OMR_OM_REF_HPP_
