#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>

#include "runner.h"

int child_process(const struct config *_config, const struct result *_result)
{
	FILE *output_file = NULL;
	FILE *input_file = NULL;
	FILE *error_file = NULL;
	
	// set stack limit
	if (_config->max_stack != UNLIMITED)
	{
		struct rlimit max_stack;
		max_stack.rlim_cur = max_stack.rlim_max = (rlim_t) (_config->max_stack);
		if (setrlimit(RLIMIT_STACK, &max_stack) != 0)
		{
			CHILD_ERROR_RETURN(SETRLIMIT_FAILED);
		}
	}

    // set memory limit
    if (_config->max_memory != UNLIMITED)
	{
        struct rlimit max_memory;
        max_memory.rlim_cur = max_memory.rlim_max = (rlim_t) (_config->max_memory);
        if (setrlimit(RLIMIT_AS, &max_memory) != 0)
		{
            CHILD_ERROR_RETURN(SETRLIMIT_FAILED);
        }
    }

    // set cpu time limit (in seconds)
    if (_config->max_cpu_time != UNLIMITED)
	{
        struct rlimit max_cpu_time;
        max_cpu_time.rlim_cur = max_cpu_time.rlim_max = (rlim_t) (_config->max_cpu_time);
        if (setrlimit(RLIMIT_CPU, &max_cpu_time) != 0) {
            CHILD_ERROR_RETURN(SETRLIMIT_FAILED);
        }
    }

    // set max process number limit
    if (_config->max_process_number != UNLIMITED)
	{
        struct rlimit max_process_number;
        max_process_number.rlim_cur = max_process_number.rlim_max = (rlim_t) _config->max_process_number;
        if (setrlimit(RLIMIT_NPROC, &max_process_number) != 0)
		{
            CHILD_ERROR_RETURN(SETRLIMIT_FAILED);
        }
    }

    // set max output size limit
    if (_config->max_output_size != UNLIMITED)
	{
        struct rlimit max_output_size;
        max_output_size.rlim_cur = max_output_size.rlim_max = (rlim_t ) _config->max_output_size;
        if (setrlimit(RLIMIT_FSIZE, &max_output_size) != 0)
		{
            CHILD_ERROR_RETURN(SETRLIMIT_FAILED);
        }
    }

    if (_config->input_path != NULL)
	{
        input_file = fopen(_config->input_path, "r");
        if (input_file == NULL) {
            CHILD_ERROR_RETURN(DUP2_FAILED);
        }
        // redirect file -> stdin
        // On success, these system calls return the new descriptor.
        // On error, -1 is returned, and errno is set appropriately.
        if (dup2(fileno(input_file), fileno(stdin)) == -1)
		{
            // todo log
            CHILD_ERROR_RETURN(DUP2_FAILED);
        }
    }

    if (_config->output_path != NULL)
	{
        output_file = fopen(_config->output_path, "w");
        if (output_file == NULL) {
            CHILD_ERROR_RETURN(DUP2_FAILED);
        }
        // redirect stdout -> file
        if (dup2(fileno(output_file), fileno(stdout)) == -1)
		{
            CHILD_ERROR_RETURN(DUP2_FAILED);
        }
    }

    if (_config->error_path != NULL)
	{
        // if outfile and error_file is the same path, we use the same file pointer
        if (_config->output_path != NULL && strcmp(_config->output_path, _config->error_path) == 0)
		{
            error_file = output_file;
        }
        else
		{
            error_file = fopen(_config->error_path, "w");
            if (error_file == NULL)
			{
                CHILD_ERROR_RETURN(DUP2_FAILED);
            }
        }
        // redirect stderr -> file
        if (dup2(fileno(error_file), fileno(stderr)) == -1)
		{
            CHILD_ERROR_RETURN(DUP2_FAILED);
        }
    }
	execve(_config->exe_path, _config->args, _config->env);
	CHILD_ERROR_RETURN(EXECVE_FAILED);
}

int kill_pid(pid_t pid) {
    return kill(pid, SIGKILL);
}

void *timeout_killer(void *timeout_killer_args) {
    // this is a new thread, kill the process if timeout
    pid_t pid = ((struct timeout_killer_args *)timeout_killer_args)->pid;
    int timeout = ((struct timeout_killer_args *)timeout_killer_args)->timeout;
    // On success, pthread_detach() returns 0; on error, it returns an error number.
    if (pthread_detach(pthread_self()) != 0) {
        kill_pid(pid);
        return NULL;
    }
    // usleep can't be used, for time args must < 1000ms
    // this may sleep longer that expected, but we will have a check at the end
    if (sleep((unsigned int)(timeout) != 0) {
        kill_pid(pid);
        return NULL;
    }
    if (kill_pid(pid) != 0) {
        return NULL;
    }
    return NULL;
}

void run(const struct config *_config, const struct result *_result)
{
	pid_t child_pid = fork();
	if (child_pid < 0)
	{
		RUN_ERROR_RETURN(FORK_FAILED);
	}
	else if (child_pid == 0)
	{
		child_process(_config, _result);
	}
	else if (child_pid > 0)
	{
        // create new thread to monitor process running time
        pthread_t tid = 0;
        if (_config->max_real_time != UNLIMITED) {
            struct timeout_killer_args killer_args;

            killer_args.timeout = _config->max_real_time;
            killer_args.pid = child_pid;
            if (pthread_create(&tid, NULL, timeout_killer, (void *) (&killer_args)) != 0) {
                kill_pid(child_pid);
                RUN_ERROR_RETURN(PTHREAD_FAILED);
            }
        }

        int status;
        struct rusage resource_usage;

        // wait for child process to terminate
        // on success, returns the process ID of the child whose state has changed;
        // On error, -1 is returned.
        if (wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1) {
            kill_pid(child_pid);
            RUN_ERROR_RETURN(WAIT_FAILED);
        }

        // process exited, we may need to cancel timeout killer thread
        if (_config->max_real_time != UNLIMITED) {
            if (pthread_cancel(tid) != 0) {

            };
        }

        _result->exit_code = WEXITSTATUS(status);
        _result->cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                  resource_usage.ru_utime.tv_usec / 1000 +
                                  resource_usage.ru_stime.tv_sec * 1000 +
                                  resource_usage.ru_stime.tv_usec / 1000);
        _result->memory = resource_usage.ru_maxrss * 1024;

        // get end time
        gettimeofday(&end, NULL);
        _result->real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);

        if (_result->exit_code != 0) {
            _result->result = RUNTIME_ERROR;
        }
        // if signaled
        if (WIFSIGNALED(status) != 0) {
            _result->signal = WTERMSIG(status);
            if (_result->signal == SIGSEGV) {
                if (_config->max_memory != UNLIMITED && _result->memory > _config->max_memory) {
                    _result->result = MEMORY_LIMIT_EXCEEDED;
                }
                else {
                    _result->result = RUNTIME_ERROR;
                }
            }
            else if(_result->signal == SIGUSR1) {
                _result->result = SYSTEM_ERROR;
            }
            else {
                _result->result = RUNTIME_ERROR;
            }
        }
        else {
            if (_config->max_memory != UNLIMITED && _result->memory > _config->max_memory) {
                _result->result = MEMORY_LIMIT_EXCEEDED;
            }
        }
        if (_config->max_real_time != UNLIMITED && _result->real_time > _config->max_real_time) {
            _result->result = REAL_TIME_LIMIT_EXCEEDED;
        }
        if (_config->max_cpu_time != UNLIMITED && _result->cpu_time > _config->max_cpu_time) {
            _result->result = CPU_TIME_LIMIT_EXCEEDED;
        }
	}
	return 0;
}
