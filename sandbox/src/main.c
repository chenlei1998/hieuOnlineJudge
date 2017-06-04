#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "runner.h"
#define BUF_SIZE 8192
#define LONGVALUE(item) (long)(item->valuedouble)
#define INTVALUE(item) (item->valueint)
#define STRVALUE(item) (item->valuestring)
int main(int argc, char *argv[])
{
    char buffer[BUF_SIZE];
    if (argc == 2)
    {
        strcpy(buffer, argv[1]);
    }
    else 
    {
        goto invalid_config;
    }
    cJSON *json_config;
    cJSON *json_result;
    json_result = cJSON_CreateObject();
    json_config = cJSON_Parse(buffer);
    if (cJSON_IsObject(json_config))
    {
        cJSON *max_stack_size;
        cJSON *max_memory_size;
        cJSON *max_cpu_time;
        cJSON *max_real_time;
        cJSON *max_process_number;
        cJSON *max_output_size;
        cJSON *input_path;
        cJSON *output_path;
        cJSON *error_path;
        cJSON *exe_path;
        cJSON *args;
        cJSON *env;
        cJSON *seccomp_rule_name;

        max_stack_size = cJSON_GetObjectItem(json_config, "maxStackSize");
        max_memory_size = cJSON_GetObjectItem(json_config, "maxMemorySize");
        max_cpu_time = cJSON_GetObjectItem(json_config, "maxCpuTime");
        max_real_time = cJSON_GetObjectItem(json_config, "maxRealTime");
        max_process_number = cJSON_GetObjectItem(json_config, "maxProcessNumber");
        max_output_size = cJSON_GetObjectItem(json_config, "maxOutputSize");
        
        input_path = cJSON_GetObjectItem(json_config, "inputPath");
        error_path = cJSON_GetObjectItem(json_config, "errorPath");
        output_path = cJSON_GetObjectItem(json_config, "outputPath");
        exe_path = cJSON_GetObjectItem(json_config, "exePath");
        args = cJSON_GetObjectItem(json_config, "args");
        env = cJSON_GetObjectItem(json_config, "env");
        
        seccomp_rule_name = cJSON_GetObjectItem(json_config, "seccompRuleName");

        if (cJSON_IsNumber(max_stack_size) && cJSON_IsNumber(max_memory_size)
            && cJSON_IsNumber(max_cpu_time) && cJSON_IsNumber(max_real_time)
            && cJSON_IsNumber(max_process_number) && cJSON_IsNumber(max_output_size)
            && cJSON_IsString(input_path) && cJSON_IsString(output_path)
            && cJSON_IsString(exe_path) && cJSON_IsString(error_path)
            && (LONGVALUE(max_stack_size) == UNLIMITED || LONGVALUE(max_stack_size) >= 1)
            && (LONGVALUE(max_memory_size) == UNLIMITED || LONGVALUE(max_memory_size) >= 1)
            && (LONGVALUE(max_output_size) == UNLIMITED || LONGVALUE(max_output_size) >= 1)
            && (INTVALUE(max_cpu_time) == UNLIMITED || INTVALUE(max_cpu_time) >= 1)
            && (INTVALUE(max_real_time) == UNLIMITED || INTVALUE(max_real_time) >= 1)
            && (INTVALUE(max_process_number) == UNLIMITED || INTVALUE(max_process_number) >= 0))
        {
            struct config _config;
            _config.max_stack = LONGVALUE(max_stack_size);
            _config.max_cpu_time = INTVALUE(max_cpu_time);
            _config.max_memory = LONGVALUE(max_memory_size);
            _config.max_real_time = INTVALUE(max_real_time);
            _config.max_process_number =INTVALUE(max_process_number);
            _config.max_output_size = LONGVALUE(max_output_size);
            _config.input_path = STRVALUE(input_path);
            _config.output_path = STRVALUE(output_path);
            _config.error_path = STRVALUE(error_path);
            _config.exe_path = STRVALUE(exe_path);
            _config.seccomp_rule_name = STRVALUE(seccomp_rule_name);
            _config.args[0] = _config.exe_path;
            _config.args[1] = NULL;
            _config.env[0] = NULL;
            _config.error_path = NULL;
            if (cJSON_IsArray(args))
            {
                int index;
                index = 1;
                for (int i = 0; i < ARGS_MAX_NUMBER && i < cJSON_GetArraySize(args); i++)
                {
                    cJSON *item = cJSON_GetArrayItem(args, i);
                    if (cJSON_IsString(item))
                    {
                        _config.args[index++] = STRVALUE(item);
                    }
                }
                _config.args[index] = NULL;
            }
            if (cJSON_IsArray(env))
            {
                int index = 0;
                for (int i = 0; i < ARGS_MAX_NUMBER && i < cJSON_GetArraySize(env); i++)
                {
                    cJSON *item = cJSON_GetArrayItem(env, i);
                    if (cJSON_IsString(item))
                    {
                        _config.env[index++] = STRVALUE(item);
                    }
                }
                _config.env[index] = NULL;
            }
            struct result _result;
            run(&_config, &_result);
            cJSON_AddNumberToObject(json_config, "cpuTime", _result.cpu_time);
            cJSON_AddNumberToObject(json_config, "realTime", _result.real_time);
            cJSON_AddNumberToObject(json_config, "memory", _result.memory);
            cJSON_AddNumberToObject(json_config, "result", _result.result);
            cJSON_AddStringToObject(json_config, "errorMsg", _result.errorMsg);
            fprintf(stdout, cJSON_Print(json_result));
        }
        else
        {
            goto invalid_config;
        }
        cJSON_Delete(json_result);
        cJSON_Delete(json_config);
        return 0;
    }
invalid_config:
        cJSON_AddStringToObject(json_config, "errorMsg", "invalid config");
        cJSON_Delete(json_result);
        cJSON_Delete(json_config);
        fprintf(stderr, cJSON_Print(json_result));
        return -1;
}