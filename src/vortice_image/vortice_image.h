// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _VORTICE_IMAGE_H
#define _VORTICE_IMAGE_H

#if defined(VIMAGE_SHARED_LIBRARY)
#    if defined(_WIN32)
#        if defined(VIMAGE_IMPLEMENTATION)
#            define VGPU_IMAGE_API  __declspec(dllexport)
#        else
#            define VGPU_IMAGE_API __declspec(dllimport)
#        endif
#    else 
#        if defined(VGPU_IMPLEMENTATION)
#            define VGPU_IMAGE_API __attribute__((visibility("default")))
#        else
#            define VGPU_IMAGE_API
#        endif
#    endif
#else
#    define VGPU_IMAGE_API
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct VImage VImage;

    VGPU_IMAGE_API VImage* vimage_load_ktx(const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VORTICE_IMAGE_H */
