/* \file       ScanDatTask.h
*  \brief      Contains declaration of the ScanDatTask class.
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

#ifndef TASKS_SCANDATTASK_H_INCLUDED
#define TASKS_SCANDATTASK_H_INCLUDED

#include "ANetStructs.h"
#include "Task.h"

namespace gw2b {
	class DatFile;
	class DatIndex;
	class DatIndexCategory;

	class ScanDatTask : public Task {
		std::shared_ptr<DatIndex>   m_index;
		Array<byte>                 m_outputBuffer;
		DatFile&                    m_datFile;
	public:
		ScanDatTask( const std::shared_ptr<DatIndex>& p_index, DatFile& p_datFile );
		virtual ~ScanDatTask( );

		virtual bool init( ) override;
		virtual void perform( ) override;
	private:
		uint requiredIdentificationSize( const byte* p_data, uint p_size, ANetFileType p_fileType );
		DatIndexCategory* categorize( ANetFileType p_fileType, const byte* p_data, uint p_size );
		void ensureBufferSize( uint p_size );
	}; // class ScanDatTask

}; // namespace gw2b

#endif // TASKS_SCANDATTASK_H_INCLUDED