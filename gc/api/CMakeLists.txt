###############################################################################
# Copyright (c) 2018, 2018 IBM Corp. and others
#
# This program and the accompanying materials are made available under
# the terms of the Eclipse Public License 2.0 which accompanies this
# distribution and is available at http://eclipse.org/legal/epl-2.0
# or the Apache License, Version 2.0 which accompanies this distribution
# and is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the
# Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
# version 2 with the GNU Classpath Exception [1] and GNU General Public
# License, version 2 with the OpenJDK Assembly Exception [2].
#
# [1] https://www.gnu.org/software/classpath/license.html
# [2] http://openjdk.java.net/legal/assembly-exception.html
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
###############################################################################

include(OmrAssert)

omr_assert(TEST OMR_GC_EXPERIMENTAL_CONTEXT)

###
### OMR GC
###

target_sources(omrgc
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/OMR_GC_System.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/MarkingDelegate.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/EnvironmentDelegate.cpp
)

if(OMR_GC_EXPERIMENTAL_ALLOCATOR)
	target_sources(omrgc
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src/OMR_GC_Allocator.cpp
	)
endif()

if(OMR_GC_MODRON_COMPACTION)
	target_sources(omrgc
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/glue/CompactDelegate.cpp
	)
endif()

target_include_directories(omrgc
	PUBLIC
		glue/
		include/
)

# https://cmake.org/cmake/help/v3.6/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html#prop_gbl:CMAKE_CXX_KNOWN_FEATURES
target_compile_features(omrgc
	PUBLIC
		cxx_constexpr
		# cxx_auto_type
		cxx_deleted_functions
		cxx_delegating_constructors
		cxx_noexcept
		cxx_nullptr
		cxx_override
		cxx_right_angle_brackets
		cxx_variadic_templates
		cxx_template_template_parameters
)

###
### OMR Util
###

target_sources(omrutil_obj
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/glue/UtilGlue.c
)

if (OMR_FVTEST)
	add_subdirectory(test)
endif()
