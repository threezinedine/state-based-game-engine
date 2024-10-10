#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    /**
     * Starting of the naming system which help to create the entities' names without
     *      duplication. This function must be called before other functions in this module.
     *
     * All the internal states will be reset to the initial state.
     */
    void NamingSystemInit();

    /**
     * When using this function, the additional information will be added to the name for
     *      distinguishing the entities with the same name with the pattern "(n)" where n is
     *      the number of the entity with the same name. Any name which matches the pattern
     *      "{} (n)" will be considered as the same name and be treated as the predefined
     *      rule.
     *
     * Example:
     *      ```C++
     *      auto name = GetName("Player");
     *      auto name2 = GetName("Player ");    /// The name2 will be "Player (1)"
     *                                          /// the space will be irgnored
     *      auto name3 = GetName("Player"); /// The name3 will be "Player (2)"
     *      auto name4 = GetName("Player (4)"); /// The name4 will be "Player (3)"
     *      auto name5 = GetName("Player"); /// The name5 will be "Player (4)"
     *      ```
     *
     * @param name The name of the entity
     */
    String GetName(const String &name);

    /**
     * Reset the states of the name, now every name can be used without any additional
     *      information.
     *
     * Example:
     *      ```C++
     *      auto name = GetName("Player");
     *      auto name2 = GetName("Player ");    /// The name2 will be "Player (1)"
     *                                          /// the space will be irgnored
     *      auto name3 = GetName("Player"); /// The name3 will be "Player (2)"
     *      auto name4 = GetName("Player (4)"); /// The name4 will be "Player (3)"
     *
     *      NamingSystemReset();
     *      auto name5 = GetName("Player (4)"); /// The name5 will be "Player"
     *      auto name6 = GetName("Player"); /// The name5 will be "Player (1)"
     *      ```
     */
    void NamingSystemReset();

    /**
     * Reset all the internal states to the initial state.
     * Should be used when the application is ended.
     */
    void NamingSystemShutdown();
} // namespace ntt