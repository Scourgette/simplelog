/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_CONFIG_PARSER
#define SIMPLELOG_CONFIG_PARSER

#include <istream>
#include <memory>

#include "casecmp.h"

namespace simplelog {

// Ini config file parser
class config_parser
{
public:
    using entries = unordered_casemap<std::string>;
    using sections = unordered_casemap<entries>;

    config_parser(std::unique_ptr<std::istream> data);
    sections take() { return std::move(m_sections); }

private:
    class line_parser;

    sections m_sections;
};

} // namespace simplelog

#endif
