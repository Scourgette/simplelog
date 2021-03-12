/**
 * @file logger.h
 * @brief Simplelog API
 *
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_LOGGER
#define SIMPLELOG_LOGGER

#include <stdio.h>
#include <stdlib.h>

/**
 * Update simplelog configuration given ini file located at path.
 *
 * It should be called at program startup, at the beginning of main function.
 * It is optional and can be replaced with SIMPLELOG_CONFIG_INI environment variable,
 * or default config files location.
 * It should be called only one time, it is not necessary to call it again when
 * logging from other c/cpp files outside of main function.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * int main(int argc, char ** argv)
 * {
 *     SLOG_CONFIG("/etc/my_service/simplelog.ini");
 *
 *     SLOGI("This is an info log");
 *     return 0;
 * }
 * @endcode
 */
#define SLOG_CONFIG(path)                                                                          \
    do {                                                                                           \
        _simplelog_config_path(path);                                                              \
    } while (0)

/**
 * Macro to create and register a new logger.
 *
 * It should be called at program startup, at the beginning of main function.
 * It is optional and can be replaced with default configuration, or a
 * configuration file.
 * It should be called only one time, it is not necessary to call it again when
 * logging from other c/cpp files outside of main function.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * int main(int argc, char ** argv)
 * {
 *     SLOG_REGISTER_LOGGER("FileTmp", "File", "/tmp/logs.txt");
 *     SLOG_REGISTER_LOGGER("Console", "Stdout");
 *     SLOG_DEFAULT_LOGGERS("TmpFile,Console");
 *
 *     SLOGI("This is an info log");
 *     return 0;
 * }
 * @endcode
 */
#define SLOG_REGISTER_LOGGER(...)                                                                  \
    _SLOG_REGISTER_LOGGER_IMPL(__VA_ARGS__, _SLOG_REGISTER_LOGGER_3, _SLOG_REGISTER_LOGGER_2)      \
    (__VA_ARGS__)

/**
 * Macro to select default loggers.
 * Each log will by default be sent to all selected loggers.
 *
 * It should be called at program startup, at the beginning of main function.
 * It is optional and can be replaced with default configuration, or a
 * configuration file.
 * It should be called only one time, it is not necessary to call it again when
 * logging from other c/cpp files outside of main function.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * int main(int argc, char ** argv)
 * {
 *     SLOG_REGISTER_LOGGER("TmpFile", "File", "/tmp/logs.txt");
 *     SLOG_REGISTER_LOGGER("Console", "Stdout");
 *     SLOG_DEFAULT_LOGGERS("TmpFile,Console");
 *
 *     SLOGI("This is an info log");
 *     return 0;
 * }
 * @endcode
 */
#define SLOG_DEFAULT_LOGGERS(loggers_names)                                                        \
    do {                                                                                           \
        _simplelog_default_loggers(loggers_names);                                                 \
    } while (0)

/**
 * Macro to set the default log level.
 * All logs below that level won't be printed.
 * Allowed levels are listed in #LOG_LEVEL
 *
 * It should be called at program startup, at the beginning of main function.
 * It is optional and can be replaced with default configuration, or a
 * configuration file.
 * It should be called only one time, it is not necessary to call it again when
 * logging from other c/cpp files outside of main function.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * int main(int argc, char ** argv)
 * {
 *     SLOG_DEFAULT_LEVEL(LOG_LEVEL_INFO);
 *
 *     SLOGI("This log is displayed");
 *     SLOGD("This log is NOT displayed");
 *     return 0;
 * }
 * @endcode
 */
#define SLOG_DEFAULT_LEVEL(level)                                                                  \
    do {                                                                                           \
        _simplelog_default_log_level(level);                                                       \
    } while (0)

