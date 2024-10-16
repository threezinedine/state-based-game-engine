#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/time.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/core/formatter.hpp>
#include <fstream>

namespace ntt
{
#define THIS(exp) this->m_impl->exp
#define DEFAULT_SECTION "Default"
#define SEPARATOR "---------------------"

    namespace
    {
        String s_currentSection = DEFAULT_SECTION;
        String s_outputFolder = RelativePath(".");
        b8 s_hasWritten = FALSE;
        b8 s_test = FALSE;
        u8 s_indent = -1;
        Scope<std::ofstream> s_stream;
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

        String data = format("[START] {} in {}:{}",
                             THIS(funcName).RawString(),
                             THIS(file).RawString(),
                             THIS(line));

        for (u8 i = 0; i < s_indent; i++)
        {
            data = format("\t{}", data.RawString());
        }

        String logFile = JoinPath({CurrentDirectory(), (s_currentSection + ".prof.txt")});

        try
        {
            if (!s_hasWritten)
            {
                s_stream = CreateScope<std::ofstream>(
                    logFile.RawString(),
                    std::ios::out | std::ios::app);
                s_hasWritten = TRUE;
            }

            if (s_stream == nullptr)
            {
                return;
            }

            if (s_stream->is_open())
            {
                *s_stream << data.RawString() << std::endl;
            }
        }
        catch (...)
        {
            NTT_ENGINE_WARN("Cannot open file {}", logFile);
        }
    }

    Profiling::~Profiling()
    {
        if (s_test)
        {
            return;
        }

        auto duration = THIS(timer).GetMilliseconds();

        String data = ntt::format("[END]   {} in {}:{} - {}ms",
                                  THIS(funcName),
                                  THIS(file),
                                  THIS(line),
                                  duration);

        for (u8 i = 0; i < s_indent; i++)
        {
            data = format("\t{}", data);
        }

        try
        {
            if (s_stream == nullptr)
            {
                return;
            }

            if (s_stream->is_open())
            {
                *s_stream << data.RawString() << std::endl;
            }
        }
        catch (...)
        {
            NTT_ENGINE_WARN("Cannot write to the file");
        }

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
            NTT_ENGINE_TRACE("End Section {}\n{}", s_currentSection, SEPARATOR);
        }

        NTT_ENGINE_TRACE("Begin Section {}\n{}", section, SEPARATOR);

        if (s_hasWritten)
        {
            try
            {
                s_stream->close();
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
            s_stream->close();
        }

        if (s_currentSection != DEFAULT_SECTION)
        {
            NTT_ENGINE_TRACE("End Section {0}\n{1}", s_currentSection, SEPARATOR);
        }
    }
} // namespace ntt
