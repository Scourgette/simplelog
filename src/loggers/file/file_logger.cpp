/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "file_logger.h"

#include <iostream>

using namespace simplelog;

#ifndef SIMPLELOG_DEFAULT_FILE
#define SIMPLELOG_DEFAULT_FILE "/tmp/logs.txt"
#endif

file_logger_factory file_logger_factory::instance;
std::string file_logger::m_defaultPath = SIMPLELOG_DEFAULT_FILE;

file_logger::file_logger(const char * tag, const char * path) :
    logger(tag), m_file(fopen(path == nullptr ? m_defaultPath.c_str() : path, "wb"))
{}

file_logger::~file_logger()
{
    if (m_file)
        fclose(m_file);
}

void file_logger::flush()
{
    if (m_file)
        fflush(m_file);
}

void file_logger::logRaw(const char * msg, size_t len)
{
    if (m_file)
        fwrite(msg, 1, len, m_file);
}
