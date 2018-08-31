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

#if !defined(OMR_OM_PRINTING_HPP_)
#define OMR_OM_PRINTING_HPP_

#include <OMR/Om/BasicSlotHandle.hpp>
#include <OMR/Om/Cell.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/SlotAttr.hpp>
#include <OMR/Om/SlotDescriptor.hpp>
#include <OMR/Om/Value.hpp>
#include <OMR/Om/ValueSlotHandle.hpp>

#include <ostream>

namespace OMR {
namespace Om {

inline std::ostream& operator<<(std::ostream& out, Id id) {
	return out << "(id " << id.raw() << ")";
}

inline std::ostream& operator<<(std::ostream& out, CoreType type) {
	switch (type) {
	case CoreType::FLOAT32: return out << "float32";
	case CoreType::FLOAT64: return out << "float64";
	case CoreType::INT8: return out << "int8";
	case CoreType::INT16: return out << "int16";
	// TODO: print all the core types.
	default: return out << "unknown";
	}
}

inline std::ostream& operator<<(std::ostream& out, Value value) {
	switch (value.kind()) {
	case Value::Kind::DOUBLE: return out << "(double " << value.getDouble() << ")";
	case Value::Kind::INT48: return out << "(int48 " << value.getInt48() << ")";
	case Value::Kind::UINT48: return out << "(uint48 " << value.getUint48() << ")";
	case Value::Kind::PTR: return out << "(ptr " << value.getPtr() << ")";
	case Value::Kind::REF: return out << "(ref " << value.getRef() << ")";
	default: return out << "(unknown " << value.raw() << ")";
	}
}

inline std::ostream& operator<<(std::ostream& out, const SlotType& type) {
	return out << "(slot-type " << type.coreType() << " " << type.id() << ")";
}

inline std::ostream& operator<<(std::ostream& out, const SlotAttr& attr) {
	return out << "(slot-attr " << attr.type() << attr.id() << ")";
}

inline std::ostream& operator<<(std::ostream& out, const SlotDescriptor& sd) {
	return out << "(slot-descriptor " << sd.offset() << " " << sd.attr() << ")";
}

inline std::ostream& operator<<(std::ostream& out, const Shape& shape) {
	out << "(shape ";
	for (const SlotAttr* iter = shape.instanceSlotAttrs().begin();
	     iter != shape.instanceSlotAttrs().end();
	     ++iter) {
		out << "\n  (slot: " << *iter << ")";
	}
	return out << ")";
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_PRINTINGVISITOR_HPP_
