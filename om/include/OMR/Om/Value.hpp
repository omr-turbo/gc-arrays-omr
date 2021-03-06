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

#if !defined(OMR_OM_VALUE_HPP_)
#define OMR_OM_VALUE_HPP_

#include <OMR/Om/BitUtilities.hpp>
#include <OMR/Om/Double.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace OMR {
namespace Om {

class Cell;

/// Type Tags
/// These types are used to disambiguate Value's constructors.

struct AsRaw {};

struct AsDouble {};

struct AsInt48 {};

struct AsUint48 {};

struct AsPtr {};

struct AsRef {};

constexpr AsRaw AS_RAW;
constexpr AsDouble AS_DOUBLE;
constexpr AsInt48 AS_INT48;
constexpr AsUint48 AS_UINT48;
constexpr AsPtr AS_PTR;
constexpr AsRef AS_REF;

/// @file
/// Boxed Values
/// Values are encoded in floating point NaNs, using the set on NaNs not used
/// for normal FP math.  All NaN FP values must have an exponent of -1.  The set
/// of NaNs we use are known as signaling NaNs, which means the quiet bit must
/// always be set to 0.  We set the `kind` in the tag to distinguish different
/// kinds of Values, e.g. Pointers from Integers.  To make sure NULL Values are
/// distiniguished from inifinity floating point numbers, the kind must be
/// non-zero.
///
/// The breakdown is:
/// NaN box:   12 bits
///  sign:        01 bits
///  exponent:    11 bits
/// tag:       04 bits
///  quiet bit:   01 bits
///  value kind:  03 bits
/// Data:     48 bits

/// A RawValue is the underlying integer type that Values encode information
/// in. The RawValue has a ValueTag and 48 bits of storage encoded in it.
using RawValue = std::uint64_t;

static_assert(sizeof(RawValue) == sizeof(double),
              "A RawValue should be wide enough to store a double.");

union ValueData {

	ValueData() {}

	constexpr ValueData(RawValue value) : asRawValue(value) {}

	RawValue asRawValue;
	double asDouble;
	void* asPtr;
	void* asRef;
	std::uint64_t asUint48;
	std::int64_t asInt48;
};

static_assert(sizeof(ValueData) == sizeof(RawValue), "RawValue is used to store doubles.");

/// A single-slot value. Pointer width.
/// https://wingolog.org/archives/2011/05/18/value-representation-in-javascript-implementations
/// https://dxr.mozilla.org/mozilla-central/source/js/public/Value.h
/// NaN boxed value.
class Value {
public:
	enum class Kind { DOUBLE, INT48, UINT48, PTR, REF };

	/// In this encoding scheme, a boxed-value is any signaling NaN. The encoded
	/// value is stored in the NaN's spare bits, masked by NAN_VALUE_MASK. This
	/// means that it is impossible to store a legitimate sNaN into a Value. NaN's
	/// must be canonicalized before being stored.
	///
	/// The Tags in this namespace indicate that a RawValue is a boxed immediate.
	static constexpr RawValue NAN_TAG = Double::SPECIAL_TAG;
	static constexpr RawValue NAN_MASK = Double::SPECIAL_TAG | Double::NAN_QUIET_TAG;

	/// The complete tag of a NaN-boxed value.
	/// The Tag is the NaN box plus a type tag. The type tag is stored in the top
	/// 4 bits of the mantissa, leaving the low 48 bits for storing the actual
	/// payload. There are some design considerations:
	/// 1. The most significant bit in the mantissa must be left unset for boxed
	///    values. This is the NaN quiet bit, and we want our NaN's to be LOUD.
	/// 2. A NaN with a zero mantissa becomes infinity. To ensure this doesn't
	///    happen, the type tags must be nonzero.
	/// This leaves us with 7 possible type tags.
	struct Tag {
		static constexpr std::size_t SHIFT = 48;
		static constexpr RawValue MASK = NAN_MASK | (RawValue(0x7) << SHIFT);
		static constexpr RawValue INT48 = NAN_TAG | (RawValue(0x1) << SHIFT);
		static constexpr RawValue UINT48 = NAN_TAG | (RawValue(0x2) << SHIFT);
		static constexpr RawValue PTR = NAN_TAG | (RawValue(0x3) << SHIFT);
		static constexpr RawValue REF = NAN_TAG | (RawValue(0x4) << SHIFT);
		static constexpr RawValue UNUSED0 = NAN_TAG | (RawValue(0x5) << SHIFT);
		static constexpr RawValue UNUSED1 = NAN_TAG | (RawValue(0x6) << SHIFT);
		static constexpr RawValue UNUSED2 = NAN_TAG | (RawValue(0x7) << SHIFT);
	};

	static constexpr RawValue PAYLOAD_MASK = ~Tag::MASK;

	/// The canonical NaN is a positive non-signaling NaN. When a NaN double is
	/// stored into a Value, the NaN is canonicalized.  Doing so ensures that the
	/// NaN is made quiet and unsigned. This is to prevent us from reading true
	/// NaNs that look like NaN-boxed values.
	static constexpr RawValue CANONICAL_NAN = Double::SPECIAL_TAG | Double::NAN_QUIET_TAG
	                                          | Double::NAN_EXTRA_BITS_MASK;

