/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_SYNC_CONSUMER
#define SIMPLELOG_SYNC_CONSUMER

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include "iconsumer.h"
#include "logger.h"

namespace simplelog {

class sync_consumer : public iconsumer
{
public:
    sync_consumer(const std::vector<std::shared_ptr<logger>> & loggers) : m_loggers(loggers) {}

    virtual void consume(const char * msg, size_t len) final;

    virtual void flush() final;

private:
    std::vector<std::shared_ptr<logger>> m_loggers;
};

} // namespace simplelog

#endif
