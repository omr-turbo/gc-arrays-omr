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

#if !defined(OMR_OM_DISPATCH_HPP_)
#define OMR_OM_DISPATCH_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/Cell.hpp>
#include <OMR/Om/CellKind.hpp>
#include <OMR/Om/CellOperations.hpp>
#include <OMR/Om/Object.hpp>
#include <OMR/Om/Shape.hpp>

#include <cassert>

namespace OMR {
namespace Om {

/// Visit the slots in a cell, based on the layout.
template<typename FunctionT>
void dispatch(Cell* cell, FunctionT function) {
	switch (cellKind(cell)) {
	case CellKind::OBJECT: function(reinterpret_cast<Object*>(cell)); break;
	case CellKind::SHAPE: function(reinterpret_cast<Shape*>(cell)); break;
	case CellKind::ARRAY: function(reinterpret_cast<Array*>(cell)); break;
	default: assert(0);
	}
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_DISPATCH_HPP_
