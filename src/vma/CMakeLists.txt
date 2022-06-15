add_library(vma ${LIBRARY_TYPE}
    vk_mem_alloc.h
    vk_mem_alloc.cpp
)

target_link_libraries(vma PRIVATE
    vulkan
)
