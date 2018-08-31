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

#ifndef OMR_OM_OBJECTSCANNER_HPP_
#define OMR_OM_OBJECTSCANNER_HPP_

#include <OMR/Om/BaseScanner.hpp>
#include <OMR/Om/Object.hpp>

#include <OMR/GC/ScanResult.hpp>
#include <iostream>

namespace OMR {
namespace Om {

class ObjectScanner : public BaseScanner<Object> {
public:
	ObjectScanner() noexcept : BaseScanner() {}

	/// Scan the target. Target returns true if complete.
	template<typename VisitorT>
	OMR::GC::ScanResult
	start(VisitorT& visitor, Object* target, std::size_t bytesToScan = std::size_t(-1)) {
		target_ = target;
		layout_ = target_->layout();
		iter_ = layout_->instanceSlots().begin();

		bool cont = visitHeader(target_->header(), visitor);
		if (!cont) {
			return {0, true};
		}

		return resume(visitor);
	}

	/// Continue scanning. Returns true if there is more to do, false if more to do.
	template<typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT& visitor, std::size_t bytesToScan = std::size_t(-1)) {
	
		if (layout_ == nullptr) {
			return {0, true}; // already complete.
		}

		std::size_t bytesScanned = 0;

		// continue scanning the current shape.
		// finish off the current iteration.
		while (iter_ != layout_->instanceSlots().end()) {
			SlotDescriptor slotd = *iter_;
			bool cont = target_->visitSlot(slotd, visitor);
			bytesScanned += slotd.attr().width();
			++iter_;
			if (!cont || bytesScanned >= bytesToScan) {
				return {bytesScanned, false}; // more slots
			}
		}

		// scan the rest of the slots.

		layout_ = layout_->parentLayout();

		while (layout_ != nullptr) {
			iter_ = layout_->instanceSlots().begin();
			while (iter_ != layout_->instanceSlots().end()) {
				SlotDescriptor slotd = *iter_;
				bool cont = target_->visitSlot(slotd, visitor);
				bytesScanned += slotd.attr().width();
				++iter_;
				if (!cont || bytesScanned >= bytesToScan) {
					return {bytesScanned, false};
				}
			}
			layout_ = layout_->parentLayout();
		}

		// complete--return "no more slots"
		return {bytesScanned, true};
	}

private:
	Shape* layout_;
	SlotDescriptorRangeIterator iter_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_OBJECTSCANNER_HPP_
