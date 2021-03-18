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

TEST_F(config_tests, default)
{
    ASSERT_EQ(m_config.async(), false);
    ASSERT_EQ(m_config.formatter(), "Default");
    ASSERT_THAT(m_config.loggers(), UnorderedElementsAre(Pair("Stdout", WithLogger("Stdout", ""))));
    ASSERT_THAT(m_config.tags(), IsEmpty());
}

TEST_F(config_tests, set_async)
{
    m_config.setAsync(true);
    ASSERT_EQ(m_config.async(), true);

    m_config.setAsync(false);
    ASSERT_EQ(m_config.async(), false);
}

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

TEST_F(config_tests, set_formatter)
{
    m_config.setFormatter("Test");
    ASSERT_EQ(m_config.formatter(), "Test");
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

TEST_F(config_tests, add_logger)
{
    m_config.addLogger("Console", "Stdout", "");
    ASSERT_THAT(m_config.loggers(), UnorderedElementsAre(Pair("Console", WithLogger("Stdout", ""))));
    m_config.addLogger("FileTmp", "File", "/tmp/logs.txt");
    ASSERT_THAT(m_config.loggers(),
                UnorderedElementsAre(Pair("Console", WithLogger("Stdout", "")),
                                     Pair("FileTmp", WithLogger("File", "/tmp/logs.txt"))));
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

TEST_F(config_tests, default_logger)
{
    m_config.addLogger("Console", "Stdout", "");
    m_config.addLogger("FileTmp", "File", "/tmp/logs.txt");
    m_config.addLogger("FileTmp2", "File", "/tmp/logs2.txt");

    m_config.setDefaultLoggers("Console,FileTmp2");
    auto t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_EQ(t->second.level, log_level::verbose);
    ASSERT_THAT(t->second.loggers, UnorderedElementsAre("Console", "FileTmp2"));

    update("[LEVELS]\n"
           "* = filetmp,coucou\n"); // coucou ignored because non-existant in registered loggers
    t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_EQ(t->second.level, log_level::verbose);
    ASSERT_THAT(t->second.loggers, UnorderedElementsAre("filetmp"));
}

TEST_F(config_tests, default_level)
{
    m_config.setDefaultLevel(log_level::info);
    auto t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_EQ(t->second.level, log_level::info);

    update("[LEVELS]\n"
           "* = warning\n");
    t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_EQ(t->second.level, log_level::warning);
}

TEST_F(config_tests, default_tag)
{
    m_config.addLogger("Console", "Stdout", "");
    m_config.addLogger("FileTmp", "File", "/tmp/logs.txt");
    m_config.addLogger("FileTmp2", "File", "/tmp/logs2.txt");

    update("[LEVELS]\n"
           "* = error,Console,FileTmp\n");
    auto t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_EQ(t->second.level, log_level::error);
    ASSERT_THAT(t->second.loggers, UnorderedElementsAre("Console", "FileTmp"));
}

TEST_F(config_tests, default_tag_errors)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[level]\n"
           "* = ,Console,,Test,,FileTmp,\n"); // empty logger, non-existant logger
    auto t = m_config.tags().find(m_config.defaultTag());
    ASSERT_NE(t, m_config.tags().end());
    ASSERT_THAT(t->second.loggers, UnorderedElementsAre("Console", "FileTmp"));

    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[LOG_LEVELS]\n"
           "* = ,,Test,,FileTmp2,\n"); // None valid
    t = m_config.tags().find(m_config.defaultTag());
    ASSERT_EQ(t, m_config.tags().end());
}

TEST_F(config_tests, tags)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[Levels]\n"
           "Kaka = Console,FileTmp\n"
           "koko = debug\n");
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(Pair("Kaka",
                                          AllOf(Field(&config::tag::level, log_level::verbose),
                                                Field(&config::tag::loggers,
                                                      UnorderedElementsAre("Console", "FileTmp")))),
                                     Pair("koko", AllOf(Field(&config::tag::level, log_level::debug),
                                                        Field(&config::tag::loggers, IsEmpty())))));
}

TEST_F(config_tests, tags_errors)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[log_level]\n"
           "Kaka = Console,,,test,FileTmp,\n" // unknown or empty loggers
           "koko = wtf?\n");                  // unknown logger and level
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(
                        Pair("Kaka", AllOf(Field(&config::tag::level, log_level::verbose),
                                           Field(&config::tag::loggers,
                                                 UnorderedElementsAre("Console", "FileTmp"))))));
}

TEST_F(config_tests, tags_loggers_modified)
{
    update("[LOGGERS]\n"
           "Console = Stdout\n"
           "Network = Tcp\n"
           "FileTmp = File\n"
           "[Levels]\n"
           "koko = debug\n");
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(Pair("koko", AllOf(Field(&config::tag::level, log_level::debug),
                                                        Field(&config::tag::loggers, IsEmpty())))));

    m_config.addLogger("NewConsole", "Stdout", "");
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(
                        Pair("koko", AllOf(Field(&config::tag::level, log_level::debug),
                                           Field(&config::tag::loggers,
                                                 UnorderedElementsAre("NewConsole"))))));

    update("[LOGGERS]\n"
           "FileTmp = File\n");
    ASSERT_THAT(m_config.tags(),
                UnorderedElementsAre(Pair("koko", AllOf(Field(&config::tag::level, log_level::debug),
                                                        Field(&config::tag::loggers,
                                                              UnorderedElementsAre("FileTmp"))))));
}
