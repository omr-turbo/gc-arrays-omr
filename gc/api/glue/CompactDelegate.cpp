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
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/

#include <OMR/GC/StackRoot.hpp>
#include <OMR/GC/System.hpp>

#include "MarkingDelegate.hpp"
#include "CompactDelegate.hpp"

#include "CompactScheme.hpp"
#include "EnvironmentBase.hpp"
#include "omr.h"
#include "omrcfg.h"
#include "SublistSlotIterator.hpp"
#include "SublistPuddle.hpp"
#include "SublistPool.hpp"
#include "SublistIterator.hpp"
#include <iostream>

namespace OMR
{
namespace GC
{

void
CompactDelegate::fixupRoots(MM_EnvironmentBase *env, MM_CompactScheme *compactScheme) {
    auto &cx = getContext(env);

    CompactingVisitor visitor(_compactScheme);

    /// System-wide roots

    for (auto &fn : cx.system().compactingFns()) {
        fn(visitor);
    }


#if defined(OMR_GC_MODRON_SCAVENGER)
    fixupRememberedSet(env, compactScheme);
#endif // OMR_GC_MODRON_SCAVENGER

    /// Per-context roots

    // Note: only scanning the stack roots for *one* context.
    for (StackRootListNode &node : cx.stackRoots()) {
        visitor.edge(NULL, RefSlotHandle((omrobjectptr_t*)&node.ref));
    }

    for (auto &fn : cx.compactingFns()) {
        fn(visitor);
    }
}

#if defined(OMR_GC_MODRON_SCAVENGER)
/// Fixup the remembered set.
/// The remembered set contains objects
void
CompactDelegate::fixupRememberedSet(MM_EnvironmentBase* env, MM_CompactScheme* compactScheme) {
    CompactingVisitor visitor(compactScheme);

    GC_SublistIterator rememberedSetIterator(&env->getExtensions()->rememberedSet);
    MM_SublistPuddle *puddle = nullptr;

    while((puddle = rememberedSetIterator.nextList()) != NULL) {
        GC_SublistSlotIterator rememberedSetSlotIterator(puddle);
        omrobjectptr_t *slot;
        while((slot = (omrobjectptr_t *)rememberedSetSlotIterator.nextSlot()) != NULL) {
            if (*slot) {
                visitor.edge(NULL, RefSlotHandle(slot));
            }
        }
    }
}
#endif // defined(OMR_GC_MODRON_SCAVENGER)

} // namespace GC
} // namespace OMR
