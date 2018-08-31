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

#if !defined(OMR_OM_CORETYPE_HPP_)
#define OMR_OM_CORETYPE_HPP_

#include <OMR/Om/Value.hpp>

#include <cstdint>
#include <cstdlib>

namespace OMR {
namespace Om {

/// Fundamental, built in types.
/// At it's core, every SlotType is represented by one of these values.
/// Om only really cares about 3 categories of slots values
///   1. Naked GC pointer (REF)
///   2. Boxed GC pointer (VALUE)
///   3. Non-pointer
/// Beyond those 3 categories, we just need to know the width, so we can iterate
/// the object.
enum class CoreType {
	/// Binary data slots, of various sizes.
	INT8,
	INT16,
	INT32,
	INT64,
	FLOAT32,
	FLOAT64,
	VALUE, //< A polymorphic `Value`.
	REF,   //< A GC pointer
	PTR
};

/// Calculate the size of a CoreType. Fundamental types have a fixed size.
inline std::size_t coreTypeWidth(CoreType t) noexcept {
	switch (t) {
	case CoreType::INT8: return 1;
	case CoreType::INT16: return 2;
	case CoreType::INT32: return 4;
	case CoreType::INT64: return 8;
	case CoreType::FLOAT32: return 4;
	case CoreType::FLOAT64: return 8;
	case CoreType::VALUE: return sizeof(Value);
	case CoreType::REF: return sizeof(void*);
	default: break;
	}
	return -1;
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CORETYPE_HPP_
