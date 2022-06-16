// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#if defined(_WIN32)
#   define VMA_CALL_PRE  __declspec(dllexport)
#   define VMA_CALL_POST __cdecl
#elif defined(__ANDROID__)
#   define VMA_CALL_PRE __attribute__((visibility("default")))
#endif

#define VMA_IMPLEMENTATION
//#define VMA_STATIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"
