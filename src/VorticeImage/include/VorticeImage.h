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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum VImageDimension {
    VIMAGE_DIMENSION_TEXTURE1D = 2,
    VIMAGE_DIMENSION_TEXTURE2D = 3,
    VIMAGE_DIMENSION_TEXTURE3D = 4,

    _VIMAGE_DIMENSION_FORCE_U32 = 0x7FFFFFFF
} VImageDimension;

typedef enum VImageFormat {
    // 8-bit formats
    IMAGE_FORMAT_R8_UNORM,
    IMAGE_FORMAT_R8_SNORM,
    IMAGE_FORMAT_R8_UINT,
    IMAGE_FORMAT_R8_SINT,
    // 16-bit formats
    IMAGE_FORMAT_R16_UNORM,
    IMAGE_FORMAT_R16_SNORM,
    IMAGE_FORMAT_R16_UINT,
    IMAGE_FORMAT_R16_SINT,
    IMAGE_FORMAT_R16_FLOAT,
    IMAGE_FORMAT_RG8_UNORM,
    IMAGE_FORMAT_RG8_SNORM,
    IMAGE_FORMAT_RG8_UINT,
    IMAGE_FORMAT_RG8_SINT,
    // 32-bit formats
    IMAGE_FORMAT_R32_UINT,
    IMAGE_FORMAT_R32_SINT,
    IMAGE_FORMAT_R32_FLOAT,
    IMAGE_FORMAT_RG16_UNORM,
    IMAGE_FORMAT_RG16_SNORM,
    IMAGE_FORMAT_RG16_UINT,
    IMAGE_FORMAT_RG16_SINT,
    IMAGE_FORMAT_RG16_FLOAT,
    IMAGE_FORMAT_RGBA8_UNORM,
    IMAGE_FORMAT_RGBA8_SNORM,
    IMAGE_FORMAT_RGBA8_UINT,
    IMAGE_FORMAT_RGBA8_SINT,
    IMAGE_FORMAT_BGRA8_UNORM,
    // Packed 32-Bit Pixel Formats
    IMAGE_FORMAT_RGB9E5_FLOAT,
    IMAGE_FORMAT_RGB10A2_UNORM,
    IMAGE_FORMAT_RGB10A2_UINT,
    IMAGE_FORMAT_RG11B10_FLOAT,
    // 64-bit formats
    IMAGE_FORMAT_RG32_UINT,
    IMAGE_FORMAT_RG32_SINT,
    IMAGE_FORMAT_RG32_FLOAT,
    IMAGE_FORMAT_RGBA16_UNORM,
    IMAGE_FORMAT_RGBA16_SNORM,
    IMAGE_FORMAT_RGBA16_UINT,
    IMAGE_FORMAT_RGBA16_SINT,
    IMAGE_FORMAT_RGBA16_FLOAT,
    // 128-bit formats
    IMAGE_FORMAT_RGBA32_UINT,
    IMAGE_FORMAT_RGBA32_SINT,
    IMAGE_FORMAT_RGBA32_FLOAT,
    // Depth and Stencil formats
    IMAGE_FORMAT_D16_UNORM,
    IMAGE_FORMAT_D32_FLOAT,
    IMAGE_FORMAT_S8,
    IMAGE_FORMAT_D24_UNORM_S8,
    IMAGE_FORMAT_D32_FLOAT_S8,
    // Compressed BC formats
    IMAGE_FORMAT_BC1_UNORM,
    IMAGE_FORMAT_BC2_UNORM,
    IMAGE_FORMAT_BC3_UNORM,
    IMAGE_FORMAT_BC4_UNORM,
    IMAGE_FORMAT_BC4_SNORM,
    IMAGE_FORMAT_BC5_UNORM,
    IMAGE_FORMAT_BC5_SNORM,
    IMAGE_FORMAT_BC6H_UF16,
    IMAGE_FORMAT_BC6H_SF16,
    IMAGE_FORMAT_BC7_UNORM,
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

VIMAGE_API bool VImage_CalculateMipLevels(uint32_t width, uint32_t height, uint32_t* pMipLevels);
VIMAGE_API bool VImage_CalculateMipLevels3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t* pMipLevels);

VIMAGE_API bool VImage_IsCompressed(VImageFormat format);
VIMAGE_API uint32_t VImage_BitsPerPixel(VImageFormat format);
VIMAGE_API bool VImage_ComputePitch(VImageFormat format, uint32_t width, uint32_t height, uint32_t* rowPitch, uint32_t* slicePitch);

VIMAGE_API VImage* VImage_Create2D(VImageFormat format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevels);
VIMAGE_API VImage* VImage_CreateFromMemory(const uint8_t* data, size_t size);
VIMAGE_API void VImage_Destroy(VImage* image);

VIMAGE_API VImageDimension VImage_GetDimension(VImage* image);
VIMAGE_API uint32_t VImage_GetBaseWidth(VImage* image);
VIMAGE_API uint32_t VImage_GetBaseHeight(VImage* image);
VIMAGE_API uint32_t VImage_GetWidth(VImage* image, uint32_t level);
VIMAGE_API uint32_t VImage_GetHeight(VImage* image, uint32_t level);

/* Conversion methods */
VIMAGE_API uint32_t VImage_GetDXGIFormat(VImageFormat format);

#endif /* _VIMAGE_H */
