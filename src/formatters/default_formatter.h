/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_DEFAULT_FORMATTER
#define SIMPLELOG_DEFAULT_FORMATTER

#include <array>
#include <vector>
#include "formatter.h"
#include "logger.h"

namespace simplelog {

class default_formatter : public iformatter
{
public:
    virtual void format(std::mutex & mutex, const log_metadata & metadata,
                        memory_buffer & formatted, const char * msg, va_list args) override final;
    virtual void format(std::mutex & mutex, const log_metadata & metadata, string_view msg,
                        memory_buffer & formatted) override final;

private:
    void formatPrefix(std::mutex & mutex, const log_metadata & metadata, memory_buffer & formatted);
    char logLevelToChar(log_level level) const;

    memory_buffer m_seconds;
    memory_buffer m_milliseconds;
};

class default_formatter_factory : public formatter_factory
{
public:
    default_formatter_factory() : formatter_factory("Default") {}
    virtual std::shared_ptr<iformatter> getformatter() override
    {
        return std::make_shared<default_formatter>();
    }
    static default_formatter_factory instance;
};

} // namespace simplelog

#endif
