#pragma once

namespace ntt
{
    class SceneReloadWindow
    {
    public:
        virtual ~SceneReloadWindow() = default;

        virtual void OnReloadScene() = 0;
    };
} // namespace ntt
