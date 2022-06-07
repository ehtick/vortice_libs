// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "vortice_image.h"
#include <ktx.h>

#ifdef __cplusplus
extern "C" {
#endif

    VGPU_IMAGE_API ktxTexture* ktx_load_from_memory(const uint8_t* data, uint32_t length, KTX_error_code* out_status) {

        KTX_error_code result;

        ktxTexture* newTex = 0;

        result = ktxTexture_CreateFromMemory(
            (const ktx_uint8_t*)data,
            (ktx_size_t)length,
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

    VGPU_IMAGE_API ktx_bool_t ktx_get_isCubemap(ktxTexture* ktx) {
        return ktx->isCubemap;
    }

    VGPU_IMAGE_API ktx_bool_t ktx_get_isCompressed(ktxTexture* ktx) {
        return ktx->isCompressed;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_baseWidth(ktxTexture* ktx) {
        return ktx->baseWidth;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_baseHeight(ktxTexture* ktx) {
        return ktx->baseHeight;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_numDimensions(ktxTexture* ktx) {
        return ktx->numDimensions;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_numLevels(ktxTexture* ktx) {
        return ktx->numLevels;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_numLayers(ktxTexture* ktx) {
        return ktx->numLayers;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_numFaces(ktxTexture* ktx) {
        return ktx->numFaces;
    }
    VGPU_IMAGE_API ktx_uint32_t ktx_get_vkFormat(ktxTexture* ktx) {
        if (ktx->classId != ktxTexture2_c) {
            return 0; // VK_FORMAT_UNDEFINED
        }
        return ((ktxTexture2*)ktx)->vkFormat;
    }
    VGPU_IMAGE_API ktxSupercmpScheme ktx_get_supercompressionScheme(ktxTexture* ktx) {
        if (ktx->classId != ktxTexture2_c) {
            return KTX_SS_NONE;
        }
        return ((ktxTexture2*)ktx)->supercompressionScheme;
    }

    VGPU_IMAGE_API ktx_uint32_t ktx_get_orientation(ktxTexture* ktx) {
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

    VGPU_IMAGE_API ktx_uint32_t ktx_transcode(ktxTexture2* ktx, ktx_transcode_fmt_e fmt, ktx_transcode_flags transcodeFlags) {
        return ktxTexture2_TranscodeBasis(ktx, fmt, transcodeFlags);
    }

    VGPU_IMAGE_API void ktx_get_data(ktxTexture* ktx, const uint8_t** data, size_t* length) {
        *data = ktx->pData;
        *length = (size_t)ktx->dataSize;
    }

    VGPU_IMAGE_API void ktx_destroy_texture(ktxTexture* ktx) {
        ktxTexture_Destroy(ktx);
    }

#ifdef __cplusplus
}
#endif
