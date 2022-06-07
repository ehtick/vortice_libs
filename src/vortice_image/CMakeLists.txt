add_library(vortice_image ${LIBRARY_TYPE}
    vortice_image.h
    ktx_c_binding.cpp
)

 target_compile_definitions(vortice_image PRIVATE
    VIMAGE_SHARED_LIBRARY
    VIMAGE_IMPLEMENTATION
 )

target_link_libraries(vortice_image
    ktx
)

if(APPLE)
    set_target_properties(vortice_image PROPERTIES BUNDLE true)
    if(KTX_EMBED_BITCODE)
        target_compile_options(vortice_image PRIVATE "-fembed-bitcode")
    endif()
elseif(WIN32)
    
endif()
