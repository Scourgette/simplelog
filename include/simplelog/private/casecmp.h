/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_CASECMP_H
#define SIMPLELOG_CASECMP_H

#include <cstring>
#include <unordered_map>

namespace simplelog {

struct icasecmp
{
    bool operator()(const std::string & lhs, const std::string & rhs) const
    {
        return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
    }
};

struct icasehash
{
    size_t operator()(const std::string & str) const
    {
        // Basic hash implementation to avoid a new dynamic allocation
        // of std::string at each call which would be needed to give a
        // modified lower-case string to std::hash.
        size_t hash = 5381;
        for (size_t i = 0; i < str.size(); ++i)
            hash = 33 * hash + char(tolower(str[i]));
        return hash;
    }
};

// unordered_map with the key being a case insensitive std::string
template<class Val>
using unordered_casemap = std::unordered_map<std::string, Val, icasehash, icasecmp>;

} // namespace simplelog

#endif
