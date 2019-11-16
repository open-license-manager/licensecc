IF(UNIX)
    ADD_LIBRARY(os STATIC
        linux/verifier.cpp
        os.c
        os-linux.c
        network_id.c)
        
	target_link_libraries(
	     os
	     base
	     OpenSSL::Crypto
	     ${EXTERNAL_LIBS}
	     ##ZLIB::ZLIB
	)
	
ELSE(UNIX)
    ADD_LIBRARY(os STATIC
        windows/verifier.cpp
        os.c
        os-win.c)

	target_link_libraries(
	     os
	     base
	)
ENDIF(UNIX)



install(TARGETS os ARCHIVE DESTINATION lib)
