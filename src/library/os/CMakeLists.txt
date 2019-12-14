IF(UNIX OR OPENSSL_FOUND)
	IF(UNIX)
		target_sources(licensecc_static PRIVATE
		    ${CMAKE_CURRENT_SOURCE_DIR}/openssl/signature_verifier.cpp
		    ${CMAKE_CURRENT_SOURCE_DIR}/os-linux.c 
		    ${CMAKE_CURRENT_SOURCE_DIR}/network_id.c)
	ELSE(UNIX)
  	    target_sources(licensecc_static PRIVATE 
  	    	${CMAKE_CURRENT_SOURCE_DIR}/os-win.c)
	ENDIF(UNIX)
ELSE(UNIX OR OPENSSL_FOUND)
    target_sources(licensecc_static STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/signature_verifier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/os-win.c)
ENDIF(UNIX OR OPENSSL_FOUND)

