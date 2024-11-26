//
// Created by xa on 24-5-29.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include "global.h"
#include "../cJSON/cJSON.h"

//得到判题结果
int getJudgeResult(const struct judgeResult* judgeResult, const struct limConfig* limConfig, const int status) {
    //正常终止
    if(WIFEXITED(status)) {
        //正常退出
        if(WEXITSTATUS(status) == EXIT_SUCCESS) {
            //正常退出时还要特判是否满足题目的限制, 由于我们的限制大于题目的限制, 所以这里的判断是必要的
            //我们设定limit的单位(s和MB)和执行结果的单位(ms和KB)不同
            const bool isMemoryExceeded = limConfig->memoryLimit*1024 < judgeResult->memoryUsed;
            const bool isCpuTimeExceeded = limConfig->timeLimit*1000 < judgeResult->cpuTime;
            const bool isRealTimeExceeded = limConfig->timeLimit*1000 < judgeResult->realTime;
            if(isMemoryExceeded) {
                return MEMORY_LIMIT_EXCEEDED;
            }
            if(isRealTimeExceeded || isCpuTimeExceeded) {
                return TIME_LIMIT_EXCEEDED;
            }
            return RUN_SUCCESS;
        }
        return RUN_ERROR;
    }

    //异常终止(肯定超过了题目的限制, 因为我们的限制会大于题目的限制)
    if(WIFSIGNALED(status)) {
        //CPU运行超时
        if(WTERMSIG(status) == SIGXCPU) {
            return TIME_LIMIT_EXCEEDED;
        }
        //运行时间超最大时间
        else if(WTERMSIG(status) == SIGALRM) {
            return TIME_LIMIT_EXCEEDED;
        }
        //程序访问到未分配给它的内存空间
        else if(WTERMSIG(status) == SIGSEGV) {
            return SEGMENTATION_FAULT;
        }
        //发生除0错误等
        else if(WTERMSIG(status) == SIGFPE) {
            return FLOAT_ERROR;
        }
        //输出超限
        else if(WTERMSIG(status) == SIGXFSZ) {
            return OUTPUT_LIMIT_EXCEEDED;
        }
        //被信号杀死
        if(WTERMSIG(status) == SIGKILL) {
            //运行超时也会出现在这种情况里面
            //注意单位
            const bool isCpuTimeExceeded = limConfig->timeLimit*1000 < judgeResult->cpuTime;
            const bool isRealTimeExceeded = limConfig->timeLimit*1000 < judgeResult->realTime;
            if(isCpuTimeExceeded || isRealTimeExceeded) {
                return TIME_LIMIT_EXCEEDED;
            }
        }
        return RUNTIME_ERROR;
    }

    return UNKNOWN_ERROR;
}


