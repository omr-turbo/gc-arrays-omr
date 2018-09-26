/*******************************************************************************
 * Copyright (c) 2018, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "omrcfg.h"

#include <OMR/GC/System.hpp>
#include "EnvironmentStandard.hpp"
#include "GCExtensionsBase.hpp"
#include "Scavenger.hpp"
#include "omrgc.h"

namespace OMR {
namespace GC {

template <typename SlotHandleT, typename ValueT>
void
preWriteBarrier(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot, ValueT value) {
}

template <typename SlotHandleT, typename ValueT>
void
postWriteBarrier(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot, ValueT value) {
#if defined(OMR_GC_MODRON_SCAVENGER) || defined(OMR_GC_MODRON_CONCURRENT_MARK)
	MM_EnvironmentBase *env = cx.env();
	MM_GCExtensionsBase *extensions = env->getExtensions();

#if defined(OMR_GC_MODRON_SCAVENGER)
	if (extensions->scavengerEnabled) {
		if (extensions->isOld(object) && !extensions->isOld(value)) {
			if (extensions->objectModel.atomicSetRememberedState(object, STATE_REMEMBERED)) {
				/* The object has been successfully marked as REMEMBERED - allocate an entry in the remembered set */
				extensions->scavenger->addToRememberedSetFragment((MM_EnvironmentStandard *)env, object);
			}
		}
	}
#endif /* defined(OMR_GC_MODRON_SCAVENGER) */

#if defined(OMR_GC_MODRON_CONCURRENT_MARK)
	if (extensions->concurrentMark) {
		extensions->cardTable->dirtyCard(env, object);
	}
#endif /* defined(OMR_GC_MODRON_CONCURRENT_MARK) */
#endif /* defined(OMR_GC_MODRON_SCAVENGER) || defined(OMR_GC_MODRON_CONCURRENT_MARK) */
}

template <typename SlotHandleT, typename ValueT>
void
preReadBarrier(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot) {
}

template <typename SlotHandleT, typename ValueT>
void
postReadBarrier(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot) {
}

template <typename SlotHandleT, typename ValueT>
void
store(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot, ValueT *value) {
	preWriteBarrier(cx, object, slot, value);
	slot.writeReference(value);
	postWriteBarrier(cx, object, slot, value);
}

template <typename SlotHandleT, typename ValueT>
ValueT
load(OMR::GC::RunContext &cx, omrobjectptr_t object, SlotHandleT slot) {
	preReadBarrier(cx, object, slot);
	ValueT value = slot.readReference();
	postReadBarrier(cx, object, slot);
    return value;
}

} // namespace GC
} // namespace OMR
