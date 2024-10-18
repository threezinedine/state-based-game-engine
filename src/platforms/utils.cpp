#include <NTTEngine/platforms/utils.hpp>
#include <windows.h>

namespace ntt
{
    void ReloadProgram(const String &program)
    {
        LPSTR path = (LPSTR)program.RawString().c_str();
        GetModuleFileNameA(NULL, path, MAX_PATH);

        // Set up the startup info and process info structures
        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        // Create a new process for the current executable
        if (CreateProcessA(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            // Successfully created the process, close handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // Exit the current program
            ExitProcess(0);
        }
        else
        {
            // Handle error
            MessageBoxA(NULL, "Failed to restart the program", "Error", MB_OK | MB_ICONERROR);
        }
    }
} // namespace ntt
