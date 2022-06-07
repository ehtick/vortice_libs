// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "vortice_image.h"
#include <ktx.h>

VImage* vimage_load_ktx(const uint8_t* data, size_t size)
{
    ktxTexture* newTexture = nullptr;
    KTX_error_code result = ktxTexture_CreateFromMemory(
        data,
        size,
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &newTexture
    );

    return (VImage*)newTexture;
}
