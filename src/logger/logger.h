//
// Created by xa on 24-7-19.
//

#ifndef JUDGE_LOGGER_H
#define JUDGE_LOGGER_H


#define FATAL       5
#define ERROR       4
#define WARNING     3
#define INFO        2
#define DEBUG       1
typedef int logType;

void makeLog(logType type, FILE* log_file, char* content);

#endif //JUDGE_LOGGER_H
