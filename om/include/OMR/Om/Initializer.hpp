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

#if !defined(OMR_OM_INITIALIZER_HPP_)
#define OMR_OM_INITIALIZER_HPP_

namespace OMR {
namespace Om {
class Context;
class Cell;

/// Preliminary initialization functor. As an object is allocated, the mutator
/// may perform some concurrent GC work, "paying the GC tax". To do this work,
/// every object must be in a walkable, GC-able state. That includes our freshly
/// allocated object. An Initializer is used to perform this basic
/// initialization. This initialization happens immediately after an object's
/// memory is allocated, but before the allocation is "completed".
///
/// Note that Objects have a complex, multi-allocation layout. There are higher
/// level APIs for constructing them. This class is for low-level, per
/// allocation initialization. Initializers may not allocate.
class Initializer {
public:
	virtual Cell* operator()(Context& cx, Cell* cell) = 0;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_INITIALIZER_HPP_
