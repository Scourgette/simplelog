/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "default_formatter.h"

#include <iostream>
#include <string.h>

using namespace simplelog;

default_formatter_factory default_formatter_factory::instance;

void default_formatter::formatPrefix(std::mutex & mutex, const log_metadata & metadata,
                                     memory_buffer & formatted)
{
    formatted.push_back('[');
    formatted.push_back(logLevelToChar(metadata.level));
    formatted.push_back(']');

    {
        std::lock_guard<std::mutex> lock(mutex);
        if (shouldUpdateSeconds(metadata)) {
            m_seconds.clear();
            m_seconds.push_back('[');
            appendDecimal(m_seconds, metadata.year, 4);
            m_seconds.push_back('-');
            appendDecimal(m_seconds, metadata.month, 2);
            m_seconds.push_back('-');
            appendDecimal(m_seconds, metadata.day, 2);
            m_seconds.push_back(' ');
            appendDecimal(m_seconds, metadata.hour, 2);
            m_seconds.push_back(':');
            appendDecimal(m_seconds, metadata.minute, 2);
            m_seconds.push_back(':');
            appendDecimal(m_seconds, metadata.second, 2);
            m_seconds.push_back('.');
        }
        if (shouldUpdateMilliseconds(metadata)) {
            m_milliseconds.clear();
            appendDecimal(m_milliseconds, metadata.millisecond, 3);
            m_milliseconds.push_back(']');
        }
        append(formatted, m_seconds);
        append(formatted, m_milliseconds);
    }

    formatted.push_back('[');
    appendDecimal(formatted, metadata.tid);
    formatted.push_back(']');
    formatted.push_back('[');
    formatted.append(metadata.tag, metadata.tag + strlen(metadata.tag));
    formatted.push_back(']');
    formatted.push_back(' ');
}

void default_formatter::format(std::mutex & mutex, const log_metadata & metadata,
                               memory_buffer & formatted, const char * msg, va_list args)
{
    formatPrefix(mutex, metadata, formatted);
    std::array<char, LOG_MAX_LINE_LENGTH> buf;
    int len = vsnprintf(buf.data(), buf.size(), msg, args);
    if (len >= 0)
        formatted.append(buf.data(), buf.data() + len);
}

void default_formatter::format(std::mutex & mutex, const log_metadata & metadata, string_view msg,
                               memory_buffer & formatted)
{
    formatPrefix(mutex, metadata, formatted);
    formatted.append(msg.data(), msg.data() + msg.size());
}

char default_formatter::logLevelToChar(log_level level) const
{
    switch (level) {
        case log_level::verbose: return 'V';
        case log_level::debug: return 'D';
        case log_level::info: return 'I';
        case log_level::warning: return 'W';
        case log_level::error: return 'E';
        case log_level::panic: return 'P';
        default: return 'X';
    }
}
