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
    : mPivot(0, 0, 0)
    , mDistance(200)
    , mYaw(0)
    , mPitch(0)
{
}

Camera::~Camera()
{
}

XMMATRIX Camera::CalculateViewMatrix() const
{
    XMFLOAT4 eyePosition       = XMFLOAT4(0, -mDistance, 0, 0);
    XMVECTOR eyePositionVector = ::XMLoadFloat4(&eyePosition);

    // Apply rotations to position
    XMMATRIX rotationMatrix = this->CalculateRotationMatrix();
    eyePositionVector       = ::XMVector4Transform(eyePositionVector, rotationMatrix);

    // Create the view matrix
    XMFLOAT4 up          = XMFLOAT4(0, 0, -1, 0);
    XMVECTOR upVector    = ::XMLoadFloat4(&up);
    XMVECTOR pivotVector = ::XMLoadFloat3(&mPivot);
    eyePositionVector    = ::XMVectorAdd(eyePositionVector, pivotVector);

    return ::XMMatrixLookAtLH(eyePositionVector, pivotVector, upVector);
}

XMMATRIX Camera::CalculateRotationMatrix() const
{
    // Yaw
    XMFLOAT4 yawAxis       = XMFLOAT4(0, 0, 1, 0);
    XMVECTOR yawAxisVector = ::XMLoadFloat4(&yawAxis);
    XMMATRIX yawMatrix     = ::XMMatrixRotationNormal(yawAxisVector, mYaw);

    // Pitch
    XMFLOAT4 pitchAxis       = XMFLOAT4(1, 0, 0, 0);
    XMVECTOR pitchAxisVector = ::XMLoadFloat4(&pitchAxis);
    XMMATRIX pitchMatrix     = ::XMMatrixRotationNormal(pitchAxisVector, mPitch);

    return ::XMMatrixMultiply(pitchMatrix, yawMatrix);
}

float Camera::GetYaw() const
{
    return mYaw;
}

void Camera::RotateYaw(float pYaw)
{
    mYaw += pYaw;
}

void Camera::SetYaw(float pYaw)
{
    mYaw = pYaw;
}

float Camera::GetPitch() const
{
    return mPitch;
}

void Camera::RotatePitch(float pPitch)
{
    mPitch = this->ClampPitch(mPitch + pPitch);
}

void Camera::SetPitch(float pPitch)
{
    mPitch = this->ClampPitch(pPitch);
}

float Camera::ClampPitch(float pPitch)
{
    float rotationLimit = (89.0f * XM_PI) / 180.0f;
    return wxMin(rotationLimit, wxMax(-rotationLimit, pPitch));
}

float Camera::GetDistance() const
{
    return mDistance;
}

void Camera::MultiplyDistance(float pMultiplier)
{
    mDistance += (mDistance * pMultiplier * 0.1f);
}

void Camera::SetDistance(float pDistance)
{
    mDistance = pDistance;
}

const XMFLOAT3& Camera::GetPivot() const
{
    return mPivot;
}

void Camera::Pan(float pX, float pY)
{
    XMMATRIX rotationMatrix = this->CalculateRotationMatrix();

    // Pan speed is based on distance from pivot, so the user doesn't have to move the mouse like 
    // a madman for big meshes
    float panSpeed = 0.001f * mDistance;

    // X axis
    XMFLOAT4 right(1, 0, 0, 0);
    XMVECTOR rightVector = ::XMLoadFloat4(&right);
    rightVector = ::XMVector4Transform(rightVector, rotationMatrix);

    // Y axis
    XMFLOAT4 up(0, 0, 1, 0);
    XMVECTOR upVector = ::XMLoadFloat4(&up);
    upVector = ::XMVector4Transform(upVector, rotationMatrix);

    // Perform the panning
    XMVECTOR pivotVector = ::XMLoadFloat3(&mPivot);

    rightVector = ::XMVectorScale(rightVector, pX * panSpeed);
    upVector    = ::XMVectorScale(upVector, pY * panSpeed);
    pivotVector = ::XMVectorAdd(pivotVector, rightVector);
    pivotVector = ::XMVectorAdd(pivotVector, upVector);

    ::XMStoreFloat3(&mPivot, pivotVector);
}

void Camera::SetPivot(const XMFLOAT3& pPivot)
{
    mPivot = pPivot;
}


}; // namespace gw2b
