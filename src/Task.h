/** \file       Task.h
 *  \brief      Contains declaration of the task base class.
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

#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

namespace gw2b
{

/** Represents a task that gets executed repeatedly until it's done. */
class Task
{
public:
    /** Event handler for task completion. */
    typedef Delegate<void()>    OnCompleteHandler;
private:
    uint                    mCurrentProgress;
    uint                    mMaxProgress;
    wxString                mLabel;
    OnCompleteHandler       mOnComplete;
public:
    /** Constructor. */
    Task() : mCurrentProgress(0), mMaxProgress(0) {}
    /** Destructor. */
    virtual ~Task() {}

    /** Gets the text that should be used to display what's going on. 
     *  \return wxString&   Message describing the task. */
    virtual const wxString& GetText() const         { return mLabel; }
    /** Gets the current progress.
     *  \return uint    Current progress. */
    virtual uint GetCurrentProgress() const         { return mCurrentProgress; }
    /** Gets the max progress.
     *  \return uint    max progress. */
    virtual uint GetMaxProgress() const             { return mMaxProgress; }
    /** Determines whether the task is done.
     *  \return bool    true if the task is done, false if not. */
    virtual bool IsDone() const                     { return (this->GetCurrentProgress() >= this->GetMaxProgress()); }

    /** Gets the OnComplete event handler.
     *  \return OnCompleteHandler&  Reference to the OnComplete event handler. */
    OnCompleteHandler& GetOnCompleteHandler()       { return mOnComplete; }

    /** Initializes this task.
     *  \return bool    true upon success, false on failure. */
    virtual bool Init()     { return true; }
    /** Performs one iteration of this task. */
    virtual void Perform()  = 0;
    /** Aborts this task. */
    virtual void Abort()    {}
    /** Cleans up after this task. */
    virtual void Clean()    {}
    /** Determines whether the task can be aborted.
     *  \return bool    true if the task is abortable, false if not. */
    virtual bool CanAbort() const { return true; }
    
protected:
    /** Used by subclasses to set current progress.
     *  \param[in]  pProgress   Current progress. */
    virtual void SetCurrentProgress(uint pProgress) { mCurrentProgress = pProgress; }
    /** Used by subclasses to set max progress.
     *  \param[in]  pProgress   max progress. */
    virtual void SetMaxProgress(uint pProgress)     { mMaxProgress     = pProgress; }
    /** Used by subclasses to set the task message.
     *  \param[in]  pText   Current task message. */
    virtual void SetText(const wxString& pText)     { mLabel = pText; }
}; // class Task

}; // namespace gw2b

#endif // TASK_H_INCLUDED
