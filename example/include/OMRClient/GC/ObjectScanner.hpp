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

#if !defined(OMRCLIENT_GC_OBJECTSCANNER_HPP_)
#define OMRCLIENT_GC_OBJECTSCANNER_HPP_

#include <Object.hpp>
#include <OMR/GC/RefSlotHandle.hpp>
#include <OMR/GC/ScanResult.hpp>
#include <stdint.h>
#include <cstddef>

class GC_ObjectModelDelegate;

namespace OMRClient
{
namespace GC
{

class ObjectScanner
{
public:
	ObjectScanner(const ObjectScanner&) = default;

	template <typename VisitorT>
	OMR::GC::ScanResult
	start(VisitorT visitor, Object *target, std::size_t bytesToScan = SIZE_MAX)
	{
		_target = target;
		_current = _target->begin();
		return resume(visitor, bytesToScan);
	}

	template <typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT visitor, std::size_t bytesToScan = SIZE_MAX)
	{
		OMR::GC::ScanResult result = {0, false};

		assert(_target != nullptr);
		assert(_current != nullptr);

		Slot *end = _target->end();

		while (true) {
			bool cont = true;
			if (*_current != 0) {
				cont = visitor.edge(_target, OMR::GC::RefSlotHandle(_current));
			}
			++_current;
			result.bytesScanned += sizeof(Slot);
			if (_current == end) {
				result.complete = true;
				break;
			} else if (!cont || result.bytesScanned >= bytesToScan) {
				result.complete = false;
				break;
			}
		}

		_target = nullptr;
		_current = nullptr;
		return result;
	}

	bool complete() const
	{
		bool result = true;
		if (_target != nullptr) {
			return _target->end() == _current;
		}
		return result;
	}

protected:
	friend class ::GC_ObjectModelDelegate;

	ObjectScanner() : _target(nullptr), _current(nullptr) {}

  private:
	Object *_target;
	Slot *_current;
};

} // namespace GC
} // namespace OMRClient

#endif // OMRCLIENT_GC_OBJECTSCANNER_HPP_
