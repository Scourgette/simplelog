/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_ICONSUMER
#define SIMPLELOG_ICONSUMER

#include <stdio.h>
#include "log_metadata.h"

namespace simplelog {

class iconsumer
{
public:
    virtual ~iconsumer() = default;
    virtual void consume(log_level level, const char * msg, size_t len) = 0;
    virtual void flush() = 0;
};

} // namespace simplelog

#endif
