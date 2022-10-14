SET(TARGET_NAME VulkanMemoryAllocator )

add_library(${TARGET_NAME} 
    vk_mem_alloc.cpp
    vk_mem_alloc.h
)

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_14)

target_link_libraries(${TARGET_NAME}  PRIVATE
    vulkan
)
