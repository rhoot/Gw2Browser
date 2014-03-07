/* \file       WriteIndexTask.h
*  \brief      Contains declaration of the WriteIndexTask class.
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

#ifndef TASKS_WRITEINDEXTASK_H_INCLUDED
#define TASKS_WRITEINDEXTASK_H_INCLUDED

#include <wx/filename.h>

#include "DatIndexIO.h"
#include "Task.h"

namespace gw2b {
	class DatIndex;

	class WriteIndexTask : public Task {
		std::shared_ptr<DatIndex>   m_index;
		DatIndexWriter              m_writer;
		wxFileName                  m_filename;
		bool                        m_errorOccured;
	public:
		WriteIndexTask( const std::shared_ptr<DatIndex>& p_index, const wxFileName& p_filename );

		virtual bool init( );
		virtual void perform( );
		virtual void abort( );
		virtual void clean( );

		virtual bool canAbort( ) const {
			return false;
		}
		virtual bool isDone( ) const;
	}; // class WriteIndexTask

}; // namespace gw2b

#endif // TASKS_WRITEINDEXTASK_H_INCLUDED
