ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
    logger.c
)

add_dependencies( base public_key )