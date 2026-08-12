ADD_EXECUTABLE(lccinspector inspector.cpp )

target_link_libraries(lccinspector 
     licensecc
     ${Boost_LIBRARIES})

install(TARGETS lccinspector RUNTIME DESTINATION bin/${LCC_PROJECT_NAME})
