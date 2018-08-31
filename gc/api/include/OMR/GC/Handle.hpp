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

#if !defined(OMR_GC_HANDLE_HPP_)
#define OMR_GC_HANDLE_HPP_

#include <OMR/CanDereference.hpp>
#include <cstddef>

namespace OMR
{
namespace GC
{

/// A pointer to a rooted GC reference to T.
template<typename T>
using RawHandle = T* const*;

template<typename T>
class HandleBase
{
public:
	constexpr T* get() const noexcept { return *_root; }

protected:
	constexpr HandleBase() = default;

	constexpr HandleBase(RawHandle<T> root) noexcept : _root(root) {}

	RawHandle<T> _root;
};

/// Default implementation of HandlePointerOperators.
/// Define pointer-style operators when T* is dereferenceable (essentially, when T* != void*).
template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class HandlePointerOperators : public HandleBase<T>
{
public:
	constexpr T& operator*() const noexcept { return *this->get(); }

	constexpr T* operator->() const noexcept { return this->get(); }

protected:
	using HandleBase<T>::HandleBase;
};

/// Empty pointer operators set, used when T* is not dereferenceable.
template<typename T>
class HandlePointerOperators<T, false> : public HandleBase<T>
{
protected:
	using HandleBase<T>::HandleBase;
};

/// The Handle<T> provides pointer semantics by indirecting through roots.
///
/// A Handle<T> is associated with a single root slot, such as an instance of StackRoot<T>. When a Handle<T> is
/// dereferenced, it will access the target indirectly through the associated root. Because Handles always indirect
/// through roots, they are safe to hold across a collection. A Handle is invalid when it's root is destroyed. Handles
/// are fast and flexible to use, but care must be taken so a Handle does not outlive it's root.
template<typename T>
class Handle : public HandlePointerOperators<T>
{
public:
	constexpr Handle() noexcept : HandlePointerOperators<T>(nullptr) {}

	constexpr Handle(std::nullptr_t) noexcept : HandlePointerOperators<T>(nullptr) {}

	template<typename U>
	constexpr Handle(RawHandle<U> rootAddr) : HandlePointerOperators<T>(rootAddr)
	{}

	template<typename U>
	constexpr Handle(const Handle<U>& other) noexcept
	        : HandlePointerOperators<T>(reinterpret_cast<RawHandle<T>>(other.raw()))
	{
		static_assert(std::is_convertible<U*, T*>::value, "U* must be convertible to T*.");
	}

	RawHandle<T> raw() const noexcept { return this->_root; }

	template<typename U>
	constexpr bool operator==(const Handle<U>& other) const noexcept
	{
		return this->get() == other.get();
	}

	template<typename U>
	constexpr bool operator!=(const Handle<U>& other) const noexcept
	{
		return this->get() != other.get();
	}

	constexpr bool operator==(std::nullptr_t) const noexcept { return this->get() == nullptr; }

	constexpr bool operator!=(std::nullptr_t) const noexcept { return this->get() != nullptr; }

	constexpr operator T*() const { return this->get(); }

	template<typename U>
	constexpr operator std::enable_if<std::is_convertible<T*, U*>::value, Handle<U>>() const
	        noexcept
	{
		return Handle<U>(reinterpret_cast<RawHandle<U>>(raw()));
	}
};

template<typename U, typename T>
constexpr Handle<U>
reinterpretHandle(Handle<T> handle) noexcept
{
	return Handle<U>(reinterpret_cast<RawHandle<U>>(handle.raw()));
}

template<typename T>
constexpr bool
operator==(std::nullptr_t, const Handle<T>& handle) noexcept
{
	return handle == nullptr;
}

template<typename T>
constexpr bool
operator!=(std::nullptr_t, const Handle<T>& handle) noexcept
{
	return handle != nullptr;
}

} // namespace GC
} // namespace OMR

#endif // OMR_GC_HANDLE_HPP_
