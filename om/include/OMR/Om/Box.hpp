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

#ifndef OMR_OM_BOX_HPP_
#define OMR_OM_BOX_HPP_

#include <OMR/Om/Cell.hpp>

namespace OMR {
namespace Om {
/// A box around a native data type, T. BoxCells provide a way to allocate
/// native data types on the managed heap. The box wraps the object in minimal
/// GC facilities. Note that, as a DataCell, there is no way to safetly store
/// GC references inside a BoxCell.
template<typename T>
class Box : public Array {
public:
	template<typename... Args>
	BoxCell(std::in_place_t, Args...&& args) : DataCell(sizeof(T)) {
		new (data()) T(std::forward<Args>(args)...);
	}

	T& unbox() { return *(T*)data(); }

	const T& unbox() const { return *(T*)data(); }
};

template<typename T>
class Box {};

class BoxRef {};

class BoxRootRef {};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_BOX_HPP_
