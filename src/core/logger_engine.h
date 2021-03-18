/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_LOGGER_ENGINE
#define SIMPLELOG_LOGGER_ENGINE

#include "iconsumer.h"
#include "logger.h"

namespace simplelog {

class logger_engine : public logger
{
public:
    logger_engine(const std::string & tag, log_level level, const std::shared_ptr<iformatter> & f,
                  std::vector<std::shared_ptr<logger>> loggers);
    void setAsync(bool async = true);

private:
    virtual void logRaw(log_level level, const char * msg, size_t len) override final
    {
        m_consumer->consume(level, msg, len);
    }
    virtual void flush() override final { m_consumer->flush(); }

    std::vector<std::shared_ptr<logger>> m_loggers;
    std::shared_ptr<iconsumer> m_consumer;
};

} // namespace simplelog

#endif
