
project(metalic_crypto)

if(WIN32)
    add_files(INCLUDE_FILES
        "include/metalicensor/crypto/CryptoHelper.h"
        "include/metalicensor/crypto/CryptoHelperWindows.h"
    )
    add_library(metalic_crypto STATIC
        "src/CryptoHelper.cpp"
        "src/CryptoHelperWindows.cpp"
        ${INCLUDE_FILES}
    )
else()
    add_files(INCLUDE_FILES
        "include/metalicensor/crypto/CryptoHelper.h"
        "include/metalicensor/crypto/CryptoHelperLinux.h"
    )
    add_library(metalic_crypto STATIC
        "src/CryptoHelper.cpp"
        "src/CryptoHelperLinux.cpp"
        ${INCLUDE_FILES}
    )
    target_link_libraries(metalic_crypto ${OPENSSL_CRYPTO_LIBRARY})
endif()
target_include_directories(metalic_crypto
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    INTERFACE "$<INSTALL_INTERFACE:include/>"
)
set_target_properties(metalic_crypto PROPERTIES FOLDER "libs")
install(TARGETS metalic_crypto
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY "include/"
    DESTINATION "include/"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "*.h"
)
