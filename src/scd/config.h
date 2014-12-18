/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <sc/ash/defs.h>

#include <limits.h> // PATH_MAX
#include <stdio.h>

struct Config {
    ScBool daemon;

    char prefix[PATH_MAX + 1];
    char bankfile[PATH_MAX + 1];
    char dbfile[PATH_MAX + 1];
    char logfile[PATH_MAX + 1];

    int journsize;
    int poolsize;
};

SC_EXTERN ScBool
parse_line(char* begin, char* end, int line, struct Config* config);

SC_EXTERN ScBool
parse_stream(FILE* stream, struct Config* config);

SC_EXTERN ScBool
parse_file(const char* path, struct Config* config);

#endif // CONFIG_H
