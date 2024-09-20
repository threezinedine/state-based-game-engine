#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/time.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt
{
    using namespace log;

#define THIS(exp) m_impl->exp
#define DEFAULT_SECTION "Default"
#define SEPARATOR "---------------------"

    namespace
    {
        String s_currentSection = DEFAULT_SECTION;
        String s_outputFolder = RelativePath(".");
        b8 s_hasWritten = FALSE;
        b8 s_test = FALSE;
        u8 s_indent = -1;
    } // namespace

    class Profiling::Impl
    {
    public:
        String funcName;
        String file;
        u32 line;
        Timer timer;
    };

    Profiling::Profiling(String funcName, String file, u32 line)
    {
        if (s_test)
        {
            return;
        }

        m_impl = CreateScope<Impl>();
        s_indent++;

        THIS(funcName) = funcName;
        THIS(file) = GetFileName(file, TRUE);
        THIS(line) = line;

        THIS(timer).Reset();

        String data = fmt::format("[START] {} in {}:{}",
                                  THIS(funcName),
                                  THIS(file),
                                  THIS(line));

        for (u8 i = 0; i < s_indent; i++)
        {
            data = fmt::format("\t{}", data);
        }

        String logFile = JoinPath({s_outputFolder, (s_currentSection + ".prof.txt")});

        try
        {
            if (!s_hasWritten)
            {
                OpenFile(logFile, FALSE);
                s_hasWritten = TRUE;
            }

            Write(data + "\n");
        }
        catch (...)
        {
            NTT_ENGINE_WARN("Cannot open file {0}", logFile);
        }
    }

    Profiling::~Profiling()
    {
        if (s_test)
        {
            return;
        }

        auto duration = THIS(timer).GetMilliseconds();

        String data = fmt::format("[END]   {0} in {1}:{2} - {3}ms",
                                  THIS(funcName),
                                  THIS(file),
                                  THIS(line),
                                  duration);

        for (u8 i = 0; i < s_indent; i++)
        {
            data = fmt::format("\t{}", data);
        }

        Write(data + "\n");

        s_indent--;
    }

    void ProfilingInit(String outputFolder, b8 test)
    {
        s_outputFolder = outputFolder;
        s_hasWritten = FALSE;
        s_indent = -1;
        s_test = test;
    }

    void ProfilingBegin(String section)
    {
        if (s_test)
        {
            return;
        }

        if (s_currentSection != DEFAULT_SECTION)
        {
            NTT_ENGINE_TRACE("End Section {0}\n{1}", s_currentSection, SEPARATOR);
        }

        NTT_ENGINE_TRACE("Begin Section {0}\n{1}", section, SEPARATOR);

        if (s_hasWritten)
        {
            try
            {
                CloseFile();
            }
            catch (...)
            {
                NTT_ENGINE_WARN("Cannot close the file");
            }
        }

        s_hasWritten = FALSE;
        s_currentSection = section;
        s_indent = -1;
    }

    void ProfilingShutdown()
    {
        if (s_test)
        {
            return;
        }

        if (s_hasWritten)
        {
            CloseFile();
        }

        if (s_currentSection != DEFAULT_SECTION)
        {
            NTT_ENGINE_TRACE("End Section {0}\n{1}", s_currentSection, SEPARATOR);
        }
    }
} // namespace ntt
