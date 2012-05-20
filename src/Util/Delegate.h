/** \file       Delegate.h
 *  \brief      Declares all the types of delegates.
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

#ifndef UTIL_DELEGATE_H_INCLUDED
#define UTIL_DELEGATE_H_INCLUDED

#include "Array.h"

// forward declare, the magic happens below
namespace gw2b
{
template <typename Signature> class Delegate;
}; // namespace gw2b

// for some unspeakable reason this has to be done in two steps
#define DELEGATE_CONCAT(a, b)     	DELEGATE_DO_CONCAT(a, b)
#define DELEGATE_DO_CONCAT(a, b)  	a##b

#define DELEGATE_ONE_PARAM_0(a)
#define DELEGATE_ONE_PARAM_1(a)		a##1
#define DELEGATE_ONE_PARAM_2(a)		a##1, a##2
#define DELEGATE_ONE_PARAM_3(a)		a##1, a##2, a##3
#define DELEGATE_ONE_PARAM_4(a)		a##1, a##2, a##3, a##4
#define DELEGATE_ONE_PARAM_5(a)		a##1, a##2, a##3, a##4, a##5

#define DELEGATE_TWO_PARAMS_0(a,b)	
#define DELEGATE_TWO_PARAMS_1(a,b)	a##1 b##1
#define DELEGATE_TWO_PARAMS_2(a,b)	a##1 b##1, a##2 b##2
#define DELEGATE_TWO_PARAMS_3(a,b)	a##1 b##1, a##2 b##2, a##3 b##3
#define DELEGATE_TWO_PARAMS_4(a,b)	a##1 b##1, a##2 b##2, a##3 b##3, a##4 b##4
#define DELEGATE_TWO_PARAMS_5(a,b)	a##1 b##1, a##2 b##2, a##3 b##3, a##4 b##4, a##5 b##5

#define DELEGATE_NUM_ARGS 0			// Delegate<R()>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#define DELEGATE_NUM_ARGS 1			// Delegate<R(T1)>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#define DELEGATE_NUM_ARGS 2			// Delegate<R(T1, T2)>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#define DELEGATE_NUM_ARGS 3			// Delegate<R(T1, T2, T3)>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#define DELEGATE_NUM_ARGS 4			// Delegate<R(T1, T2, T3, T4)>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#define DELEGATE_NUM_ARGS 5			// Delegate<R(T1, T2, T3, T4, T5)>
#include "DelegateTemplate.h"
#undef DELEGATE_NUM_ARGS

#endif // UTIL_DELEGATE_H_INCLUDED