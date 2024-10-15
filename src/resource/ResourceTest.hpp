#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt
{
    /**
     * This class is only for testing purposes.
     *      Do not use it in production code.
     */
    class ResourceTest : public Resource
    {
    public:
        ResourceTest(const ResourceInfo &info);

        virtual ~ResourceTest() override;

        u8 loadCalls = 0;
        u8 unloadCalls = 0;
        resource_id_t id = 0;

        static Dictionary<String, ResourceTest *> s_resources;

    protected:
        resource_id_t LoadImpl() override;

        virtual void UnloadImpl() override;
    };
} // namespace ntt

#define GET_LOAD_CALL(name) ResourceTest::s_resources[name]->loadCalls
#define GET_UNLOAD_CALL(name) ResourceTest::s_resources[name]->unloadCalls
#define SET_RESOURCE_ID(name, id) ResourceTest::s_resources[name]->id = id
