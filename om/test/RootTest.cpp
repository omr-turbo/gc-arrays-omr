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

#include <OMR/Om/Allocator.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/Om/InnerHandle.hpp>
#include <OMR/Om/Object.hpp>
#include <OMR/Om/ObjectOperations.hpp>
#include <OMR/Om/Runtime.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/StackRoot.hpp>
#include <OMR/Om/TransitionSet.hpp>

#include <OMR/GC/Handle.hpp>
#include <gtest/gtest.h>

namespace OMR {
namespace Om {
namespace Test {

Runtime runtime;

TEST(RootTest, basic) {
	System system(runtime);
	RunContext cx(system);
	GC::StackRoot<Object> root(cx.gc(), allocateEmptyObject(cx));
	GC::Handle<Object> handle(root);
	// InnerHandle<Object::Base> member(handle, &Object::base);
}

} // namespace Test
} // namespace Om
} // namespace OMR
