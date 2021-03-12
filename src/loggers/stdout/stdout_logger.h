/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_STDOUT_LOGGER
#define SIMPLELOG_STDOUT_LOGGER

#include "logger.h"

namespace simplelog {

class stdout_logger : public logger
{
public:
    stdout_logger(const char * tag);

protected:
    virtual void logRaw(const char * msg, size_t len) override final;
    virtual void flush() override final;

private:
    FILE * m_file;
};

class stdout_logger_factory : public logger_factory
{
public:
    stdout_logger_factory() : logger_factory("Stdout") {}
    virtual std::shared_ptr<logger> getLogger(const char * tag, const char *) override
    {
        return std::make_shared<stdout_logger>(tag);
    }
    static stdout_logger_factory instance;
};

} // namespace simplelog

#endif
