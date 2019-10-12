ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
    FileUtils.cpp
    logger.c
)

add_dependencies( base public_key )

install(TARGETS base ARCHIVE DESTINATION lib)
