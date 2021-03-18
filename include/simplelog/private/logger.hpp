/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_LOGGER_HPP
#define SIMPLELOG_LOGGER_HPP

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <fmt/format.h>
#include <mutex>
#include <thread>
#include <vector>

#include "casecmp.h"
#include "formatter.h"
#include "log_metadata.h"
#include "os.h"

namespace simplelog {

class logger
{
public:
    logger(const std::string & tag) : logger(tag, log_level::verbose, formatter_factory::get()) {}
    logger(const std::string & tag, log_level level, const std::shared_ptr<iformatter> & f) :
        m_tag(tag),
        m_level(level),
        m_formatter(f),
        m_year(0),
        m_month(0),
        m_day(0),
        m_hour(0),
        m_minute(0),
        m_second(0),
        m_millisecond(0)
    {}
    virtual ~logger() = default;

    virtual void flush() {}
    virtual void logRaw(log_level level, const char * msg, size_t len) = 0;

    template<typename... Args>
    void log(log_level level, const char * filename, const char * funcname, int line,
             string_view msg, Args &&... args)
    {
        if (level > m_level)
            return;
        memory_buffer buf;
        format_to(buf, msg, std::forward<Args>(args)...);
        memory_buffer formatted;
        m_formatter->format(m_mutexFormat, getMetadata(level, filename, funcname, line),
                            string_view(buf.begin(), buf.size()), formatted);
        formatted.append(os::getEol(), os::getEol() + strlen(os::getEol()));
        std::lock_guard<std::mutex> lock(m_mutexlogger);
        logRaw(level, formatted.begin(), formatted.size());
    }

    void log(log_level level, const char * filename, const char * funcname, int line,
             const char * msg, va_list args)
    {
        if (level > m_level)
            return;
        memory_buffer formatted;
        m_formatter->format(m_mutexFormat, getMetadata(level, filename, funcname, line), formatted,
                            msg, args);
        formatted.append(os::getEol(), os::getEol() + strlen(os::getEol()));
        std::lock_guard<std::mutex> lock(m_mutexlogger);
        logRaw(level, formatted.begin(), formatted.size());
    }

private:
    log_metadata getMetadata(log_level level, const char * filename, const char * funcname, int line)
    {
        updateTimestamp();
        return log_metadata{ m_tag.c_str(), level,    os::getThreadId(), filename, funcname,
                             line,          m_year,   m_month,           m_day,    m_hour,
                             m_minute,      m_second, m_millisecond };
    }

    void updateTimestamp()
    {
        // Use a cache on seconds and milliseconds levels to improve efficiency
        auto now = std::chrono::system_clock::now();
        if (now != m_lastTimestamp) {
            using namespace std::chrono;
            seconds secs = duration_cast<seconds>(now.time_since_epoch());
            if (m_lastSeconds != secs) {
                time_t tt = system_clock::to_time_t(now);
                tm timestamp;
#if defined(_WIN32)
                localtime_s(&timestamp, &tt);
#else
                localtime_r(&tt, &timestamp);
#endif
                m_lastSeconds = secs;
                m_year = timestamp.tm_year + 1900;
                m_month = timestamp.tm_mon + 1;
                m_day = timestamp.tm_mday;
                m_hour = timestamp.tm_hour;
                m_minute = timestamp.tm_min;
                m_second = timestamp.tm_sec;
            }
            m_lastTimestamp = now;
            milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch());
            m_millisecond = ms.count() % 1000;
        }
    }

    const std::string m_tag;
    log_level m_level;
    std::shared_ptr<iformatter> m_formatter;
    std::chrono::system_clock::time_point m_lastTimestamp;
    std::chrono::seconds m_lastSeconds;
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
    int m_millisecond;
    std::mutex m_mutexFormat;
    std::mutex m_mutexlogger;
};

class logger_factory
{
public:
    virtual ~logger_factory() = default;

    static void registerLogger(const std::string & name, std::string type, std::string address);

    static std::vector<std::shared_ptr<logger>> get(const std::string & tag,
                                                    const std::vector<std::string> & names);
    static std::vector<std::shared_ptr<logger>> get(const std::string & tag);

protected:
    logger_factory(const std::string & type);
    logger_factory(const logger_factory &) = delete;
    logger_factory & operator=(const logger_factory &) = delete;

    virtual std::shared_ptr<logger> getLogger(const std::string & tag,
                                              const std::string & address = nullptr) = 0;

private:
    struct instance
    {
        std::string type;
        std::string address;
    };
    static unordered_casemap<logger_factory *> & factories();
    static unordered_casemap<instance> & instances();
    static unordered_casemap<std::shared_ptr<logger>> & opened();
};

} // namespace simplelog

#endif
