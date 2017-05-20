#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/resource.h>

#define FORK_FAILED "FORK_FAILED"
#define WAIT_FAILED "WAIT_FAILED"
#define EXECVE_FAILED "EXECVE_FAILED"
#define DUP2_FAILED "DUP2_FAILED"
#define KILLPID(pid)\
	{\
		return kill(pid, SIGKILL);\
	}

#define CHILD_ERROR_EXIT(msg)\
	{\
		fclose(input_file);\
		fclose(output_file);\
		fclose(err_file);\
		raise(SIGUSR1);\
		fprintf(stderr, msg);\
		return -1;\
	}

#define ERROR_EXIT(msg)\
	{\
		fprintf(stderr, msg);\
		return -1;\
	}

int child_process(const char *input_file_path, const char *output_file_path, const char *err_file_path) {
	FILE *output_file = NULL;
	FILE *input_file = NULL;
	FILE *err_file = NULL;
	if (output_file_path != NULL)
	{
		output_file = fopen(output_file_path, "w");
		if (dup2(fileno(output_file), fileno(stdout)) == -1)
		{
			CHILD_ERROR_EXIT(DUP2_FAILED);
		}
	}
	if (input_file_path != NULL)
	{
		input_file = fopen(input_file_path, "r");
		if (dup2(fileno(input_file), fileno(stdin)) == -1)
		{
			CHILD_ERROR_EXIT(DUP2_FAILED);
		}
	}
	if (err_file_path != NULL)
	{
		err_file = fopen(err_file_path, "w");
		if (dup2(fileno(err_file), fileno(stderr)) == -1)
		{
			CHILD_ERROR_EXIT(DUP2_FAILED);
		}
	}
	char *args[] = {"ping", "www.baidu.com", "-c", "3", NULL};
	execve("/bin/ping", args, NULL);
	CHILD_ERROR_EXIT(EXECVE_FAILED);
}

int main(int argc, char* argv[])
{     
	pid_t child_pid = fork();
	if (child_pid < 0)
	{
		ERROR_EXIT(FORK_FAILED);
	}
	else if (child_pid == 0)
	{
		child_process(NULL, NULL, NULL);
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
		printf("signal: %d\n", WTERMSIG(status));	
	}
	return 0;
}
