# licensecc-config.cmake - package configuration file

#structure:
#$CMAKE_INSTALL_PATH
#├── bin
#│   ├── <<PROJECT_NAME>>
#│   │   └── lccinspector
#│   ├── lccgen -> lccgen-2.1.0
#│   └── lccgen-2.1.0
#├── include
#│   └── licensecc
#│       ├── datatypes.h
#│       ├── <<PROJECT_NAME>>
#│       │   ├── licensecc_properties.h
#│       │   └── public_key.h
#│       └── licensecc.h
#└── lib
#    ├── cmake
#    │   └── licensecc
#    │       ├── licensecc-config.cmake
#    │       └── licensecc-config-version.cmake
#    └── licensecc
#        └── <<PROJECT_NAME>>
#            ├── cmake
#            │   ├── licensecc.cmake
#            │   └── licensecc-debug.cmake
#            └── liblicensecc.a


get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
#TODO windows?
set(LCC_PRJ_BASE "${SELF_DIR}/../../licensecc") #($CMAKE_INSTALL_PATH/lib/licensecc)
set(LCC_FOUND false)

if(licensecc_FIND_COMPONENTS)
	foreach(component ${licensecc_FIND_COMPONENTS})
		set(cmakefile "${LCC_PRJ_BASE}/${component}/cmake/licensecc.cmake")
		if(EXISTS "${cmakefile}")
			include("${cmakefile}")
			set(${component}_FOUND true CACHE BOOL "Licensecc ${component} present")
			set(LCC_FOUND true)
		else(EXISTS "${cmakefile}")
			set(${component}_FOUND false CACHE BOOL "Licensecc ${component} present")
			message( WARNING "Component ${component} declared but not found." )
		endif(EXISTS "${cmakefile}")
		mark_as_advanced(${component}_FOUND)
	endforeach(component)
else(licensecc_FIND_COMPONENTS)
	if(LCC_PROJECT_NAME)
		if(EXISTS "${LCC_PRJ_BASE}/${LCC_PROJECT_NAME}/cmake/licensecc.cmake")
			include("${LCC_PRJ_BASE}/${LCC_PROJECT_NAME}/cmake/licensecc.cmake")
			set(${LCC_PROJECT_NAME}_FOUND true CACHE BOOL "Licensecc ${LCC_PROJECT_NAME} present")
			set(LCC_FOUND true)
		else()
			message( WARNING "Variable LCC_PROJECT_NAME declared but project ${LCC_PROJECT_NAME} not found. ")
		endif()
		mark_as_advanced(LCC_${LCC_PROJECT_NAME}_FOUND)
	else(LCC_PROJECT_NAME)
		if(PROJECT_NAME AND (EXISTS "${LCC_PRJ_BASE}/${PROJECT_NAME}/cmake/licensecc.cmake"))
			message(STATUS "licensecc found in ${LCC_PRJ_BASE}/${PROJECT_NAME}/cmake/licensecc.cmake")
			include("${LCC_PRJ_BASE}/${PROJECT_NAME}/cmake/licensecc.cmake")
			set(${PROJECT_NAME}_FOUND true CACHE BOOL "Licensecc ${PROJECT_NAME} present")
		    mark_as_advanced(LCC_${PROJECT_NAME}_FOUND)
		    set(LCC_FOUND true)
		else() #Try "DEFAULT" as last hope
			if(EXISTS "${LCC_PRJ_BASE}/DEFAULT/cmake/licensecc.cmake")
				message( WARNING "Licensecc found under DEFAULT project. You should generate your own project in production systems. ")
				include("${LCC_PRJ_BASE}/DEFAULT/cmake/licensecc.cmake")
				set(LCC_DEFAULT_FOUND true CACHE BOOL "Licensecc ${PROJECT_NAME} present")
		    	mark_as_advanced(LCC_DEFAULT_FOUND)
		    	set(LCC_FOUND true)
			endif(EXISTS "${LCC_PRJ_BASE}/DEFAULT/cmake/licensecc.cmake")
		endif()
	endif(LCC_PROJECT_NAME)
endif(licensecc_FIND_COMPONENTS)

if(LCC_FOUND)
	get_property(COMPILE_DEF TARGET licensecc::licensecc PROPERTY INTERFACE_COMPILE_DEFINITIONS)
	if("HAS_OPENSSL" IN_LIST COMPILE_DEF AND NOT OpenSSL_FOUND)
		#message( VERBOSE "Trying to find openssl (required by the target)")
	    SET ( OPENSSL_USE_STATIC_LIBS ON )
	    find_package(OpenSSL REQUIRED COMPONENTS Crypto)
	endif()
endif(LCC_FOUND)