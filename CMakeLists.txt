cmake_minimum_required(VERSION 3.16)
project(FileEncryptionTool LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Place executables in the project root for easy Windows Explorer launch.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR})

# ── Original CLI tool (unchanged) ────────────────────────────────
add_executable(encrypt_tool main.cpp crypto_core.cpp txt_crypt.cpp img_crypt.cpp utils.cpp)

if(MSVC)
    target_compile_options(encrypt_tool PRIVATE /W4 /WX)
else()
    target_compile_options(encrypt_tool PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── GUI tool with Dear ImGui + GLFW + OpenGL ─────────────────────

# GLFW (built from source, no examples/tests/docs)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

# Dear ImGui sources
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# tinyfiledialogs
set(TFD_DIR ${CMAKE_SOURCE_DIR}/libs/tinyfiledialogs)

# GUI executable
add_executable(encrypt_tool_gui
    gui_main.cpp
    crypto_core.cpp
    ${IMGUI_SOURCES}
    ${TFD_DIR}/tinyfiledialogs.c
)

target_include_directories(encrypt_tool_gui PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${TFD_DIR}
    ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(encrypt_tool_gui PRIVATE glfw)

# Link OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(encrypt_tool_gui PRIVATE OpenGL::GL)

# On Windows, link comdlg32 + ole32 for tinyfiledialogs, bcrypt for AES
if(WIN32)
    target_link_libraries(encrypt_tool_gui PRIVATE comdlg32 ole32 bcrypt)
    target_link_libraries(encrypt_tool PRIVATE bcrypt)
endif()

if(MSVC)
    target_compile_options(encrypt_tool_gui PRIVATE /W4)
else()
    target_compile_options(encrypt_tool_gui PRIVATE -Wall -Wextra)
    target_link_options(encrypt_tool_gui PRIVATE -mwindows)
endif()
