/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_FORMATTER
#define SIMPLELOG_FORMATTER

#include <fmt/format.h>
#include <math.h>
#include <memory>
#include <mutex>
#include <string>

#include "casecmp.h"
#include "log_metadata.h"

namespace simplelog {

using memory_buffer = fmt::basic_memory_buffer<char, LOG_MAX_LINE_LENGTH>;
using string_view = fmt::basic_string_view<char>;

class iformatter
{
public:
    iformatter() :
        m_year(0), m_month(0), m_day(0), m_hour(0), m_minute(0), m_second(0), m_millisecond(0)
    {}
    virtual ~iformatter() = default;
    virtual void format(std::mutex & mutex, const log_metadata & metadata,
                        memory_buffer & formatted, const char * msg, va_list args) = 0;
    virtual void format(std::mutex & mutex, const log_metadata & metadata, string_view msg,
                        memory_buffer & formatted) = 0;

protected:
    template<typename T> void appendDecimal(memory_buffer & buffer, T decimal, int padding = -1)
    {
        fmt::format_int i(decimal);
        for (int i = padding - 1; i > 0 && decimal < pow(10, i); i--)
            buffer.push_back('0');
        buffer.append(i.data(), i.data() + i.size());
    }
    void append(memory_buffer & buffer, memory_buffer & str)
    {
        string_view view(str.data());
        buffer.append(view.data(), view.data() + view.size());
    }
    bool shouldUpdateSeconds(const log_metadata & metadata)
    {
        if (metadata.hour == m_hour && metadata.minute == m_minute && metadata.second == m_second)
            return false;
        m_hour = metadata.hour;
        m_minute = metadata.minute;
        m_second = metadata.second;
        return true;
    }
    bool shouldUpdateMilliseconds(const log_metadata & metadata)
    {
        if (metadata.millisecond == m_millisecond)
            return false;
        m_millisecond = metadata.millisecond;
        return true;
    }

private:
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
    int m_millisecond;
};

class formatter_factory
{
public:
    virtual ~formatter_factory() = default;

    static std::shared_ptr<iformatter> get();
    static std::shared_ptr<iformatter> get(const std::string & name);

protected:
    formatter_factory(const std::string & name);
    formatter_factory(const formatter_factory &) = delete;
    formatter_factory & operator=(const formatter_factory &) = delete;

    virtual std::shared_ptr<iformatter> getformatter() = 0;

private:
    static unordered_casemap<formatter_factory *> & factories();
};

} // namespace simplelog

#endif
