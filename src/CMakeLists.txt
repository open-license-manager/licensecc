add_subdirectory("library")


configure_file (
	"templates/licensecc_properties_test.h.in" 
	"${CMAKE_BINARY_DIR}/licensecc_properties_test.h"
)
configure_file (
	"templates/licensecc_properties.h.in" 
	"${LCC_INCLUDE_DIR}/licensecc_properties.h"
)