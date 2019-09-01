#include "weaponatts_collection.h"
#include "weaponregistry.h"

namespace WeaponAtts
{
	CWACollection::CWACollection(const std::function<void(CWACollection&)>& initialiser)
	{
		initialiser(*this);
	}

	CWACollection& CWACollection::operator =(const CWACollection& other)
	{
		// Assignment is the final operation that happens once all
		// other components have been initialised.
		Validate();
		Register();
		return *this;
	}

	void CWACollection::Register() const
	{
		// TODO: Update weapon registry with new attribute class support!
		//CWeaponRegistry::StaticInstance().Add(this);
	}

	void CWACollection::Validate() const
	{
		Core.Validate();
		ViewModel.Validate();
	}

	void CWACollection::RegisterCvars() const
	{
		FOR_EACH_VEC(SkillRecords, index)
		{
			SkillRecords[index].RegisterCvars();
		}

		FOR_EACH_VEC(CustomCvars, index)
		{
			CVAR_REGISTER(CustomCvars[index]);
		}
	}
}
