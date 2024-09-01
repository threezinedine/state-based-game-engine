#pragma once
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    /**
     * Configure Source path for whole application
     *
     * @param path: The path of the source if the path does
     *      not exist then the warning will be logged and the
     *      nothing will be changed
     */
    void ConfigureSourcePath(const String &path);

    /**
     * Get the file with the base path is the source path
     *      which is configured by ConfigureSourcePath
     * If the source path is not configured then the current
     *      working directory will be the base path
     *
     * @param path: The path of the file
     */
    String RelativePath(const String &path);

    /**
     * Checking whether the file/directory exists or not
     *
     * @param path: The path of the file/directory
     * @return true if the file/directory exists, else false
     */
    b8 IsExist(const String &path);

    /**
     * Read the content from the file if that file does not exist
     *      then an empty string will be returned and the warning
     *      will be logged
     *
     * @param path: The path of the file
     */
    String ReadFile(const String &path);

    /**
     * Write the content to the file
     * If the file does not exist then the file will be created
     *
     * @param path: The path of the file
     * @param content: The content to be written to the file
     * @param append: If true then the content will be appended to the file
     *          else the content will be overwritten
     */
    String WriteFile(const String &path, const String &content, b8 append = false);

    /**
     * Get filename from the path of the file
     *
     * @param path: The path of the file
     *      - Ex: "C:/Users/Acer/Games Dev/state-based-game-engine
     *             /examples/Hero/assets/images/button.png" will return "button.png"
     * @param containBase: If true then the base path will be the source path which is
     *      configured by ConfigureSourcePath, else only the filename will be returned
     *      (default is false)
     */
    String GetFileName(const String &path, b8 containBase = false);

    /**
     * Create a directory/file with a series of directories and the eventual file
     *      and the path will be created step by step
     * If there are any the path which is not the final one contains the '.' then
     *      the warning will be logged and it becomes the final path, the remaining
     *      elements will be ignored
     * @param paths: The list of directories and the eventual file (optional)
     *      - Ex: {"__test__", "config", "file.txt"} will create the directory
     *          __test__/config and the file file.txt
     * @param containBase: If true then the base path will be the source path which is
     *      configured by ConfigureSourcePath, else the current working directory will
     *      be only the paths you provided (default is true)
     */
    String JoinPath(List<String> paths, b8 useBase = true);
} // namespace ntt
