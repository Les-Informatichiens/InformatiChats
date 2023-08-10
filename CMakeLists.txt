cmake_minimum_required(VERSION 3.26)
project(InformatiChats VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/console.h
        src/Chat.cpp
        src/Chat.h
        src/imgui_impl_opengl3_loader.h
        src/imgui_impl_opengl3_pixel.cpp
        src/imgui_impl_opengl3_pixel.h
        src/BatchRenderer.cpp
        src/BatchRenderer.h
        src/pxlui/BatchRenderer.cpp
        src/pxlui/BatchRenderer.h
        src/pxlui/GLShaderUtil.cpp
        src/pxlui/GLShaderUtil.h
        src/pxlui/ShaderProgram.cpp
        src/pxlui/ShaderProgram.h
        src/imgui_impl_glfw_pixel.cpp
        src/imgui_impl_glfw_pixel.h
        src/Shaders.h
        src/Application.cpp
        src/Application.h
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
if(NOT TARGET LibDataChannel::LibDataChannelStatic)
    CPMFindPackage(
        NAME "LibDataChannel"
        VERSION "0.18.5"
        GITHUB_REPOSITORY "paullouisageneau/libdatachannel"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannelStatic)
# =====================================================================================================

# nlohmann-json =======================================================================================
if(NOT TARGET nlohmann_json::nlohmann_json)
    CPMFindPackage(
            NAME          "nlohmann_json"
            VERSION       "3.11.2"
            GITHUB_REPOSITORY "nlohmann/json"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
# =====================================================================================================

# glm =================================================================================================
CPMAddPackage(
    NAME "glm"
    GITHUB_REPOSITORY "g-truc/glm"
    GIT_TAG "0.9.9.8"
)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
# =====================================================================================================

# glew ================================================================================================
if(NOT TARGET libglew_static)
    CPMAddPackage(
        NAME "GLEW"
        GITHUB_REPOSITORY "Perlmint/glew-cmake"
        GIT_TAG "glew-cmake-2.2.0"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
# =====================================================================================================

# imgui ===============================================================================================
if(NOT TARGET imgui::imgui)
    CPMFindPackage(
        NAME "imgui"
        GITHUB_REPOSITORY "Les-Informatichiens/imgui-cmake"
        GIT_TAG "docking-cmake-experimental"
        OPTIONS
            "IMGUI_BUILD_OPENGL3_BINDING ON"
            "IMGUI_BUILD_GLFW_BINDING ON"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
# =====================================================================================================

# tr-and-slate-or =====================================================================================
if(NOT TARGET trandslateor)
    CPMFindPackage(
        NAME "trandslateor"
        GITHUB_REPOSITORY "ThierryCantin-Demers/tr-and-slate-or"
        GIT_TAG "v0.1"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE trandslateor)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/translations")
    message(STATUS "Copying translations to build directory")
    file(COPY translations DESTINATION "${CMAKE_BINARY_DIR}")
endif()
# =====================================================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    message(STATUS "Copying res to build directory")
    file(COPY res DESTINATION "${CMAKE_BINARY_DIR}")
endif()