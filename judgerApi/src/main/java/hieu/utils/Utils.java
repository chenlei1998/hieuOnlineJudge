package hieu.utils;

import java.io.*;
import java.util.Random;

/**
 * 常用函数集合
 * @author 陈磊
 */
public class Utils {

    /**
     * 从InputStream中读取字符串
     * @param inputStream 输入流
     * @return 读取成功返回读取到的字符串，读取失败返回空字符串
     * @throws IOException
     */
    public static String insToString(InputStream inputStream) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        StringBuilder builder = new StringBuilder();
        char[] buffer = new char[1024];
        int readNum;
        while ((readNum = reader.read(buffer)) != -1) {
            builder.append(buffer, 0, readNum);
        }
        return builder.toString();
    }

    /**
     * 递归删除文件夹
     * @param directory 待删除的文件夹
     * @return 返回<code>false</code>为删除失败，返回<code>true</code>为删除成功
     */
    public static boolean deleteDirectory(File directory) {
        if (directory == null || !directory.isDirectory() || !directory.exists())
            return false;
        for (File file : directory.listFiles()) {
            if (file.isDirectory()) {
                deleteDirectory(file);
            } else {
                file.delete();
            }
        }
        return directory.delete();
    }

    /**
     * 获取指定长度的随机字符串
     * @param len 要获取的随机字符串长度
     * @return 当len==0时，返回空串
     */
    public static String getRandomString(int len) {
        Random random = new Random();
        random.setSeed(System.currentTimeMillis());
        StringBuilder builder = new StringBuilder();
        String str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < len; i++) {
            builder.append(str.charAt(random.nextInt(str.length())));
        }
        return builder.toString();
    }

    /**
     * 在指定文件夹下创建一个临时文件夹
     * @param rootDirectory 在此文件夹下创建临时文件夹
     * @return 创建好的临时文件夹
     */
    public static File makeTempDirectory(File rootDirectory) {
        File directory = null;
        do {
            directory = new File(rootDirectory, getRandomString(10));
        } while(directory.exists());
        directory.mkdir();
        return directory;
    }

    /**
     * 把字符串写到指定文件中
     * @throws IOException
     */
    public static void writeStringToFile(File destFile, String str) throws IOException {
        FileWriter writer = new FileWriter(destFile);
        writer.write(str);
        writer.flush();
        writer.close();
    }
}