#include <gtest/gtest.h>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/ecs/ecs_helper.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

class ECSHelperTest : public ::testing::Test
{
protected:
};

TEST_F(ECSHelperTest, TestGeometry)
{
    Ref<Geometry> geometryOrigin = CreateRef<Geometry>();
    geometryOrigin->pos = Position(100, 200);
    geometryOrigin->size = Size(50, 50);
    geometryOrigin->rotation = 0.0;
    geometryOrigin->priority = 0;
    geometryOrigin->color = RGBAColor(255, 255, 255, 255);

    JSON json = geometryOrigin->ToJSON();

    JSON componentsJSON = JSON("{}");
    componentsJSON.Set("Geometry", json);

    auto components = ECS_From(componentsJSON);

    if (components.size() == 0)
    {
        FAIL() << "The components list is empty";
        return;
    }

    Ref<Geometry> geometry = std::static_pointer_cast<Geometry>(components[typeid(Geometry)]);

    ASSERT_EQ(geometry->pos.x, 100);
    ASSERT_EQ(geometry->pos.y, 200);

    ASSERT_EQ(geometry->size.width, 50);
    ASSERT_EQ(geometry->size.height, 50);

    ASSERT_FLOAT_EQ(geometry->rotation, 0.0);

    ASSERT_EQ(geometry->priority, 0);

    ASSERT_EQ(geometry->color.r, 255);
    ASSERT_EQ(geometry->color.g, 255);
    ASSERT_EQ(geometry->color.b, 255);
    ASSERT_EQ(geometry->color.a, 255);
}