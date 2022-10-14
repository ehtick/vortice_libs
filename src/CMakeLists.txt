add_subdirectory(VorticeImage)

if (NOT (WINDOWS_STORE OR EMSCRIPTEN OR ANDROID))
    add_subdirectory(vma)
endif ()

if (WIN32 OR WINDOWS_STORE)
    add_subdirectory(D3D12MemoryAllocator)
endif ()
