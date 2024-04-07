project(ICEditor)

add_executable(${PROJECT_NAME}
    main.cpp
)

# Build the editor in the parent/root build directory.
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (NOT TARGET IC::ICEngine)
    find_package(icengine REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE IC::ICEngine)
