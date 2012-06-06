/** \file       Viewers/ModelViewer/Camera.cpp
 *  \brief      Contains the declaration of the camera class.
 *  \author     Rhoot
 */

/*	Copyright (C) 2012 Rhoot <https://github.com/rhoot>

    This file is part of Gw2Browser.

    Gw2Browser is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "Camera.h"

namespace gw2b
{

Camera::Camera()
    : m_pivot(0, 0, 0)
    , m_distance(200)
    , m_yaw(0)
    , m_pitch(0)
{
}

Camera::~Camera()
{
}

XMMATRIX Camera::calculateViewMatrix() const
{
    auto eyePosition           = XMFLOAT4(0, -m_distance, 0, 0);
    XMVECTOR eyePositionVector = ::XMLoadFloat4(&eyePosition);

    // Apply rotations to position
    XMMATRIX rotationMatrix    = this->calculateRotationMatrix();
    eyePositionVector          = ::XMVector4Transform(eyePositionVector, rotationMatrix);

    // Create the view matrix
    auto up                    = XMFLOAT4(0, 0, -1, 0);
    XMVECTOR upVector          = ::XMLoadFloat4(&up);
    XMVECTOR pivotVector       = ::XMLoadFloat3(&m_pivot);
    eyePositionVector          = ::XMVectorAdd(eyePositionVector, pivotVector);

    return ::XMMatrixLookAtLH(eyePositionVector, pivotVector, upVector);
}

XMMATRIX Camera::calculateRotationMatrix() const
{
    // Yaw
    auto yawAxis             = XMFLOAT4(0, 0, 1, 0);
    XMVECTOR yawAxisVector   = ::XMLoadFloat4(&yawAxis);
    XMMATRIX yawMatrix       = ::XMMatrixRotationNormal(yawAxisVector, m_yaw);

    // Pitch
    auto pitchAxis           = XMFLOAT4(1, 0, 0, 0);
    XMVECTOR pitchAxisVector = ::XMLoadFloat4(&pitchAxis);
    XMMATRIX pitchMatrix     = ::XMMatrixRotationNormal(pitchAxisVector, m_pitch);

    return ::XMMatrixMultiply(pitchMatrix, yawMatrix);
}

float Camera::yaw() const
{
    return m_yaw;
}

void Camera::addYaw(float p_yaw)
{
    m_yaw += p_yaw;
}

void Camera::setYaw(float p_yaw)
{
    m_yaw = p_yaw;
}

float Camera::pitch() const
{
    return m_pitch;
}

void Camera::addPitch(float p_pitch)
{
    m_pitch = this->clampPitch(m_pitch + p_pitch);
}

void Camera::setPitch(float p_pitch)
{
    m_pitch = this->clampPitch(p_pitch);
}

float Camera::clampPitch(float p_pitch)
{
    float rotationLimit = (89.0f * XM_PI) / 180.0f;
    return wxMin(rotationLimit, wxMax(-rotationLimit, p_pitch));
}

float Camera::distance() const
{
    return m_distance;
}

void Camera::multiplyDistance(float p_multiplier)
{
    m_distance += (m_distance * p_multiplier * 0.1f);
}

void Camera::setDistance(float p_distance)
{
    m_distance = p_distance;
}

const XMFLOAT3& Camera::pivot() const
{
    return m_pivot;
}

void Camera::pan(float p_x, float p_y)
{
    XMMATRIX rotationMatrix = this->calculateRotationMatrix();

    // Pan speed is based on distance from pivot, so the user doesn't have to move the mouse like 
    // a madman for big meshes
    float panSpeed = 0.001f * m_distance;

    // X axis
    XMFLOAT4 right(1, 0, 0, 0);
    XMVECTOR rightVector = ::XMLoadFloat4(&right);
    rightVector          = ::XMVector4Transform(rightVector, rotationMatrix);

    // Y axis
    XMFLOAT4 up(0, 0, 1, 0);
    XMVECTOR upVector    = ::XMLoadFloat4(&up);
    upVector             = ::XMVector4Transform(upVector, rotationMatrix);

    // Perform the panning
    XMVECTOR pivotVector = ::XMLoadFloat3(&m_pivot);

    rightVector          = ::XMVectorScale(rightVector, p_x * panSpeed);
    upVector             = ::XMVectorScale(upVector, p_y * panSpeed);
    pivotVector          = ::XMVectorAdd(pivotVector, rightVector);
    pivotVector          = ::XMVectorAdd(pivotVector, upVector);

    ::XMStoreFloat3(&m_pivot, pivotVector);
}

void Camera::setPivot(const XMFLOAT3& p_pivot)
{
    m_pivot = p_pivot;
}


}; // namespace gw2b