/**
 * Macro to force asynchronous logging.
 *
 * It should be called at program startup, at the beginning of main function.
 * It is optional and can be replaced with default configuration, or a
 * configuration file.
 * It should be called only one time, it is not necessary to call it again when
 * logging from other c/cpp files outside of main function.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * int main(int argc, char ** argv)
 * {
 *     SLOG_SET_ASYNC(1);
 *
 *     SLOGI("This log will be sent to an asynchronous logger engine");
 *     return 0;
 * }
 * @endcode
 */
#define SLOG_SET_ASYNC(async)                                                                      \
    do {                                                                                           \
        _simplelog_default_async_logging(async);                                                   \
    } while (0)

/**
 * Macro to declare a tag.
 *
 * It should be defined at the beginning of a c/cpp file,
 * after \c \#include but before other definitions.
 * It is mandatory before any call to log macros, and should be called only 1 time for each source file.
 *
 * @code
 * #include <simplelog/logger.h>
 * SLOG_DECLARE_MODULE("MyTag");
 * void my_class::func()
 * {
 *     SLOGI("This is an info log");
 * }
 * @endcode
 *
 * Can be used to force the module to ouput its logs on a specified logger.
 * That logger may have been previously registered in a configuration file or through
 * #SLOG_REGISTER_LOGGER.
 *
 * @code
 * SLOG_DECLARE_MODULE("MyTag", "Console");
 * @endcode
 */
#define SLOG_DECLARE_MODULE(...)                                                                   \
    _SLOG_DECLARE_MODULE_IMPL(__VA_ARGS__, _SLOG_DECLARE_MODULE_2, _SLOG_DECLARE_MODULE_1)         \
    (__VA_ARGS__)

/**
 * \def LOG_LEVEL
 * Current log levels are:
 * @code
 * LOG_LEVEL_VERBOSE = 6
 * LOG_LEVEL_DEBUG = 5
 * LOG_LEVEL_INFO = 4
 * LOG_LEVEL_WARNING = 3
 * LOG_LEVEL_ERROR = 2
 * LOG_LEVEL_PANIC = 1
 * @endcode
 */
#define LOG_LEVEL_VERBOSE 6
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_PANIC 1
#define LOG_LEVEL_DISABLED 0
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_VERBOSE
#endif

/**
 * Verbose log @code SLOGV("packet header {}", packet.header.toHex()); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define SLOGV(...) _SLOG_PRIO(LOG_LEVEL_VERBOSE, __VA_ARGS__)
#else
#define SLOGV(...)
#endif
/**
 * Debug log @code SLOGD("command sent"); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define SLOGD(...) _SLOG_PRIO(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define SLOGD(...)
#endif
/**
 * Info log @code SLOGI("received {} new messages", messages.size()); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_INFO
#define SLOGI(...) _SLOG_PRIO(LOG_LEVEL_INFO, __VA_ARGS__)
#else
#define SLOGI(...)
#endif
/**
 * Warning log @code SLOGW("message was '{}'", messages); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_WARNING
#define SLOGW(...) _SLOG_PRIO(LOG_LEVEL_WARNING, __VA_ARGS__)
#else
#define SLOGW(...)
#endif
/**
 * Error log @code SLOGE("error: {}", error); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define SLOGE(...) _SLOG_PRIO(LOG_LEVEL_ERROR, __VA_ARGS__)
#else
#define SLOGE(...)
#endif
/**
 * Panic log @code SLOGP("critical error: {}", error); @endcode
 */
#if LOG_LEVEL >= LOG_LEVEL_PANIC
#define SLOGP(...) _SLOG_PRIO(LOG_LEVEL_PANIC, __VA_ARGS__)
#else
#define SLOGP(...)
#endif

/**
 * Abort without condition.
 * May contain a custom message with optional arguments.
 * @code
 * SLOG_FAIL();
 * SLOG_FAIL("This should never happen");
 * SLOG_FAIL("This should never happen, reason: {}", reason);
 * @endcode
 */
#define SLOG_FAIL(...) SLOG_ASSERT(0, ##__VA_ARGS__)
/**
 * Assert condition is true.
 * Condition may be followed by a custom message with optional arguments.
 * @code
 * SLOG_ASSERT(str != NULL);
 * SLOG_ASSERT(str != NULL, "string is null");
 * SLOG_ASSERT(str == NULL, "string is not null, value: {}", str);
 * @endcode
 */
