package hieu.Judger;

import java.io.File;

/**
 * Created by chenlei on 2017/6/4.
 */
public abstract class abstractJudger {
    public static final String OUTPUT_FILENAME = "output.txt";
    public static final String INPUT_FILENAME = "input.txt";
    public static final String ANSWER_FILENAME = "answer.txt";

    public JudgeResult build(File workDirectory, int maxCpuTime, int maxRealTime, int maxMemorySize, int maxStackSize) {
        File output = new File(workDirectory, OUTPUT_FILENAME);
        File answer = new File(workDirectory, ANSWER_FILENAME);
        if (output.exists() && answer.exists()) {

        }
    }

    public abstract JudgeResult judge(File workDirectory);


}
