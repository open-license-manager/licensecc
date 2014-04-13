ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
)

add_dependencies( base public_key )