/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_OS_H
#define SIMPLELOG_OS_H

#ifdef _WIN32
#include <process.h>
#include <processthreadsapi.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#endif

#if defined(thread_local)
#define THREAD_LOCAL thread_local
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 7)
#define THREAD_LOCAL thread_local
#elif __GNUC__
#define THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
#define THREAD_LOCAL _Thread_local
#elif __clang__
#if __has_feature(cxx_thread_local)
#define THREAD_LOCAL thread_local
#endif
#elif defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#endif

namespace simplelog {

class os
{
public:
    static size_t getThreadId()
    {
#ifdef THREAD_LOCAL
        THREAD_LOCAL static const size_t tid = getThreadIdImpl();
        return tid;
#else
        return std::hash<std::thread::id>()(std::this_thread::get_id());
#endif
    }

    static const char * getEol()
    {
#ifdef _WIN32
        return "\r\n";
#else
        return "\n";
#endif
    }

private:
    static size_t getThreadIdImpl()
    {
#ifdef _WIN32
        return static_cast<size_t>(GetCurrentThreadId());
#elif defined(__linux__)
#if defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
        return static_cast<size_t>(syscall(__NR_gettid));
#else
        return static_cast<size_t>(syscall(SYS_gettid));
#endif
#elif __APPLE__
        uint64_t tid;
        pthread_threadid_np(nullptr, &tid);
        return static_cast<size_t>(tid);
#else
        return std::hash<std::thread::id>()(std::this_thread::get_id());
#endif
    }
};

} // namespace simplelog

#endif
