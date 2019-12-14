ADD_LIBRARY(base STATIC
    EventRegistry.cpp
    StringUtils.cpp
    FileUtils.cpp
    logger.c
    base64.c
)

add_dependencies( base project_initialize )

install(TARGETS base EXPORT licensecc ARCHIVE DESTINATION lib/${PROJECT_NAME}/${LCC_PROJECT_NAME})

