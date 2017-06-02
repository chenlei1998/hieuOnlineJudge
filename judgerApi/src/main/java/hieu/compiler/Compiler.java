package hieu.compiler;

import hieu.utils.Utils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
/**
 * @author 陈磊
 */
public class Compiler {
    public static final String DEFAULT_JAVA_NAME = "Main";
    public static final String DEFAULT_C_LANG_NAME = "clang";
    public static final String DEFAULT_C_PLUSPLUS_NAME = "c_plus_plus";

    protected static CompileResult compile(List<String> commands) throws IOException {
        ProcessBuilder processBuilder = new ProcessBuilder(commands);
        Process process = null;
        CompileResult result = null;
        try {
            process = processBuilder.start();
            String errorMsg  = Utils.insToString(process.getErrorStream());
            process.waitFor();
            int exitCode = process.exitValue();
            if (exitCode == 0) {
                return new CompileResult(true);
            } else {
                result = new CompileResult(false);
                result.setErrorMsg(errorMsg);
                return result;
            }
        } catch (InterruptedException e) {
            return new CompileResult(false);
        }
    }

    public static CompileResult compileJavaProgram(File outputDirectory, String sourceCode) throws IOException {
        String filename = DEFAULT_JAVA_NAME + ".java";
        Utils.writeStringToFile(new File(outputDirectory, filename), sourceCode);
        List<String> commands = new ArrayList<>();
        commands.add("javac");
        commands.add("-d");
        commands.add(String.format("\"%s\"", outputDirectory.getCanonicalPath()));
        commands.add(String.format("\"%s\"", new File(outputDirectory, filename).getCanonicalPath()));
        CompileResult result = compile(commands);
        result.setOutputDirectory(outputDirectory);
        result.setCompileType(CompileType.JAVA);
        return result;
    }

    public static CompileResult compileClangProgram(File outputDirectory, String sourceCode) throws IOException {
        String filename = DEFAULT_C_LANG_NAME + ".c";
        Utils.writeStringToFile(new File(outputDirectory, filename), sourceCode);
        List<String> commands = new ArrayList<>();
        commands.add("gcc");
        commands.add(String.format("\"%s\"", new File(outputDirectory, filename).getCanonicalPath()));
        commands.add("-o");
        commands.add(String.format("\"%s\"", new File(outputDirectory, DEFAULT_C_LANG_NAME).getCanonicalPath()));
        commands.add("-x");
        commands.add("c");
        commands.add("-std=c99");
        commands.add("-fno-asm");
        commands.add("-O2");
        commands.add("-Wall");
        commands.add("-lm");
        commands.add("--static");
        commands.add("-DONLINE_JUDGE");
        CompileResult result = compile(commands);
        result.setOutputDirectory(outputDirectory);
        result.setCompileType(CompileType.CLANG);
        return result;
    }

    public static CompileResult compileCPlusPlusProgram(File outputDirectory, String sourceCode) throws IOException {
        String filename = "Main.cpp";
        Utils.writeStringToFile(new File(outputDirectory, filename), sourceCode);
        List<String> commands = new ArrayList<>();
        commands.add("g++");
        commands.add(String.format("\"%s\"", new File(outputDirectory, filename).getCanonicalPath()));
        commands.add("-o");
        commands.add(String.format("\"%s\"", new File(outputDirectory, DEFAULT_C_PLUSPLUS_NAME).getCanonicalPath()));
        commands.add("-x");
        commands.add("c++");
        commands.add("-std=c++11");
        commands.add("-fno-asm");
        commands.add("-O2");
        commands.add("-Wall");
        commands.add("-lm");
        commands.add("--static");
        commands.add("-DONLINE_JUDGE");
        CompileResult result = compile(commands);
        result.setOutputDirectory(outputDirectory);
        result.setCompileType(CompileType.CPLUSPLUS);
        return result;
    }
}
