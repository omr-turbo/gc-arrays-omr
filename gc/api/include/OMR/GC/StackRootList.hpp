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

#if !defined(OMR_GC_STACKROOTLIST_HPP_)
#define OMR_GC_STACKROOTLIST_HPP_

namespace OMR
{
namespace GC
{

/// A node in a StackRootList.
struct StackRootListNode
{
	constexpr StackRootListNode(void* ref, StackRootListNode* next) : ref(ref), next(next) {}

	void* ref;
	StackRootListNode* next;
};

/// A foward iterator through a StackRootList.
/// @see-also: StackRootList::begin()
class StackRootListIterator
{
public:
	constexpr StackRootListIterator(const StackRootListIterator& other) noexcept
	        : _current(other._current)
	{}

	constexpr StackRootListNode& operator*() const noexcept { return *_current; }

	constexpr StackRootListNode* operator->() const noexcept { return _current; }

	StackRootListIterator& operator++() noexcept
	{
		_current = _current->next;
		return *this;
	}

	StackRootListIterator operator++(int) noexcept
	{
		StackRootListIterator tmp(*this);
		_current = _current->next;
		return tmp;
	}

	constexpr bool operator==(const StackRootListIterator& rhs) const noexcept
	{
		return _current == rhs._current;
	}

	constexpr bool operator!=(const StackRootListIterator& rhs) const noexcept
	{
		return _current != rhs._current;
	}

	StackRootListIterator& operator=(const StackRootListIterator& rhs) noexcept
	{
		_current = rhs._current;
		return *this;
	}

protected:
	friend class StackRootList;
	friend class ConstStackRootListIterator;

	constexpr StackRootListIterator() noexcept : _current(nullptr) {}

	constexpr StackRootListIterator(StackRootListNode* start) noexcept : _current(start)
	{}

private:
	StackRootListNode* _current;
};

/// A forward iterator through a const StackRootList.
/// @see-also: StackRootList::cbegin()
class ConstStackRootListIterator
{
public:
	constexpr ConstStackRootListIterator(const ConstStackRootListIterator& other) noexcept
	        : _current(other._current)
	{}

	constexpr ConstStackRootListIterator(const StackRootListIterator& other) noexcept
	        : _current(other._current)
	{}

	constexpr const StackRootListNode& operator*() const noexcept { return *_current; }

	constexpr const StackRootListNode* operator->() const noexcept { return _current; }

	ConstStackRootListIterator& operator++() noexcept
	{
		_current = _current->next;
		return *this;
	}

	ConstStackRootListIterator operator++(int) noexcept
	{
		ConstStackRootListIterator tmp(*this);
		_current = _current->next;
		return tmp;
	}

	constexpr bool operator==(const ConstStackRootListIterator& rhs) const noexcept
	{
		return _current == rhs._current;
	}

	constexpr bool operator!=(const ConstStackRootListIterator& rhs) const noexcept
	{
		return _current != rhs._current;
	}

	constexpr bool operator==(const StackRootListIterator& rhs) const noexcept
	{
		return _current == rhs._current;
	}

	constexpr bool operator!=(const StackRootListIterator& rhs) const noexcept
	{
		return _current != rhs._current;
	}

	ConstStackRootListIterator& operator=(const ConstStackRootListIterator& rhs) noexcept
	{
		_current = rhs._current;
		return *this;
	}

	ConstStackRootListIterator& operator=(const StackRootListIterator& rhs) noexcept
	{
		_current = rhs._current;
		return *this;
	}

protected:
	friend class StackRootList;

	constexpr ConstStackRootListIterator() noexcept : _current(nullptr) {}

	constexpr ConstStackRootListIterator(const StackRootListNode* start) noexcept
	        : _current(start)
	{}

private:
	const StackRootListNode* _current;
};

/// The StackRootList is an untyped linked list of rooted, untagged pointers.
/// Nodes are manually added and removed from the list.
/// The StackRootList explicitly tracks untyped node elements.
/// Users should not use StackRootList directly, instead root objects using the automatic RootRef
/// type, or register a RootFn callback.
class StackRootList
{
public:
	constexpr StackRootList() noexcept : _head(nullptr) {}

	StackRootListIterator begin() noexcept { return StackRootListIterator(head()); }

	StackRootListIterator end() noexcept { return StackRootListIterator(nullptr); }

	ConstStackRootListIterator begin() const noexcept { return cbegin(); }

	ConstStackRootListIterator end() const noexcept { return cend(); }

	ConstStackRootListIterator cbegin() const noexcept
	{
		return ConstStackRootListIterator(head());
	}

	ConstStackRootListIterator cend() const noexcept
	{
		return ConstStackRootListIterator(nullptr);
	}

	StackRootListNode* head() const noexcept { return _head; }

	StackRootList& head(StackRootListNode* head) noexcept
	{
		_head = head;
		return *this;
	}

	StackRootList& push(StackRootListNode* node) noexcept
	{
		node->next = _head;
		_head = node;
		return *this;
	}

private:
	StackRootListNode* _head;
};

} // namespace GC
} // namespace OMR

#endif // OMR_GC_STACKROOTLIST_HPP_
