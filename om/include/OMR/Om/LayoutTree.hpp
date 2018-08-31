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

#if !defined(OMR_OM_LAYOUTTREE_HPP_)
#define OMR_OM_LAYOUTTREE_HPP_

#include <OMR/Om/Cell.hpp>
#include <OMR/GC/Handle.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/Value.hpp>

#include <type_traits>

namespace OMR {
namespace Om {

/// An adapter class that provides macro operations on the shape hierachy of an
/// object. Most notably, the LayoutTree is iterable.
class LayoutTree {
public:
	class Iterator {
	public:
		explicit constexpr Iterator() noexcept : current_(nullptr) {}

		explicit constexpr Iterator(Shape* start) noexcept : current_(start) {}

		Iterator(const Iterator&) = default;

		Iterator operator++(int) noexcept {
			Iterator copy(*this);
			current_ = current_->parentLayout();
			return copy;
		}

		Iterator& operator++() noexcept {
			current_ = current_->parentLayout();
			return *this;
		}

		constexpr Shape& operator*() const noexcept { return *current_; }

		constexpr bool operator==(const Iterator& rhs) const noexcept {
			return current_ == rhs.current_;
		}

		constexpr bool operator!=(const Iterator& rhs) const noexcept {
			return current_ != rhs.current_;
		}

	protected:
		friend class LayoutTree;

	private:
		Shape* current_;
	};

	constexpr LayoutTree(Shape* start) : start_(start) {}

	Iterator begin() const noexcept { return Iterator(start_); }

	Iterator end() const noexcept { return Iterator(nullptr); }

private:
	Shape* start_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_LAYOUTTREE_HPP_
