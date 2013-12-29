/* \file       Task.cpp
*  \brief      Contains definition of the task base class.
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

#include "stdafx.h"
#include "Task.h"

namespace gw2b {

	void Task::addOnCompleteHandler( const OnCompleteHandler& p_handler ) {
		m_onComplete.push_back( p_handler );
	}

	void Task::addOnCompleteHandler( OnCompleteHandler&& p_handler ) {
		m_onComplete.push_back( std::forward<OnCompleteHandler>( p_handler ) );
	}

	void Task::invokeOnCompleteHandler( ) {
		for ( auto iter = m_onComplete.begin( ); iter != m_onComplete.end( ); iter++ ) {
			( *iter )( );
		}
	}

}; // namespace gw2b
