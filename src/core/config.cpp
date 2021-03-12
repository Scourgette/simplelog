/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "config.h"

#include <algorithm>
#include "config_parser.h"

using namespace simplelog;

const std::string config::m_defaultTag = "*";
const unordered_casemap<log_level> config::m_logLevelNames = {
    { "0", log_level::panic },   { "p", log_level::panic },   { "panic", log_level::panic },
    { "1", log_level::error },   { "e", log_level::error },   { "error", log_level::error },
    { "2", log_level::warning }, { "w", log_level::warning }, { "warning", log_level::warning },
    { "3", log_level::info },    { "i", log_level::info },    { "info", log_level::info },
    { "4", log_level::debug },   { "d", log_level::debug },   { "debug", log_level::debug },
    { "5", log_level::verbose }, { "v", log_level::verbose }, { "verbose", log_level::verbose },
};

config::config() :
    m_defaultLoggers(true),
    m_async(false),
    m_formatter("Default"),
#ifdef __ANDROID__
    m_loggers({ { "Android", logger{ "Android", "" } } })
#else
    m_loggers({ { "Stdout", logger{ "Stdout", "" } } })
#endif
{}

config & config::get()
{
    static config c;
    return c;
}

void config::setDefaultLoggers(const std::string & loggers_names)
{
    m_tags[m_defaultTag].loggers = splitLoggers(loggers_names);
}

void config::checkTags()
{
    for (auto & t : m_tags) {
        auto & tls = t.second.loggers;
        tls.erase(std::remove_if(tls.begin(), tls.end(),
                                 [&](const std::string & l) {
                                     return m_loggers.find(l) == m_loggers.end();
                                 }),
                  tls.end());
        if (tls.empty() && !m_loggers.empty())
            tls.emplace_back(m_loggers.begin()->first);
    }
}

void config::addLogger(const std::string & name, const std::string & type, const std::string & address)
{
    if (m_defaultLoggers) {
        // If loggers are manually added through API, clear automatic loggers added in constructor.
        m_defaultLoggers = false;
        m_loggers.clear();
    }
    m_loggers[name] = logger{ type, address };
    checkTags();
}

void config::update(std::unique_ptr<std::istream> data)
{
    auto sections = config_parser(std::move(data)).take();
    // General section
    auto section = sections.find("general");
    if (section != sections.end())
        parseGeneral(section->second);
    // Loggers section
    for (const auto & s : { "logger", "loggers" }) {
        section = sections.find(s);
        if (section != sections.end()) {
            parseLoggers(section->second);
            break;
        }
    }
    // Tags section
    for (const auto & s : { "level", "levels", "log_level", "log_levels" }) {
        section = sections.find(s);
        if (section != sections.end()) {
            parseTags(section->second);
            break;
        }
    }
}

void config::parseGeneral(const config_parser::entries & e)
{
    auto entry = e.find("async");
    if (entry != e.end())
        m_async = entry->second[0] == '1' || entry->second[0] == 'T' || entry->second[0] == 't';
    entry = e.find("formatter");
    if (entry != e.end())
        m_formatter = entry->second;
}

void config::parseLoggers(const config_parser::entries & e)
{
    if (!e.empty()) {
        m_defaultLoggers = false;
        m_loggers.clear();
    }
    for (const auto & entry : e) {
        const std::string & name = entry.first;
        logger l;
        splitPair(entry.second, l.type, l.address);
        if (!l.type.empty())
            m_loggers[name] = l;
    }
    checkTags();
}

void config::parseTags(const config_parser::entries & e)
{
    if (!e.empty())
        m_tags.clear();
    for (const auto & entry : e) {
        if (entry.second.empty())
            continue;
        tag t;
        std::string levelStr;
        auto pos = entry.second.find(',');
        levelStr = pos == std::string::npos ? entry.second : entry.second.substr(0, pos);
        parseLevel(levelStr, t.level);
        t.loggers = splitLoggers(entry.second);
        if (t.level != log_level::verbose || !t.loggers.empty())
            m_tags[entry.first] = t;
    }
}

config::loggers_names config::splitLoggers(const std::string & loggers_str) const
{
    // Format: <logger_name>,<logger_name>[,...]
    loggers_names loggers;
    size_t cur = 0, next;
    while ((next = loggers_str.find(',', cur)) != std::string::npos) {
        std::string l = loggers_str.substr(cur, next - cur);
        if (m_loggers.find(l) != m_loggers.end()) // Ignore non-existing loggers
            loggers.push_back(std::move(l));
        cur = next + 1;
    }
    if (cur < loggers_str.size()) {
        std::string l = loggers_str.substr(cur, loggers_str.size() - cur);
        if (m_loggers.find(l) != m_loggers.end()) // Ignore non-existing loggers
            loggers.push_back(l);
    }
    return loggers;
}

void config::splitPair(const std::string & str, std::string & key, std::string & value)
{
    // Format: <key>:<value>
    auto delimiter = str.find(':');
    if (delimiter == 0) {
        key.clear();
        value = str.substr(1, str.size() - 1);
    } else if (delimiter == std::string::npos) {
        key = str;
        value.clear();
    } else {
        key = str.substr(0, delimiter);
        value.clear();
        if (++delimiter < str.size())
            value = str.substr(delimiter, str.size() - delimiter);
    }
}

bool config::parseLevel(const std::string & level_str, log_level & level)
{
    auto lvl = m_logLevelNames.find(level_str);
    if (lvl != m_logLevelNames.end()) {
        level = lvl->second;
        return true;
    } else {
        return false;
    }
}
