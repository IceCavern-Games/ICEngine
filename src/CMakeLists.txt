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
)

target_sources(${PROJECT_NAME} PRIVATE ${Source_Files__Renderer})