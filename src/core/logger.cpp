/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "logger.h"

#include <fstream>
#include <memory>
#include <mutex>
#include <unordered_set>

#include "config.h"
#include "logger_engine.h"

using namespace simplelog;

namespace {
std::mutex & engineMutex()
{
    static std::mutex mutex;
    return mutex;
}
std::unordered_set<std::shared_ptr<logger>> & loggers()
{
    static std::unordered_set<std::shared_ptr<logger>> list;
    return list;
}
void initConfig()
{
    static bool initialized = false;
    if (!initialized) {
        const char * env_path = reinterpret_cast<const char *>(getenv("SIMPLELOG_CONFIG_INI"));
        for (const char * path : { env_path, LOG_CONFIG_INI, "./simplelog.ini", "~/.simplelog.ini",
                                   "/etc/simplelog.ini" }) {
            if (!path || path[0] == '\0')
                continue;
            auto stream = std::make_unique<std::ifstream>(path);
            if (stream->good()) {
                config::get().update(std::move(stream));
                break;
            }
        }
        initialized = true;
    }
}
void initLoggers()
{
    static bool initialized = false;
    if (!initialized) {
        const auto & ls = config::get().loggers();
        for (const auto & l : ls)
            logger_factory::registerLogger(l.first, l.second.type, l.second.address);
        initialized = true;
    }
}
} // namespace

extern "C" void _simplelog_config_path(const char * path)
{
    auto stream = std::make_unique<std::ifstream>(path);
    if (stream->good())
        config::get().update(std::move(stream));
}

extern "C" void _simplelog_register_logger(const char * name, const char * type, const char * address)
{
    if (!name || !type)
        return;
    config::get().addLogger(name, type, address ? address : "");
    logger_factory::registerLogger(name, type, address ? address : "");
}

extern "C" void _simplelog_default_loggers(const char * loggers_names)
{
    config::get().setDefaultLoggers(loggers_names);
}

extern "C" void _simplelog_default_async_logging(int async) { config::get().setAsync(async != 0); }

extern "C" void _simplelog_default_log_level(int level)
{
    config::get().setDefaultLevel(log_level(level));
}

extern "C" void * _simplelog_create(const char * tag, const char * loggers_names)
{
    std::lock_guard<std::mutex> lock(engineMutex());
    initConfig();
    log_level level = log_level::verbose;
    // Init formatter
    auto f = formatter_factory::get(config::get().formatter());
    if (f == nullptr)
        f = formatter_factory::get();
    // Init loggers
    initLoggers();
    std::string names = loggers_names == nullptr ? std::string() : loggers_names;
    auto ls = logger_factory::get(tag, config::get().splitLoggers(names));
    // Tag configuration
    auto ts = config::get().tags();
    auto t = ts.find(std::string(tag));
    if (t == ts.end())
        t = ts.find(config::defaultTag());
    if (t != ts.end()) {
        if (!t->second.loggers.empty())
            ls = logger_factory::get(tag, t->second.loggers);
        level = t->second.level;
    }
    // Create engine
    auto engine = std::make_shared<logger_engine>(tag, level, f, std::move(ls));
    if (config::get().async())
        engine->setAsync();
    loggers().insert(engine);
    return engine.get();
}

extern "C" void _simplelog_log(void * thiz, int prio, const char * filename, const char * funcname,
                               int line, const char * msg, ...)
{
    if (thiz == nullptr)
        return;
    va_list args;
    va_start(args, msg);
    reinterpret_cast<logger *>(thiz)->log(log_level(prio), filename, funcname, line, msg, args);
    va_end(args);
}

extern "C" void _simplelog_flush(void * thiz)
{
    if (thiz == nullptr)
        return;
    reinterpret_cast<logger *>(thiz)->flush();
}

logger_factory::logger_factory(const std::string & type) { factories()[type] = this; }

unordered_casemap<logger_factory *> & logger_factory::factories()
{
    static unordered_casemap<logger_factory *> ret;
    return ret;
}

unordered_casemap<logger_factory::instance> & logger_factory::instances()
{
    static unordered_casemap<logger_factory::instance> ret;
    return ret;
}

unordered_casemap<std::shared_ptr<logger>> & logger_factory::opened()
{
    static unordered_casemap<std::shared_ptr<logger>> ret;
    return ret;
}

void logger_factory::registerLogger(const std::string & name, std::string type, std::string address)
{
    // avoid duplicates
    auto & is = instances();
    for (const auto & i : is) {
        if (i.second.type == type && i.second.address == address)
            return;
    }
    is[name] = instance{ std::move(type), std::move(address) };
}

std::vector<std::shared_ptr<logger>> logger_factory::get(const char * tag,
                                                         const std::vector<std::string> & names)
{
    if (names.empty())
        return get(tag);
    std::vector<std::shared_ptr<logger>> ret;
    for (const auto & name : names) {
        auto o = opened().find(name);
        if (o != opened().end()) {
            ret.push_back(o->second);
            continue;
        }
        auto i = instances().find(name);
        if (i == instances().end())
            continue;
        auto f = factories().find(i->second.type);
        if (f == factories().end())
            continue;
        auto l = f->second->getLogger(tag, i->second.address.c_str());
        opened().emplace(name, l);
        ret.push_back(l);
    }
    return ret;
}

std::vector<std::shared_ptr<logger>> logger_factory::get(const char * tag)
{
    std::vector<std::shared_ptr<logger>> ret;
    for (const auto & instance : instances()) {
        auto o = opened().find(instance.first);
        if (o != opened().end()) {
            ret.push_back(o->second);
            continue;
        }
        auto f = factories().find(instance.second.type);
        if (f == factories().end())
            continue;
        auto l = f->second->getLogger(tag, instance.second.address.c_str());
        opened().emplace(instance.first, l);
        ret.push_back(l);
    }
    return ret;
}
