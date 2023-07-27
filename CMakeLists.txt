cmake_minimum_required(VERSION 3.26)
project(InformatiChat)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        ${PROJECT_NAME}
         src/main.cpp
)
# LibDataChannel ======================================================================================
find_package(LibDataChannel CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannel)
# imgui ===============================================================================================
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
# glew ================================================================================================
find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
# SDL ================================================================================================
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# All libraries which cannot be installed with vcpkg should be a submodule in the "extern" subdirectory
#add_subdirectory(extern)