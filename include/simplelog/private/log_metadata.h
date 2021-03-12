/*
 * Copyright(c) 2020-present simplelog contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#ifndef SIMPLELOG_LOGMETADATA_H
#define SIMPLELOG_LOGMETADATA_H

namespace simplelog {

// Should match with LOG_LEVEL_* from logger.h
enum log_level : int {
    verbose = 6,
    debug = 5,
    info = 4,
    warning = 3,
    error = 2,
    panic = 1,
};

struct log_metadata
{
    const char * tag;
    log_level level;
    size_t tid;
    const char * filename;
    const char * funcname;
    int line;

    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
};

} // namespace simplelog

#endif
