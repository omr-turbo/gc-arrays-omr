###############################################################################
# Copyright (c) 2017, 2018 IBM Corp. and others
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

omr_assert(FATAL_ERROR TEST OMR_OM)

omr_assert(FATAL_ERROR
	TEST OMR_GC # AND OMR_JITBUILDER
	MESSAGE "Om requires the OMR_GC and OMR_JITBUILDER to be enabled"
)

omr_assert(FATAL_ERROR
	TEST OMR_GC_EXPERIMENTAL_CONTEXT AND OMR_GC_EXPERIMENTAL_OBJECT_SCANNER
	MESSAGE "Om requires the GC's experimental APIs to be enabled"
)

omr_assert(FATAL_ERROR
	TEST NOT OMR_EXAMPLE
	MESSAGE "The OMR example code is temporarily incompatible with Om"
)

# Om Debug Options

set(OMR_OM_POISON_RECLAIMED_MEMORY OFF CACHE BOOL "Debug: Posion reclaimed memory")
set(OMR_OM_COLLECT_ON_ALLOCATE OFF CACHE BOOL "Debug: do a full system collect on every allocation")
set(OMR_OM_TRACE OFF CACHE BOOL "Debug: Verbose tracing prints to stderr")
set(OMR_OM_TEST ${OMR_FVTEST} CACHE BOOL "Enable the Om tests")

# Om configuration

configure_file(
	include/OMR/Om/Config.hpp.in
	include/OMR/Om/Config.hpp
)

# Base: an interface library for Om providing common configuration and flags.

add_library(omr_om_base INTERFACE)

target_compile_features(omr_om_base
	INTERFACE
		cxx_alias_templates
		cxx_constexpr
		cxx_defaulted_functions
		cxx_noexcept
		cxx_nullptr
)

target_include_directories(omr_om_base
	INTERFACE
		include/
		glue/
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_BINARY_DIR}/../include_core
		${CMAKE_CURRENT_BINARY_DIR}/..
		${CMAKE_BINARY_DIR}
		../include_core/
)

target_compile_definitions(omr_om_base
	INTERFACE
		-DUT_DIRECT_TRACE_REGISTRATION
)

# GC glue

set(OMR_GC_GLUE_TARGET "omr_om_gc_glue" CACHE INTERNAL "Om's GC glue")

add_library(omr_om_gc_glue INTERFACE)

target_sources(omr_om_gc_glue
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/glue/CollectorLanguageInterfaceImpl.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/CompactSchemeFixupObject.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/ConcurrentMarkingDelegate.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/FrequentObjectsStats.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/GlobalCollectorDelegate.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/ObjectModelDelegate.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/Profiling.c
		${CMAKE_CURRENT_SOURCE_DIR}/glue/StartupManagerImpl.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/VerboseManagerImpl.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Value.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Globals.cpp
)

target_link_libraries(omr_om_gc_glue
	INTERFACE
		omr_om_base
)

# Util glue

set(OMR_UTIL_GLUE_TARGET "omr_om_util_glue" CACHE INTERNAL "Om's util glue")

add_library(omr_om_util_glue INTERFACE
	# no sources. Provided by OMR_GC_EXPERIMENTAL_CONTEXT
)

target_link_libraries(omr_om_util_glue
	INTERFACE
		omr_om_base
)

# Core glue

set(OMR_CORE_GLUE_TARGET "omr_om_core_glue" CACHE INTERNAL "Om's core glue")

add_library(omr_om_core_glue INTERFACE)

target_sources(omr_om_core_glue
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/glue/LanguageVMGlue.c
)

target_link_libraries(omr_om_core_glue
	INTERFACE
		omr_om_base
)

# RAS glue

set(OMR_RAS_GLUE_TARGET "omr_om_ras_glue" CACHE INTERNAL "Om's RAS glue")

add_library(omr_om_ras_glue INTERFACE)

target_sources(omr_om_ras_glue
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/glue/Profiling.c
)

target_link_libraries(omr_om_ras_glue
	INTERFACE
		omr_om_base
)

# Testing

if(OMR_FVTEST AND OMR_OM_TEST)
	add_subdirectory(test)
endif()

# Installation

install(
	DIRECTORY include
	DESTINATION usr/include
)
