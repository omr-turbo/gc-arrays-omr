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

#if !defined(OMR_OM_ARRAYBUFFERINITIALIZER_HPP_)
#define OMR_OM_ARRAYBUFFERINITIALIZER_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/Om/Initializer.hpp>
#include <OMR/Om/Shape.hpp>

namespace OMR {
namespace Om {

class ArrayInitializer : public Initializer {
public:
	explicit ArrayInitializer(std::size_t size) : size_(size) {}

	virtual Cell* operator()(Context& cx, Cell* cell) override {
		auto array = reinterpret_cast<Array*>(cell);
		Shape* layout = cx.globals().arrayBufferShape();
		new (array) Array(layout, size_);
		return reinterpret_cast<Cell*>(array);
	}

private:
	std::size_t size_; //< Size of buffer, not number of elements.
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAYBUFFERINITIALIZER_HPP_
