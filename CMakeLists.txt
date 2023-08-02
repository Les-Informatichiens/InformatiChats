cmake_minimum_required(VERSION 3.26)
project(InformatiChats VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        ${PROJECT_NAME}
         src/main.cpp
)

if(MSVC)
    # Set the app as solution startup project
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )

    # Parallel c++ compilation
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
endif()

# Dependencies ========================================================================================
include(cmake/CPM.cmake)

# LibDataChannel ======================================================================================
CPMAddPackage(
    NAME "LibDataChannel"
    VERSION "0.18.5"
    GITHUB_REPOSITORY "paullouisageneau/libdatachannel"
)
# find_package(LibDataChannel CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannel)
# glew ================================================================================================
# find_package(GLEW REQUIRED)
CPMAddPackage(
    NAME "GLEW"
    GITHUB_REPOSITORY "Perlmint/glew-cmake"
    GIT_TAG "glew-cmake-2.2.0"
)
target_link_libraries(${PROJECT_NAME} PRIVATE libglew_shared)
# # SDL ================================================================================================
# # find_package(glfw3 CONFIG REQUIRED)
# CPMAddPackage(
#     NAME "GLFW"
#     GIT_TAG "3.3.8"
#     GITHUB_REPOSITORY "glfw/glfw"
# )
# target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
# imgui ===============================================================================================
# find_package(imgui CONFIG REQUIRED)
CPMAddPackage(
    NAME "imgui"
    GITHUB_REPOSITORY "Les-Informatichiens/imgui-cmake"
    GIT_TAG "docking-cmake-experimental"
    OPTIONS 
        "IMGUI_BUILD_OPENGL3_BINDING ON" 
        "IMGUI_BUILD_GLFW_BINDING ON"
)
find_package(imgui REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

# All libraries which cannot be installed with vcpkg should be a submodule in the "extern" subdirectory
#add_subdirectory(extern)

## tr-and-slate-or =====================================================================================
## Add translator library and copy translation files to build
#target_link_libraries( ${PROJECT_NAME} PRIVATE trandslateor )
#if(EXISTS translations)
#    file(COPY translations DESTINATION "${CMAKE_BINARY_DIR}")
#endif()
#
## The translations dir will be copied to the same dir as the executable when building (only works if compilation is not up to date)
#if(MSVC)
#    add_custom_command(TARGET ${PROJECT_NAME}
#            PRE_BUILD
#            COMMAND xcopy /E /Y "$(ProjectDir)translations\\" "$(OutDir)translations\\"
#    )
#endif()
## =====================================================================================================