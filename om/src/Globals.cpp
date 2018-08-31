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

#include <OMR/Om/Globals.hpp>
#include <OMR/Om/ArrayOperations.hpp>
#include <OMR/Om/ShapeOperations.hpp>

namespace OMR {
namespace Om {

bool Globals::init(StartupContext& cx) {
	// The order of allocation matters. The MetaShape must be allocated before any
	// other shape.
	metaShape_ = allocateShapeLayout(cx);
	if (metaShape_ == nullptr) {
		return false;
	}

	// Tricky note: The object shape's transition table is backed by an array
	// buffer. If the ArrayBufferShape isn't brought up first, than the first
	// empty object shape allocated will not be able to allocate it's transition
	// table.
	arrayBufferShape_ = allocateArrayLayout(cx);
	if (arrayBufferShape_ == nullptr) {
		return false;
	}

	return true;
}

} // namespace Om
} // namespace OMR
