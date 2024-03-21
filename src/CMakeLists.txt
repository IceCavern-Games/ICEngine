#================== Engine ==================
set(Source_Files__Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${Source_Files__Engine})

#================== Renderer ==================

set(Source_Files__Renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_renderer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_window.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_swap_chain.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ic_swap_chain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/initializers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/initializers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/pipeline_builder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/pipeline_builder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/images.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/util/images.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${Source_Files__Renderer})