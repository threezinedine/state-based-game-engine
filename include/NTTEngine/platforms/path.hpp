#pragma once
#include <NTTEngine/structures/string.hpp>

namespace ntt
{

    enum class PathType
    {
        NTT_SOURCE = 0,
        NTT_BINARY = 1,
        NTT_ENGINE = 2,
    };

    /**
     * Configure Source path for whole application
     *
     * @param path: The path of the source if the path does
     *      not exist then the warning will be logged and the
     *      nothing will be changed
     */
    void ConfigureSourcePath(const String &path);

    /**
     * Get the source path via the type of path, only a certain type
     *      of path can be stored, at the default, the source path
     *      will be the source dir which is configured by ConfigureSourcePath
     *
     * @param type: The type of path, if the type is larger than the the storage
     *      then the source dir will be returned
     * @return the path of the source
     */
    const String &GetStoredPath(PathType type = PathType::NTT_SOURCE);

    /**
     * Configure the path for a certain type of path.
     * If the path is not exist or be a file then the warning will be logged
     *      and nothing will be changed
     *
     * @param type: The type of path, if the type is larger than the the storage
     *      then the source dir will be returned
     * @param path: The path of the source
     */
    void SetSetPath(PathType type, const String &path);

    /**
     * Get the file with the base path is the source path
     */

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
     * Create the folder if not exist
     */
    void CreateFolder(const String &path);

    /**
     * Open a file for writing new content or appending content
     *      to the file
     * Every Write operation after this OpenFile will be written
     *      into to file
     * If the file does not exist then the file will be created
     * When a file which is already opened (or another file) then
     *      the new file will be opened and the old file will be closed
     *
     * @param path: The path of the file
     * @param append: If true then the content will be appended to the file
     *          else the content will be overwritten
     */
    void OpenFile(const String &path, b8 append = false);

    /**
     * The data will be appended to the file if the file is opened
     * If no file is opened then the warning will be logged and
     *      nothing will be written
     *
     * This Write function must be called after the OpenFile function
     *      and before the CloseFile function
     *
     * @param content: The content to be written to the file
     */
    void Write(const String &content);

    /**
     * Close the file if the file is opened
     * If no file is opened then the warning will be logged and
     *      nothing will be closed
     */
    void CloseFile();

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
     * Get current running directory (the location of the .exe file)
     */
    String CurrentDirectory();

    /**
     * Get folder of the given file path
     */
    String GetFileFolder(const String &file);

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
