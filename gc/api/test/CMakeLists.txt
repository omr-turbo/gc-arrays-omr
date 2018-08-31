include(OmrAssert)

omr_assert(TEST OMR_FVTEST))

function(omr_add_gc_test testname)
	add_executable("${testname}"
		"${testname}.cpp"
		main.cpp
	)
	target_link_libraries("${testname}"
		PUBLIC
			omrgc
			omrGtestGlue
	)
	add_test(
		NAME    "${testname}"
		COMMAND "${testname}"
	)
	set_property(
		TARGET "${testname}"
		PROPERTY CXX_STANDARD 11
	)
endfunction(omr_add_gc_test)
