//
// Created by xa on 24-5-29.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "std_check.h"
#include "../global/global.h"

void processFile(char* outputFilename, char* answerFilename) {
    //处理文件行末空格,将windows的换行符转为linux并删除文件末尾换行(windows是\r\n, linux是\n)
    char* argv = malloc(strlen(outputFilename)+ strlen(answerFilename)+30);
    sprintf(argv, "sed -i 's/\r$//;s/[ \\t]*$//g' %s", outputFilename);
    system(argv);
    sprintf(argv, "perl -pi -e 'chomp if eof' %s", outputFilename);
    system(argv);
    sprintf(argv, "sed -i 's/\r$//;s/[ \\t]*$//g' %s", answerFilename);
    system(argv);
    sprintf(argv, "perl -pi -e 'chomp if eof' %s", answerFilename);
    system(argv);
    free(argv);
}

int stdCheck(char* outputFilename, char* answerFilename) {
    //预处理输出文件和答案文件
    processFile(outputFilename, answerFilename);

    char* argv = malloc(strlen(outputFilename)+ strlen(answerFilename)+30);
    sprintf(argv,"diff -q %s %s > /dev/null 2>&1", outputFilename, answerFilename);

    int result = ACCEPT;
    //shell 执行 diff -q a.out b.out > /dev/null 2>&1 进行代码对比
    pid_t status = system(argv);
    free(argv);

    if(status == -1 || status == 1 || status == 0x7f00) {
        result = CHECK_ERROR;
    } else {
        //正常退出返回非零值
        if(WIFEXITED(status)) {
            //Ubuntu22.04中, 如果返回状态码为1, 表示两个文件不同
            if(WEXITSTATUS(status) == 1) {
                result  = WRONG_ANSWER;
            } else if(WEXITSTATUS(status) > 1){
                //如果返回状态码非0且不为1, 表示文件不存在或无法访问
                result = CHECK_ERROR;
            }
        } else {
            //非正常退出
            result = CHECK_ERROR;
        }
    }
    return result;
}
