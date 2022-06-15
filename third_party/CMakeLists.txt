# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

# ktx
set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "")
add_subdirectory(ktx)

# vulkan
add_library(vulkan INTERFACE)
set(VULKAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include)
target_sources(vulkan INTERFACE 
    ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.h
    ${VULKAN_INCLUDE_DIR}/vulkan/vk_platform.h
    ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_core.h
)
target_include_directories(vulkan INTERFACE ${VULKAN_INCLUDE_DIR})
target_compile_definitions(vulkan INTERFACE VK_NO_PROTOTYPES)
#target_compile_definitions(vulkan INTERFACE VK_ENABLE_BETA_EXTENSIONS)

if (ANDROID)
    target_sources(vulkan INTERFACE ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_android.h)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_ANDROID_KHR)
elseif(WIN32)
    target_sources(vulkan INTERFACE ${VULKAN_INCLUDE_DIR}/vulkan/vulkan_win32.h)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX)
    #find_package(PkgConfig REQUIRED)
    #pkg_check_modules(XCB xcb REQUIRED)
    #if (XCB_FOUND)
    #    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XCB_KHR)
    #endif()

    target_compile_definitions(vulkan INTERFACE VK_USE_PLATFORM_XCB_KHR)
endif()
