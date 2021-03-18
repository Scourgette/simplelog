/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "async_consumer.h"

using namespace simplelog;

const size_t async_consumer::m_maxQueueSize = LOG_ASYNCHRONOUS_MAX_QUEUE_SIZE;
const std::string async_consumer::m_overflowMessage = "ERROR: Log overflow!";

async_consumer::async_consumer(const std::vector<std::shared_ptr<logger>> & loggers) :
    m_loggers(loggers),
    m_running(true),
    m_flushing(false),
    m_overflow(false),
    m_thread(&async_consumer::threadEntry, this)
{}

async_consumer::~async_consumer()
{
    m_running = false;
    m_cv.notify_one();
    m_thread.join();
}

void async_consumer::consume(log_level level, const char * msg, size_t len)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_queue.size() >= m_maxQueueSize) {
        m_overflow = true;
        return;
    }
    m_queue.emplace_back(std::piecewise_construct, std::forward_as_tuple(level),
                         std::forward_as_tuple(msg, len));
    m_cv.notify_one();
}

void async_consumer::flush()
{
    std::cv_status res = std::cv_status::timeout;
    while (res == std::cv_status::timeout) {
        m_flushing = true;
        m_cv.notify_one();
        std::unique_lock<std::mutex> lock(m_mutex);
        res = m_ackCv.wait_for(lock, std::chrono::milliseconds(50));
    }
}

void async_consumer::threadEntry()
{
    while (m_running) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock);
            std::swap(m_queue, m_workingQueue);
        }
        auto begin = std::chrono::steady_clock::now();
        for (auto & msg : m_workingQueue) {
            for (auto & logger : m_loggers)
                logger->logRaw(msg.first, msg.second.c_str(), msg.second.size());
        }
        bool expected = true;
        if (m_overflow.compare_exchange_weak(expected, false)) {
            for (auto & logger : m_loggers)
                logger->logRaw(log_level::warning, m_overflowMessage.c_str(),
                               m_overflowMessage.size());
        }
        if (m_flushing) {
            for (auto & logger : m_loggers)
                logger->flush();
            m_ackCv.notify_one();
            m_flushing = false;
        }
        m_workingQueue.clear();
        auto end = std::chrono::steady_clock::now();

        // Don't loop immediatly so the queue can be filled efficiently
        if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() == 0) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);
        }
    }
}
