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

#ifndef OMR_OM_ANYSCANNER_HPP_
#define OMR_OM_ANYSCANNER_HPP_

#include <OMR/Om/Any.hpp>
#include <OMR/Om/ArrayScanner.hpp>
#include <OMR/Om/ObjectScanner.hpp>
#include <OMR/Om/ShapeScanner.hpp>

#include <OMR/GC/ScanResult.hpp>

namespace OMR {
namespace Om {

class AnyScanner {
public:
	AnyScanner() = default;

	/// Start scanning a cell. Returns true if there are more slots to scan.
	template<typename VisitorT>
	OMR::GC::ScanResult
	start(VisitorT&& visitor, Any* tgt, std::size_t bytesToScan = std::size_t(-1)) {
		switch (tgt->kind()) {
		case CellKind::OBJECT:
			new (&subscanner_.as.object) ObjectScanner();
			return subscanner_.as.object.start(visitor, &tgt->as.object, bytesToScan);
		case CellKind::ARRAY:
			new (&subscanner_.as.array) ArrayScanner();
			return subscanner_.as.array.start(visitor, &tgt->as.array, bytesToScan);
		case CellKind::SHAPE:
			new (&subscanner_.as.shape) ShapeScanner();
			return subscanner_.as.shape.start(visitor, &tgt->as.shape, bytesToScan);
		default: assert(0); // unreachable.
		}
	}

	/// Continue scanning an existing cell.
	template<typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT&& visitor, std::size_t bytesToScan = std::size_t(-1)) {
		assert(target() != nullptr);
		switch (target()->kind()) {
		case CellKind::OBJECT: return subscanner_.as.object.resume(visitor, bytesToScan);
		case CellKind::ARRAY: return subscanner_.as.array.resume(visitor, bytesToScan);
		case CellKind::SHAPE: return subscanner_.as.shape.resume(visitor, bytesToScan);
		}
	}

	constexpr Any* target() const { return subscanner_.as.base.target(); }

private:
	union SubscannerUnion {
		SubscannerUnion() : base() {}

		SubscannerUnion(Any* target) : base(target) {}

		BaseScanner<Any> base;
		ObjectScanner object;
		ArrayScanner array;
		ShapeScanner shape;
	};

	struct Subscanner {
		SubscannerUnion as;
	};

	Subscanner subscanner_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ANYSCANNER_HPP_
