package hieu.compiler;

import hieu.utils.Utils;

import java.io.File;

/**
 * Created by chenlei on 2017/5/31.
 */
public class CompileResult {
    private boolean success;
    private File outputDirectory;
    private String errorMsg;
    private CompileType type;

    public CompileResult(File outputDirectory, boolean success) {
        this.outputDirectory = outputDirectory;
        this.success = success;
    }

    public CompileResult(boolean success) {
        this.success = success;
    }

    public void setOutputDirectory(File outputDirectory) {
        this.outputDirectory = outputDirectory;
    }

    public File getOutputDirectory() {
        return outputDirectory;
    }

    public boolean isSuccess() {
        return success;
    }

    public CompileType getCompileType() {
        return type;
    }

    public void setCompileType(CompileType type) {
        this.type = type;
    }

    public String getErrorMsg() {
        return errorMsg;
    }

    public void setErrorMsg(String errorMsg) {
        this.errorMsg = errorMsg;
    }

}