#define SLOG_ASSERT(condition, ...)                                                                \
    _SLOG_GENERIC_ASSERT(SLOG_ASSERT_TYPE_ABORT, (condition), #condition, ##__VA_ARGS__)
/**
 * Trigger a warning assert if condition is false.
 * Condition may be followed by a custom message with optional arguments.
 * @code
 * SLOG_WARN(str != NULL);
 * SLOG_WARN(str != NULL, "string is null");
 * SLOG_WARN(str == NULL, "string is not null, value: {}", str);
 * @endcode
 */
#define SLOG_WARN(condition, ...)                                                                  \
    _SLOG_GENERIC_ASSERT(SLOG_ASSERT_TYPE_PRINT, (condition), #condition, ##__VA_ARGS__)
/**
 * Assert that condition is true and return retval if condition is false. Retval may be omitted
 * if return type is void.
 * Can be used to ensure function won't be executed even when assert are disabled.
 * Expression is evaluated even if asserts are disabled.
 * @code
 * int strlen(const char * str)
 * {
 *     SLOG_RETURN(str != NULL, 0);
 *     //...//
 * }
 * @endcode
 */
#define SLOG_RETURN(condition, retval...)                                                          \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            _SLOG_GENERIC_ASSERT(SLOG_ASSERT_TYPE_ABORT, 0, #condition);                           \
            return retval;                                                                         \
        }                                                                                          \
    } while (0)
/**
 * Assert if expression result is different than value.
 * Expression is evaluated even if asserts are disabled.
 * @code
 * SLOG_CHECKVAL(close(fd), 0);
 * @endcode
 */
