#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/ecs/ecs_helper.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    void SceneInfo::RemoveAllEntities()
    {
        PROFILE_FUNCTION();
        for (auto &entity : m_entityIDs)
        {
            ECSDeleteEntity(entity);
        }
    }

    void SceneInfo::ReloadEntities()
    {
        PROFILE_FUNCTION();
        RemoveAllEntities();

        List<ResourceInfo> resourcesInfo;
        JSON config = ReadFile(filePath);
        List<JSON> entitiesCfg = config.GetList<JSON>("entities");
        entities.clear();
        m_entityIDs.clear();

        for (auto &entity : entitiesCfg)
        {
            EntityInfo entityInfo;
            entityInfo.FromJSON(entity);
            entities.push_back(entityInfo);

            m_entityIDs.push_back(ECSCreateEntity(entityInfo.name, entityInfo.components));
        }
    }

    void SceneInfo::SaveEntitiesInfo()
    {
        PROFILE_FUNCTION();
        JSON config = ReadFile(filePath);
        List<JSON> entitiesJSON =
            entities
                .Map<JSON>([](const EntityInfo &info, ...) -> JSON
                           { return info.ToJSON(); });

        config.Set("entities", JSON::FromList(entitiesJSON));

        OpenFile(filePath);
        Write(config.ToString());
        CloseFile();
    }

    void SceneInfo::AddEntity()
    {
        PROFILE_FUNCTION();
        EntityInfo entity;
        entity.name = "Entity";
        entities.push_back(entity);
    }

    JSON SceneInfo::ToJSON()
    {
        PROFILE_FUNCTION();
        JSON scene = JSON("{}");
        scene.Set("sceneName", sceneName);
        scene.Set("filePath", filePath);

        List<JSON> resourceJSONs =
            resources.Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                                { return info.ToJSON(); });

        scene.Set("resources", JSON::FromList(resourceJSONs));

        List<JSON> entitiesJSON =
            entities
                .Map<JSON>([](const EntityInfo &info, ...) -> JSON
                           { return info.ToJSON(); });

        scene.Set("entities", JSON::FromList(entitiesJSON));

        return scene;
    }

    void SceneInfo::FromJSON(const JSON &scene)
    {
        PROFILE_FUNCTION();
        sceneName = scene.Get<String>("sceneName");
        filePath = scene.Get<String>("filePath");
    }

    void SceneInfo::ReloadResourceInfo()
    {
        PROFILE_FUNCTION();
        resources.clear();
        JSON config = ReadFile(filePath);

        List<JSON> resourcesCfg = config.GetList<JSON>("resources");

        for (auto &resource : resourcesCfg)
        {
            ResourceInfo info;
            info.From(resource);
            resources.push_back(info);
        }
    }

    void SceneInfo::SaveResourceInfo()
    {
        PROFILE_FUNCTION();
        JSON config = ReadFile(filePath);
        List<JSON> resourceJSONs =
            resources.Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                                { return info.ToJSON(); });

        config.Set("resources", JSON::FromList(resourceJSONs));

        OpenFile(filePath);
        Write(config.ToString());
        CloseFile();
    }

} // namespace ntt
