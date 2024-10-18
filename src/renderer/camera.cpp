#include <NTTEngine/renderer/Camera.hpp>

namespace ntt
{
    Camera::Camera(position_t outputPosX, position_t outputPosY, size_t outputWidth, size_t outputHeight)
        : ouputPos(outputPosX, outputPosY), outputSize(outputWidth, outputHeight)
    {
        Reset();
    }

    void Camera::Reset()
    {
        camPos = Position(outputSize.width / 2, outputSize.height / 2);
        camZoom = 1.0f;
    }

    position_t Camera::TransformX(position_t x) const
    {
        return (x + camPos.x - outputSize.width / 2) * camZoom;
    }

    position_t Camera::TransformY(position_t y) const
    {
        return (y + camPos.y - outputSize.height / 2) * camZoom;
    }

    ntt_size_t Camera::TransformWidth(ntt_size_t width) const
    {
        return width * camZoom;
    }

    ntt_size_t Camera::TransformHeight(ntt_size_t height) const
    {
        return height * camZoom;
    }

    position_t Camera::ReverseTransformX(position_t x) const
    {
        return x / camZoom + outputSize.width / 2 - camPos.x;
    }

    position_t Camera::ReverseTransformY(position_t y) const
    {
        return y / camZoom + outputSize.height / 2 - camPos.y;
    }

    ntt_size_t Camera::ReverseTransformWidth(ntt_size_t width) const
    {
        return width / camZoom;
    }

    ntt_size_t Camera::ReverseTransformHeight(ntt_size_t height) const
    {
        return height / camZoom;
    }

    void Camera::ShiftCamera(Position startPos, Position targetPos)
    {
        camPos.x = (startPos.x * (1 - camZoom) + outputSize.width / 2 * camZoom) / camZoom;
        camPos.y = (startPos.y * (1 - camZoom) + outputSize.height / 2 * camZoom) / camZoom;
    }
} // namespace ntt
