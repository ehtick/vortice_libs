// Copyright � Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "VorticeImagePrivate.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <float.h> // FLT_MAX
#include <assert.h>

// TODO: Use WIC on windows platform

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_JPEG
//#define STBI_ONLY_PNG
#include "stb_image.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>

#include <ktx.h>

typedef struct Mipmap {
    void* data;
    size_t size;
    size_t stride;
} Mipmap;

struct VImage {
    VImageDimension dimension;
    VImageFormat    format;
    uint32_t        baseWidth;
    uint32_t        baseHeight;
    uint32_t        depthOrArraySize;
    uint32_t        mipLevels;
    void* data;
    size_t          size;
    Mipmap* mipmaps;
};

static VImage* VImage_LoadDDS(const uint8_t* data, size_t size)
{
    typedef struct DDSHeader {
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitchOrLinearSize;
        uint32_t depth;
        uint32_t mipmapCount;
        uint32_t reserved[11];
        struct {
            uint32_t size;
            uint32_t flags;
            uint32_t fourCC;
            uint32_t rgbBitCount;
            uint32_t rMask;
            uint32_t gMask;
            uint32_t bMask;
            uint32_t aMask;
        } format;
        uint32_t caps[4];
        uint32_t reserved2;
    } DDSHeader;

    if (size < 4 + sizeof(DDSHeader))
        return NULL;

    // Magic
    size_t length = size;
    uint32_t* magic = (uint32_t*)data;
    if (*magic != 0x20534444)
        return NULL;
    length -= 4;
    data += 4;

    // Header
    DDSHeader* header = (DDSHeader*)data;
    if (length < sizeof(DDSHeader))
        return NULL;

    if (header->size != sizeof(DDSHeader) || header->format.size != sizeof(header->format))
        return NULL;

    length -= sizeof(DDSHeader);
    data += sizeof(DDSHeader);
    return NULL;
}

static VImage* VImage_LoadASTC(const uint8_t* data, size_t size)
{
    return NULL;
}

static VImage* VImage_LoadKTX1(const uint8_t* data, size_t size)
{
    return NULL;
}

static VImage* VImage_LoadKTX2(const uint8_t* data, size_t size)
{
    return NULL;
}

static VImage* VImage_LoadSTB(const uint8_t* data, size_t size)
{
    void* stb_data;
    VImageFormat format;
    int width, height, channels;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        stb_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels) {
        case 1: format = IMAGE_FORMAT_R16; break;
        case 2: format = IMAGE_FORMAT_RG16; break;
        case 4: format = IMAGE_FORMAT_RGBA16; break;
        default:
            //assert(FALSE && "Unsupported channel count for 16 bit image: %d", channels);
            VIMAGE_UNREACHABLE();
        }
    }
    else if (stbi_is_hdr_from_memory(data, (int)size))
    {
        stb_data = stbi_loadf_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA32F;
    }
    else {
        stb_data = stbi_load_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA8;
    }

    if (!stb_data) {
        return NULL;
    }

    uint32_t memorySize = VImage_GetMemorySize(format, width, height);

    VImage* image = (VImage*)malloc(sizeof(VImage));
    image->baseWidth = width;
    image->baseHeight = height;
    image->format = format;
    image->depthOrArraySize = 1;
    image->mipLevels = 1;
    image->data = stb_data;
    image->size = memorySize;
    image->mipmaps[0].data = stb_data;
    image->mipmaps[0].size = memorySize;
    return image;
}

uint32_t _VImage_CountMips(uint32_t width, uint32_t height)
{
    uint32_t mipLevels = 1;

    while (height > 1 || width > 1)
    {
        if (height > 1)
            height >>= 1;

        if (width > 1)
            width >>= 1;

        ++mipLevels;
    }

    return mipLevels;
}

uint32_t _VImage_CountMips3D(uint32_t width, uint32_t height, uint32_t depth)
{
    uint32_t mipLevels = 1;

    while (height > 1 || width > 1 || depth > 1)
    {
        if (height > 1)
            height >>= 1;

        if (width > 1)
            width >>= 1;

        if (depth > 1)
            depth >>= 1;

        ++mipLevels;
    }

    return mipLevels;
}

