#include "ResourceTest.hpp"

namespace ntt
{
    Dictionary<String, ResourceTest *> ResourceTest::s_resources;

    ResourceTest::ResourceTest(const ResourceInfo &info)
        : Resource(info)
    {
        s_resources[info.name] = this;
    }

    ResourceTest::~ResourceTest()
    {
        // s_resources.erase(GetName());
    }

    resource_id_t ResourceTest::LoadImpl()
    {
        loadCalls++;
        return id;
    }

    void ResourceTest::UnloadImpl()
    {
        unloadCalls++;
    }
} // namespace ntt