#define SLOG_CHECKVAL(expression, value, ...)                                                      \
    do {                                                                                           \
        if ((expression) != (value)) {                                                             \
            _SLOG_GENERIC_ASSERT(SLOG_ASSERT_TYPE_ABORT, 0, #expression " != " #value,             \
                                 ##__VA_ARGS__);                                                   \
        }                                                                                          \
    } while (0)
/**
 * Assert if expression result is false.
 * Expression is evaluated even if asserts are disabled.
 * @code
 * SLOG_CHECK(config.update());
 * @endcode
 */
#define SLOG_CHECK(expression, ...)                                                                \
    do {                                                                                           \
        if (!(expression)) {                                                                       \
            _SLOG_GENERIC_ASSERT(SLOG_ASSERT_TYPE_ABORT, 0, #expression, ##__VA_ARGS__);           \
        }                                                                                          \
    } while (0)

// Assert behaviour
#define SLOG_ASSERT_TYPE_ABORT 0
#define SLOG_ASSERT_TYPE_PRINT 1

// Log max line size
#ifndef LOG_MAX_LINE_LENGTH
#define LOG_MAX_LINE_LENGTH 256
#endif

// Asynchronous engine max queue size
#ifndef LOG_ASYNCHRONOUS_MAX_QUEUE_SIZE
#define LOG_ASYNCHRONOUS_MAX_QUEUE_SIZE 16384
#endif

// **** Private impl **** //

#ifdef __cplusplus
extern "C" {
#endif

void _simplelog_config_path(const char * path);
void _simplelog_register_logger(const char * name, const char * type, const char * address);
void _simplelog_default_loggers(const char * loggers_names);
void _simplelog_default_log_level(int level);
void _simplelog_default_async_logging(int async);
void * _simplelog_create(const char * tag, const char * loggers_names);
void _simplelog_log(void * thiz, int prio, const char * filename, const char * funcname, int line,
                    const char * msg, ...) __attribute__((format(printf, 6, 7)));
void _simplelog_flush(void * thiz);

#ifdef __cplusplus
}
#endif

#define _SLOG_REGISTER_LOGGER_IMPL(_1, _2, _3, FUNC, ...) FUNC
#define _SLOG_REGISTER_LOGGER_3(name, type, address)                                               \
    do {                                                                                           \
        _simplelog_register_logger(name, type, address);                                           \
    } while (0)
#define _SLOG_REGISTER_LOGGER_2(name, type)                                                        \
    do {                                                                                           \
        _simplelog_register_logger(name, type, 0);                                                 \
    } while (0)

#define _SLOG_DECLARE_MODULE_IMPL(_1, _2, FUNC, ...) FUNC
#define _SLOG_DECLARE_MODULE_1(tag)                                                                \
    static void * __simplelog_module__USE__SLOG_DECLARE_MODULE()                                   \
    {                                                                                              \
        static void * module = NULL;                                                               \
        if (module == NULL)                                                                        \
            module = _simplelog_create(tag, 0);                                                    \
        return module;                                                                             \
    }
#define _SLOG_DECLARE_MODULE_2(tag, loggers_names)                                                 \
    static void * __simplelog_module__USE__SLOG_DECLARE_MODULE()                                   \
    {                                                                                              \
        static void * module = NULL;                                                               \
        if (module == NULL)                                                                        \
            module = _simplelog_create(tag, loggers_names);                                        \
        return module;                                                                             \
    }

#ifdef __cplusplus

#include "private/logger.hpp"
#define _SLOG_PRIO(prio, ...)                                                                      \
    do {                                                                                           \
        reinterpret_cast<simplelog::logger *>(__simplelog_module__USE__SLOG_DECLARE_MODULE())      \
                ->log(simplelog::log_level(prio), __FILE__, __func__, __LINE__, __VA_ARGS__);      \
    } while (0)

#else // __cplusplus

#define _SLOG_PRIO(prio, ...)                                                                      \
    do {                                                                                           \
        _simplelog_log(__simplelog_module__USE__SLOG_DECLARE_MODULE(), prio, __FILE__, __func__,   \
                       __LINE__, __VA_ARGS__);                                                     \
    } while (0)

#endif // __cplusplus

#define _LOG_FLUSH()                                                                               \
    do {                                                                                           \
        _simplelog_flush(__simplelog_module__USE__SLOG_DECLARE_MODULE());                          \
    } while (0)

#ifdef LOG_ASSERT_ENABLED

#ifdef __cplusplus
#define _SLOG_GENERIC_ASSERT(type, assert, condition, ...)                                         \
    do {                                                                                           \
        if (!(assert)) {                                                                           \
            _SLOG_PRIO(LOG_LEVEL_PANIC, "Assert failed: " condition ": " __VA_ARGS__);             \
            _SLOG_PRIO(LOG_LEVEL_PANIC, "file \"{}\" function \"{}\" line {}", __FILE__, __func__, \
                       __LINE__);                                                                  \
            if (type == SLOG_ASSERT_TYPE_ABORT) {                                                  \
                _LOG_FLUSH();                                                                      \
                abort();                                                                           \
            }                                                                                      \
        }                                                                                          \
    } while (0)
#else
#define _SLOG_GENERIC_ASSERT(type, assert, condition, ...)                                         \
    do {                                                                                           \
        if (!(assert)) {                                                                           \
            _SLOG_PRIO(LOG_LEVEL_PANIC, "Assert failed: " condition ": " __VA_ARGS__);             \
            _SLOG_PRIO(LOG_LEVEL_PANIC, "file \"%s\" function \"%s\" line %d", __FILE__, __func__, \
                       __LINE__);                                                                  \
            if (type == SLOG_ASSERT_TYPE_ABORT) {                                                  \
                _LOG_FLUSH();                                                                      \
                abort();                                                                           \
            }                                                                                      \
        }                                                                                          \
    } while (0)
#endif

#else // !LOG_ASSERT_ENABLED

#define _SLOG_GENERIC_ASSERT(type, assert, ...)

#endif // !LOG_ASSERT_ENABLED

#endif
