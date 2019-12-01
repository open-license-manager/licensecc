IF(UNIX OR OPENSSL_FOUND)
	IF(UNIX)
	    ADD_LIBRARY(os STATIC openssl/signature_verifier.cpp os.c os-linux.c network_id.c)
	ELSE(UNIX)
  	    ADD_LIBRARY(os STATIC openssl/signature_verifier.cpp os.c os-win.c)
	ENDIF(UNIX)
	
	if (CMAKE_VERSION VERSION_GREATER 3.5)
		target_link_libraries(os base OpenSSL::Crypto ${EXTERNAL_LIBS} ${CMAKE_DL_LIBS})
	#patch for old cmake in Centos 7
	ELSE(CMAKE_VERSION VERSION_GREATER 3.5)
		target_link_libraries(os base ${OPENSSL_CRYPTO_LIBRARY} ${EXTERNAL_LIBS} ${CMAKE_DL_LIBS})
	ENDIF(CMAKE_VERSION VERSION_GREATER 3.5)
ELSE(UNIX OR OPENSSL_FOUND)
    ADD_LIBRARY(os STATIC
        windows/signature_verifier.cpp
        os.c
        os-win.c)

	target_link_libraries(
	     os
	     base
	     ${EXTERNAL_LIBS} 
	)
ENDIF(UNIX OR OPENSSL_FOUND)

install(TARGETS os ARCHIVE DESTINATION lib/${PROJECT_NAME}/${LCC_PROJECT_NAME})
