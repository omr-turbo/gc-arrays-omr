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

#ifndef OMR_OM_SHAPE_HPP_
#define OMR_OM_SHAPE_HPP_

#include <OMR/Om/BasicSlotHandle.hpp>
#include <OMR/Om/CellHeader.hpp>
#include <OMR/Om/CellKind.hpp>
#include <OMR/Om/Initializer.hpp>
#include <OMR/Om/SlotAttr.hpp>
#include <OMR/Om/SlotDescriptorRange.hpp>
#include <OMR/Om/Span.hpp>
#include <OMR/Om/TransitionSet.hpp>
#include <OMR/Om/ValueSlotHandle.hpp>

namespace OMR {
namespace Om {

/// Common properties shared by all Shapes in a Shape tree. Particularly, invariant facts about the
/// instance are stored here.
struct ShapeTreeData {
	/// Total amount of space, in bytes, available for inline aka fixed slots.
	std::size_t instanceInlineSlotsSize;
	/// The CellKind this ShapeTree describes. Are the instances Shapes, Objects, or Arrays?
	CellKind instanceKind;
};

/// A description of a cell's layout. The Shape is like a Cell's class. Shapes may be
/// shared by Cells. The instanceKind can be examined to tell what kind of thing the
/// cell is.
///
/// Since Shapes are Cell's, they too have a Shape describing their layout.
class Shape {
public:
	/// Calculate the allocation size
	static constexpr std::size_t calculateCellSize(std::size_t slotCount) {
		return sizeof(Shape) + (sizeof(SlotAttr) * slotCount);
	}

	/// Convert this object to a cell.
	CellHeader& header() { return header_; }

	const CellHeader& header() const { return header_; }

	/// Return the shape that describes _this_ Shape.
	Shape* layout() const { return header().layout(); }

	/// Set the layout of _this_ Shape.
	Shape& layout(Shape* shape) {
		header().layout(shape);
		return *this;
	}

	/// @group Instance information
	/// @{

	CellKind instanceKind() const { return shapeTreeData_.instanceKind; }

	void instanceKind(CellKind k) { shapeTreeData_.instanceKind = k; }

	std::size_t instanceInlineSlotSize() const {
		return shapeTreeData_.instanceInlineSlotsSize;
	}

	/// Get the Span of `SlotAttr` described by this shape.
	Span<SlotAttr> instanceSlotAttrs() noexcept {
		return Span<SlotAttr>(instanceSlotAttrs_, instanceSlotCount_);
	}

	Span<const SlotAttr> instanceSlotAttrs() const noexcept {
		return Span<const SlotAttr>(instanceSlotAttrs_, instanceSlotCount_);
	}

#if 0
	/// TODO: Implement assignSlotAttrs with iterators
	template <typename IteratorT>
	void assignSlotAttrs(IteratorT begin, IteratorT end) {
		for (; begin != end; ++begin) {

		}
	}
#endif

	/// Get the SlotDescriptors described by this shape.
	SlotDescriptorRange instanceSlotDescriptorRange() const noexcept {
		return SlotDescriptorRange(
		        instanceSlotAttrs(), instanceSlotOffset(), instanceSlotWidth());
	}

	SlotDescriptorRange instanceSlots() const noexcept { return instanceSlotDescriptorRange(); }

	/// Number of slots described by this shape.
	std::size_t instanceSlotCount() const noexcept { return instanceSlotCount_; }

	/// The offset into the object, where the shape's slots actually begin.
	/// This is the total size of the slots represented by the parent hierachy of
	/// this shape.
	std::size_t instanceSlotOffset() const noexcept { return instanceSlotOffset_; }

	/// Total footprint of slots in an object.
	std::size_t instanceSlotWidth() const noexcept { return instanceSlotWidth_; }

	/// ObjectMaps form a heirachy of "inherited layouts."
	/// The parent object shape describes the slots immediately preceding
	/// If this `Shape` has no parent, this function returns `nullptr`.
	/// Having no parent implies the `slotOffset` is zero.
	Shape* parentLayout() const noexcept { return parentLayout_; }

	/// Common properties shared by all shapes in the shape-tree.
	ShapeTreeData* shapeTreeData() noexcept { return &shapeTreeData_; }

	const ShapeTreeData* constShapeTreeData() const noexcept { return &shapeTreeData_; }

	/// @}

	/// @group GC support
	/// @{

	/// Size of this GC cell.
	std::size_t cellSize() const { return calculateCellSize(instanceSlotCount()); }

	/// @}

	CellHeader header_;
	Shape* parentLayout_;
	TransitionSet transitions_;
	std::size_t instanceSlotOffset_;
	std::size_t instanceSlotWidth_;
	std::size_t instanceSlotCount_;
	ShapeTreeData shapeTreeData_; // TODO: Move to seperate allocation for sharing.
	SlotAttr instanceSlotAttrs_[0];
};

static_assert(std::is_standard_layout<Shape>::value, "Shape must be a StandardLayoutType.");

/// An offset table for compilers.
struct ShapeOffsets {};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SHAPE_HPP_
