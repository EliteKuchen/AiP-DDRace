/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include "flag.h"

CFlag::CFlag(CGameWorld *pGameWorld, int Team, vec2 Pos, CCharacter *pOwner)
: CEntity(pGameWorld, NETOBJTYPE_FLAG)
{
	m_Team = Team;
	m_Pos = Pos;
	m_ProximityRadius = ms_PhysSize;
	m_pCarryingCharacter = pOwner;
	
	GameServer()->m_World.InsertEntity(this);
}

void CFlag::Reset()
{
	if(!m_pCarryingCharacter)
		return;
		
	GameServer()->m_World.DestroyEntity(this);
}

void CFlag::Tick()
{
	if(m_pCarryingCharacter)
		m_Pos = m_pCarryingCharacter->m_Pos;
}

void CFlag::Snap(int SnappingClient)
{
	if((!m_pCarryingCharacter && GameServer()->m_apPlayers[SnappingClient]->GetTeam() != m_Team && GameServer()->m_apPlayers[SnappingClient]->GetCharacter() && GameServer()->m_apPlayers[SnappingClient]->GetCharacter()->m_RaceState == CCharacter::RACE_STARTED)
		||(m_pCarryingCharacter && !GameServer()->m_apPlayers[SnappingClient]->m_ShowOthers && SnappingClient != m_pCarryingCharacter->GetPlayer()->GetCID()))
		return;
	
	CNetObj_Flag *pFlag = (CNetObj_Flag *)Server()->SnapNewItem(NETOBJTYPE_FLAG, m_Id, sizeof(CNetObj_Flag));
	pFlag->m_X = (int)m_Pos.x;
	pFlag->m_Y = (int)m_Pos.y;
	pFlag->m_Team = m_Team;
	pFlag->m_CarriedBy = -1;
	
	if(!m_pCarryingCharacter)
		pFlag->m_CarriedBy = -2;
	else if(m_pCarryingCharacter && m_pCarryingCharacter->GetPlayer())
		pFlag->m_CarriedBy = m_pCarryingCharacter->GetPlayer()->GetCID();
}
