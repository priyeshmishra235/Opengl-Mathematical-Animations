cmake_minimum_required(VERSION 3.20)
project(AnimOpengl VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    include
    .
    /usr/local/include/imgui
)

file(GLOB SRC_FILES "src/*.cpp")

set(IMGUI_SOURCES
    /usr/local/include/imgui/imgui.cpp
    /usr/local/include/imgui/imgui_draw.cpp
    /usr/local/include/imgui/imgui_widgets.cpp
    /usr/local/include/imgui/imgui_tables.cpp
    /usr/local/include/imgui/imgui_impl_glfw.cpp
    /usr/local/include/imgui/imgui_impl_opengl3.cpp
)

set(GLAD_KHR_SOURCES
    glad/glad.c
)

add_executable(AnimOpengl
    ${SRC_FILES}
    ${IMGUI_SOURCES}
    ${GLAD_KHR_SOURCES}
)

find_package(OpenGL REQUIRED)
target_link_libraries(AnimOpengl OpenGL::GL)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(AnimOpengl glfw)

find_package(Threads REQUIRED)
target_link_libraries(AnimOpengl Threads::Threads)

target_compile_options(AnimOpengl PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(AnimOpengl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

add_custom_command(TARGET AnimOpengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:AnimOpengl>/assets
)
