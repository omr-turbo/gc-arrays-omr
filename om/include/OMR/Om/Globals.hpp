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

#if !defined(OMR_OM_GLOBALS_HPP_)
#define OMR_OM_GLOBALS_HPP_

#include <OMR/Om/Shape.hpp>

#include <OMR/GC/MarkingFn.hpp>

#include <MarkingScheme.hpp>

#include <iostream>

namespace OMR {
namespace Om {

class StartupContext;
class Cell;

/// A collection of singleton GC cells. The collection is initialized at startup.
class Globals {
public:
	Shape* metaShape() const noexcept { return metaShape_; }

	Shape* arrayBufferShape() const noexcept { return arrayBufferShape_; }

	void scan(GC::MarkingVisitor& visitor) {
		if (metaShape_ != nullptr)
			visitor.edge(this, BasicSlotHandle(&metaShape_));

		if (arrayBufferShape_ != nullptr) {
			visitor.edge(this, BasicSlotHandle(&arrayBufferShape_));
		}
	}

protected:
	friend class System;

	/// Allocate the globals. Throws StartupError if any allocation fails.
	bool init(StartupContext& cx);

private:
	Shape* metaShape_ = nullptr;
	Shape* arrayBufferShape_ = nullptr;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_GLOBALS_HPP_
