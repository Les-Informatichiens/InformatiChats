cmake_minimum_required(VERSION 3.26)
project(InformatiChats)

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