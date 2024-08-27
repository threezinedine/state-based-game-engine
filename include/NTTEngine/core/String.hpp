#pragma once

// TODO: Redesign the String class
namespace ntt
{
    /**
     * The String interface for all string in the engine.
     *      This String interface provides multiple added functionalities
     *      for the string manipulation, such as:
     *          - Concatenation
     *          - Substring
     *          - Strip,
     *          - Split,
     *          - etc.
     */
    class String
    {
    public:
        /**
         * The constructor which receives the normal string `const char*`
         *      and store it
         */
        String(const char *str);
        /**
         * The constructor which stores empty string
         *
         */
        String();

        ~String();

        /**
         * Get the raw c++ string
         *
         * @return `const char*` string in c++
         */
        inline const char *ToCStr() const { return m_Str; }

    private:
        const char *m_Str;
    };
} // namespace ntt
