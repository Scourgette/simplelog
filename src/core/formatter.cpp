/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "formatter.h"

#include <stdexcept>

using namespace simplelog;

formatter_factory::formatter_factory(const std::string & name) { factories()[name] = this; }

unordered_casemap<formatter_factory *> & formatter_factory::factories()
{
    static unordered_casemap<formatter_factory *> ret;
    return ret;
}

std::shared_ptr<iformatter> formatter_factory::get()
{
    if (factories().empty())
        throw new std::runtime_error("No registered formatter");
    return factories().begin()->second->getformatter();
}

std::shared_ptr<iformatter> formatter_factory::get(const std::string & name)
{
    auto it = factories().find(name);
    return it == factories().end() ? nullptr : it->second->getformatter();
}
