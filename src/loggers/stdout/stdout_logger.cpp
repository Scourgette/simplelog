/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#include "stdout_logger.h"
#include <iostream>
#include <stdio.h>

using namespace simplelog;

stdout_logger_factory stdout_logger_factory::instance;

stdout_logger::stdout_logger(const std::string & tag) : logger(tag), m_file(stdout) {}

void stdout_logger::logRaw(log_level, const char * msg, size_t len) { fwrite(msg, 1, len, m_file); }

void stdout_logger::flush() { fflush(m_file); }
