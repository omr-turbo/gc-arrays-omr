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

#ifndef SCAVENGERBACKOUTSCANNER_HPP_
#define SCAVENGERBACKOUTSCANNER_HPP_

#include "omr.h"
#include "omrcfg.h"
#include "omrhashtable.h"

#include "Base.hpp"
#include "EnvironmentStandard.hpp"
#include "Scavenger.hpp"

#if defined(OMR_GC_MODRON_SCAVENGER)

class MM_ScavengerBackOutScanner : public MM_Base
{
	/*
	 * Member data and types
	 */
private:
protected:
public:

	/*
	 * Member functions
	 */
private:
protected:
public:
	MM_ScavengerBackOutScanner(MM_EnvironmentBase *env, bool singleThread, MM_Scavenger *scavenger)
		: MM_Base()
	{
	};

	void
	scanAllSlots(MM_EnvironmentBase *env)
	{
	}
};

#endif /* defined(OMR_GC_MODRON_SCAVENGER) */
#endif /* SCAVENGERBACKOUTSCANNER_HPP_ */
