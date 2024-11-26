//
// Created by xa on 24-5-29.
//

#include <sys/resource.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "../global/global.h"
#include "set_limit.h"

/**
 * 设置子进程exec执行代码时的限制
 *
 * @param limConfig
 */
void setLimitation(struct limConfig* limConfig, struct exeConfig* exeConfig) {
    // CPU时间限制: 单位: s
    struct rlimit maxCpuTime;
    maxCpuTime.rlim_cur = maxCpuTime.rlim_max = limConfig->maxTimeLimit;
    if(setrlimit(RLIMIT_CPU, &maxCpuTime) != 0) {
        exit(SET_LIMIT_ERROR);
    }

    // 实际运行时间限制: 单位: s
    struct itimerval maxRealTime;
    maxRealTime.it_value.tv_sec = (long)limConfig->maxTimeLimit;
    maxRealTime.it_value.tv_usec = 0;
    maxRealTime.it_interval.tv_sec = maxRealTime.it_interval.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &maxRealTime, (struct itimerval*) 0) != 0) {
        exit(SET_LIMIT_ERROR);
    }

    // 最大内存限制: 单位 字节
    struct rlimit maxMemory;
    maxMemory.rlim_cur = maxMemory.rlim_max = limConfig->maxMemoryLimit;
    // 对JVM和python解释器不做虚拟内存限制, 只限制物理内存
    if(strcmp(exeConfig->lan, "java")!=0 && strcmp(exeConfig->lan, "java")!=0) {
        if(setrlimit(RLIMIT_AS, &maxMemory) != 0) {
            exit(SET_LIMIT_ERROR);
        }
    }
    if(setrlimit(RLIMIT_RSS, &maxMemory) != 0) {
            exit(SET_LIMIT_ERROR);
    }


    // 栈限制: 单位: 字节
    struct rlimit maxStack;
    maxStack.rlim_cur = maxStack.rlim_max = limConfig->maxMemoryLimit;
    if(setrlimit(RLIMIT_STACK, &maxStack) != 0) {
        exit(SET_LIMIT_ERROR);
    }

    // 输出文件大小限制: 单位: 字节
    struct rlimit maxOutput;
    maxOutput.rlim_cur = maxOutput.rlim_max = limConfig->outputLimit;
    if(setrlimit(RLIMIT_FSIZE, &maxOutput) != 0) {
        exit(SET_LIMIT_ERROR);
    }

    // 最大进程数限制: 单位: 个
    struct rlimit maxProcess;
    maxProcess.rlim_cur = maxProcess.rlim_max = limConfig->processLimit;
    if(strcmp(exeConfig->lan, "java")!=0) {  //对java不做进程数限制
        if(setrlimit(RLIMIT_NPROC, &maxProcess) != 0) {
            exit(SET_LIMIT_ERROR);
        }
    }
}
