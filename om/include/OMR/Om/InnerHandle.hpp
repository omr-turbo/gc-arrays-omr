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

#if !defined(OMR_GC_INNERHANDLE_HPP_)
#define OMR_GC_INNERHANDLE_HPP_

#include <OMR/Om/PointerUtilities.hpp>

#include <OMR/GC/Handle.hpp>

namespace OMR {
namespace Om {
class Cell;

template<typename T>
class InnerHandleBase {
public:
	constexpr T* get() const noexcept { return ptradd<T>(_base.get(), _offset); }

protected:
	InnerHandleBase() noexcept = default;

	constexpr InnerHandleBase(GC::Handle<void> base, std::ptrdiff_t offset) noexcept
	        : _base(base), _offset(offset) {}

	GC::Handle<void> _base;
	std::ptrdiff_t _offset;
};

template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class InnerHandlePointerOperators : public InnerHandleBase<T> {
public:
	using InnerHandleBase<T>::InnerHandleBase;

	T* operator->() const { return this->get(); }

	T& operator*() const { return *this->get(); }
};

template<typename T>
class InnerHandlePointerOperators<T, false> : public InnerHandleBase<T> {
	using InnerHandleBase<T>::InnerHandleBase;
};

/// A handle to a field inside a cell. A InnerHandle has two
/// parts: a handle to the cell, and an offset into that cell. This class acts
/// as a handle, which ensures that the base Cell is rooted, and is safe to hold
/// across GC operations. See also: OMR::Om::Ref, OMR::Om::Root,
/// OMR::Om::Handle, OMR::Om::MemRef.
template<typename T>
class InnerHandle : public InnerHandlePointerOperators<T> {
public:
	InnerHandle() noexcept : InnerHandlePointerOperators<T>() {}

	/// Construct an inner handle into base, referencing p.
	template<typename U>
	InnerHandle(GC::Handle<U> base, T U::*p) noexcept
	        : InnerHandlePointerOperators<T>(base, ptrdiff(base.get(), &(base.get()->*p))) {}

	/// Construct an inner handle at a given offset into base.
	template<typename U>
	constexpr InnerHandle(const GC::Handle<U>& base, std::uintptr_t offset) noexcept
	        : InnerHandlePointerOperators<T>(base, offset) {}

	/// Copy a Member Handle
	template<typename U>
	constexpr InnerHandle(const InnerHandle<U>& other)
	        : InnerHandlePointerOperators<T>(other.base(), other.offset()) {}

	/// Construct a nested member from another member handle.
	template<typename U>
	constexpr InnerHandle(const InnerHandle<U>& base, std::uintptr_t offset)
	        : InnerHandlePointerOperators<T>(base.base(), base.offset() + offset) {}

	/// Construct a nested member from another member handle.
	template<typename U>
	InnerHandle(const InnerHandle<U>& base, T U::*member)
	        : InnerHandlePointerOperators<T>(
	                  base.base(),
	                  ptrdiff(base.base().get(), &(base.get()->*member))) {}

	constexpr GC::Handle<void> base() const noexcept { return this->_base; }

	InnerHandle<T>& base(GC::Handle<void>& base) noexcept {
		this->_base = base;
		return *this;
	}

	constexpr std::ptrdiff_t offset() const noexcept { return this->_offset; }

	constexpr operator T*() const noexcept { return ptradd<T>(base().get(), offset()); }
};

} // namespace Om
} // namespace OMR

#endif // OMR_GC_INNERHANDLE_HPP_
