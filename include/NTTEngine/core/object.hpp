#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    /**
     * Base object for all objects in the engine, every object (which is not a primitive type)
     *      must inherit from this class, this class will provide some basic functionalities
     *      for all objects in the engine
     */
    class Object
    {
    public:
        Object();
        virtual ~Object();

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
