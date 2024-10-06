#pragma once
#include <NTTEngine/defines.hpp>
#include <string>
#include <stack>

namespace ntt
{
    template <typename T>
    class Stack : public std::stack<T>
    {
    public:
        const char *ToString() const
        {
            static std::string buffer = "[";
            buffer = "[";

            // Create a copy of the stack to iterate through
            std::stack<T> tempStack = *this;
            std::vector<T> elements;

            // Transfer elements to a vector to maintain order
            while (!tempStack.empty())
            {
                elements.push_back(tempStack.top());
                tempStack.pop();
            }

            // Iterate through the vector in reverse order
            for (ntt_size_t i = elements.size(); i > 0; --i)
            {
                buffer += std::to_string(elements[i - 1]);
                if (i != 1)
                {
                    buffer += ", ";
                }
            }

            buffer += "]";
            return buffer.c_str();
        }
    };
} // namespace ntt
