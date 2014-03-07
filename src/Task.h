/* \file       Task.h
*  \brief      Contains declaration of the task base class.
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

#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include <functional>
#include <list>

namespace gw2b {

	/** Represents a task that gets executed repeatedly until it's done. */
	class Task {
	public:
		/** Event handler for task completion. */
		typedef std::function<void( )>   OnCompleteHandler;
	private:
		std::list<OnCompleteHandler>    m_onComplete;

		uint                            m_currentProgress;
		uint                            m_maxProgress;
		wxString                        m_label;
	public:
		/** Constructor. */
		Task( ) : m_currentProgress( 0 ), m_maxProgress( 0 ) {
		}
		/** Destructor. */
		virtual ~Task( ) {
		}

		/** Gets the text that should be used to display what's going on.
		*  \return wxString&   Message describing the task. */
		virtual const wxString& text( ) const {
			return m_label;
		}
		/** Gets the current progress.
		*  \return uint    Current progress. */
		virtual uint currentProgress( ) const {
			return m_currentProgress;
		}
		/** Gets the max progress.
		*  \return uint    max progress. */
		virtual uint maxProgress( ) const {
			return m_maxProgress;
		}
		/** Determines whether the task is done.
		*  \return bool    true if the task is done, false if not. */
		virtual bool isDone( ) const {
			return ( this->currentProgress( ) >= this->maxProgress( ) );
		}

		/** Gets the OnComplete event handler.
		*  \return OnCompleteHandler&  Reference to the OnComplete event handler. */
		void addOnCompleteHandler( const OnCompleteHandler& p_handler );
		void addOnCompleteHandler( OnCompleteHandler&& p_handler );
		void invokeOnCompleteHandler( );

		/** Initializes this task.
		*  \return bool    true upon success, false on failure. */
		virtual bool init( ) {
			return true;
		}
		/** Performs one iteration of this task. */
		virtual void perform( ) = 0;
		/** Aborts this task. */
		virtual void abort( ) {
		}
		/** Cleans up after this task. */
		virtual void clean( ) {
		}
		/** Determines whether the task can be aborted.
		*  \return bool    true if the task is abortable, false if not. */
		virtual bool canAbort( ) const {
			return true;
		}

	protected:
		/** Used by subclasses to set current progress.
		*  \param[in]  p_progress   Current progress. */
		virtual void setCurrentProgress( uint p_progress ) {
			m_currentProgress = p_progress;
		}
		/** Used by subclasses to set max progress.
		*  \param[in]  p_progress   max progress. */
		virtual void setMaxProgress( uint p_progress ) {
			m_maxProgress = p_progress;
		}
		/** Used by subclasses to set the task message.
		*  \param[in]  p_text   Current task message. */
		virtual void setText( const wxString& p_text ) {
			m_label = p_text;
		}
	}; // class Task

}; // namespace gw2b

#endif // TASK_H_INCLUDED
