#pragma once

#include "ic_log.h"

#define IC_ASSERT(x, ...)                                                                                              \
    {                                                                                                                  \
        if (!(x)) {                                                                                                    \
            IC_APP_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                        \
        }                                                                                                              \
    }
#define IC_CORE_ASSERT(x, ...)                                                                                         \
    {                                                                                                                  \
        if (!(x)) {                                                                                                    \
            IC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                       \
        }                                                                                                              \
    }
