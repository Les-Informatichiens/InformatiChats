cmake_minimum_required(VERSION 3.26)
project(InformatiChat)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        ${PROJECT_NAME}
         src/main.cpp
#        src/getopt.cpp
#        src/parse_cl.cpp
#        src/imgui_example.cpp
)

# All libraries which cannot be installed with vcpkg should be a submodule in the "extern" subdirectory
add_subdirectory(extern)