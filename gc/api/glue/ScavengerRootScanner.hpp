/*******************************************************************************
 * Copyright (c) 2015, 2018 IBM Corp. and others
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

#ifndef OMR_GC_SCAVENGERROOTSCANNER_HPP_
#define OMR_GC_SCAVENGERROOTSCANNER_HPP_

#include "omr.h"
#include "omrcfg.h"
#include "omrExampleVM.hpp"
#include "omrhashtable.h"

#include "Base.hpp"
#include "EnvironmentStandard.hpp"
#include "ForwardedHeader.hpp"
#include "Scavenger.hpp"
#include "SublistFragment.hpp"
#include "OMR/GC/StackRoot.hpp"
#include "OMR/GC/System.hpp"
#include "OMR/GC/RefSlotHandle.hpp"

#if defined(OMR_GC_MODRON_SCAVENGER)

namespace OMR {
namespace GC {

class ScavengerRootScanner : public MM_Base
{
	/*
	 * Member data and types
	 */
private:
	MM_Scavenger *_scavenger;

protected:
public:

	/*
	 * Member functions
	 */
private:
protected:
public:
	ScavengerRootScanner(MM_EnvironmentBase *env, MM_Scavenger *scavenger)
		: MM_Base()
		, _scavenger(scavenger)
	{
	};

	void
	scavengeRememberedSet(MM_EnvironmentStandard *env)
	{
		MM_SublistFragment::flush((J9VMGC_SublistFragment*)&env->_scavengerRememberedSet);
		_scavenger->scavengeRememberedSet(env);
	}

	void
	pruneRememberedSet(MM_EnvironmentStandard *env)
	{
		_scavenger->pruneRememberedSet(env);
	}

	void
	scanRoots(MM_EnvironmentStandard *env)
	{
		auto &cx = getContext(env);
		ScavengingRootVisitor visitor(env, _scavenger);

		/// System-wide roots

		for (auto &fn : cx.system().scavengingFns()) {
			fn(visitor);
		}

		/// Per-context roots

		// Note: only scanning the stack roots for *one* context.
		for (StackRootListNode &node : cx.stackRoots()) {
			visitor.edge(NULL, RefSlotHandle((omrobjectptr_t*)&node.ref));
		}

		for (auto &fn : cx.scavengingFns()) {
			fn(visitor);
		}
	}

	void rescanThreadSlots(MM_EnvironmentStandard *env) { }

	void scanClearable(MM_EnvironmentBase *env)
	{
		// TODO
	}

	void flush(MM_EnvironmentStandard *env) { }
};

} // namespace GC
} // namespace OMR

using MM_ScavengerRootScanner = OMR::GC::ScavengerRootScanner;

#endif /* defined(OMR_GC_MODRON_SCAVENGER) */
#endif /* OMR_GC_SCAVENGERROOTSCANNER_HPP_ */
