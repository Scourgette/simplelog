/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>

#include "config_parser.h"

using namespace simplelog;
using namespace testing;

TEST(config_parser_tests, empty_stream)
{
    auto config = std::make_unique<std::stringstream>("");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res, IsEmpty());
}

TEST(config_parser_tests, no_section)
{
    auto config = std::make_unique<std::stringstream>("tag1 = val1\n"
                                                      "tag2 = val2\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res, IsEmpty());
}

TEST(config_parser_tests, empty_section)
{
    auto config = std::make_unique<std::stringstream>("[section1]\n"
                                                      "[section2]\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res, IsEmpty());
}

TEST(config_parser_tests, valid_sections)
{
    auto config = std::make_unique<std::stringstream>("[section1]\n"
                                                      "tag1 = val1\n"
                                                      "tag2 = val2\n"
                                                      "[section2]\n"
                                                      "[section3]\n"
                                                      "tag3 = val3\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag2", "val2"))),
                                     Pair("section3", UnorderedElementsAre(Pair("tag3", "val3")))));
}

TEST(config_parser_tests, duplicate_sections)
{
    auto config = std::make_unique<std::stringstream>("[section1]\n"
                                                      "tag1 = val1\n"
                                                      "[section2]\n"
                                                      "tag2 = val2\n"
                                                      "[section1]\n"
                                                      "tag3 = val3\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag3", "val3"))),
                                     Pair("section2", UnorderedElementsAre(Pair("tag2", "val2")))));
}

TEST(config_parser_tests, duplicate_entries)
{
    auto config = std::make_unique<std::stringstream>("[section1]\n"
                                                      "tag1 = val1\n"
                                                      "tag2 = val2\n"
                                                      "tag1 = val3\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val3"),
                                                                           Pair("tag2", "val2")))));
}

TEST(config_parser_tests, empty_lines)
{
    auto config = std::make_unique<std::stringstream>("\n\n"
                                                      "[section1]\n"
                                                      "tag1 = val1\n"
                                                      "tag2 = val2\n"
                                                      "\n"
                                                      "[section3]\n"
                                                      "tag3 = val3\n\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag2", "val2"))),
                                     Pair("section3", UnorderedElementsAre(Pair("tag3", "val3")))));
}

TEST(config_parser_tests, comments)
{
    auto config = std::make_unique<std::stringstream>("#\n"
                                                      "# comment\n"
                                                      "[section1]\n"
                                                      "tag1 = val1\n"
                                                      "tag2 = val2\n"
                                                      "#[section3]\n"
                                                      "#tag3 = val3\n\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag2", "val2")))));
}

TEST(config_parser_tests, spaces)
{
    auto config = std::make_unique<std::stringstream>(" # comment\n"
                                                      "[section1]       \n"
                                                      "       tag1= val1\t\n"
                                                      " tag2 =val2 \n"
                                                      "\ttag3  =    val3  \n"
                                                      "   #[section3]\n"
                                                      "#tag3 = val3\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag2", "val2"),
                                                                           Pair("tag3", "val3")))));
}

TEST(config_parser_tests, section_bas_syntax)
{
    auto config = std::make_unique<std::stringstream>(
            "[section1\n"
            "tag1 = val1\n"
            "[section 2 coucou]\n" // with spaces
            "tag2 = val2\n"
            "[section3] coucou haha\n" // with garbage after section
            "tag3 = val3\n"
            "[]\n" // empty
            "tag4 = val4\n"
            "[\n" // empty
            "tag5 = val5\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"))),
                                     Pair("section 2 coucou",
                                          UnorderedElementsAre(Pair("tag2", "val2"))),
                                     Pair("section3", UnorderedElementsAre(Pair("tag3", "val3"),
                                                                           Pair("tag4", "val4"),
                                                                           Pair("tag5", "val5")))));
}

TEST(config_parser_tests, bad_syntax)
{
    auto config = std::make_unique<std::stringstream>("[section1]\n"
                                                      "tag1 = val1\n"
                                                      "=\n"                  // only =
                                                      "    \n"               // only spaces
                                                      "tag2 val2\n"          // missing =
                                                      "tag2 == val2\n"       // double =
                                                      "vjsfir()k(gé\n"       // random chars
                                                      "= val2\n"             // missing tag
                                                      "tag2 =\n"             // missing value
                                                      "tag2 = val2 = val3\n" // double value
                                                      "tag3 = val3\n");
    auto res = config_parser(std::move(config)).take();
    ASSERT_THAT(res,
                UnorderedElementsAre(Pair("section1", UnorderedElementsAre(Pair("tag1", "val1"),
                                                                           Pair("tag3", "val3")))));
}
