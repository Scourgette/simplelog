/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_NULL_FORMATTER
#define SIMPLELOG_NULL_FORMATTER

#include <array>
#include <vector>
#include "formatter.h"
#include "logger.h"

namespace simplelog {

class null_formatter : public iformatter
{
public:
    virtual void format(std::mutex & mutex, const log_metadata & metadata,
                        memory_buffer & formatted, const char * msg, va_list args) override final;
    virtual void format(std::mutex & mutex, const log_metadata & metadata, string_view msg,
                        memory_buffer & formatted) override final;
};

class null_formatter_factory : public formatter_factory
{
public:
    null_formatter_factory() : formatter_factory("Null") {}
    virtual std::shared_ptr<iformatter> getformatter() override
    {
        return std::make_shared<null_formatter>();
    }
    static null_formatter_factory instance;
};

} // namespace simplelog

#endif
