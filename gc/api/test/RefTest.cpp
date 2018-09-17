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

#include <OMR/GC/Ref.hpp>

#include <gtest/gtest.h>

namespace OMR {
namespace GC {
namespace Test {

TEST(RefTest, nullVoidReferent) {
	Ref<void> ref(nullptr);
	EXPECT_EQ(ref, nullptr);
	EXPECT_EQ(ref, Ref<int>(nullptr));
	EXPECT_EQ(ref, ref);
}

TEST(RefTest, nullIntReferent) {
	Ref<int> ref(nullptr);
	EXPECT_EQ(ref, nullptr);
	EXPECT_EQ(ref, Ref<void>(nullptr));
	EXPECT_EQ(ref, ref);
}

TEST(RefTest, intReferent) {
	int a = 99;
	int b = 88;

	Ref<int> ra = &a;
	Ref<int> rb = &b;

	EXPECT_EQ(ra, &a);
	EXPECT_EQ(*ra, 99);
	EXPECT_NE(ra, nullptr);
	EXPECT_EQ(ra, ra);
	EXPECT_EQ(ra.get(), &a);
	EXPECT_EQ(ra, Ref<void>(&a));

	EXPECT_NE(ra, rb);
	EXPECT_EQ(*rb, b);
	EXPECT_NE(*ra, *rb);
}

class A {};

class B : public A {};

bool check(const Ref<A>& a) { return true; }

TEST(RefTest, referentInheritsFromType) {
	A a;
	B b;

	{
		Ref<A> ra = &a;
		Ref<B> rb = &b;
		EXPECT_NE(ra, rb);
		EXPECT_NE(rb, ra);
	}

	{
		Ref<A> ra = &a;
		Ref<A> rb = &b;
		EXPECT_NE(ra, rb);
		EXPECT_NE(rb, ra);
	}

	{
		Ref<A> rb1 = &b;
		Ref<B> rb2 = &b;
		EXPECT_EQ(rb1, rb2);
		EXPECT_EQ(rb2, rb1);
	}

	{
		Ref<B> r = &b;
		check(r);
	}
}

} // namespace Test
} // namespace Om
} // namespace OMR
