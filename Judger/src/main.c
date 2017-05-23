#include <stdio.h>
#include "cJSON.h"
#include "runner.h"
#define BUF_SIZE 8192
#define LONGVALUE(item) (long)(item->valuedouble)
#define INTVALUE(item) (item->valueint)
#define STRVALUE(item) (item->string)
int main()
{
	char buffer[BUF_SIZE];
	scanf("%s", buffer);
	cJSON *root;
	root = cJSON_Parse(buffer);
	if (cJSON_IsObject(root))
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

		max_stack_size = cJSON_GetObjectItem(root, "max_stack_size");
		max_memory_size = cJSON_GetObjectItem(root, "max_memory_size");
		max_cpu_time = cJSON_GetObjectItem(root, "max_cpu_time");
		max_real_time = cJSON_GetObjectItem(root, "max_real_time");
		max_process_number = cJSON_GetObjectItem(root, "max_process_number");
		max_output_size = cJSON_GetObjectItem(root, "max_output_size");
		
		input_path = cJSON_GetObjectItem(root, "input_path");
		error_path = cJSON_GetObjectItem(root, "error_path");
		output_path = cJSON_GetObjectItem(root, "output_path");
		exe_path = cJSON_GetObjectItem(root, "exe_path");
		args = cJSON_GetObjectItem(root, "args");
		env = cJSON_GetObjectItem(root, "env");
		
		seccomp_rule_name = cJSON_GetObjectItem(root, "seccomp_rule_name");

		if (cJSON_IsNumber(max_stack_size) && cJSON_IsNumber(max_memory_size)
			&& cJSON_IsNumber(max_cpu_time) && cJSON_IsNumber(max_real_time)
			&& cJSON_IsNumber(max_process_number) && cJSON_IsNumber(max_output_size)
			&& cJSON_IsString(input_path) && cJSON_IsString(output_path)
			&& cJSON_IsString(exe_path)
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
			_config.exe_path = STRVALUE(exe_path);
			_config.seccomp_rule_name = STRVALUE(seccomp_rule_name);
			
			_config.args[0] = NULL;
			_config.env[0] = NULL;
			_config.error_path = NULL;
			if (cJSON_IsArray(args))
			{
				int i;
				for (i = 0; i < ARGS_MAX_NUMBER && i < cJSON_GetArraySize(args); i++)
				{
					cJSON *item = cJSON_GetArrayItem(args, i);
					_config.args[i] = STRVALUE(item);
				}
				_config.args[i] = NULL;
			}
			if (cJSON_IsArray(env))
			{
				int i;
				for (i = 0; i < ARGS_MAX_NUMBER && i < cJSON_GetArraySize(env); i++)
				{
					cJSON *item = cJSON_GetArrayItem(env, i);
					_config.env[i] = STRVALUE(item);
				}
				_config.env[i] = NULL;
			}
			if (cJSON_IsString(error_path))
			{
				_config.error_path = STRVALUE(error_path);
			}
		}
		else
		{
			goto invalid_config;
		}
		cJSON_Delete(root);
		return 0;
	}
invalid_config:
		cJSON_Delete(root);
		MAIN_ERROR_EXIT("invalid config");
}