//
// Created by xa on 24-5-29.
//

#include <stdio.h>
#include "global/global.h"
#include "judge/run.h"
#include "logger/logger.h"
#include "cJSON/cJSON.h"

int main(int argc, char* argv[]) {
    struct exeConfig exeConfig;
    struct limConfig limConfig;
    struct judgeResult judgeResult;

    initGlobal(&exeConfig, &limConfig, &judgeResult);
    FILE* fp = fopen("/home/xa/PycharmProjects/Judger/log/log.txt", "a+");
    exeConfig.logPath = fp;

    if(argc < 2) {
        printf("json parse error\n");
        return 0;
    }
    cJSON* judgeJson = cJSON_Parse(argv[1]);

    if(initConfig(judgeJson, &exeConfig, &limConfig)) {
        runJudge(&exeConfig, &limConfig, &judgeResult);
//        makeLog(INFO, exeConfig.logPath, "complete run judge");
        parseResult(&judgeResult);
    }

    returnResult(exeConfig.id, &judgeResult);
//    makeLog(INFO, exeConfig.logPath, "complete return result");
    cJSON_Delete(judgeJson);
    fclose(fp);
    return 0;
}