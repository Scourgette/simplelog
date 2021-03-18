/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "sync_consumer.h"

using namespace simplelog;

void sync_consumer::consume(log_level level, const char * msg, size_t len)
{
    for (auto & logger : m_loggers)
        logger->logRaw(level, msg, len);
}

void sync_consumer::flush()
{
    for (auto & logger : m_loggers)
        logger->flush();
}
