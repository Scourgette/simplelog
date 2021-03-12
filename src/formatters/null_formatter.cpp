/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "null_formatter.h"

#include <iostream>
#include <string.h>

using namespace simplelog;

null_formatter_factory null_formatter_factory::instance;

void null_formatter::format(std::mutex & /*mutex*/, const log_metadata & /*metadata*/,
                            memory_buffer & formatted, const char * msg, va_list args)
{
    std::array<char, LOG_MAX_LINE_LENGTH> buf;
    int len = vsnprintf(buf.data(), buf.size(), msg, args);
    if (len >= 0)
        formatted.append(buf.data(), buf.data() + len);
}

void null_formatter::format(std::mutex & /*mutex*/, const log_metadata & /*metadata*/,
                            string_view msg, memory_buffer & formatted)
{
    formatted.append(msg.data(), msg.data() + msg.size());
}
