#include "genericweapon.h"

void CGenericWeapon::SendWeaponAnim(int iAnim, int body)
{
	m_pPlayer->pev->weaponanim = iAnim;

#if defined( CLIENT_WEAPONS )
	if( UseDecrement() && ENGINE_CANSKIP( m_pPlayer->edict() ) )
		return;
#endif
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, m_pPlayer->pev );
		WRITE_BYTE( iAnim );		// sequence number
		WRITE_BYTE( pev->body );	// weaponmodel bodygroup.
	MESSAGE_END();
}

int CGenericWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
	int baseAddSucceeded = CBasePlayerItem::AddToPlayer(pPlayer);

	pPlayer->pev->weapons |= ( 1 << m_iId );

	if( !m_iClip0AmmoType )
	{
		m_iClip0AmmoType = pPlayer->GetAmmoIndex( pszAmmo1() );
		m_iClip1AmmoType = pPlayer->GetAmmoIndex( pszAmmo2() );
	}

	if( baseAddSucceeded )
	{
		return AddWeapon();
	}

	return FALSE;
}

int CGenericWeapon::AddDuplicate(CBasePlayerItem *pOriginal)
{
	if( m_iDefaultAmmo )
	{
		return ExtractAmmo( (CBasePlayerWeapon *)pOriginal );
	}
	else
	{
		// a dead player dropped this.
		return ExtractClipAmmo( (CBasePlayerWeapon *)pOriginal );
	}
}

int CGenericWeapon::AddWeapon()
{
	return TRUE;
}
