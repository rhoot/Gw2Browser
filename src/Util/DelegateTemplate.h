/** \file       DelegateTemplate.h
 *  \brief      Contains declaration of the Delegate class.
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

// Note: Header include-file, must *not* have include guards
// .... but make sure it's included from Delegate.h

#ifdef DELEGATE_NUM_ARGS

// define magic stuff
#define DELEGATE_TEMPLATE_PARAMS   DELEGATE_CONCAT(DELEGATE_ONE_PARAM_,  DELEGATE_NUM_ARGS)(typename T)
#define DELEGATE_TEMPLATE_ARGS     DELEGATE_CONCAT(DELEGATE_ONE_PARAM_,  DELEGATE_NUM_ARGS)(T)
#define DELEGATE_ARGS              DELEGATE_CONCAT(DELEGATE_TWO_PARAMS_, DELEGATE_NUM_ARGS)(T, arg)
#define DELEGATE_CALL_ARGS         DELEGATE_CONCAT(DELEGATE_ONE_PARAM_,  DELEGATE_NUM_ARGS)(arg)

#if DELEGATE_NUM_ARGS == 0
#define DELEGATE_SEPARATOR
#else
#define DELEGATE_SEPARATOR ,
#endif

namespace gw2b
{

template <typename ReturnType DELEGATE_SEPARATOR DELEGATE_TEMPLATE_PARAMS>
    class Delegate<ReturnType(DELEGATE_TEMPLATE_ARGS)>
{
    struct DelegateDataBase;
    typedef Array<DelegateDataBase*,0x3> CallList;
public:
    typedef ReturnType(*PointerType)(DELEGATE_TEMPLATE_ARGS);
    typedef Delegate<ReturnType(DELEGATE_TEMPLATE_ARGS)> ThisType;
private:
    CallList mCallList;
private:
    struct DelegateDataBase
    {
        public:
            DelegateDataBase(int type) : type(type)							{ }
            ~DelegateDataBase() 											{ }

            virtual ReturnType Invoke(DELEGATE_ARGS) const = 0;
            virtual bool IsEqual(const DelegateDataBase& pOther) const = 0;
            virtual DelegateDataBase* Clone() const = 0;
        public:
            int type;
    };
    struct DelegateStaticData : public DelegateDataBase
    {
        public:
            DelegateStaticData(const PointerType staticFunction) : DelegateDataBase(1), func(staticFunction) { }

            ReturnType Invoke(DELEGATE_ARGS) const						    { return (*func)(DELEGATE_CALL_ARGS); }
            bool IsEqual(const DelegateDataBase& pOther) const				{ if (pOther.type == 1) { return static_cast<const DelegateStaticData&>(pOther).func == this->func; } return false; }
            DelegateDataBase* Clone() const									{ return new DelegateStaticData(this->func); }
        private:
            PointerType func;
    };
    template <typename InstanceType, typename FunctionType>
        struct DelegateMemberData : public DelegateDataBase
    {
        public:
            DelegateMemberData(InstanceType* instance, FunctionType memberFunction) : DelegateDataBase(2), instance(instance), func(memberFunction) { }

            ReturnType Invoke(DELEGATE_ARGS) const						    { return (instance->*func)(DELEGATE_CALL_ARGS); }
            bool IsEqual(const DelegateDataBase& pOther) const				{ if (pOther.type == 2) { const DelegateMemberData& o = static_cast<const DelegateMemberData<InstanceType, FunctionType>&>(pOther); return (o.instance == this->instance && o.func == this->func); } return false; }
            DelegateDataBase* Clone() const									{ return new DelegateMemberData<InstanceType, FunctionType>(instance, func); }
        private:
            InstanceType* instance;
            FunctionType func;
    };

public:
    Delegate()																{ }
    Delegate(const PointerType staticFunction) 								{ mCallList.Add(new DelegateStaticData(staticFunction)); }
    template <typename InstanceType, typename FunctionType>
        Delegate(InstanceType* instance, FunctionType memberFunction)		{ mCallList.Add(new DelegateMemberData<InstanceType, FunctionType>(instance, memberFunction)); }
    template <typename InstanceType, typename FunctionType>
        Delegate(InstanceType& instance, FunctionType memberFunction)		{ mCallList.Add(new DelegateMemberData<InstanceType, FunctionType>(&instance, memberFunction)); }
    Delegate(const ThisType& pOther)  										{ Add(pOther); }

    virtual ~Delegate() 													{ Clear(); }

    /* properties */
    inline bool IsEmpty() const												{ return (mCallList.GetSize() == 0); }
    inline bool IsMulticast() const											{ return (mCallList.GetSize() > 1); }

    /* concat */
    void Add(const ThisType& pOther)  										{ if (!pOther.IsEmpty()) { for (uint i = 0; i < pOther.mCallList.GetSize(); ++i) { mCallList.Add(pOther.mCallList[i]->Clone()); } } }
    ThisType& operator+=(const ThisType& pOther)								{ Add(pOther); return *this; }

    /* removal */
    void Clear()															{ for (uint i = 0; i < mCallList.GetSize(); i++) { delete mCallList[i]; } mCallList.Clear(); }
    void Remove(const ThisType& pOther)  									{ if (!pOther.IsEmpty()) { for (uint i = 0; i < pOther.mCallList.GetSize(); ++i) { RemoveItem(pOther.mCallList[i]); } } }
    ThisType& operator-=(const ThisType& pOther)								{ Remove(pOther); return *this; }

    /* invokation */
    ReturnType Invoke(DELEGATE_ARGS)									    { 
        for (int i = 0; i < ((int)mCallList.GetSize()) - 1; i++) { 
            mCallList[i]->Invoke(DELEGATE_CALL_ARGS); 
        } 
        if (mCallList.GetSize() > 0) { 
            return mCallList[mCallList.GetSize()-1]->Invoke(DELEGATE_CALL_ARGS); 
        } 
        return ReturnType(); 
    }
    ReturnType Invoke(DELEGATE_ARGS) const								    { 
        for (int i = 0; i < ((int)mCallList.GetSize()) - 1; i++) { 
            mCallList[i]->Invoke(DELEGATE_CALL_ARGS); 
        } 
        if (mCallList.GetSize() > 0) {
            return mCallList[mCallList.GetSize()-1]->Invoke(DELEGATE_CALL_ARGS);
        } 
        return ReturnType(); 
    }
    ReturnType operator()(DELEGATE_ARGS) const 						        { return Invoke(DELEGATE_CALL_ARGS); }

private:
    // HOLY UNOPTIMIZED COW
    void RemoveItem(const DelegateDataBase* value)							{ for (int i = mCallList.GetSize() - 1; i >= 0; i--) { if (mCallList[i]->IsEqual(value)) { delete mCallList[i]; mCallList.RemoveAt(i); } } }
}; // class Delegate

}; // namespace Lona

// undefine magic stuff to avoid collisions
#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_ARGS
#undef DELEGATE_CALL_ARGS
#undef DELEGATE_SEPARATOR

#endif // DELEGATE_NUM_ARGS