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

#include <OMR/Om/Allocation.hpp>
#include <OMR/Om/Allocator.hpp>
#include <OMR/Om/AnyScanner.hpp>
#include <OMR/Om/Array.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/Om/ObjectOperations.hpp>
#include <OMR/GC/StackRoot.hpp>
#include <OMR/Om/Runtime.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/ShapeOperations.hpp>
#include <OMR/Om/Span.hpp>

#include <gtest/gtest.h>
#include <omrgc.h>

namespace OMR {
namespace Om {
namespace Test {

Runtime runtime;

TEST(OmSystemTest, StartUpAMemorySystem) {
	System system(runtime);
	EXPECT_NE(system.globals().metaShape(), nullptr);
	EXPECT_NE(system.globals().arrayBufferShape(), nullptr);
}

TEST(OmSystemTest, StartUpAContext) {
	System system(runtime);
	RunContext cx(system);
	EXPECT_NE(cx.globals().metaShape(), nullptr);
	EXPECT_NE(cx.globals().arrayBufferShape(), nullptr);
}

TEST(OmSystemTest, loseAnObjects) {
	System system(runtime);
	RunContext cx(system);

	GC::StackRoot<Shape> shape(cx.gc(), allocateRootObjectLayout(cx, {}));
	Object* object = allocateObject(cx, shape);

	EXPECT_NE(object->layout(), nullptr);
	// EXPECT_EQ(object->layout(), shape);
	OMR_GC_SystemCollect(cx.gc().vm(), 0);
	// EXPECT_EQ(object->layout(), (Shape*)0x5e5e5e5e5e5e5e5eul);
}

TEST(OmSystemTest, keepAnObject) {
	System system(runtime);
	RunContext cx(system);

	GC::StackRoot<Shape> shape(cx.gc(), allocateRootObjectLayout(cx, {}));
	GC::StackRoot<Object> object(cx.gc(), allocateObject(cx, shape));
	EXPECT_EQ(object->layout(), shape.get());
	OMR_GC_SystemCollect(cx.gc().vm(), 0);
	EXPECT_EQ(object->layout(), shape.get());
}

class PrintVisitor {
public:
	template<typename SlotHandleT>
	bool edge(void* cell, SlotHandleT slot) const {
		std::cerr << "(ref :cell " << cell << " :target " << slot.readReference() << ")"
		          << std::endl;
		return false; // pause
	}
};

TEST(OmSystemTest, print) {
	System system(runtime);
	RunContext cx(system);

	GC::StackRoot<Shape> shape(cx.gc(), allocateRootObjectLayout(cx, {}, DEFAULT_INLINE_DATA_SIZE));

	ASSERT_NE(shape->shapeTreeData()->instanceInlineSlotsSize, 0);

	GC::StackRoot<Object> object(cx.gc(), allocateObject(cx, shape));

	Om::Id id(1);
	Om::SlotType type(Om::Id(0), Om::CoreType::VALUE);

	auto map = Om::transitionLayout(cx, object, {{type, id}});
	assert(map != nullptr);

	Om::SlotDescriptor descriptor;
	Om::lookupSlot(cx, object, id, descriptor);

	Om::setValue(cx, object.get(), descriptor, {AS_REF, object.get()});

	std::cerr << "object: " << object.get() << std::endl;
	std::cerr << "obj-map:" << object->layout() << std::endl;
	std::cerr << "map1:   " << shape.get() << std::endl;
	std::cerr << "map2:   " << map << std::endl;

	AnyScanner scanner;
	PrintVisitor printer;

	bool complete = scanner.start(printer, (Any*)object.get()).complete;
	EXPECT_TRUE(complete);

	// EXPECT_EQ(object->layout(), shape.get());
	// OMR_GC_SystemCollect(cx.vmContext(), 0);
	// EXPECT_EQ(object->layout(), shape.get());
}

TEST(OmSystemTest, objectTransition) {
	System system(runtime);
	RunContext cx(system);

	GC::StackRoot<Shape> emptyObjectMap(cx.gc(), allocateRootObjectLayout(cx, {}));
	GC::StackRoot<Object> obj1(cx.gc(), allocateObject(cx, emptyObjectMap));

	const std::array<const SlotAttr, 1> attributes{
	        {SlotAttr(SlotType(Id(0), CoreType::VALUE), Id(0))}};

	const Span<const Om::SlotAttr> span(attributes);
	transitionLayout(cx, obj1, span);

	// check
	{
		auto m = obj1->layout();
		EXPECT_EQ(m->layout(), cx.globals().metaShape());
		EXPECT_EQ(m->instanceSlotAttrs(), span);
		EXPECT_EQ(m->instanceSlotCount(), 1);
		EXPECT_EQ(m->parentLayout()->instanceSlotOffset(), 0);
	}

	// second object
	{
		GC::StackRoot<Object> obj2(cx.gc(), allocateObject(cx, emptyObjectMap));
		auto m = takeExistingTransition(cx, obj2, attributes);
		EXPECT_NE(m, nullptr);
		EXPECT_EQ(m, obj1->layout());
		EXPECT_EQ(m, obj2->layout());
	}
}

TEST(OmSystemTest, objectTransitionReuse) {
	System system(runtime);
	RunContext cx(system);
}

} // namespace Test
} // namespace Om
} // namespace OMR
