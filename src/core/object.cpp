#include <NTTEngine/core/object.hpp>
#include <iostream>

namespace ntt
{
#define THIS(exp) this->m_impl->exp

    class Object::Impl
    {
    public:
    };

    Object::Object()
    {
        this->m_impl = CreateScope<Object::Impl>();
        CREATE_NEW();
    }

    Object::~Object()
    {
        DELETE_OBJ();
    }
} // namespace ntt