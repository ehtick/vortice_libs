add_subdirectory(vortice_image)

if (NOT (WINDOWS_STORE OR EMSCRIPTEN OR ANDROID))
    add_subdirectory(vma)
endif ()
