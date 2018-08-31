/*******************************************************************************
 * Copyright (c) 2017, 2018 IBM Corp. and others
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
#include "MarkingDelegate.hpp"

#include <OMR/GC/StackRoot.hpp>
#include <OMR/GC/System.hpp>

#include "EnvironmentBase.hpp"
#include "MarkingScheme.hpp"
#include "omr.h"
#include "omrcfg.h"
#include "omrhashtable.h"
#include <iostream>

namespace OMR
{
namespace GC
{

void
MarkingDelegate::scanRoots(MM_EnvironmentBase *env)
{
	auto &cx = getContext(env);
	MM_MarkingScheme::MarkingVisitor marker(env, _markingScheme);

	/// System-wide roots

	for (auto &fn : cx.system().markingFns()) {
		fn(marker);
	}

	/// Per-context roots

	// Note: only scanning the stack roots for *one* context.
	for (const StackRootListNode &node : cx.stackRoots()) {
		_markingScheme->markObject(env, omrobjectptr_t(node.ref));
	}

	for (auto &fn : cx.markingFns()) {
		fn(marker);
	}
}

void
MarkingDelegate::masterCleanupAfterGC(MM_EnvironmentBase *env)
{}

} // namespace GC
} // namespace OMR
