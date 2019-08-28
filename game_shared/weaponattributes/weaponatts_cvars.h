#pragma once

#include <cstdint>
#include "cvardef.h"
#include "utlvector.h"
#include "enginecallback.h"

class CGenericWeaponAttributes_CustomCVars
{
public:
	CGenericWeaponAttributes_CustomCVars();
	CGenericWeaponAttributes_CustomCVars(const CGenericWeaponAttributes_CustomCVars& other);
	CGenericWeaponAttributes_CustomCVars& operator =(const CGenericWeaponAttributes_CustomCVars& other);

	inline CGenericWeaponAttributes_CustomCVars& AddCVar(cvar_t* var)
	{
		m_Vars.AddToTail(var);
		return *this;
	}

	// Takes an array of cvar_t* items.
	inline CGenericWeaponAttributes_CustomCVars& AddCVars(cvar_t** vars, size_t count)
	{
		m_Vars.AddMultipleToTail(count, vars);
		return *this;
	}

	template<size_t size>
	inline CGenericWeaponAttributes_CustomCVars& AddCVarsStaticArray(cvar_t*(&vars)[size])
	{
		return AddCVars(vars, size);
	}

	// Takes an array of actual cvar_t instances.
	inline CGenericWeaponAttributes_CustomCVars& AddCVars(cvar_t* vars, size_t count)
	{
		m_Vars.EnsureCapacity(m_Vars.Count() + count);

		for ( uint32_t index = 0; index < count; ++index )
		{
			m_Vars.AddToTail(vars + index);
		}

		return *this;
	}

	template<size_t size>
	inline CGenericWeaponAttributes_CustomCVars& AddCVarsStaticArray(cvar_t(&vars)[size])
	{
		return AddCVars(vars, size);
	}

	inline void RegisterCvars() const
	{
		FOR_EACH_VEC(m_Vars, index)
		{
			CVAR_REGISTER(m_Vars[index]);
		}
	}

private:
	CUtlVector<cvar_t*> m_Vars;
};