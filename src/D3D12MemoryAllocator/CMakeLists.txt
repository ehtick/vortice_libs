SET(TARGET_NAME D3D12MemoryAllocator)

add_library(${TARGET_NAME} 
    D3D12MemAlloc.cpp
    c_exports.cpp
    D3D12MemAlloc.h
)

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_14)

target_link_libraries(D3D12MemoryAllocator PUBLIC
    d3d12.lib
    dxgi.lib
    dxguid.lib
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(D3D12MemoryAllocator PRIVATE
        D3D12MA_EXPORTS
    )

    target_compile_definitions(D3D12MemoryAllocator INTERFACE
        D3D12MA_IMPORTS
    )
endif()

