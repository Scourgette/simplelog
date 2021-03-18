/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_CONFIG
#define SIMPLELOG_CONFIG

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "casecmp.h"
#include "config_parser.h"
#include "log_metadata.h"

namespace simplelog {

// Simplelog configuration
class config
{
public:
    struct logger
    {
        std::string type;
        std::string address;
    };
    using loggers_names = std::vector<std::string>;
    struct tag
    {
        log_level level;
        loggers_names loggers;
        tag() : level(log_level::verbose) {}
    };

    static config & get();

    // Setters
    void update(std::unique_ptr<std::istream> data);
    void setDefaultLevel(log_level level) { m_tags[m_defaultTag].level = level; }
    void setDefaultLoggers(const std::string & loggers_names);
    void setAsync(bool async) { m_async = async; }
    void setFormatter(const std::string & formatter) { m_formatter = formatter; }
    void addLogger(const std::string & name, const std::string & type, const std::string & address);

    // Getters
    static const std::string & defaultTag() { return m_defaultTag; }
    bool async() const { return m_async; }
    const std::string & formatter() const { return m_formatter; }
    const unordered_casemap<logger> & loggers() const { return m_loggers; }
    const unordered_casemap<tag> & tags() const { return m_tags; }

    // Utility to split a string into several registered loggers
    loggers_names splitLoggers(const std::string & str) const;

private:
    config();
    config(const config &) = delete;
    config & operator=(const config &) = delete;

    void parseGeneral(const config_parser::entries & e);
    void parseLoggers(const config_parser::entries & e);
    void parseTags(const config_parser::entries & e);
    void checkTags();

    static void splitPair(const std::string & str, std::string & key, std::string & value);
    static bool parseLevel(const std::string & level_str, log_level & level);

    bool m_defaultLoggers;
    bool m_async;
    std::string m_formatter;
    unordered_casemap<logger> m_loggers;
    unordered_casemap<tag> m_tags;

    static const std::string m_defaultTag;
    static const unordered_casemap<log_level> m_logLevelNames;
};

} // namespace simplelog

#endif
