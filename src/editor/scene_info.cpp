#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/ecs/ecs_helper.hpp>

namespace ntt
{
    void SceneInfo::ReloadEntities()
    {
        for (auto &entity : m_entityIDs)
        {
            ECSDeleteEntity(entity);
        }

        List<ResourceInfo> resourcesInfo;
        JSON config = ReadFile(filePath);
        List<JSON> entitiesCfg = config.GetList<JSON>("entities");
        entities.clear();
        m_entityIDs.clear();

        for (auto &entity : entitiesCfg)
        {
            auto entityInfo = CreateRef<EntityInfo>();
            entityInfo->FromJSON(entity);
            entities.push_back(entityInfo);

            m_entityIDs.push_back(ECSCreateEntity(entityInfo->name, entityInfo->components));
        }
    }

    void SceneInfo::SaveEntitiesInfo()
    {
        JSON config = ReadFile(filePath);
        List<JSON> entitiesJSON =
            entities
                .Map<JSON>([](const Ref<EntityInfo> &info, ...) -> JSON
                           { return info->ToJSON(); });

        config.Set("entities", JSON::FromList(entitiesJSON));

        OpenFile(filePath);
        Write(config.ToString());
        CloseFile();
    }

    void SceneInfo::AddEntity()
    {
        auto entity = CreateRef<EntityInfo>();
        entity->name = "Entity";
        entities.push_back(entity);
    }

    JSON SceneInfo::ToJSON()
    {
        JSON scene = JSON("{}");
        scene.Set("sceneName", sceneName);
        scene.Set("filePath", filePath);

        List<JSON> resourceJSONs =
            resources.Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                                { return info.ToJSON(); });

        scene.Set("resources", JSON::FromList(resourceJSONs));

        List<JSON> entitiesJSON =
            entities
                .Map<JSON>([](const Ref<EntityInfo> &info, ...) -> JSON
                           { return info->ToJSON(); });

        scene.Set("entities", JSON::FromList(entitiesJSON));

        return scene;
    }

    void SceneInfo::FromJSON(const JSON &scene)
    {
        sceneName = scene.Get<String>("sceneName");
        filePath = scene.Get<String>("filePath");
    }

    void SceneInfo::ReloadResourceInfo()
    {
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
