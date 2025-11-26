set(PROJECT_NAME Project32.Core)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "src/BackEnd/backend.h"
    "src/BackEnd/common.h"
    "src/BackEnd/OpenGL/GL_backEnd.h"
    "src/BackEnd/OpenGL/GL_common.h"
    "src/BackEnd/OpenGL/Types/GL_mesh.h"
    "src/BackEnd/OpenGL/Types/GL_shader.h"
    "src/BackEnd/OpenGL/Types/GL_shadow.h"
    "src/BackEnd/OpenGL/Types/GL_skybox.h"
    "src/BackEnd/OpenGL/Types/GL_textures.h"
    "src/BackEnd/types.h"
    "src/common.h"
    "src/core/context_guard.h"
    "src/core/engine.h"
    "src/core/input.h"
    "src/core/ui_app.h"
    "src/core/window.h"
    "src/renderer/lighting.h"
    "src/renderer/render_data.h"
    "src/renderer/renderer.h"
    "src/renderer/vertex.h"
    "src/scene/camera.h"
    "src/scene/model.h"
    "src/scene/object.h"
    "src/scene/scene.h"
    "src/scene/transform.h"
    "src/scene/wall.h"
    "src/scripting/script_system.h"
    "src/types/constants.h"
    "src/types/math.h"
    "src/types/timer.hpp"
    "src/types/util.hpp"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "../Vendor/glad/src/glad.c"
    "../Vendor/imgui/backends/imgui_impl_glfw.cpp"
    "../Vendor/imgui/backends/imgui_impl_opengl3.cpp"
    "../Vendor/imgui/imgui.cpp"
    "../Vendor/imgui/imgui_demo.cpp"
    "../Vendor/imgui/imgui_draw.cpp"
    "../Vendor/imgui/imgui_tables.cpp"
    "../Vendor/imgui/imgui_widgets.cpp"
    "../Vendor/stb_image/stb_image.cpp"
    "src/BackEnd/backend.cpp"
    "src/BackEnd/OpenGL/GL_backEnd.cpp"
    "src/BackEnd/OpenGL/Types/GL_mesh.cpp"
    "src/BackEnd/OpenGL/Types/GL_shader.cpp"
    "src/BackEnd/OpenGL/Types/GL_shadow.cpp"
    "src/BackEnd/OpenGL/Types/GL_skybox.cpp"
    "src/BackEnd/OpenGL/Types/GL_textures.cpp"
    "src/BackEnd/types.cpp"
    "src/common.cpp"
    "src/core/context_guard.cpp"
    "src/core/engine.cpp"
    "src/core/input.cpp"
    "src/core/ui.cpp"
    "src/core/ui.h"
    "src/core/ui_app.cpp"
    "src/core/window.cpp"
    "src/main.cpp"
    "src/renderer/render_data.cpp"
    "src/renderer/renderer.cpp"
    "src/scene/camera.cpp"
    "src/scene/model.cpp"
    "src/scene/object.cpp"
    "src/scene/scene.cpp"
    "src/scene/wall.cpp"
    "src/scripting/script_system.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/common.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Project32Core)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../Project32.UIX/src"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/vulkan"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/spdlog/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/RBPak/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/imgui;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/Jolt/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/glad/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/tiny_obj;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/stb_image;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/nlohmann;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/GLFW/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/lua/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/glm"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "PROJECT32CORE_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "PROJECT32CORE_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /utf-8;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(../Vendor/glad/src/glad.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/backends/imgui_impl_glfw.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/backends/imgui_impl_opengl3.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/imgui.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/imgui_demo.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/imgui_draw.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/imgui_tables.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Release>:"
                "/Y-"
            ">"
            "/w"
        )
    endif()
    source_file_compile_options(../Vendor/imgui/imgui_widgets.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/W3;/Y-"
        )
    endif()
    source_file_compile_options(../Vendor/stb_image/stb_image.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Release>:"
            "glfw3;"
            "RBPak;"
            "vulkan-1"
        ">"
        "zlib;"
        "lua54"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CMAKE_SOURCE_DIR}/Vendor/RBPak/lib/Debug;"
            "${CMAKE_SOURCE_DIR}/Vendor/jolt/lib/Debug;"
            "${CMAKE_SOURCE_DIR}/Vendor/GLFW/lib/Debug"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_SOURCE_DIR}/Vendor/lua/lib;"
            "${CMAKE_SOURCE_DIR}/Vendor/RBPak/lib/Release;"
            "${CMAKE_SOURCE_DIR}/Vendor/Jolt/lib/Release;"
            "${CMAKE_SOURCE_DIR}/Vendor/vulkan/lib;"
            "${CMAKE_SOURCE_DIR}/Vendor/GLFW/lib/Release"
        ">"
    )
endif()