	/// if value is a NaN, return the canonical NaN.
	static RawValue canonicalizeNaN(RawValue value) {
		if (Double::isNaN(value)) {
			return CANONICAL_NAN;
		}
		return value;
	}

	static double canonicalizeNaN(double value) {
		return Double::fromRaw(canonicalizeNaN(Double::toRaw(value)));
	}

	Value() = default;

	constexpr Value(const Value&) = default;

	constexpr Value(Value&&) = default;

	constexpr Value(AsRaw, RawValue value) : data_(value) {}

	Value(AsDouble, double value) { setDouble(value); }

	constexpr Value(AsInt48, std::int64_t value)
	        : data_(Tag::INT48 | (RawValue(value) & PAYLOAD_MASK)) {}

	constexpr Value(AsUint48, std::uint64_t value)
	        : data_(Tag::UINT48 | (RawValue(value) & PAYLOAD_MASK)) {}

	template<typename T>
	Value(AsPtr, T* value)
	        : data_{Tag::PTR | (reinterpret_cast<RawValue>(value) & PAYLOAD_MASK)} {
		assert((reinterpret_cast<RawValue>(value) & ~PAYLOAD_MASK) == 0);
	}

	template<typename T>
	Value(AsRef, T* value)
	        : data_{Tag::REF | (reinterpret_cast<RawValue>(value) & PAYLOAD_MASK)} {
		assert((reinterpret_cast<RawValue>(value) & ~PAYLOAD_MASK) == 0);
	}

	/// Get the underlying raw storage.
	constexpr RawValue raw() const noexcept { return data_.asRawValue; }

	/// Set the underlying raw storage.
	Value& raw(RawValue raw) noexcept {
		data_.asRawValue = raw;
		return *this;
	}

	bool constexpr isBoxedValue() const noexcept { return Double::isSNaN(raw()); }

	constexpr bool isDouble() const noexcept { return !isBoxedValue(); }

	Value& setDouble(double d) noexcept { return setDoubleUnsafe(canonicalizeNaN(d)); }

	/// Store a double into this Value. The double may not be a NaN.
	Value& setDoubleUnsafe(double d) noexcept {
		data_.asDouble = d;
		return *this;
	}

	double getDouble() const noexcept {
		assert(isDouble());
		return data_.asDouble;
	}

	bool isRef() const noexcept { return (raw() & Tag::MASK) == Tag::REF; }

	template<typename T = void>
	T* getRef() const noexcept {
		assert(isRef());
		return (T*)(raw() & PAYLOAD_MASK);
	}

	template<typename T>
	Value& setRef(T* ref) noexcept {
		assert(areNoBitsSet(RawValue(ref), ~PAYLOAD_MASK));
		return raw(Tag::REF | (RawValue(ref) & PAYLOAD_MASK));
	}

	bool isPtr() const noexcept { return (raw() & Tag::MASK) == Tag::PTR; }

	template<typename T = void>
	T* getPtr() const noexcept {
		assert(isPtr());
		return (T*)(raw() & PAYLOAD_MASK);
	}

	template<typename T>
	Value& setPtr(T* ptr) noexcept {
		assert(areNoBitsSet(RawValue(ptr), ~PAYLOAD_MASK));
		return raw(Tag::PTR | (RawValue(ptr) & PAYLOAD_MASK));
	}

	bool isUint48() const noexcept { return (raw() & Tag::MASK) == Tag::UINT48; }

	std::uint64_t getUint48() const noexcept {
		assert(isUint48());
		return std::uint64_t(raw() & PAYLOAD_MASK);
	}

	Value& setUint48(std::uint64_t value) noexcept {
		return raw(Tag::UINT48 | (RawValue(value) & PAYLOAD_MASK));
	}

	bool isInt48() const noexcept { return (raw() & Tag::MASK) == Tag::INT48; }

	std::int64_t getInt48() const noexcept {
		assert(isInt48());
		std::int64_t result = raw() & PAYLOAD_MASK;
		if ((result & (RawValue(1) << 47)) != 0) {
			result |= 0xFFFF000000000000ul;
		}
		return result;
	}

	Value& setInt48(std::int64_t value) noexcept {
		return raw(Tag::INT48 | (RawValue(value) & PAYLOAD_MASK));
	}

	/// Get the tag bits from the Value.
	constexpr std::uint64_t tag() const noexcept { return raw() & Tag::MASK; }

	inline Kind kind() const noexcept {
		if (isDouble()) {
			return Kind::DOUBLE;
		}

		switch (tag()) {
		case Tag::INT48: return Kind::INT48;
		case Tag::UINT48: return Kind::UINT48;
		case Tag::PTR: return Kind::PTR;
		case Tag::REF: return Kind::REF;
		default: assert(0);
		}
	}

	explicit operator RawValue() const noexcept { return data_.asRawValue; }

	Value& operator=(const Value& rhs) noexcept { return raw(rhs.raw()); }

	constexpr bool operator==(const Value& rhs) const noexcept { return raw() == rhs.raw(); }

	constexpr bool operator!=(const Value& rhs) const noexcept { return raw() != rhs.raw(); }

private:
	ValueData data_;
};

static_assert(sizeof(std::uintptr_t) == sizeof(Value),
              "A Value is a thin struct wrapper around it's base storage type.");

static_assert(std::is_standard_layout<Value>::value, "A value must be simple.");

} // namespace Om
} // namespace OMR

#endif // OMR_OM_VALUE_HPP_
