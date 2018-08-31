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

#if !defined(OMR_GC_STACKROOT_HPP_)
#define OMR_GC_STACKROOT_HPP_

#include <OMR/GC/Handle.hpp>
#include <OMR/GC/StackRootList.hpp>
#include <OMR/GC/System.hpp>

#include <OMR/CanDereference.hpp>

namespace OMR
{
namespace GC
{

template<typename T>
class StackRoot;

template<typename T>
class StackRootBase
{
public:
	constexpr StackRootBase(void* ref, StackRootListNode* next) : _node(ref, next) {}

	T* get() const noexcept { return reinterpret_cast<T*>(_node.ref); }

	StackRootListNode& node() noexcept { return _node; }

	const StackRootListNode& node() const noexcept { return _node; }

	const StackRootListNode& cnode() const noexcept { return _node; }

protected:
	StackRootListNode _node;
};

/// This mixin adds Pointer-style operations to the StackRoot.
template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class StackRootPointerOperators : public StackRootBase<T>
{
public:
	using StackRootBase<T>::StackRootBase;

	T* operator->() const noexcept { return this->get(); }

	T& operator*() const noexcept { return *this->get(); }
};

/// This specialization removes pointer operations for void referents.
/// You cannot dereference a StackRoot<void>.
template<typename T>
class StackRootPointerOperators<T, false> : public StackRootBase<T>
{
public:
	using StackRootBase<T>::StackRootBase;
};

/// A rooted GC pointer. T must be a managed allocation. This class must be
/// stack allocated. StackRoots push themselves onto the linked list of stack
/// roots at construction. When A GC occurs, the sequence of StackRoots will be
/// walked, the referents marked, and the references fixed up. StackRoots have a
/// strict LIFO lifetime, when a StackRoot is destroyed, it must be the head of
/// the list.
template<typename T>
class StackRoot : public StackRootPointerOperators<T>
{
public:
	/// StackRoots must be stack allocated, so the new operator is explicitly removed.
	void* operator new(std::size_t) = delete;

	StackRoot(StackRootList& list, T* ref) noexcept
	        : StackRootPointerOperators<T>(ref, list.head()), _list(list)
	{
		_list.head(&this->node());
	}

	/// Attach a new StackRoot to the context's stackRoots.
	explicit StackRoot(Context& cx, T* ref = nullptr) noexcept : StackRoot(cx.stackRoots(), ref)
	{}

	~StackRoot() noexcept
	{
		assert(isHead());
		_list.head(this->node().next);
	}

	StackRoot& operator=(T* p) noexcept
	{
		this->node().ref = p;
		return *this;
	}

	/// Comparison of the referent.
	constexpr bool operator==(const StackRoot<T>& rhs) const noexcept
	{
		return this->node().ref == rhs.node().ref;
	}

	constexpr bool operator==(T* ptr) const noexcept { return this->node().ref == ptr; }

	constexpr bool operator==(std::nullptr_t) const noexcept
	{
		return this->node().ref == nullptr;
	}

	constexpr bool operator!=(const StackRoot<T>& rhs) const noexcept
	{
		return this->node().ref != rhs.node().ref;
	}

	constexpr bool operator!=(T* ptr) const noexcept { return this->node().ref != ptr; }

	constexpr bool operator!=(std::nullptr_t) const noexcept
	{
		return this->node().ref == nullptr;
	}

	/// The address of the reference slot in this StackRoot.
	T** address() noexcept { return reinterpret_cast<T**>(&this->node().ref); }

	T* const* address() const noexcept
	{
		return reinterpret_cast<T* const*>(&this->node().ref);
	}

	bool isHead() const noexcept { return _list.head() == &(this->node()); }

	StackRootList& list() const noexcept { return _list; }

	/// The StackRoot freely degrades into a Handle.
	operator Handle<T>() const { return Handle<T>(address()); }

	/// The StackRoot degrades into a raw pointer.
	operator T*() const { return this->get(); }

private:
	StackRootList& _list;
};

template<typename T>
constexpr bool
operator==(std::nullptr_t, const StackRoot<T>& root)
{
	return root == nullptr;
}

template<typename T>
constexpr bool
operator!=(std::nullptr_t, const StackRoot<T>& root)
{
	return root != nullptr;
}

} // namespace GC
} // namespace OMR

#endif // OMR_GC_STACKROOT_HPP_
