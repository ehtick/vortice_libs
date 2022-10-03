add_subdirectory(VorticeImage)

if (NOT (WINDOWS_STORE OR EMSCRIPTEN OR ANDROID))
    add_subdirectory(vma)
endif ()
