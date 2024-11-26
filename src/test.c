//
// Created by xa on 24-8-4.
//

#include <stdio.h>
#include "cJSON/cJSON.h"

int main() {
    // 创建JSON对象
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "mike");
    cJSON_AddNumberToObject(root, "age", 18);
    cJSON_AddBoolToObject(root, "student", cJSON_True);

    // 创建JSON数组
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateString("football"));
    cJSON_AddItemToArray(array, cJSON_CreateString("basketball"));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(114514));
    cJSON_AddItemToObject(root, "hobby", array);

    // 解析JSON数据
    const char* jsonStr = "{\"name\":\"Mike\",\"age\":24}";
    cJSON* parsed = cJSON_Parse(jsonStr);
    /*
     * valuestring 用于存储 JSON 字符串值。
     * valueint 用于存储 JSON 对象中整数值
     * valuedouble 用于存储 JSON 对象中的浮点数值
     * string 用于存储 JSON 对象中的键
     * type 是一个整数，用于表示 cJSON 对象的类型
     *
     * */
    char* name = cJSON_GetObjectItem(parsed, "name")->valuestring;
    int age = cJSON_GetObjectItem(parsed, "age")->valueint;

    // 打印JSON数据
    char* msg = cJSON_Print(root);
    printf("%s\n", msg);

    // 释放内存
    cJSON_Delete(root);
    cJSON_Delete(parsed);

    return 0;
}

