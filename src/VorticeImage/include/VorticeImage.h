// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _VIMAGE_H
#define _VIMAGE_H

#if defined(VIMAGE_SHARED_LIBRARY_BUILD)
#   if defined(_MSC_VER)
#       define _VIMAGE_EXPORT __declspec(dllexport)
#   elif defined(__GNUC__)
#       define _VIMAGE_EXPORT __attribute__((visibility("default")))
#   else
#       define _VIMAGE_EXPORT
#       pragma warning "Unknown dynamic link import/export semantics."
#   endif
#elif defined(VIMAGE_SHARED_LIBRARY_INCLUDE)
#   if defined(_MSC_VER)
#       define _VIMAGE_EXPORT __declspec(dllimport)
#   else
#       define _VIMAGE_EXPORT
#   endif
#else
#   define _VIMAGE_EXPORT
#endif

#ifdef __cplusplus
#    define _VIMAGE_EXTERN extern "C"
#else
#    define _VIMAGE_EXTERN extern
#endif


#define VIMAGE_API _VIMAGE_EXTERN _VIMAGE_EXPORT

#include <stddef.h>
#include <stdint.h>

typedef enum VImageFormat {
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

    _VIMAGE_FORMAT_COUNT,
    _VIMAGE_FORMAT_FORCE_U32 = 0x7FFFFFFF
} VImageFormat;

typedef struct VImage VImage;

VIMAGE_API uint32_t VImage_GetMemorySize(VImageFormat format, uint32_t width, uint32_t height);
VIMAGE_API VImage* VImage_CreateFromMemory(const uint8_t* data, size_t size);
VIMAGE_API void VImage_Destroy(VImage* image);

VIMAGE_API uint32_t VImage_GetBaseWidth(VImage* image);
VIMAGE_API uint32_t VImage_GetBaseHeight(VImage* image);
VIMAGE_API uint32_t VImage_GetWidth(VImage* image, uint32_t level);
VIMAGE_API uint32_t VImage_GetHeight(VImage* image, uint32_t level);

#endif /* _VIMAGE_H */
