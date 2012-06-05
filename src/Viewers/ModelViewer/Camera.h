/** \file       Viewers/ModelViewer/Camera.h
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

#pragma once

#ifndef VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
#define VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED

namespace gw2b
{

class Camera
{
    XMFLOAT3 mPivot;
    float mDistance;
    float mYaw;
    float mPitch;
public:
    Camera();
    ~Camera();

    XMMATRIX CalculateViewMatrix() const;
    XMMATRIX CalculateRotationMatrix() const;

    float GetYaw() const;
    void RotateYaw(float pYaw);
    void SetYaw(float pYaw);

    float GetPitch() const;
    void RotatePitch(float pPitch);
    void SetPitch(float pPitch);
    float ClampPitch(float pPitch);

    float GetDistance() const;
    void MultiplyDistance(float pMultiplier);
    void SetDistance(float pDistance);

    const XMFLOAT3& GetPivot() const;
    void Pan(float pX, float pY);
    void SetPivot(const XMFLOAT3& pPivot);
    
}; // class Camera

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
