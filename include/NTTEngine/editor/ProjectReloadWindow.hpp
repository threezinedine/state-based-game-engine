#pragma once

namespace ntt
{
    class ProjectReloadWindow
    {
    public:
        virtual ~ProjectReloadWindow() = default;

        virtual void OnReloadProject() = 0;
    };
} // namespace ntt
