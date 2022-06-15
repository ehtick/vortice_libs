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

    typedef enum ImageFormat {
        IMAGE_FORMAT_R8,
        IMAGE_FORMAT_RG8,
        IMAGE_FORMAT_RGBA8,
        IMAGE_FORMAT_R16,
        IMAGE_FORMAT_RG16,
        IMAGE_FORMAT_RGBA16,
        IMAGE_FORMAT_R16F,
        IMAGE_FORMAT_RG16F,
        IMAGE_FORMAT_RGBA16F,
        IMAGE_FORMAT_R32F,
        IMAGE_FORMAT_RG32F,
        IMAGE_FORMAT_RGBA32F,
        IMAGE_FORMAT_RGB565,
        IMAGE_FORMAT_RGB5A1,
        IMAGE_FORMAT_RGB10A2,
        IMAGE_FORMAT_RG11B10F,
        IMAGE_FORMAT_BC1,
        IMAGE_FORMAT_BC2,
        IMAGE_FORMAT_BC3,
        IMAGE_FORMAT_BC4U,
        IMAGE_FORMAT_BC4S,
        IMAGE_FORMAT_BC5U,
        IMAGE_FORMAT_BC5S,
        IMAGE_FORMAT_BC6UF,
        IMAGE_FORMAT_BC6SF,
        IMAGE_FORMAT_BC7,
        IMAGE_FORMAT_ASTC_4x4,
        IMAGE_FORMAT_ASTC_5x4,
        IMAGE_FORMAT_ASTC_5x5,
        IMAGE_FORMAT_ASTC_6x5,
        IMAGE_FORMAT_ASTC_6x6,
        IMAGE_FORMAT_ASTC_8x5,
        IMAGE_FORMAT_ASTC_8x6,
        IMAGE_FORMAT_ASTC_8x8,
        IMAGE_FORMAT_ASTC_10x5,
        IMAGE_FORMAT_ASTC_10x6,
        IMAGE_FORMAT_ASTC_10x8,
        IMAGE_FORMAT_ASTC_10x10,
        IMAGE_FORMAT_ASTC_12x10,
        IMAGE_FORMAT_ASTC_12x12,

        IMAGE_FORMAT_COUNT,
        _ImageFormat_Force32 = 0x7FFFFFFF
    } ImageFormat;

    typedef struct Image Image;

    VGPU_IMAGE_API uint32_t vimageGetMemorySize(ImageFormat format, uint32_t width, uint32_t height);
    VGPU_IMAGE_API Image* vimageCreateFromMemory(const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VORTICE_IMAGE_H */
