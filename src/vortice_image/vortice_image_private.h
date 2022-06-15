// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "vortice_image.h"

 /* Compiler defines */
#if defined(__clang__)
#    define UNREACHABLE() __builtin_unreachable()
#    define DEBUG_BREAK() __builtin_trap()
#elif defined(__GNUC__)
#    define UNREACHABLE() __builtin_unreachable()
#    define DEBUG_BREAK() __builtin_trap()
#elif defined(_MSC_VER)
#    define UNREACHABLE() __assume(false)
#    define DEBUG_BREAK() __debugbreak()
#endif
