#ifndef SANDBOX
#define SANDBOX

#define FORK_FAILED "fork failed"
#define WAIT_FAILED "wait failed"
#define EXECVE_FAILED "execve failed"
#define DUP2_FAILED "dup2 failed"
#define SETRLIMIT_FAILED "setrlimit failed"
#define PTHREAD_FAILED "pthread failed"
#define LOAD_SECCOMP_FAILED_STR "load seccomp failed"
#define LOAD_SECCOMP_FAILED -1
#define UNLIMITED -1
#define ARGS_MAX_NUMBER 256
#define ENV_MAX_NUMBER 256
#define MSGLEN 128

#define CHILD_ERROR_RETURN(msg)\
    {\
        fflush(stdout);\
        fflush(stderr);\
        fclose(input_file);\
        fclose(output_file);\
        fclose(error_file);\
        _result->error_msg = msg;\
        raise(SIGUSR1);\
        return -1;\
    }

#define RUN_ERROR_RETURN(msg)\
    {\
        _result->result = SYSTEM_ERROR;\
        _result->error_msg = msg;\
        return;\
    }
    
enum
{
    SUCCESS = 0,
    WRONG_ANSWER = 1,
    CPU_TIME_LIMIT_EXCEEDED = 2,
    REAL_TIME_LIMIT_EXCEEDED = 3,
    MEMORY_LIMIT_EXCEEDED = 4,
    RUNTIME_ERROR = 5,
    SYSTEM_ERROR = 6,
};

struct config
{
    int max_cpu_time;
    int max_real_time;
    long max_memory;
    long max_stack;
    int max_process_number;
    long max_output_size;
    char *exe_path;
    char *input_path;
    char *output_path;
    char *error_path;
    char *args[ARGS_MAX_NUMBER];
    char *env[ENV_MAX_NUMBER];
    char *seccomp_rule_name;
};

struct result
{
    int cpu_time;
    int real_time;
    long memory;
    int signal;
    int exit_code;
    int result;
    char *error_msg;
};

struct timeout_killer_args
{
    int pid;
    int timeout;
};
void run(struct config *_config, struct result *_result);
#endif