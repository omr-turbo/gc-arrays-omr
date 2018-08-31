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
#include <OMR/Om/Context.hpp>
#include <OMR/Om/Object.hpp>
#include <OMR/GC/StackRoot.hpp>
#include <OMR/Om/Runtime.hpp>

#include <gtest/gtest.h>
#include <omrgc.h>

namespace OMR {
namespace Om {
namespace Test {
// clang-format off
std::vector<std::int64_t> signedIntegers = {
  0l, 1l, -1l, 42l, -42l, 1l << 46,
  std::numeric_limits<std::int32_t>::max(),
  std::numeric_limits<std::int32_t>::min(),
};
// clang-format on

TEST(ValueTest, signedIntegerConstructorRoundTrip) {
	for (auto i : signedIntegers) {
		Value value(AS_INT48, i);
		EXPECT_TRUE(value.isBoxedValue());
		EXPECT_TRUE(value.isInt48());
		EXPECT_FALSE(value.isUint48());
		EXPECT_FALSE(value.isDouble());
		EXPECT_EQ(value.kind(), Value::Kind::INT48);
		EXPECT_EQ(i, value.getInt48());
	}
}

TEST(ValueTest, setSignedIntegerRoundTrip) {
	for (auto i : signedIntegers) {
		Value value;
		value.setInt48(i);
		EXPECT_TRUE(value.isBoxedValue());
		EXPECT_TRUE(value.isInt48());
		EXPECT_FALSE(value.isUint48());
		EXPECT_FALSE(value.isDouble());
		EXPECT_EQ(value.kind(), Value::Kind::INT48);
		EXPECT_EQ(i, value.getInt48());
	}
}

// clang-format off
std::vector<std::uint64_t> unsignedIntegers = {
  0ul, 1ul, 42l, 42l, 0x0000FFFFFFFFFFFFul, 1l << 47,
  std::numeric_limits<std::uint32_t>::max(),
  std::numeric_limits<std::uint32_t>::min(),
};
// clang-format on

TEST(ValueTest, unsignedIntegerConstructorRoundTrip) {
	for (auto i : unsignedIntegers) {
		Value value(AS_UINT48, i);
		EXPECT_TRUE(value.isBoxedValue());
		EXPECT_TRUE(value.isUint48());
		EXPECT_FALSE(value.isInt48());
		EXPECT_FALSE(value.isDouble());
		EXPECT_EQ(value.kind(), Value::Kind::UINT48);
		EXPECT_EQ(i, value.getUint48());
	}
}

TEST(ValueTest, setUnsignedIntegerRoundTrip) {
	for (auto i : unsignedIntegers) {
		Value value;
		value.setUint48(i);
		EXPECT_TRUE(value.isBoxedValue());
		EXPECT_TRUE(value.isUint48());
		EXPECT_FALSE(value.isInt48());
		EXPECT_FALSE(value.isDouble());
		EXPECT_EQ(value.kind(), Value::Kind::UINT48);
		EXPECT_EQ(i, value.getUint48());
	}
}

TEST(ValueTest, canonicalNan) {
	EXPECT_TRUE(std::isnan(Double::fromRaw(Value::CANONICAL_NAN)));
	EXPECT_FALSE(std::signbit(Double::fromRaw(Value::CANONICAL_NAN)));
	EXPECT_TRUE(Double::isNaN(Value::CANONICAL_NAN));
	EXPECT_TRUE(Double::isQNaN(Value::CANONICAL_NAN));
	EXPECT_FALSE(Double::isSNaN(Value::CANONICAL_NAN));
	EXPECT_NE(Double::fromRaw(Value::CANONICAL_NAN), Double::fromRaw(Value::CANONICAL_NAN));
}

TEST(ValueTest, doubleRoundTrip) {
	const std::vector<double> doubles = {0.0,
	                                     1.0,
	                                     43.21,
	                                     std::numeric_limits<double>::infinity(),
	                                     std::numeric_limits<double>::max(),
	                                     std::numeric_limits<double>::min()};

	for (auto d : doubles) {
		for (auto sign : {+1.0, -1.0}) {
			auto x = d * sign;
			Value value(AS_DOUBLE, x);
			EXPECT_EQ(x, value.getDouble());
			EXPECT_FALSE(value.isBoxedValue());
			EXPECT_TRUE(value.isDouble());
			EXPECT_FALSE(value.isInt48());
		}
	}
}

TEST(ValueTest, signalingNanDouble) {
	Value value(AS_DOUBLE, std::numeric_limits<double>::signaling_NaN());
	EXPECT_TRUE(std::isnan(value.getDouble()));
	EXPECT_FALSE(value.isBoxedValue());
	EXPECT_NE(value.getDouble(), Double::fromRaw(Value::CANONICAL_NAN));
	EXPECT_EQ(value.raw(), Value::CANONICAL_NAN);
}

TEST(ValueTest, quietNanDouble) {
	Value value(AS_DOUBLE, std::numeric_limits<double>::quiet_NaN());
	EXPECT_TRUE(std::isnan(value.getDouble()));
	EXPECT_FALSE(value.isBoxedValue());
	EXPECT_NE(value.getDouble(), Double::fromRaw(Value::CANONICAL_NAN));
	EXPECT_EQ(value.raw(), Value::CANONICAL_NAN);
}

TEST(ValueTest, pointerRoundTrip) {
	for (void* p : {(void*)0, (void*)1, (void*)(-1 & Value::PAYLOAD_MASK)}) {
		Value value(AS_PTR, p);
		EXPECT_EQ(p, value.getPtr());
		EXPECT_TRUE(value.isBoxedValue());
		EXPECT_TRUE(value.isPtr());
	}
}

} // namespace Test
} // namespace Om
} // namespace OMR
