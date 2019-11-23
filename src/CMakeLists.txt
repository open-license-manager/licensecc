add_subdirectory("library")

#for no reason overwrite it (maybe it's manually customized)
IF(NOT EXISTS "${LCC_INCLUDE_DIR}/licensecc_properties.h")
	configure_file ("templates/licensecc_properties.h.in" 
		"${LCC_INCLUDE_DIR}/licensecc_properties.h")
ENDIF(NOT EXISTS "${LCC_INCLUDE_DIR}/licensecc_properties.h")