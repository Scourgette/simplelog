/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "logger_engine.h"

#include "async_consumer.h"
#include "sync_consumer.h"

using namespace simplelog;

logger_engine::logger_engine(const std::string & tag, log_level level,
                             const std::shared_ptr<iformatter> & f,
                             std::vector<std::shared_ptr<logger>> loggers) :
    logger(tag, level, f),
    m_loggers(std::move(loggers)),
    m_consumer(std::make_shared<sync_consumer>(m_loggers))
{}

void logger_engine::setAsync(bool async)
{
    if (async)
        m_consumer = std::make_shared<async_consumer>(m_loggers);
    else
        m_consumer = std::make_shared<sync_consumer>(m_loggers);
}
