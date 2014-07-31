add_subdirectory("bootstrap")
add_subdirectory("library")

IF(Boost_FOUND)
	add_subdirectory("pc-identifier")
	add_subdirectory("license-generator")
ENDIF(Boost_FOUND)
	