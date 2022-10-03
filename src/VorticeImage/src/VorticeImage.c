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
    VImageFormat format = IMAGE_FORMAT_RGBA8_UNORM;
    int width, height, channels;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        stb_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels) {
        case 1: format = IMAGE_FORMAT_R16_UNORM; break;
        case 2: format = IMAGE_FORMAT_RG16_UNORM; break;
        case 4: format = IMAGE_FORMAT_RGBA16_UNORM; break;
        default:
            //assert(FALSE && "Unsupported channel count for 16 bit image: %d", channels);
            VIMAGE_UNREACHABLE();
        }
    }
    else if (stbi_is_hdr_from_memory(data, (int)size))
    {
        stb_data = stbi_loadf_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA32_FLOAT;
    }
    else {
        stb_data = stbi_load_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA8_UNORM;
    }

    if (!stb_data) {
        return NULL;
    }

    VImage* image = VImage_Create2D(format, width, height, 1, 1);
    //image->data = stb_data;
    //image->size = memorySize;
    //image->mipmaps[0].data = stb_data;
    //image->mipmaps[0].size = memorySize;
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

uint32_t VImage_BitsPerPixel(VImageFormat format) {
    switch (format)
    {
    case IMAGE_FORMAT_RGBA32_UINT:
    case IMAGE_FORMAT_RGBA32_SINT:
    case IMAGE_FORMAT_RGBA32_FLOAT:
        return 128;

    //case IMAGE_FORMAT_RGB32_UINT:
    //case IMAGE_FORMAT_RGB32_SINT:
    //case IMAGE_FORMAT_RGB32_FLOAT:
    //    return 96;

    case IMAGE_FORMAT_RG32_UINT:
    case IMAGE_FORMAT_RG32_SINT:
    case IMAGE_FORMAT_RG32_FLOAT:
    case IMAGE_FORMAT_RGBA16_UNORM:
    case IMAGE_FORMAT_RGBA16_SNORM:
    case IMAGE_FORMAT_RGBA16_UINT:
    case IMAGE_FORMAT_RGBA16_SINT:
    case IMAGE_FORMAT_RGBA16_FLOAT:
    case IMAGE_FORMAT_D32_FLOAT_S8:
    //case IMAGE_FORMAT_Y416:
    //case IMAGE_FORMAT_Y210:
    //case IMAGE_FORMAT_Y216:
        return 64;

    case IMAGE_FORMAT_RGB10A2_UNORM:
    case IMAGE_FORMAT_RGB10A2_UINT:
    case IMAGE_FORMAT_RG11B10_FLOAT:
    case IMAGE_FORMAT_RGBA8_UNORM:
    case IMAGE_FORMAT_RGBA8_SNORM:
    case IMAGE_FORMAT_RGBA8_UINT:
    case IMAGE_FORMAT_RGBA8_SINT:
    case IMAGE_FORMAT_RG16_FLOAT:
    case IMAGE_FORMAT_RG16_UNORM:
    case IMAGE_FORMAT_RG16_UINT:
    case IMAGE_FORMAT_RG16_SNORM:
    case IMAGE_FORMAT_RG16_SINT:
    case IMAGE_FORMAT_D32_FLOAT:
    case IMAGE_FORMAT_R32_FLOAT:
    case IMAGE_FORMAT_R32_UINT:
    case IMAGE_FORMAT_R32_SINT:
    case IMAGE_FORMAT_D24_UNORM_S8:
    case IMAGE_FORMAT_RGB9E5_FLOAT:
    case IMAGE_FORMAT_BGRA8_UNORM:
    //case IMAGE_FORMAT_AYUV:
    //case IMAGE_FORMAT_Y410:
    //case IMAGE_FORMAT_YUY2:
    //case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    //case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
    //case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        return 32;

    //case IMAGE_FORMAT_P010:
    //case IMAGE_FORMAT_P016:
    //case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    //case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    //case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
    //case WIN10_DXGI_FORMAT_V408:
    //    return 24;

    case IMAGE_FORMAT_RG8_UNORM:
    case IMAGE_FORMAT_RG8_SNORM:
    case IMAGE_FORMAT_RG8_UINT:
    case IMAGE_FORMAT_RG8_SINT:
    case IMAGE_FORMAT_R16_FLOAT:
    case IMAGE_FORMAT_D16_UNORM:
    case IMAGE_FORMAT_R16_UNORM:
    case IMAGE_FORMAT_R16_UINT:
    case IMAGE_FORMAT_R16_SNORM:
    case IMAGE_FORMAT_R16_SINT:
    //case IMAGE_FORMAT_B5G6R5_UNORM:
    //case IMAGE_FORMAT_B5G5R5A1_UNORM:
    //case IMAGE_FORMAT_A8P8:
    //case IMAGE_FORMAT_FORMAT_B4G4R4A4_UNORM:
    //case WIN10_DXGI_FORMAT_P208:
    //case WIN10_DXGI_FORMAT_V208:
        return 16;

    //case DXGI_FORMAT_NV12:
    //case DXGI_FORMAT_420_OPAQUE:
    //case DXGI_FORMAT_NV11:
    //    return 12;

    case IMAGE_FORMAT_R8_UNORM:
    case IMAGE_FORMAT_R8_SNORM:
    case IMAGE_FORMAT_R8_UINT:
    case IMAGE_FORMAT_R8_SINT:
    //case IMAGE_FORMAT_A8_UNORM:
    case IMAGE_FORMAT_BC2_UNORM:
    case IMAGE_FORMAT_BC3_UNORM:
    case IMAGE_FORMAT_BC5_UNORM:
    case IMAGE_FORMAT_BC5_SNORM:
    case IMAGE_FORMAT_BC6H_UF16:
    case IMAGE_FORMAT_BC6H_SF16:
    case IMAGE_FORMAT_BC7_UNORM:
    //case IMAGE_FORMAT_BC7_UNORM_SRGB:
    //case IMAGE_FORMAT_AI44:
    //case IMAGE_FORMAT_IA44:
    //case IMAGE_FORMAT_P8:
    //case XBOX_DXGI_FORMAT_R4G4_UNORM:
        return 8;

    //case IMAGE_FORMAT_R1_UNORM:
    //    return 1;

    case IMAGE_FORMAT_BC1_UNORM:
    case IMAGE_FORMAT_BC4_UNORM:
    case IMAGE_FORMAT_BC4_SNORM:
        return 4;
    default:
        return 0;
    }
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

#if TODO
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_YUY2:
        assert(IsPacked(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 4u;
        slice = pitch * uint64_t(height);
        break;

    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        assert(IsPacked(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 8u;
        slice = pitch * uint64_t(height);
        break;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 2u;
        slice = pitch * (uint64_t(height) + ((uint64_t(height) + 1u) >> 1));
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case XBOX_DXGI_FORMAT_D16_UNORM_S8_UINT:
    case XBOX_DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case XBOX_DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 4u;
        slice = pitch * (uint64_t(height) + ((uint64_t(height) + 1u) >> 1));
        break;

    case DXGI_FORMAT_NV11:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 3u) >> 2) * 4u;
        slice = pitch * uint64_t(height) * 2u;
        break;

    case WIN10_DXGI_FORMAT_P208:
        assert(IsPlanar(fmt));
        pitch = ((uint64_t(width) + 1u) >> 1) * 2u;
        slice = pitch * uint64_t(height) * 2u;
        break;

    case WIN10_DXGI_FORMAT_V208:
        assert(IsPlanar(fmt));
        pitch = uint64_t(width);
        slice = pitch * (uint64_t(height) + (((uint64_t(height) + 1u) >> 1) * 2u));
        break;

    case WIN10_DXGI_FORMAT_V408:
        assert(IsPlanar(fmt));
        pitch = uint64_t(width);
        slice = pitch * (uint64_t(height) + (uint64_t(height >> 1) * 4u));
        break;
#endif // TODO

    default:
        //assert(!VImage_IsCompressed(format) && !IsPacked(format) && !IsPlanar(format));
        assert(!VImage_IsCompressed(format));
        {
            uint32_t bpp = VImage_BitsPerPixel(format);
            // Default byte alignment
            pitch = (width * bpp + 7u) / 8u;
            slice = pitch * height;
        }
        break;
    }

    *rowPitch = pitch;
    *slicePitch = slice;
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
    // TODO
    //image->data = stb_data;
    //image->size = memorySize;
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


uint32_t VImage_GetDXGIFormat(VImageFormat format) {
    return 0;
}
