package hieu.Judger;

import hieu.compiler.CompileResult;

import java.io.File;

/**
 * 判题器，使用沙箱(sandboxRunner)进行结果判定
 * @author 陈磊
 * */
public interface Judger {
    JudgeResult judge(File input, File answer, CompileResult compileResult);
}