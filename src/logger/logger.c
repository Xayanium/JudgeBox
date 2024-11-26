//
// Created by xa on 24-7-19.
//

#include <stdio.h>
#include <time.h>
#include "logger.h"

void getLogType(logType type, FILE * log_file) {
    fprintf(log_file, "[");
    if(type == DEBUG) {
        fprintf(log_file, "DEBUG");
    } else if(type == INFO) {
        fprintf(log_file, "INFO");
    } else if(type == WARNING) {
        fprintf(log_file, "WARNING");
    } else if(type == ERROR) {
        fprintf(log_file, "ERROR");
    } else if(type == FATAL) {
        fprintf(log_file, "FATAL");
    }
    fprintf(log_file, "]  ");
}

void getTime(FILE* log_file) {
    time_t current_time;
    struct tm* time_info;

    time(&current_time);
    time_info = localtime(&current_time);
    fprintf(log_file, "[%d-%d-%d %d:%d:%d]",
            time_info->tm_year+1900,  //tm_year=YEAR-1900
            time_info->tm_mon+1,  //tm_mon begin with 0
            time_info->tm_mday,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec);
}

void makeLog(logType type, FILE* log_file, char* content) {
    if(log_file == NULL) {
        return;
    }
    getLogType(type, log_file);
    getTime(log_file);
    fprintf(log_file, "%s\n", content);
}