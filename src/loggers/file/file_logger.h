/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_FILE_LOGGER
#define SIMPLELOG_FILE_LOGGER

#include <fstream>
#include <stdio.h>
#include "logger.h"

namespace simplelog {

class file_logger : public logger
{
public:
    file_logger(const std::string & tag, const std::string & path);
    ~file_logger();

    static void setDefaultPath(std::string path) { m_defaultPath = std::move(path); }

protected:
    virtual void logRaw(log_level level, const char * msg, size_t len) override final;
    virtual void flush() override final;

private:
    FILE * m_file;
    static std::string m_defaultPath;
};

class file_logger_factory : public logger_factory
{
public:
    file_logger_factory() : logger_factory("File") {}
    virtual std::shared_ptr<logger> getLogger(const std::string & tag,
                                              const std::string & address) override
    {
        return std::make_shared<file_logger>(tag, address);
    }
    static file_logger_factory instance;
};

} // namespace simplelog

#endif
