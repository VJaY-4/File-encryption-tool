cmake_minimum_required(VERSION 3.16)
project(FileEncryptionTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(encrypt_tool main.cpp txt_crypt.cpp img_crypt.cpp)

# Enable stricter warnings
if(MSVC)
    target_compile_options(encrypt_tool PRIVATE /W4 /WX)
else()
    target_compile_options(encrypt_tool PRIVATE -Wall -Wextra -Wpedantic)
endif()
