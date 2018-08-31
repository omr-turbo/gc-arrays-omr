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

#include <OMR/Om/BitUtilities.hpp>
#include <OMR/Om/Double.hpp>

#include <gtest/gtest.h>

namespace OMR {
namespace Om {
namespace Test {
TEST(DoubleTest, signBit) {
	EXPECT_TRUE(std::signbit(Double::fromRaw(Double::SIGN_MASK)));
	EXPECT_TRUE(std::signbit(Double::fromRaw(Double::toRaw(-0.1) & Double::SIGN_MASK)));
	EXPECT_FALSE(std::signbit(Double::fromRaw(Double::toRaw(0.45) & Double::SIGN_MASK)));
}

TEST(DoubleTest, Raw) {
	double d = 0.1;
	EXPECT_TRUE(Double::fromRaw(Double::toRaw(d)) == d);
}

TEST(DoubleTest, NaN) {
	// not NaN
	double d = 0.1;
	EXPECT_FALSE(Double::isNaN(Double::toRaw(d)));
	EXPECT_FALSE(Double::isSNaN(Double::toRaw(d)));
	EXPECT_FALSE(Double::isQNaN(Double::toRaw(d)));

	// qNaN
	EXPECT_TRUE(std::numeric_limits<double>::has_quiet_NaN);
	double qnan = std::numeric_limits<double>::quiet_NaN();
	EXPECT_TRUE((Double::toRaw(qnan) & Double::NAN_QUIET_TAG) == Double::NAN_QUIET_TAG);
	EXPECT_TRUE(Double::isNaN(Double::toRaw(qnan)));
	EXPECT_FALSE(Double::isSNaN(Double::toRaw(qnan)));
	EXPECT_TRUE(Double::isQNaN(Double::toRaw(qnan)));

	// sNaN
	EXPECT_TRUE(std::numeric_limits<double>::has_signaling_NaN);
	double snan = std::numeric_limits<double>::signaling_NaN();
	EXPECT_TRUE((Double::toRaw(snan) & Double::NAN_QUIET_TAG) == 0);
	EXPECT_TRUE(Double::isNaN(Double::toRaw(snan)));
	EXPECT_TRUE(Double::isSNaN(Double::toRaw(snan)));
	EXPECT_FALSE(Double::isQNaN(Double::toRaw(snan)));

	// infinity
	double inf = INFINITY;
	EXPECT_FALSE(Double::isNaN(Double::toRaw(inf)));
	EXPECT_FALSE(Double::isSNaN(Double::toRaw(inf)));
	EXPECT_FALSE(Double::isQNaN(Double::toRaw(inf)));
}

} // namespace Test
} // namespace Om
} // namespace OMR
