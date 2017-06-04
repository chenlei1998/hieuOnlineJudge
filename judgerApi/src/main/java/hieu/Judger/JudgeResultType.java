package hieu.Judger;

/**
 * Created by chenlei on 2017/6/4.
 */
public enum JudgeResultType {
    // 成功
    SUCCESS,
    // 答案错误
    WRONG_ANSWER,
    // 超时
    CPU_TIME_LIMIT_EXCEEDED,
    // 超时
    REAL_TIME_LIMIT_EXECEDED,
    // 超内存
    MEMORY_LIMIT_EXCEEDED,
    // 运行时错误
    RUNTIME_ERROR,
    // 系统错误
    SYSTEM_ERROR,
    // 编译错误
    COMPILE_ERROR
}