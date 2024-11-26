//
// Created by xa on 24-5-29.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../global/global.h"
#include "../limit/set_limit.h"
#include "child.h"
#include "../logger/logger.h"

/**
 * 执行子进程的函数
 *
 * @param exeConfig
 * @param limConfig
 */
void runChild(struct limConfig* limConfig, struct exeConfig* exeConfig) {

    // 重定向标准输入
    FILE* inputFile = NULL;
    if(exeConfig->inputPath[0] != '\0') {
        inputFile = fopen(exeConfig->inputPath, "r");
        if(!inputFile) {
//            makeLog(ERROR, exeConfig->logPath, "input path error");
            exit(INPUT_PATH_ERROR);
        }
        if(dup2(fileno(inputFile), STDIN_FILENO) == -1) {
            fclose(inputFile);
//            makeLog(ERROR, exeConfig->logPath, "dup2 failed for stdin");
            exit(DUP_ERROR);
        }
        fclose(inputFile);
//        makeLog(INFO, exeConfig->logPath, "redirect stdin success");
    }
    // 重定向标准输出
    FILE* outputFile = NULL;
    if(exeConfig->outputPath[0] != '\0') {
        outputFile = fopen(exeConfig->outputPath, "w");
        if(!outputFile) {
//            makeLog(ERROR, exeConfig->logPath, "output path error");
            exit(OUTPUT_PATH_ERROR);
        }
        if(dup2(fileno(outputFile), STDOUT_FILENO) == -1) {
            fclose(outputFile);
//            makeLog(ERROR, exeConfig->logPath, "dup2 failed for stdout");
            exit(DUP_ERROR);
        }
        fclose(outputFile);
//        makeLog(INFO, exeConfig->logPath, "redirect stdout success");
    }
    // 重定向标准错误
    FILE* errorFile = NULL;
    if(exeConfig->errorPath[0] != '\0') {
        errorFile = fopen(exeConfig->errorPath, "w");
        if(!errorFile) {
//            makeLog(ERROR, exeConfig->logPath, "error path error");
            exit(OUTPUT_PATH_ERROR);
        }
        if(dup2(fileno(errorFile), STDERR_FILENO) == -1) {
            fclose(errorFile);
//            makeLog(ERROR, exeConfig->logPath, "dup2 failed for stderr");
            exit(DUP_ERROR);
        }
        fclose(errorFile);
//        makeLog(INFO, exeConfig->logPath, "redirect stderr success");
    }

    // 设置对child的限制
    setLimitation(limConfig, exeConfig);
//    makeLog(INFO, exeConfig->logPath, "set limitation success");

    // 执行指定目录下的可执行文件, 运行用户的提交代码(采用execve()系统调用)
    char* env[] = {"PATH=/bin", NULL};
//    makeLog(INFO, exeConfig->logPath, "exec submitted code");
    if(execve(exeConfig->execPath[0], exeConfig->execPath, env) == -1) {  //执行用户代码
        perror("exe error\n");
        exit(RUN_ERROR);
    }

}

