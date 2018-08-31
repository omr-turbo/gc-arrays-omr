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

#ifndef OMR_OM_CELL_HPP_
#define OMR_OM_CELL_HPP_

#include <OMR/Om/CellHeader.hpp>

#include <cstddef>
#include <cstdint>

namespace OMR {
namespace Om {

/// A managed blob of memory. All Cells have a one slot header.
class Cell {
public:
	/// Cells are not visitable. Cast the Cell to it's specific type before visiting.
	template<typename VisitorT>
	void visit(VisitorT& visitor) = delete;

	CellHeader& header() noexcept { return header_; }

	const CellHeader& header() const noexcept { return header_; }

	Shape* layout() const { return header().layout(); }

protected:
	friend struct CellOffsets;

	CellHeader header_;
};

static_assert(std::is_standard_layout<Cell>::value, "Cell must be a StandardLayoutType");

struct CellOffsets {
	static constexpr std::size_t HEADER = offsetof(Cell, header_);
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CELL_HPP_
