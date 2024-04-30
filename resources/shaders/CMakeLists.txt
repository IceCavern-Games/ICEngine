file (GLOB SHADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.vert 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.frag 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.comp 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.geom 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.tesc 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.tese 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.mesh 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.task 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.rgen 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.rchit 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.rmiss)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}.spv
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}.spv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILENAME}")
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(${PROJECT_NAME} shaders)