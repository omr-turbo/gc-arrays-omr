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

#ifndef OMR_INFRA_DOUBLES_HPP_
#define OMR_INFRA_DOUBLES_HPP_

/// @file
/// Tools for working with doubles. Especially, for working with the underlying
/// representation of doubles.

#include <OMR/Om/BitUtilities.hpp>

#include <cmath>
#include <cstdint>

namespace OMR {
namespace Om {
/// raw integer constants for working with doubles.
struct Double {
	union RawUnion {
		constexpr RawUnion(double d) : asDouble(d) {}

		constexpr RawUnion(std::uint64_t r) : asRaw(r) {}

		double asDouble;
		std::uint64_t asRaw;
	};

	static_assert(sizeof(std::uint64_t) == sizeof(double),
	              "Decoding and encoding expects 64 bit doubles.");

	/// Decomposition masks.
	/// @{
	static constexpr std::uint64_t SIGN_MASK = 0x8000000000000000ul;
	static constexpr std::uint64_t EXPONENT_MASK = 0x7FF0000000000000ul;
	static constexpr std::uint64_t MANTISSA_MASK = 0x000FFFFFFFFFFFFFul;
	/// @}

	/// NaN and +/- Inf
	/// @{
	/// A special value is any double where the exponent is maximized, aka all
	/// 1's. Note that a special value with a mantissa of zero designates +/-
	/// infinity. A Non-zero mantissa indicates NaN.
	static constexpr std::uint64_t SPECIAL_TAG = EXPONENT_MASK;

	/// There are two kinds of NaN's: signalling and quiet. Using a signalling NaN
	/// in any floating point arithmetic will result in a FPU interrupt. Quiet
	/// NaNs propagate through expressions without signaling. Quiet NaNs are
	/// designated by a positive most-significant-bit in the mantissa.
	static constexpr std::uint64_t NAN_QUIET_TAG = 0x0008000000000000ul;

	/// A mask for the remaining bits in a NaN after the NAN_TAG and the
	/// NAN_QUIET_TAG;
	static constexpr std::uint64_t NAN_EXTRA_BITS_MASK = 0x0007FFFFFFFFFFFFul;
	/// @}

	/// True if value is any NaN. False if Inf or a valid number. A double is a
	/// NaN if it's tagged special and has a non-zero mantissa.
	static constexpr bool isNaN(std::uint64_t value) {
		return areAllBitsSet(value, SPECIAL_TAG) && areAnyBitsSet(value, MANTISSA_MASK);
	}

	/// True for any quiet NaN.
	static constexpr bool isQNaN(std::uint64_t value) {
		return isNaN(value) && areAllBitsSet(value, NAN_QUIET_TAG);
	}

	/// True for any signaling NaN.
	static constexpr bool isSNaN(std::uint64_t value) {
		return isNaN(value) && areNoBitsSet(value, NAN_QUIET_TAG);
	}

	/// Reinterpret std::uint64_t to a double
	static constexpr double fromRaw(std::uint64_t value) { return RawUnion(value).asDouble; }

	/// Reinterpret a double as a std::uint64_t.
	static constexpr std::uint64_t toRaw(double value) { return RawUnion(value).asRaw; }
};

} // namespace Om
} // namespace OMR

#endif // OMR_INFRA_DOUBLEUTILITIES_HPP_
