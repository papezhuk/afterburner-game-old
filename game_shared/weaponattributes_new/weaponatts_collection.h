#pragma once

#include <functional>
#include "weaponatts_core.h"
#include "weaponatts_skillrecord.h"
#include "weaponatts_viewmodel.h"
#include "weaponatts_soundset.h"
#include "utlvector.h"

namespace WeaponAtts
{
	struct CWACollection
	{
		CWACore Core;
		CUtlVector<CWASkillRecord> SkillRecords;
		CUtlVector<cvar_t*> CustomCvars;

		CWAViewModel ViewModel;

		CWACollection(const std::function<void(CWACollection&)>& initialiser);
		CWACollection& operator =(const CWACollection& other);

		void RegisterCvars() const;

	private:
		void Register() const;
		void Validate() const;
	};
}
