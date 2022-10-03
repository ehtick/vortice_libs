// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "VorticeImage.h"

 /* Compiler defines */
#if defined(__clang__)
#    define VIMAGE_UNREACHABLE() __builtin_unreachable()
#    define VIMAGE_DEBUG_BREAK() __builtin_trap()
#elif defined(__GNUC__)
#    define VIMAGE_UNREACHABLE() __builtin_unreachable()
#    define VIMAGE_DEBUG_BREAK() __builtin_trap()
#elif defined(_MSC_VER)
#    define VIMAGE_UNREACHABLE() __assume(false)
#    define VIMAGE_DEBUG_BREAK() __debugbreak()
#endif

#define _VIMAGE_MIN(a,b) (((a)<(b))?(a):(b))
#define _VIMAGE_MAX(a,b) (((a)>(b))?(a):(b))
