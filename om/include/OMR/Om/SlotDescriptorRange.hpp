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

#if !defined(OMR_OM_SLOTDESCRIPTORRANGE_HPP_)
#define OMR_OM_SLOTDESCRIPTORRANGE_HPP_

#include <OMR/Om/SlotDescriptor.hpp>
#include <OMR/Om/Span.hpp>

namespace OMR {
namespace Om {

/// An iterable view of the slot described by an Shape. Slots attributes are immutable.
/// The Slot Attributes are unowned, heap-allocated data. Do not hold a range or iterator across GC
/// safepoints.
///
/// TODO: Provide a mutable view of SlotDescriptors inside Shape objects.
class SlotDescriptorRange {
public:
	/// An iterator that also tracks slot offsets using the width of the
	/// slot's type. Results in `SlotDescriptors`, fancy handles that describe a
	/// slot's type and offset.
	class Iterator {
	public:
		Iterator() = default;

		Iterator(const Iterator&) = default;

		Iterator(const SlotAttr* current, std::size_t offset = 0)
		        : current_(current), offset_(offset) {}

		Iterator operator++(int) {
			Iterator copy(*this);
			offset_ += current_->type().width();
			++current_;
			return copy;
		}

		Iterator& operator++() {
			offset_ += current_->type().width();
			current_++;
			return *this;
		}

		bool operator==(const Iterator& rhs) const { return current_ == rhs.current_; }

		bool operator!=(const Iterator& rhs) const { return current_ != rhs.current_; }

		SlotDescriptor operator*() const {
			return SlotDescriptor{SlotIndex{offset_}, current_};
		}

	private:
		const SlotAttr* current_;
		std::size_t offset_;
	};

	SlotDescriptorRange(const SlotDescriptorRange&) = default;

	Iterator begin() const { return Iterator(attributes_.begin(), offset_); }

	Iterator end() const { return Iterator(attributes_.end(), offset_ + width_); }

	Span<const SlotAttr> attributes() const { return attributes_; }

protected:
	friend class Shape;

	constexpr SlotDescriptorRange(Span<const SlotAttr> attributes,
	                              std::size_t offset,
	                              std::size_t width)
	        : attributes_(attributes), offset_(offset), width_(width) {}

private:
	const Span<const SlotAttr> attributes_;
	std::size_t offset_;
	std::size_t width_;
};

using SlotDescriptorRangeIterator = SlotDescriptorRange::Iterator;

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SLOTDESCRIPTORRANGE_HPP_