bool VImage_CalculateMipLevels(uint32_t width, uint32_t height, uint32_t* pMipLevels)
{
    if (*pMipLevels > 1)
    {
        const size_t maxMips = _VImage_CountMips(width, height);
        if (*pMipLevels > maxMips)
            return false;
    }
    else if (*pMipLevels == 0)
    {
        *pMipLevels = _VImage_CountMips(width, height);
    }
    else
    {
        *pMipLevels = 1;
    }

    return true;
}

bool VImage_CalculateMipLevels3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t* pMipLevels)
{
    if (*pMipLevels > 1)
    {
        const size_t maxMips = _VImage_CountMips3D(width, height, depth);
        if (*pMipLevels > maxMips)
            return false;
    }
    else if (*pMipLevels == 0)
    {
        *pMipLevels = _VImage_CountMips3D(width, height, depth);
    }
    else
    {
        *pMipLevels = 1;
    }

    return true;
}

bool VImage_IsCompressed(VImageFormat format) {
    return format >= IMAGE_FORMAT_BC1_UNORM && format <= IMAGE_FORMAT_ASTC_12x12;
}

bool VImage_ComputePitch(VImageFormat format, uint32_t width, uint32_t height, uint32_t* rowPitch, uint32_t* slicePitch) {
    uint64_t pitch = 0;
    uint64_t slice = 0;

    switch (format)
    {
    case IMAGE_FORMAT_BC1_UNORM:
    case IMAGE_FORMAT_BC4_UNORM:
    case IMAGE_FORMAT_BC4_SNORM:
        assert(VImage_IsCompressed(format));
        {
            const uint32_t nbw = _VIMAGE_MAX(1u, (width + 3u) / 4u);
            const uint32_t nbh = _VIMAGE_MAX(1u, (height + 3u) / 4u);
            pitch = nbw * 8u;
            slice = pitch * nbh;
        }
        break;

    case IMAGE_FORMAT_BC2_UNORM:
    case IMAGE_FORMAT_BC3_UNORM:
    case IMAGE_FORMAT_BC5_UNORM:
    case IMAGE_FORMAT_BC5_SNORM:
    case IMAGE_FORMAT_BC6H_UF16:
    case IMAGE_FORMAT_BC6H_SF16:
    case IMAGE_FORMAT_BC7_UNORM:
        assert(VImage_IsCompressed(format));
        {
            const uint64_t nbw = _VIMAGE_MAX(1u, (width + 3u) / 4u);
            const uint64_t nbh = _VIMAGE_MAX(1u, (height + 3u) / 4u);
            pitch = nbw * 16u;
            slice = pitch * nbh;
        }
        break;
    }
}

