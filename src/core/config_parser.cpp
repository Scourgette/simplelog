/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "config_parser.h"

#include <algorithm>
#include <stdexcept>

using namespace simplelog;

class config_parser::line_parser
{
public:
    line_parser(std::string line) : m_line(std::move(line)), m_it(m_line.begin()) {}
    std::string::const_iterator nextNonSpace()
    {
        return m_it = std::find_if_not(m_it, m_line.end(), [](char c) { return std::isspace(c); });
    }
    std::string::const_iterator nextSpace()
    {
        return m_it = std::find_if(m_it, m_line.end(), [](char c) { return std::isspace(c); });
    }
    std::string::const_iterator nextSpaceOrChar(char ch)
    {
        return m_it = std::find_if(m_it, m_line.end(),
                                   [&](char c) { return std::isspace(c) || c == ch; });
    }
    std::string::const_iterator nextChar(char ch)
    {
        return m_it = std::find_if(m_it, m_line.end(), [&](char c) { return c == ch; });
    }
    std::string::const_iterator next() { return ++m_it; }
    std::string::const_iterator end() const { return m_line.end(); }

private:
    const std::string m_line;
    std::string::const_iterator m_it;
};

config_parser::config_parser(std::unique_ptr<std::istream> data)
{
    if (data->fail())
        throw std::runtime_error("Invalid configuration file");
    std::string line;
    std::string currentSection;
    while (std::getline(*data, line)) {
        if (line.empty()) // ignore empty lines
            continue;

        // look for tag begin
        line_parser parser(std::move(line));
        auto tagBegin = parser.nextNonSpace();
        if (tagBegin == parser.end()) // only spaces
            continue;
        if (*tagBegin == '#') // comment
            continue;

        // section
        if (*tagBegin == '[') {
            // skip [
            tagBegin++;
            if (tagBegin == parser.end()) // empty section
                continue;
            // look for section end
            const auto tagEnd = parser.nextChar(']');
            if (tagBegin == tagEnd) // empty section
                continue;

            currentSection = std::string(tagBegin, tagEnd);
            continue;
        }

        if (currentSection.empty())
            continue; // ignore tags with empty section

        // look for tag end
        const auto tagEnd = parser.nextSpaceOrChar('=');
        if (tagEnd == parser.end()) // no tag
            continue;
        if (tagBegin == tagEnd) // empty tag
            continue;

        // look for =
        const auto equal = parser.nextNonSpace();
        if (equal == parser.end() || *equal != '=') // no equal sign
            continue;

        // look for value
        parser.next(); // skip =
        const auto valBegin = parser.nextNonSpace();
        if (valBegin == parser.end()) // no value
            continue;
        // look for value end
        const auto valEnd = parser.nextSpace();

        // look for trash after value
        if (parser.nextNonSpace() != parser.end())
            continue;

        const std::string tag(tagBegin, tagEnd);
        const std::string val(valBegin, valEnd);

        // save entry
        auto entriesIt = m_sections.find(currentSection);
        if (entriesIt == m_sections.end())
            std::tie(entriesIt, std::ignore) = m_sections.emplace(currentSection, entries());
        entriesIt->second[tag] = val;
    }
}
