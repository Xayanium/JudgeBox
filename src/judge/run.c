//
// Created by xa on 24-5-29.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "run.h"
#include "../global/global.h"
#include "../compare/std_check.h"
#include "child.h"
#include "../logger/logger.h"

void runJudge(struct exeConfig* exeConfig, struct limConfig* limConfig, struct judgeResult* judgeResult) {
    struct timeval start, end;

    gettimeofday(&start, NULL); // 记录执行开始时刻
    pid_t pid = fork(); //生成父子进程
    if(pid == -1) {
        judgeResult->execResult = FORK_ERROR;
//        makeLog(ERROR, exeConfig->logPath, "fork error");
        return;
    }

    if(pid == 0) {
        //执行子进程(在exec之前要加上一些限制和安全措施)
        runChild(limConfig, exeConfig);
    }

    if(pid > 0) {
        int status;
        struct rusage rusage;
        wait4(pid, &status, WSTOPPED, &rusage);
        gettimeofday(&end, NULL); // 记录执行结束时刻

//        makeLog(INFO, exeConfig->logPath, "finish child process");

        //将判题结果存入全局变量中, 方便调用
        //秒数存为ms, 方便比较
        judgeResult->realTime = end.tv_sec*1000 + end.tv_usec/1000 - start.tv_sec*1000 - start.tv_usec/1000;
        judgeResult->cpuTime = rusage.ru_utime.tv_sec*1000 + rusage.ru_utime.tv_usec/1000
        + rusage.ru_stime.tv_sec*1000 + rusage.ru_stime.tv_usec/1000;
        judgeResult->memoryUsed = rusage.ru_maxrss;  //KB

        //得到judgeResult->execResult的值, 我们将该功能封装起来, 放入全局函数中
        judgeResult->execResult = getJudgeResult(judgeResult, limConfig, status);
        //如果子进程运行提交的代码正常运行, 才进行之后的判断
        if(judgeResult->execResult == RUN_SUCCESS) {
            if(exeConfig->isSpj) {
                //如果是special judge, 使用special judge
            } else {
                //如果不是special judge, 直接比较文件
                judgeResult->execResult = stdCheck(exeConfig->outputPath, exeConfig->answerPath);
            }
//            makeLog(INFO, exeConfig->logPath, "get result success");
        }
    }
}

