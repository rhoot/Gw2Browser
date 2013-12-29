/* \file       Viewers/ModelViewer/Camera.h
*  \brief      Contains the declaration of the camera class.
*  \author     Rhoot
*/

/*
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

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

namespace gw2b {

	class Camera {
		XMFLOAT3 m_pivot;
		float m_distance;
		float m_yaw;
		float m_pitch;
	public:
		Camera( );
		~Camera( );

		XMMATRIX calculateViewMatrix( ) const;
		XMMATRIX calculateRotationMatrix( ) const;

		float yaw( ) const;
		void addYaw( float p_yaw );
		void setYaw( float p_yaw );

		float pitch( ) const;
		void addPitch( float p_pitch );
		void setPitch( float p_pitch );
		float clampPitch( float p_pitch );

		float distance( ) const;
		void multiplyDistance( float p_multiplier );
		void setDistance( float p_distance );

		const XMFLOAT3& pivot( ) const;
		void pan( float p_x, float p_y );
		void setPivot( const XMFLOAT3& p_pivot );

	}; // class Camera

}; // namespace gw2b

#endif // VIEWERS_MODELVIEWER_CAMERA_H_INCLUDED
