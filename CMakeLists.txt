cmake_minimum_required(VERSION 3.22)
project(Judge C)

set(CMAKE_C_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)

add_executable(judge
        src/compare/std_check.c
        src/compare/std_check.h
        src/global/global.c
        src/global/global.h
        src/limit/set_limit.c
        src/limit/set_limit.h
        src/judge/child.c
        src/judge/child.h
        src/judge/run.c
        src/judge/run.h
        src/logger/logger.c
        src/logger/logger.h
        src/cJSON/cJSON.c
        src/cJSON/cJSON.h
        src/main.c
)


