/** \file       WriteIndexTask.h
 *  \brief      Contains definition of the WriteIndexTask class.
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
#include "WriteIndexTask.h"

namespace gw2b
{

WriteIndexTask::WriteIndexTask(const std::shared_ptr<DatIndex>& p_index, const wxFileName& p_filename)
    : m_index(p_index)
    , m_writer(*p_index)
    , m_filename(p_filename)
    , m_errorOccured(false)
{
    Ensure::notNull(p_index.get());
}

bool WriteIndexTask::init()
{
    if (!m_filename.DirExists()) {
        m_filename.Mkdir(511, wxPATH_MKDIR_FULL);
    }

    if (m_index->isDirty()) {
        bool result = m_writer.open(m_filename.GetFullPath());
        if (result) { this->setMaxProgress(m_writer.numEntries() + m_writer.numCategories()); }
        return result;
    }
    return false;
}

void WriteIndexTask::perform()
{
    if (!this->isDone()) {
        m_errorOccured = !m_writer.write(7);
        uint progress = m_writer.currentEntry() + m_writer.currentCategory();
        this->setCurrentProgress(progress);
        this->setText(wxT("Saving .dat index..."));
        // If something went wrong, we should delete the file again since it's half-complete
        auto path = m_filename.GetFullPath();
        if (m_errorOccured && wxFile::Exists(path)) {
            wxRemoveFile(path);
        }
        // If done, remove the dirty flag from the index
        if (this->isDone()) {
            m_index->setDirty(false);
        }
    }
}

void WriteIndexTask::abort()
{
    m_writer.close();
    // Remove the file again
    auto path = m_filename.GetFullPath();
    if (wxFile::Exists(path)) {
        wxRemoveFile(path);
    }
}

void WriteIndexTask::clean()
{
    m_writer.close();
}

bool WriteIndexTask::isDone() const
{
    return (m_writer.isDone() || m_errorOccured);
}

}; // namespace gw2b
