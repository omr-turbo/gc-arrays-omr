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

#ifndef OMR_OM_SHAPESCANNER_HPP_
#define OMR_OM_SHAPESCANNER_HPP_

#include <OMR/Om/BaseScanner.hpp>
#include <OMR/Om/Shape.hpp>

#include <OMR/GC/ScanResult.hpp>

namespace OMR {
namespace Om {

class ShapeScanner : public BaseScanner<Shape> {
public:
	ShapeScanner() noexcept : BaseScanner() {}

	constexpr ShapeScanner(Shape* target) : BaseScanner(target) {}

	template<typename VisitorT>
	OMR::GC::ScanResult
	start(VisitorT& visitor, Shape* target, std::size_t bytesToScan = std::size_t(-1)) {
		target_ = target;

		visitHeader(target_->header(), visitor);
		if (target_->parentLayout_ != nullptr) {
			visitor.edge(this, BasicSlotHandle(&target_->parentLayout_));
		}
		target_->transitions_.visit(visitor);
		// visitor.edge(this, BasicSlotHandle(&shapeTreeData_));
		return {0, true}; // TODO: Report bytes scanned correctly.
	}

	/// Visit the references from the shape. For simplicity, scanning is non-interruptible for
	/// shapes.
	template<typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT& visitor, std::size_t bytesToScan = std::size_t(-1)) {
		assert(0); // unreachable
		return {0, true};
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SHAPESCANNER_HPP_
