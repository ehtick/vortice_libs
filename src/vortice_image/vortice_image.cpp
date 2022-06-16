// Copyright � Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "vortice_image_private.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <float.h> // FLT_MAX
#include <assert.h>

// TODO: Use WIC on windows platform

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_JPEG
//#define STBI_ONLY_PNG
#include <stb_image.h>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>

#include <ktx.h>

struct Mipmap {
    void* data;
    size_t size;
    size_t stride;
};

struct Image
{
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t layers;
    uint32_t levels;
    void* data;
    size_t size;
    Mipmap mipmaps[1];
};

static Image* loadDDS(const uint8_t* data, size_t size)
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
        return nullptr;

    // Magic
    size_t length = size;
    uint32_t* magic = (uint32_t*)data;
    if (*magic != 0x20534444)
        return nullptr;
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
}

static Image* loadASTC(const uint8_t* data, size_t size)
{
    return nullptr;
}

static Image* loadKTX1(const uint8_t* data, size_t size)
{
    return nullptr;
}

static Image* loadKTX2(const uint8_t* data, size_t size)
{
    return nullptr;
}

static Image* loadSTB(const uint8_t* data, size_t size)
{
    void* stb_data;
    ImageFormat format;
    int width, height, channels;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        stb_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels) {
            case 1: format = IMAGE_FORMAT_R16; break;
            case 2: format = IMAGE_FORMAT_RG16; break;
            case 4: format = IMAGE_FORMAT_RGBA16; break;
            default:
                assert(FALSE && "Unsupported channel count for 16 bit image: %d", channels);
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

    uint32_t memorySize = vimageGetMemorySize(format, width, height);

    Image* image = new Image();
    image->width = width;
    image->height = height;
    image->format = format;
    image->layers = 1;
    image->levels = 1;
    image->data = stb_data;
    image->size = memorySize;
    image->mipmaps[0].data = stb_data;
    image->mipmaps[0].size = memorySize;
    return image;
}

uint32_t vimageGetMemorySize(ImageFormat format, uint32_t w, uint32_t h)
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
        case IMAGE_FORMAT_BC1: return ((w + 3) / 4) * ((h + 3) / 4) * 8;
        case IMAGE_FORMAT_BC2:
        case IMAGE_FORMAT_BC3:
        case IMAGE_FORMAT_BC4U:
        case IMAGE_FORMAT_BC4S:
        case IMAGE_FORMAT_BC5U:
        case IMAGE_FORMAT_BC5S:
        case IMAGE_FORMAT_BC6UF:
        case IMAGE_FORMAT_BC6SF:
        case IMAGE_FORMAT_BC7: return ((w + 3) / 4) * ((h + 3) / 4) * 16;
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
        default: UNREACHABLE();
    }
}

Image* vimageCreateFromMemory(const uint8_t* data, size_t size)
{
    Image* image = NULL;

    if ((image = loadDDS(data, size)) != NULL) return image;
    if ((image = loadASTC(data, size)) != NULL) return image;
    if ((image = loadKTX1(data, size)) != NULL) return image;
    if ((image = loadKTX2(data, size)) != NULL) return image;
    if ((image = loadSTB(data, size)) != NULL) return image;

    return nullptr;
}

#ifdef __cplusplus
extern "C" {
#endif

    VGPU_IMAGE_API ktxTexture* ktx_load_from_memory(const uint8_t* data, uint32_t size, KTX_error_code* out_status) {

        KTX_error_code result;

        ktxTexture* newTex = 0;

        result = ktxTexture_CreateFromMemory(
            (const ktx_uint8_t*)data,
            (ktx_size_t)size,
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &newTex
        );

        *out_status = result;
        return newTex;
    }

    VGPU_IMAGE_API class_id ktxTexture_get_classId(ktxTexture* ktx) {
        return ktx->classId;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_get_isArray(ktxTexture* ktx) {
        return ktx->isArray;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_get_isCubemap(ktxTexture* ktx)
    {
        return ktx->isCubemap;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_get_isCompressed(ktxTexture* ktx)
    {
        return ktx->isCompressed;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_get_IsPremultiplied(ktxTexture* texture)
    {
        return (texture->classId == ktxTexture2_c
            && ktxTexture2_GetPremultipliedAlpha((ktxTexture2*)texture));
    }

    VGPU_IMAGE_API uint32_t ktx_get_NumComponents(ktxTexture* texture)
    {
        if (texture->classId != ktxTexture2_c)
        {
            return 0;
        }

        return ktxTexture2_GetNumComponents((ktxTexture2*)texture);
    }

    VGPU_IMAGE_API uint32_t ktx_get_baseWidth(ktxTexture* ktx)
    {
        return ktx->baseWidth;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_baseHeight(ktxTexture* ktx)
    {
        return ktx->baseHeight;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_numDimensions(ktxTexture* ktx)
    {
        return ktx->numDimensions;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_numLevels(ktxTexture* ktx)
    {
        return ktx->numLevels;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_numLayers(ktxTexture* ktx)
    {
        return ktx->numLayers;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_numFaces(ktxTexture* ktx)
    {
        return ktx->numFaces;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_vkFormat(ktxTexture* ktx)
    {
        if (ktx->classId != ktxTexture2_c) {
            return 0; // VK_FORMAT_UNDEFINED
        }
        return ((ktxTexture2*)ktx)->vkFormat;
    }

    VGPU_IMAGE_API ktxSupercmpScheme ktx_get_supercompressionScheme(ktxTexture* ktx)
    {
        if (ktx->classId != ktxTexture2_c) {
            return KTX_SS_NONE;
        }
        return ((ktxTexture2*)ktx)->supercompressionScheme;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_orientation(ktxTexture* ktx)
    {
        ktx_uint32_t orientation = 0;
        if (ktx->orientation.x == KTX_ORIENT_X_LEFT) {
            orientation |= 0x1;
        }
        if (ktx->orientation.y == KTX_ORIENT_Y_UP) {
            orientation |= 0x2;
        }
        if (ktx->orientation.z == KTX_ORIENT_Z_IN) {
            orientation |= 0x4;
        }
        return orientation;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_NeedsTranscoding(ktxTexture* ktx)
    {
        return ktxTexture_NeedsTranscoding(ktx);
    }

    VGPU_IMAGE_API ktx_error_code_e ktx_TranscodeBasis(ktxTexture* texture, ktx_transcode_fmt_e fmt, ktx_transcode_flags transcodeFlags)
    {
        if (texture->classId != ktxTexture2_c)
        {
            //std::cout << "ERROR: transcodeBasis is only supported for KTX2" << std::endl;
            return KTX_INVALID_VALUE;
        }

        return ktxTexture2_TranscodeBasis((ktxTexture2*)texture, fmt, transcodeFlags);
    }

    VGPU_IMAGE_API void ktx_get_data(ktxTexture* ktx, const uint8_t** data, uint32_t* length)
    {
        *data = ktx->pData;
        *length = (uint32_t)ktx->dataSize;
    }

    VGPU_IMAGE_API void ktx_destroy_texture(ktxTexture* ktx) {
        ktxTexture_Destroy(ktx);
    }

#ifdef __cplusplus
}
#endif