uint32_t VImage_GetMemorySize(VImageFormat format, uint32_t w, uint32_t h)
{
    switch (format)
    {
    case IMAGE_FORMAT_R8: return w * h * 1;
    case IMAGE_FORMAT_RG8: return w * h * 2;
    case IMAGE_FORMAT_RGBA8: return w * h * 4;
    case IMAGE_FORMAT_R16: return w * h * 2;
    case IMAGE_FORMAT_RG16: return w * h * 4;
    case IMAGE_FORMAT_RGBA16: return w * h * 8;
    case IMAGE_FORMAT_R16F: return w * h * 2;
    case IMAGE_FORMAT_RG16F: return w * h * 4;
    case IMAGE_FORMAT_RGBA16F: return w * h * 8;
    case IMAGE_FORMAT_R32F: return w * h * 4;
    case IMAGE_FORMAT_RG32F: return w * h * 8;
    case IMAGE_FORMAT_RGBA32F: return w * h * 16;
    case IMAGE_FORMAT_RGB565: return w * h * 2;
    case IMAGE_FORMAT_RGB5A1: return w * h * 2;
    case IMAGE_FORMAT_RGB10A2: return w * h * 4;
    case IMAGE_FORMAT_RG11B10F: return w * h * 4;
    case IMAGE_FORMAT_BC1_UNORM:
        return ((w + 3) / 4) * ((h + 3) / 4) * 8;
    case IMAGE_FORMAT_BC2_UNORM:
    case IMAGE_FORMAT_BC3_UNORM:
    case IMAGE_FORMAT_BC4_UNORM:
    case IMAGE_FORMAT_BC4_SNORM:
    case IMAGE_FORMAT_BC5_UNORM:
    case IMAGE_FORMAT_BC5_SNORM:
    case IMAGE_FORMAT_BC6H_UF16:
    case IMAGE_FORMAT_BC6H_SF16:
    case IMAGE_FORMAT_BC7_UNORM:
        return ((w + 3) / 4) * ((h + 3) / 4) * 16;

    case IMAGE_FORMAT_ASTC_4x4: return ((w + 3) / 4) * ((h + 3) / 4) * 16;
    case IMAGE_FORMAT_ASTC_5x4: return ((w + 4) / 5) * ((h + 3) / 4) * 16;
    case IMAGE_FORMAT_ASTC_5x5: return ((w + 4) / 5) * ((h + 4) / 5) * 16;
    case IMAGE_FORMAT_ASTC_6x5: return ((w + 5) / 6) * ((h + 4) / 5) * 16;
    case IMAGE_FORMAT_ASTC_6x6: return ((w + 5) / 6) * ((h + 5) / 6) * 16;
    case IMAGE_FORMAT_ASTC_8x5: return ((w + 7) / 8) * ((h + 4) / 5) * 16;
    case IMAGE_FORMAT_ASTC_8x6: return ((w + 7) / 8) * ((h + 5) / 6) * 16;
    case IMAGE_FORMAT_ASTC_8x8: return ((w + 7) / 8) * ((h + 7) / 8) * 16;
    case IMAGE_FORMAT_ASTC_10x5: return ((w + 9) / 10) * ((h + 4) / 5) * 16;
    case IMAGE_FORMAT_ASTC_10x6: return ((w + 9) / 10) * ((h + 5) / 6) * 16;
    case IMAGE_FORMAT_ASTC_10x8: return ((w + 9) / 10) * ((h + 7) / 8) * 16;
    case IMAGE_FORMAT_ASTC_10x10: return ((w + 9) / 10) * ((h + 9) / 10) * 16;
    case IMAGE_FORMAT_ASTC_12x10: return ((w + 11) / 12) * ((h + 9) / 10) * 16;
    case IMAGE_FORMAT_ASTC_12x12: return ((w + 11) / 12) * ((h + 11) / 12) * 16;
    default: VIMAGE_UNREACHABLE();
    }
}

VImage* VImage_Create2D(VImageFormat format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevels) {
    if (!VImage_CalculateMipLevels(width, height, &mipLevels)) {
        return NULL;
    }

    VImage* image = (VImage*)malloc(sizeof(VImage));
    assert(image);

    image->dimension = VIMAGE_DIMENSION_TEXTURE2D;
    image->baseWidth = width;
    image->baseHeight = height;
    image->format = format;
    image->depthOrArraySize = arraySize;
    image->mipLevels = mipLevels;
    return image;
}

VImage* VImage_CreateFromMemory(const uint8_t* data, size_t size)
{
    VImage* image = NULL;

    if ((image = VImage_LoadDDS(data, size)) != NULL) return image;
    if ((image = VImage_LoadASTC(data, size)) != NULL) return image;
    if ((image = VImage_LoadKTX1(data, size)) != NULL) return image;
    if ((image = VImage_LoadKTX2(data, size)) != NULL) return image;
    if ((image = VImage_LoadSTB(data, size)) != NULL) return image;

    return NULL;
}

void VImage_Destroy(VImage* image)
{
    if (image)
    {
        free(image);
    }
}

VImageDimension VImage_GetDimension(VImage* image) {
    return image->dimension;
}

uint32_t VImage_GetBaseWidth(VImage* image) {
    return image->baseWidth;
}

uint32_t VImage_GetBaseHeight(VImage* image) {
    return image->baseHeight;
}

uint32_t VImage_GetWidth(VImage* image, uint32_t level) {
    return _VIMAGE_MAX(image->baseWidth >> level, 1);
}

uint32_t VImage_GetHeight(VImage* image, uint32_t level) {
    return _VIMAGE_MAX(image->baseHeight >> level, 1);
}
