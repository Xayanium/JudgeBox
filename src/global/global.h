//
// Created by xa on 24-5-29.
//

#ifndef JUDGE_GLOBAL_H
#define JUDGE_GLOBAL_H
#include <sys/resource.h>
#include <stdbool.h>
#include <stdio.h>
#include "../cJSON/cJSON.h"

enum RUNNING_CONDITION {
    //用户代码导致的结果
    ACCEPT = 1,
    WRONG_ANSWER,
    SEGMENTATION_FAULT,  //缓冲区溢出或堆栈溢出
    TIME_LIMIT_EXCEEDED,
    MEMORY_LIMIT_EXCEEDED,
    OUTPUT_LIMIT_EXCEEDED,
    RUNTIME_ERROR,
    FLOAT_ERROR,

    //判题机执行过程中的状态
    RESTRICTED_FUNCTION,  //调用了不该使用的函数
    CHECK_ERROR,  //对比代码时出错
    SET_LIMIT_ERROR,  //设置限制时出错
    INPUT_PATH_ERROR,  //输入文件找不到
    OUTPUT_PATH_ERROR,  //输出路径错误
    DUP_ERROR,  //dup2操作失败
    FORK_ERROR,  //创建子进程出错
    RUN_SUCCESS, //子进程正常结束
    RUN_ERROR, //子进程运行出错
    UNKNOWN_ERROR, //未知错误
};

struct limConfig {
    //rlim_t cpuTimeLimit;    //题目允许运行时间, 单位: s
    //rlim_t realTimeLimit;   //题目允许运行时间, 单位: s
    rlim_t timeLimit;       //题目允许运行时间, 单位: s
    rlim_t maxTimeLimit;    //系统最大允许运行时间, 单位: s
    rlim_t memoryLimit;     //题目允许运行内存, 单位: Byte
    rlim_t maxMemoryLimit;  //系统最大允许运行内存, 单位: Byte
    rlim_t stackLimit;      //单位: Byte
    rlim_t outputLimit;     //单位: Byte
    rlim_t processLimit;    //单位: 个
};

struct exeConfig {
    int id;  //当前处于几号test case
    char* lan;
    char* inputPath;
    char* outputPath;
    char* errorPath;
    char* execPath[10];
    char* answerPath;
    FILE* logPath;
    bool isSpj;  //是否使用special judge
};

struct judgeResult {
    rlim_t cpuTime;
    rlim_t realTime;
    rlim_t memoryUsed;
    int execResult;  //判题状态记录(enum RUNNING_CONDITION 结构体)
    char* execCondition;  //判题状态(字符串)
};



int getJudgeResult(const struct judgeResult* judgeResult, const struct limConfig* limConfig, const int status);
bool initConfig(cJSON* parsed, struct exeConfig* exeConfig, struct limConfig* limConfig);
void returnResult(const int id, const struct judgeResult* judgeResult);
void initGlobal(struct exeConfig* exeConfig, struct limConfig* limConfig, struct judgeResult* judgeResult);
void parseResult(struct judgeResult* judgeResult);
#endif //JUDGE_GLOBAL_H
