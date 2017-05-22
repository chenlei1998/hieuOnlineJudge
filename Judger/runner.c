#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <sys/resource.h>

#define FORK_FAILED "FORK_FAILED"
#define WAIT_FAILED "WAIT_FAILED"
#define EXECVE_FAILED "EXECVE_FAILED"
#define DUP2_FAILED "DUP2_FAILED"
#define SETRLIMIT_FAILED "SETRLIMIT_FAILED"
#define UNLIMITED -1

#define KILLPID(pid)\
	{\
		return kill(pid, SIGKILL);\
	}

#define CHILD_ERROR_EXIT(msg)\
	{\
		fclose(input_file);\
		fclose(output_file);\
		fclose(error_file);\
		raise(SIGUSR1);\
		fprintf(stderr, msg);\
		return -1;\
	}

#define ERROR_EXIT(msg)\
	{\
		fprintf(stderr, msg);\
		return -1;\
	}

struct config
{
	long stack_size;
	long memory_size;
	long cpu_time;
	long process_number;
	long output_size;
	char input_path[PATH_MAX];
	char output_path[PATH_MAX];
	char error_path[PATH_MAX];
};

int child_process(const struct config *_config) {
	FILE *output_file = NULL;
	FILE *input_file = NULL;
	FILE *error_file = NULL;
	output_file = fopen(_config->output_path, "w");
	if (dup2(fileno(output_file), fileno(stdout)) == -1)
	{
		CHILD_ERROR_EXIT(DUP2_FAILED);
	}

	input_file = fopen(_config->input_path, "r");
	if (dup2(fileno(input_file), fileno(stdin)) == -1)
	{
		CHILD_ERROR_EXIT(DUP2_FAILED);
	}
	error_file = fopen(_config->error_path, "w");
	if (dup2(fileno(error_file), fileno(stderr)) == -1)
	{
		CHILD_ERROR_EXIT(DUP2_FAILED);
	}
	if (_config->stack_size != UNLIMITED)
	{
		struct rlimit max_stack;
		max_stack.rlim_cur = max_stack.rlim_max = (rlim_t) (_config->stack_size);
		if (setrlimit(RLIMIT_STACK, &max_stack) != 0)
		{
			CHILD_ERROR_EXIT(SETRLIMIT_FAILED);
		}
	}
	if (_config->memory_size != UNLIMITED)
	{
		struct rlimit max_memory;
		max_memory.rlim_cur = max_memory.rlim_max = (rlim_t) (_config->memory_size);
		if (setrlimit(RLIMIT_AS, &max_memory) != 0)
		{
			CHILD_ERROR_EXIT(SETRLIMIT_FAILED);
		}
	}
	if (_config->cpu_time != UNLIMITED)
	{
		struct rlimit max_cpu_time;
		max_cpu_time.rlim_cur = max_cpu_time.rlim_max = (rlim_t) (_config->cpu_time);
		if (setrlimit(RLIMIT_CPU, &max_cpu_time) != 0)
		{
			CHILD_ERROR_EXIT(SETRLIMIT_FAILED);
		}
	}
	char *args[] = {"ping", "www.baidu.com", "-c", "3", NULL};
	execve("/bin/ping", args, NULL);
	CHILD_ERROR_EXIT(EXECVE_FAILED);
}

int run(const struct config *_config, const struct *result)
{     
	struct config _config;
	int opt;
	pid_t child_pid = fork();
	if (child_pid < 0)
	{
		ERROR_EXIT(FORK_FAILED);
	}
	else if (child_pid == 0)
	{
		child_process(_config);
	}
	else if (child_pid > 0)
	{
		int status;
		struct rusage resource_usage;
		if (wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1 )
		{
			KILLPID(child_pid);
			ERROR_EXIT(WAIT_FAILED);
		}
		printf("args: %s\n", argv[optind]);
		printf("signal: %d\n", WTERMSIG(status));	
	}
	return 0;
	help:
		printf("Usage: runner\n");
		printf("\t-s, --stack-size=number        set stack size limit.(byte)\n");
		printf("\t-m, --memory-size=number       set memory limit.(byte)\n");
		printf("\t-c, --cpu-time=number          set cpu time limit.(second)\n");
		printf("\t-i, --input-file=file          redirect stdin -> file.\n");
		printf("\t-o, --output-file=file         redirect stdout -> file.\n");
		printf("\t-e, --error-file=file          redirect stderr -> file.\n");
		return 0;
}