//根据命令行参数初始化配置
bool initConfig(cJSON* parsed, struct exeConfig* exeConfig, struct limConfig* limConfig) {
    // cJson解析传过来的判题参数
    if(!parsed) {
        printf("parse json error\n");
        return false;
    }

    const cJSON* item = cJSON_GetObjectItem(parsed, "pid");
    if(item!=NULL && cJSON_IsNumber(item)) {
        exeConfig->id = item->valueint;
    } else {
        printf("parse pid error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "ti_lim");
    if(item!=NULL && cJSON_IsNumber(item)) {
        limConfig->timeLimit = item->valueint;  // 单位: s
    } else {
        printf("parse ti_lim error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "mem_lim");
    if(item!=NULL && cJSON_IsNumber(item)) {
        limConfig->memoryLimit = item->valueint*1024*1024;  // 单位: B
    } else {
        printf("parse mem_lim error\n");
        return false;
    }

    // item = cJSON_GetObjectItem(parsed, "exec_path");
    // if(item!=NULL && item->type==cJSON_String) {
    //     exeConfig->execPath = item->valuestring;
    // } else {
    //     printf("parse exec_path error\n");
    //     return false;
    // }
    //
    // item = cJSON_GetObjectItem(parsed, "code_path");
    // if(item!=NULL && item->type==cJSON_String) {
    //     exeConfig->codePath = item->valuestring;
    // } else {
    //     printf("parse code_path error\n");
    //     return false;
    // }

    item = cJSON_GetObjectItem(parsed, "lan");
    if(item!=NULL && cJSON_IsString(item)) {
        exeConfig->lan = item->valuestring;
    } else {
        printf("parse lan error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "exec_path");
    if(item!=NULL && cJSON_IsArray(item)) {
        for(int i = 0; i < cJSON_GetArraySize(item); i++) {
            const cJSON* _item = cJSON_GetArrayItem(item, i);
            if(_item!=NULL && cJSON_IsString(_item)) {
                exeConfig->execPath[i] = _item->valuestring;
            } else {
                printf("exec_path not string\n");
                return false;
            }
        }
    } else {
        printf("parse exec_path error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "in_path");
    if(item!=NULL && cJSON_IsString(item)) {
        exeConfig->inputPath = item->valuestring;
    } else {
        printf("parse in_path error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "out_path");
    if(item!=NULL && cJSON_IsString(item)) {
        exeConfig->outputPath = item->valuestring;
    } else {
        printf("parse out_path error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "err_path");
    if(item!=NULL && cJSON_IsString(item)) {
        exeConfig->errorPath = item->valuestring;
    } else {
        printf("parse err_path error\n");
        return false;
    }

    item = cJSON_GetObjectItem(parsed, "ans_path");
    if(item!=NULL && cJSON_IsString(item)) {
        exeConfig->answerPath = item->valuestring;
    } else {
        printf("parse ans_path error\n");
        return false;
    }

//    item = cJSON_GetObjectItem(parsed, "is_spj");
//    if(item!=NULL && cJSON_IsNumber(item)) {
//        exeConfig->isSpj = item->valueint;
//    } else {
//        printf("parse is_spj error\n");
//        return false;
//    }

    return true;
}

//传回判题结果
void returnResult(const int id, const struct judgeResult* judgeResult) {
    //传回给调用者(json格式):
    //耗时(ms), 耗内存(KB)
    cJSON* result = cJSON_CreateObject();
    cJSON_AddNumberToObject(result, "test_case", id);
    cJSON_AddNumberToObject(result, "ti_use", (double)judgeResult->cpuTime);
    cJSON_AddNumberToObject(result, "mem_use", (double)judgeResult->memoryUsed);
    cJSON_AddStringToObject(result, "result", judgeResult->execCondition);
    cJSON_AddNumberToObject(result, "re_code", judgeResult->execResult);

    char* judge_result = cJSON_Print(result);
    printf("%s\n", judge_result);
    cJSON_Delete(result);
}

//初始化全局结构体变量
void initGlobal(struct exeConfig* exeConfig, struct limConfig* limConfig, struct judgeResult* judgeResult)
{
    exeConfig->id = 0;
    // 初始化时将各指针设为空指针, 在initConfig中将指针指向json字符串的地址
    exeConfig->lan="\0";
    exeConfig->inputPath = "\0";
    exeConfig->outputPath = "\0";
    exeConfig->errorPath = "\0";
    exeConfig->answerPath = "\0";
    exeConfig->logPath = NULL;
    exeConfig->isSpj=false;
    for(int i=0; i<10; i++) {
        exeConfig->execPath[i] = NULL;
    }

    limConfig->timeLimit = 3; //题目允许运行时间(初始化, 具体和传入时间有关)(单位s)
    limConfig->maxTimeLimit = 3; //系统最大允许运行时间(单位s)
    limConfig->memoryLimit = 128*1024*1024; //题目允许运行内存(初始化, 具体和传入时间有关)(单位B)
    limConfig->maxMemoryLimit =256*1024*1024; //系统最大允许运行内存(单位B)
    limConfig->stackLimit = 128*1024*1024;  //(单位B)
    limConfig->outputLimit = 256*1024*1024;  //(单位B)
    limConfig->processLimit = 20;

    judgeResult->realTime = 0;
    judgeResult->cpuTime = 0;
    judgeResult->memoryUsed = 0;
    judgeResult->execResult = 0;
}

//将enum存为相应的字符串
void parseResult(struct judgeResult* judgeResult) {
    switch (judgeResult->execResult) {
        case ACCEPT: {judgeResult->execCondition = "ACCEPT"; break;}
        case WRONG_ANSWER: {judgeResult->execCondition = "WRONG_ANSWER"; break;}
        case SEGMENTATION_FAULT: {judgeResult->execCondition = "SEGMENTATION_FAULT"; break;}
        case TIME_LIMIT_EXCEEDED: {judgeResult->execCondition = "TIME_LIMIT_EXCEEDED"; break;}
        case MEMORY_LIMIT_EXCEEDED: {judgeResult->execCondition = "MEMORY_LIMIT_EXCEEDED"; break;}
        case OUTPUT_LIMIT_EXCEEDED: {judgeResult->execCondition = "OUTPUT_LIMIT_EXCEEDED"; break;}
        case RUNTIME_ERROR: {judgeResult->execCondition = "RUNTIME_ERROR"; break;}
        case FLOAT_ERROR: {judgeResult->execCondition = "FLOAT_ERROR"; break;}
        default: {judgeResult->execCondition = "UNKNOWN_ERR0R"; break;}
    }
}