/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_ASYNC_CONSUMER
#define SIMPLELOG_ASYNC_CONSUMER

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include "iconsumer.h"
#include "logger.h"

namespace simplelog {

class async_consumer : public iconsumer
{
public:
    async_consumer(const std::vector<std::shared_ptr<logger>> & loggers);
    virtual ~async_consumer();

    virtual void consume(const char * msg, size_t len) override final;
    virtual void flush() override final;

private:
    async_consumer(const async_consumer &) = delete;
    async_consumer & operator=(const async_consumer &) = delete;

    void threadEntry();

    std::vector<std::shared_ptr<logger>> m_loggers;
    std::atomic_bool m_running;
    std::atomic_bool m_flushing;
    std::atomic_bool m_overflow;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_ackCv;
    std::vector<std::string> m_queue;
    std::vector<std::string> m_workingQueue;
    std::thread m_thread;

    static const size_t m_maxQueueSize;
    static const std::string m_overflowMessage;
};

} // namespace simplelog

#endif
