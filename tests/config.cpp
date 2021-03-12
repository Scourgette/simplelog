/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

#include "config.h"

using namespace simplelog;
using namespace testing;

MATCHER_P2(WithLogger, type, address, "") { return arg.type == type && arg.address == address; }

class config_tests : public Test
{
protected:
    config_tests() : m_config(config::get()) {}
    void update(std::string str) { m_config.update(std::make_unique<std::stringstream>(str)); }

    config & m_config;
};

/*TEST_F(config_tests, default)
{
    ASSERT_EQ(m_config.formatter(), "Default");
    ASSERT_THAT(m_config.loggers(), UnorderedElementsAre(Pair("Stdout", WithLogger("Stdout", ""))));
    ASSERT_THAT(m_config.defaultLevel(), ElementsAre("Stdout"));
    ASSERT_THAT(m_config.levels(), IsEmpty());
    ASSERT_THAT(m_config.tags(), IsEmpty());
}*/

TEST_F(config_tests, general_async)
{
    update("[GENERAL]\n"
           "Async = 1\n");
    ASSERT_EQ(m_config.async(), true);

    update("[GENERAL]\n"
           "async = 0\n");
    ASSERT_EQ(m_config.async(), false);
    ASSERT_EQ(m_config.formatter(), "Default");
}

TEST_F(config_tests, general_formatter)
{
    update("[General]\n"
           "Formatter = Test\n");
    ASSERT_EQ(m_config.formatter(), "Test");

    update("[general]\n"
           "formatter = coucou\n");
    ASSERT_EQ(m_config.formatter(), "coucou");
}

TEST_F(config_tests, general_unknown)
{
    update("[General]\n"
           "Formatter = Default\n"
           "coucou = Test\n");
    ASSERT_EQ(m_config.formatter(), "Default");
}

TEST_F(config_tests, loggers)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "FileTmp = File:/tmp/logs.txt\n");
    ASSERT_THAT(m_config.loggers(),
                UnorderedElementsAre(Pair("Console", WithLogger("Stdout", "")),
                                     Pair("FileTmp", WithLogger("File", "/tmp/logs.txt"))));
}

TEST_F(config_tests, loggers_limits)
{
    update("[logger]\n"
           "Console = Stdout:\n"            // empty address
           "Network = Tcp:127.0.0.5:1234\n" // address contains ':'
           "FileTmp = :/tmp/logs.txt\n");   // no type
    ASSERT_THAT(m_config.loggers(),
                UnorderedElementsAre(Pair("Console", WithLogger("Stdout", "")),
                                     Pair("Network", WithLogger("Tcp", "127.0.0.5:1234"))));
}

/*TEST_F(config_tests, default_level_none)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "FileTmp = File\n");
    ASSERT_THAT(m_config.defaultLevel(), UnorderedElementsAre("Console", "FileTmp"));
    ASSERT_THAT(m_config.levels(), IsEmpty());
}

TEST_F(config_tests, default_level)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[LEVELS]\n"
           "* = Console\n");
    ASSERT_THAT(m_config.defaultLevel(), UnorderedElementsAre("Console"));
    ASSERT_THAT(m_config.levels(), IsEmpty());

    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[Log_Level]\n"
           "* = Console,FileTmp\n");
    ASSERT_THAT(m_config.defaultLevel(), UnorderedElementsAre("Console", "FileTmp"));
    ASSERT_THAT(m_config.levels(), IsEmpty());
}

TEST_F(config_tests, default_level_errors)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[level]\n"
           "* = ,Console,,Test,,FileTmp,\n"); // empty logger, non-existant logger
    ASSERT_THAT(m_config.defaultLevel(), UnorderedElementsAre("Console", "FileTmp"));
    ASSERT_THAT(m_config.levels(), IsEmpty());

    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[LOG_LEVELS]\n"
           "* = ,,Test,,FileTmp2,\n"); // None valid, using default values
    ASSERT_THAT(m_config.defaultLevel(), UnorderedElementsAre("Console", "Network", "FileTmp"));
    ASSERT_THAT(m_config.levels(), IsEmpty());
}

TEST_F(config_tests, levels)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[LEVELS]\n"
           "* = Network\n"
           "Error = Console,FileTmp\n"
           "debug = Console\n");
    ASSERT_THAT(m_config.levels(),
                ElementsAre(Pair(log_level::error, UnorderedElementsAre("Console", "FileTmp")),
                            Pair(log_level::debug, UnorderedElementsAre("Console"))));
    ASSERT_THAT(m_config.defaultLevel(), ElementsAre("Network"));
}

TEST_F(config_tests, levels_errors)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[LEVELS]\n"
           "E = Console,,,Test\n" // Empty and unknown loggers
           "3 = Network\n"
           "2 = Coucou,,,\n"        // No valid logger
           "debug345 = Console\n"); // Invalid log_level
    ASSERT_THAT(m_config.levels(),
                ElementsAre(Pair(log_level::error, UnorderedElementsAre("Console")),
                            Pair(log_level::info, UnorderedElementsAre("Network"))));
    ASSERT_THAT(m_config.defaultLevel(), IsEmpty());
}

TEST_F(config_tests, tags)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[TAGS]\n"
           "Kaka = Console,FileTmp\n"
           "koko = debug\n");
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(Pair("Kaka",
                                          AllOf(Field(&config::tag::level, log_level::verbose),
                                                Field(&config::tag::loggers,
                                                      UnorderedElementsAre("Console", "FileTmp")))),
                                     Pair("koko", AllOf(Field(&config::tag::level,
log_level::debug), Field(&config::tag::loggers, UnorderedElementsAre("Console", "Network",
                                                                                   "FileTmp"))))));
}

TEST_F(config_tests, tags_errors)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[Tag]\n"
           "Kaka = Console,,,test,FileTmp,\n" // unknown or empty loggers
           "koko = weird\n");                 // unknown logger and level
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(
                        Pair("Kaka", AllOf(Field(&config::tag::level, log_level::verbose),
                                           Field(&config::tag::loggers,
                                                 UnorderedElementsAre("Console", "FileTmp"))))));
}*/
