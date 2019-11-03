ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
    FileUtils.cpp
    logger.c
    base64.c
)

add_dependencies( base product_initialize )

install(TARGETS base ARCHIVE DESTINATION lib)
