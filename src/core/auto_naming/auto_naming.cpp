#include <NTTEngine/core/auto_naming.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt
{
    namespace
    {
        Dictionary<String, u32> s_nameCount;
    }

    void NamingSystemInit()
    {
        s_nameCount.clear();
    }

    String GetName(const String &name)
    {
        String temp = name;
        temp.Trim();

        if (s_nameCount.Contains(temp))
        {
            s_nameCount[temp]++;
            return format("{} ({})", temp, s_nameCount[temp]);
        }
        else
        {
            if (temp.MatchPattern("@ (@)"))
            {
                i32 index = temp.FindFirst(" (");
                auto subStr = temp.SubString(0, index);

                if (s_nameCount.Contains(subStr))
                {
                    s_nameCount[subStr]++;
                    return format("{} ({})", subStr, s_nameCount[subStr]);
                }
                else
                {
                    s_nameCount[subStr] = 0;
                    return temp;
                }
            }
            else
            {
                s_nameCount[temp] = 0;
                return temp;
            }
        }

        return temp;
    }

    void NamingSystemShutdown()
    {
        s_nameCount.clear();
    }
} // namespace ntt
