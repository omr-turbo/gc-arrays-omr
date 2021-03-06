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

#ifndef OMR_OM_ARRAYSCANNER_HPP_
#define OMR_OM_ARRAYSCANNER_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/BaseScanner.hpp>

#include <OMR/GC/ScanResult.hpp>
#include <cassert>

namespace OMR {
namespace Om {

class ArrayScanner : public BaseScanner<Array> {
public:
	/// Construct without a target.
	ArrayScanner() = default;

	/// Construct with a target.
	ArrayScanner(Array* target) : BaseScanner(target) {}

	/// Returns true if there's more to scan.
	template<typename VisitorT>
	OMR::GC::ScanResult start(VisitorT& visitor, Array* target, std::size_t bytesToScan = std::size_t(-1)) {
		target_ = target;
		visitHeader(target_->header(), visitor);
		return {0, true};
	}

	/// Continue scanning the target.
	template<typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT& visitor, std::size_t bytesToScan = std::size_t(-1)) {
		assert(0);
		return {0, true};
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAYSCANNER_HPP_
