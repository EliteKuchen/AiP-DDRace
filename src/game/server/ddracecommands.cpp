/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include "gamecontext.h"
#include <string.h>
#include <engine/shared/config.h>
#include <engine/server/server.h>
#include <game/server/teams.h>
#include <game/server/gamemodes/DDRace.h>
#include <game/version.h>
#if defined(CONF_SQL)
	#include <game/server/score/sql_score.h>
#endif

void CGameContext::ConCallHelper(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	char aBuf[512];
	unsigned int called_helper = 0;

	if(pSelf->m_apPlayers[ClientID]->m_Last_HelperCall && pSelf->m_apPlayers[ClientID]->m_Last_HelperCall+pSelf->Server()->TickSpeed() > pSelf->Server()->Tick()/2)
		return;

	for(int hID = 0; hID < MAX_CLIENTS; hID++)
	{
		if(hID != ClientID && pSelf->m_apPlayers[hID] && pSelf->m_apPlayers[hID]->m_Authed > 0)
		{
			char aBuf[512];
			str_format(aBuf, sizeof(aBuf), "%s need a helper. ID:%d", pSelf->Server()->ClientName(ClientID), ClientID);
			pSelf->SendChatTarget(hID, aBuf);
			called_helper ++;
		}
	}

	if(called_helper > 0)
		str_format(aBuf, sizeof(aBuf), "You called %d helper", called_helper);
	else
		str_format(aBuf, sizeof(aBuf), "There's no helper online");

	//SpamProtection
	pSelf->m_apPlayers[ClientID]->m_Last_HelperCall = pSelf->Server()->Tick();

	pSelf->SendChatTarget(ClientID, aBuf);		
}

void CGameContext::ConRescue(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	if(!g_Config.m_SvRescue)
		return;

	CGameContext *pSelf = (CGameContext *)pUserData;

	CCharacter* pChr = pSelf->GetPlayerChar(ClientID);

	if(!pChr)
		return;
	if(!pChr->m_FreezeTime)
		return;

	pChr->resetting = true;
}

void CGameContext::ConClistAdd(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ClanList->Add(pResult->GetVictim());
}

void CGameContext::ConClistRem(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ClanList->Remove(pResult->GetVictim());
}

void CGameContext::ConClistChk(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	char aBuf[128];

	if(pSelf->ClanList->Check(pResult->GetVictim()))
		str_format(aBuf, sizeof(aBuf), "%s is on your ClanList", pSelf->Server()->ClientName(pResult->GetVictim()));
	else	
		str_format(aBuf, sizeof(aBuf), "%s is !NOT! on your ClanList", pSelf->Server()->ClientName(pResult->GetVictim()));
	
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
}

void CGameContext::ConDamage(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Damage = true;
}

void CGameContext::ConUnDamage(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Damage = false;
}

void CGameContext::ConIceHammer(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.IHammer = true;
}

void CGameContext::ConUnIceHammer(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.IHammer = false;
}

void CGameContext::ConBloody(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Bloody = true;
}

void CGameContext::ConUnBloody(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Bloody = false;
}

void CGameContext::ConSetSkin(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;
	strcpy(pChr->GetPlayer()->m_TeeInfos.m_SkinName, pResult->GetString(0));
}

void CGameContext::ConSetScore(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;
	pChr->GetPlayer()->m_Score = pResult->GetInteger(0);
}

void CGameContext::ConSetName(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Server()->SetClientName(pResult->GetVictim(), pResult->GetString(0));
}

void CGameContext::ConSayBy(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "%s", pResult->GetString(0));
	pSelf->SendChat(pResult->GetVictim(), CGameContext::CHAT_ALL, aBuf);
}

void CGameContext::ConWhisper(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "%s", pResult->GetString(0));
	pSelf->SendChatTarget(pResult->GetVictim(), aBuf);
}

void CGameContext::ConReload(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Reload = true;

	if(!g_Config.m_SvCheatTime)
		pChr->m_DDRaceState = DDRACE_CHEAT;
}

void CGameContext::ConUnReload(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CCharacter* pChr = pSelf->GetPlayerChar(pResult->GetVictim());

	if(!pChr)
		return;

	pSelf->m_apPlayers[pResult->GetVictim()]->Cheats.Reload = false;
}

void CGameContext::ConRainbow(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	
	int Victim = pResult->GetVictim();

	if(!pSelf->m_apPlayers[Victim])
		return;
	
	if(!pSelf->m_apPlayers[Victim]->Cheats.Rainbow)
		pSelf->m_apPlayers[Victim]->Cheats.Rainbow = true;
	else
		pSelf->m_apPlayers[Victim]->Cheats.Rainbow = false;
}

void CGameContext::ConGoLeft(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), -1, 0);
}

void CGameContext::ConGoRight(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), 1, 0);
}

void CGameContext::ConGoDown(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), 0, 1);
}

void CGameContext::ConGoUp(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), 0, -1);
}

void CGameContext::ConMove(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), pResult->GetInteger(0), pResult->GetInteger(1));
}

void CGameContext::ConMoveRaw(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->MoveCharacter(ClientID, pResult->GetVictim(), pResult->GetInteger(0), pResult->GetInteger(1), true);
}

void CGameContext::MoveCharacter(int ClientID, int Victim, int X, int Y, bool Raw)
{
	CCharacter* pChr = GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->Core()->m_Pos.x += ((Raw) ? 1 : 32) * X;
	pChr->Core()->m_Pos.y += ((Raw) ? 1 : 32) * Y;

	if(!g_Config.m_SvCheatTime)
		pChr->m_DDRaceState = DDRACE_CHEAT;
}

void CGameContext::ConSetlvl3(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CServer* pServ = (CServer*)pSelf->Server();
	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->m_Authed = 3;
		pServ->SetRconLevel(Victim, 3);
	}
}

void CGameContext::ConSetlvl2(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CServer* pServ = (CServer*)pSelf->Server();
	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->m_Authed = 2;
		pServ->SetRconLevel(Victim, 2);
	}
}

void CGameContext::ConSetlvl1(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CServer* pServ = (CServer*)pSelf->Server();
	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->m_Authed = 1;
		pServ->SetRconLevel(Victim, 1);
	}
}

void CGameContext::ConLogOut(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	CServer* pServ = (CServer*)pSelf->Server();
	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->m_Authed = -1;
		pServ->SetRconLevel(Victim, -1);
	}
}

void CGameContext::ConKillPlayer(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->KillCharacter(WEAPON_GAME);
		char aBuf[512];
		str_format(aBuf, sizeof(aBuf), "%s was killed by %s", pSelf->Server()->ClientName(Victim), pSelf->Server()->ClientName(ClientID));
		pSelf->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
	}
}

void CGameContext::ConNinja(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_NINJA, false);
}


void CGameContext::ConHammer(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	char aBuf[128];
	int Type = pResult->GetInteger(0);

	CCharacter* pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	CServer* pServ = (CServer*)pSelf->Server();
	if(Type>3 || Type<0)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Select hammer between 0 and 3");
	}
	else
	{
		pSelf->m_apPlayers[Victim]->Cheats.HammerType = Type;
		if(!g_Config.m_SvCheatTime)
			pChr->m_DDRaceState = DDRACE_CHEAT;
		str_format(aBuf, sizeof(aBuf), "Hammer of '%s' ClientID=%d setted to %d", pServ->ClientName(Victim), Victim, Type);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}


void CGameContext::ConHammerMe(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	char aBuf[128];
	int Type = pResult->GetInteger(0);

	CCharacter* pChr = pSelf->GetPlayerChar(ClientID);

	if(!pChr)
		return;

	CServer* pServ = (CServer*)pSelf->Server();
	if(Type>3 || Type<0)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Select hammer between 0 and 3");
	}
	else
	{
		pSelf->m_apPlayers[ClientID]->Cheats.HammerType = Type;
		if(!g_Config.m_SvCheatTime)
			pChr->m_DDRaceState = DDRACE_CHEAT;
		str_format(aBuf, sizeof(aBuf), "Hammer of '%s' ClientID=%d setted to %d", pServ->ClientName(ClientID), ClientID, Type);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConSuper(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(pChr && !pSelf->m_apPlayers[Victim]->Cheats.Super)
	{
		pSelf->m_apPlayers[Victim]->Cheats.Super = true;
		pChr->UnFreeze();
		pChr->m_TeamBeforeSuper = pChr->Team();
		pChr->Teams()->SetCharacterTeam(Victim, TEAM_SUPER);
		if(!g_Config.m_SvCheatTime)
			pChr->m_DDRaceState = DDRACE_CHEAT;
	}
}

void CGameContext::ConUnSuper(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(pChr && pSelf->m_apPlayers[Victim]->Cheats.Super)
	{
		pSelf->m_apPlayers[Victim]->Cheats.Super = false;
		pChr->Teams()->SetForceCharacterTeam(Victim, pChr->m_TeamBeforeSuper);
	}
}

void CGameContext::ConShotgun(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_SHOTGUN, false);
}

void CGameContext::ConGrenade(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_GRENADE, false);
}

void CGameContext::ConRifle(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_RIFLE, false);
}

void CGameContext::ConWeapons(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), -1, false);
}

void CGameContext::ConUnShotgun(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_SHOTGUN, true);
}

void CGameContext::ConUnGrenade(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_GRENADE, true);
}

void CGameContext::ConUnRifle(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), WEAPON_RIFLE, true);
}

void CGameContext::ConUnWeapons(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), -1, true);
}

void CGameContext::ConAddWeapon(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), pResult->GetInteger(0), false);
}

void CGameContext::ConRemoveWeapon(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ModifyWeapons(ClientID, pResult->GetVictim(), pResult->GetInteger(0), true);
}

void CGameContext::ModifyWeapons(int ClientID, int Victim, int Weapon, bool Remove)
{
	if(clamp(Weapon, -1, NUM_WEAPONS - 1) != Weapon)
	{
		Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "invalid weapon id");
		return;
	}

	CCharacter* pChr = GetPlayerChar(Victim);
	if(!pChr)
		return;

	if(Weapon == -1)
	{
		if(Remove && (pChr->GetActiveWeapon() == WEAPON_SHOTGUN || pChr->GetActiveWeapon() == WEAPON_GRENADE || pChr->GetActiveWeapon() == WEAPON_RIFLE))
			pChr->SetActiveWeapon(WEAPON_GUN);

		if(Remove)
		{
			pChr->SetWeaponGot(WEAPON_SHOTGUN, false);
			pChr->SetWeaponGot(WEAPON_GRENADE, false);
			pChr->SetWeaponGot(WEAPON_RIFLE, false);
		}
		else
			pChr->GiveAllWeapons();
	}
	else if(Weapon != WEAPON_NINJA)
	{
		if(Remove && pChr->GetActiveWeapon() == Weapon)
			pChr->SetActiveWeapon(WEAPON_GUN);

		if(Remove)
			pChr->SetWeaponGot(Weapon, false);
		else
			pChr->GiveWeapon(Weapon, -1);
	}
	else
	{
		if(Remove)
		{
			Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "you can't remove ninja");
			return;
		}

		pChr->GiveNinja();
	}

	if(!Remove && !g_Config.m_SvCheatTime)
		pChr->m_DDRaceState =	DDRACE_CHEAT;
}

void CGameContext::ConTeleport(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	int TeleTo = clamp(pResult->GetInteger(0), 0, (int)MAX_CLIENTS-1);
	if(pSelf->m_apPlayers[TeleTo])
	{
		{
			CCharacter* pChr = pSelf->GetPlayerChar(Victim);
			if(pChr)
			{
				pChr->Core()->m_Pos = pSelf->m_apPlayers[TeleTo]->m_ViewPos;
				if(!g_Config.m_SvCheatTime)
					pChr->m_DDRaceState = DDRACE_CHEAT;
			}
		}
	}
}

void CGameContext::ConTeleportMe(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int TeleTo = clamp(pResult->GetInteger(0), 0, (int)MAX_CLIENTS-1);
	if(pSelf->m_apPlayers[TeleTo])
	{
		{
			CCharacter* pChr = pSelf->GetPlayerChar(ClientID);
			if(pChr)
			{
				pChr->Core()->m_Pos = pSelf->m_apPlayers[TeleTo]->m_ViewPos;
				if(!g_Config.m_SvCheatTime)
					pChr->m_DDRaceState = DDRACE_CHEAT;
			}
		}
	}
}

void CGameContext::ConTimerStop(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CServer* pServ = (CServer*)pSelf->Server();
	int Victim = pResult->GetVictim();

	char aBuf[128];
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;
	if(pSelf->m_apPlayers[Victim])
	{
		pChr->m_DDRaceState=DDRACE_CHEAT;
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d Hasn't time now (Timer Stopped)", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConTimerStart(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CServer* pServ = (CServer*)pSelf->Server();
	int Victim = pResult->GetVictim();

	char aBuf[128];
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;
	if(pSelf->m_apPlayers[Victim])
	{
		pChr->m_DDRaceState = DDRACE_STARTED;
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d Has time now (Timer Started)", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConTimerZero(IConsole::IResult *pResult, void *pUserData, int ClientID)
{

	CGameContext *pSelf = (CGameContext *)pUserData;
	CServer* pServ = (CServer*)pSelf->Server();
	int Victim = pResult->GetVictim();

	char aBuf[128];
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;
	if(pSelf->m_apPlayers[Victim])
	{
		pChr->m_StartTime = pSelf->Server()->Tick();
		pChr->m_RefreshTime = pSelf->Server()->Tick();
		pChr->m_DDRaceState=DDRACE_CHEAT;
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d time has been reset & stopped.", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConTimerReStart(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	CServer* pServ = (CServer*)pSelf->Server();

	char aBuf[128];
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;
	if(pSelf->m_apPlayers[Victim])
	{
		pChr->m_StartTime = pSelf->Server()->Tick();
		pChr->m_RefreshTime = pSelf->Server()->Tick();
		pChr->m_DDRaceState=DDRACE_STARTED;
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d time has been reset & stopped.", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConFreeze(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Seconds = -1;
	int Victim = pResult->GetVictim();

	char aBuf[128];

	if(pResult->NumArguments())
		Seconds = clamp(pResult->GetInteger(0), -2, 9999);

	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;

	if(pSelf->m_apPlayers[Victim])
	{
		pChr->Freeze(Seconds);
		pChr->GetPlayer()->m_RconFreeze = Seconds != -2;
		CServer* pServ = (CServer*)pSelf->Server();
		if(Seconds >= 0)
			str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d has been Frozen for %d.", pServ->ClientName(Victim), Victim, Seconds);
		else if(Seconds == -2)
		{
			pChr->m_DeepFreeze = true;
			str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d has been Deep Frozen.", pServ->ClientName(Victim), Victim);
		}
		else
			str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d is Frozen until you unfreeze him.", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}

}

void CGameContext::ConUnFreeze(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	static bool Warning = false;
	char aBuf[128];
	CCharacter* pChr = pSelf->GetPlayerChar(Victim);
	if(!pChr)
		return;
	if(pChr->m_DeepFreeze && !Warning)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "warning", "This client is deeply frozen, repeat the command to defrost him.");
		Warning = true;
		return;
	}
	if(pChr->m_DeepFreeze && Warning)
	{
		pChr->m_DeepFreeze = false;
		Warning = false;
	}
	pChr->m_FreezeTime = 2;
	pChr->GetPlayer()->m_RconFreeze = false;
	CServer* pServ = (CServer*)pSelf->Server();
	str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d has been defrosted.", pServ->ClientName(Victim), Victim);
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
}

void CGameContext::ConInvis(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	char aBuf[128];
	int Victim = pResult->GetVictim();

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->Cheats.Invisible = true;
		CServer* pServ = (CServer*)pSelf->Server();
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d is now invisible.", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConVis(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(!pSelf->m_apPlayers[ClientID])
		return;
	char aBuf[128];
	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->Cheats.Invisible = false;
		CServer* pServ = (CServer*)pSelf->Server();
		str_format(aBuf, sizeof(aBuf), "'%s' ClientID=%d is visible.", pServ->ClientName(Victim), Victim);
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
	}
}

void CGameContext::ConCredits(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Teeworlds Team takes most of the credits also");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "This mod was originally created by \'3DA\'");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Now it is maintained & re-coded by:");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "\'[Egypt]GreYFoX@GTi\' and \'[BlackTee]den\'");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Others Helping on the code: \'heinrich5991\', \'noother\', \'LemonFace\' & \'<3 fisted <3\'");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Documentation: Zeta-Hoernchen, Entities: Fisico");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Code (in the past): \'3DA\' and \'Fluxid\'");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Please check the changelog on DDRace.info.");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Also the commit log on github.com/GreYFoXGTi/DDRace.");
}

void CGameContext::ConInfo(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "DDRace Mod. Version: " GAME_VERSION);
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Official website: DDRace.info");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Download Source: EliteKuchen.tk");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No id behind a command like 'super' is the same like super_me");
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "AiP-DDRace Version : " AIP_VERSION);
}

void CGameContext::ConHelp(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "/cmdlist will show a list of all chat commands");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "/help + any command will show you the help for this command");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Example /help settings will display the help about ");
	}
	else
	{
		const char *pArg = pResult->GetString(0);
		IConsole::CCommandInfo *pCmdInfo = pSelf->Console()->GetCommandInfo(pArg, CFGFLAG_SERVER);
		if(pCmdInfo && pCmdInfo->m_pHelp)
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", pCmdInfo->m_pHelp);
		else
				pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Command is either unknown or you have given a blank command without any parameters.");
	}
}

void CGameContext::ConSettings(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "to check a server setting say /settings and setting's name, setting names are:");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "teams, cheats, collision, hooking, endlesshooking, me, ");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "hitting, oldlaser, timeout, votes, pause and scores");
	}
	else
	{
		const char *pArg = pResult->GetString(0);
		char aBuf[256];
		float ColTemp;
		float HookTemp;
		pSelf->m_Tuning.Get("player_collision", &ColTemp);
		pSelf->m_Tuning.Get("player_hooking", &HookTemp);
		if(str_comp(pArg, "cheats") == 0)
		{
			str_format(aBuf, sizeof(aBuf), "%s%s",
					g_Config.m_SvCheats?"People can cheat":"People can't cheat",
					(g_Config.m_SvCheats)?(g_Config.m_SvCheatTime)?" with time":" without time":"");
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
			if(g_Config.m_SvCheats)
			{
				str_format(aBuf, sizeof(aBuf), "%s", g_Config.m_SvEndlessSuperHook?"super can hook you forever":"super can only hook you for limited time");
				pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
				str_format(aBuf, sizeof(aBuf), "%s", g_Config.m_SvTimer?"admins have the power to control your time":"admins have no power over your time");
				pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
			}
		}
		else if(str_comp(pArg, "teams") == 0)
		{
			str_format(aBuf, sizeof(aBuf), "%s %s", g_Config.m_SvTeam == 1 ? "Teams are available on this server" : !g_Config.m_SvTeam ? "Teams are not available on this server" : "You have to be in a team to play on this server", !g_Config.m_SvTeamStrict ? "and if you die in a team only you die" : "and if you die in a team all of you die");
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
		}
		else if(str_comp(pArg, "collision") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", ColTemp?"Players can collide on this server":"Players Can't collide on this server");
		}
		else if(str_comp(pArg, "hooking") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", HookTemp?"Players can hook each other on this server":"Players Can't hook each other on this server");
		}
		else if(str_comp(pArg, "endlesshooking") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvEndlessDrag?"Players can hook time is unlimited":"Players can hook time is limited");
		}
		else if(str_comp(pArg, "hitting") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvHit?"Player's weapons affect each other":"Player's weapons has no affect on each other");
		}
		else if(str_comp(pArg, "oldlaser") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvOldLaser?"Lasers can hit you if you shot them and that they pull you towards the bounce origin (Like DDRace Beta)":"Lasers can't hit you if you shot them, and they pull others towards the shooter");
		}
		else if(str_comp(pArg, "me") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvSlashMe?"Players can use /me commands the famous IRC Command":"Players Can't use the /me command");
		}
		else if(str_comp(pArg, "timeout") == 0)
		{
			str_format(aBuf, sizeof(aBuf), "The Server Timeout is currently set to %d", g_Config.m_ConnTimeout);
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", aBuf);
		}
		else if(str_comp(pArg, "votes") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvVoteKick?"Players can use Callvote menu tab to kick offenders":"Players Can't use the Callvote menu tab to kick offenders");
			if(g_Config.m_SvVoteKick)
				str_format(aBuf, sizeof(aBuf), "Players are banned for %d second(s) if they get voted off", g_Config.m_SvVoteKickBantime);
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvVoteKickBantime?aBuf:"Players are just kicked and not banned if they get voted off");
		}
		else if(str_comp(pArg, "pause") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvPauseable?g_Config.m_SvPauseTime?"/pause is available on this server and it pauses your time too":"/pause is available on this server but it doesn't pause your time":"/pause is NOT available on this server");
		}
		else if(str_comp(pArg, "scores") == 0)
		{
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvHideScore?"Scores are private on this server":"Scores are public on this server");
		}
	}
}

void CGameContext::ConRules(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	bool Printed = false;
	if(g_Config.m_SvDDRaceRules)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No blocking.");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No insulting / spamming.");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No fun voting / vote spamming.");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Breaking any of these rules will result in a penalty, decided by server admins.");
		Printed = true;
	}
	if(g_Config.m_SvRulesLine1[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine1);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine2[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine2);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine3[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine3);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine4[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine4);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine5[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine5);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine6[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine6);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine7[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine7);
		Printed = true;
	}
	if(g_Config.m_SvRulesLine8[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine8);
		Printed=true;
	}
	if(g_Config.m_SvRulesLine9[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine9);
		Printed=true;
	}
	if(g_Config.m_SvRulesLine10[0])
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", g_Config.m_SvRulesLine10);
		Printed = true;
	}
	if(!Printed)
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No Rules Defined, Kill em all!!");
}

void CGameContext::ConKill(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];

	if(!pPlayer || (pPlayer->m_Last_Kill && pPlayer->m_Last_Kill + pSelf->Server()->TickSpeed() * g_Config.m_SvKillDelay > pSelf->Server()->Tick()))
		return;

	pPlayer->m_Last_Kill = pSelf->Server()->Tick();
	pPlayer->KillCharacter(WEAPON_SELF);
	pPlayer->m_RespawnTick = pSelf->Server()->Tick() + pSelf->Server()->TickSpeed() * g_Config.m_SvSuicidePenalty;
}

void CGameContext::ConTogglePause(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;

	if(g_Config.m_SvPauseable)
	{
		CCharacter* pChr = pPlayer->GetCharacter();
		if(!pPlayer->GetTeam() && pChr && (!pChr->GetWeaponGot(WEAPON_NINJA) || pChr->m_FreezeTime) && pChr->IsGrounded() && pChr->m_Pos==pChr->m_PrevPos && !pChr->Team() && !pPlayer->m_InfoSaved)
		{
			if(pPlayer->m_Last_Pause + pSelf->Server()->TickSpeed() * g_Config.m_SvPauseFrequency <= pSelf->Server()->Tick())
			{
				pPlayer->SaveCharacter();
				pPlayer->SetTeam(TEAM_SPECTATORS);
				pPlayer->m_InfoSaved = true;
				pPlayer->m_Last_Pause = pSelf->Server()->Tick();
			}
			else
				pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "You can\'t pause that often.");
		}
		else if(pPlayer->GetTeam()==TEAM_SPECTATORS && pPlayer->m_InfoSaved)
		{
			pPlayer->m_InfoSaved = false;
			pPlayer->m_PauseInfo.m_Respawn = true;
			pPlayer->SetTeam(TEAM_RED);
			//pPlayer->LoadCharacter();//TODO:Check if this system Works
		}
		else if(pChr)
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", (pChr->Team())?"You can't pause while you are in a team":pChr->GetWeaponGot(WEAPON_NINJA)?"You can't use /pause while you are a ninja":(!pChr->IsGrounded())?"You can't use /pause while you are a in air":"You can't use /pause while you are moving");
		else
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "No pause data saved.");
	}
	else
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Pause isn't allowed on this server.");
}

void CGameContext::ConTop5(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;

	if(g_Config.m_SvHideScore)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Showing the top 5 is not allowed on this server.");
		return;
	}

		if(pResult->NumArguments() > 0)
			pSelf->Score()->ShowTop5(pPlayer->GetCID(), pResult->GetInteger(0));
		else
			pSelf->Score()->ShowTop5(pPlayer->GetCID());
}
#if defined(CONF_SQL)
void CGameContext::ConTimes(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	if(g_Config.m_SvUseSQL)
	{
		CGameContext *pSelf = (CGameContext *)pUserData;
		CSqlScore *pScore = (CSqlScore *)pSelf->Score();
		CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
		if(!pPlayer)
			return;

		if(pResult->NumArguments() == 0)
		{
			pScore->ShowTimes(pPlayer->GetCID(),1);
			return;
		}

		else if(pResult->NumArguments() < 3)
		{
			if (pResult->NumArguments() == 1)
			{
				if(pResult->GetInteger(0) != 0)
					pScore->ShowTimes(pPlayer->GetCID(),pResult->GetInteger(0));
				else
					pScore->ShowTimes(pPlayer->GetCID(), (str_comp(pResult->GetString(0), "me") == 0) ? pSelf->Server()->ClientName(ClientID) : pResult->GetString(0),1);
				return;
			}
			else if (pResult->GetInteger(1) != 0)
			{
				pScore->ShowTimes(pPlayer->GetCID(), (str_comp(pResult->GetString(0), "me") == 0) ? pSelf->Server()->ClientName(ClientID) : pResult->GetString(0),pResult->GetInteger(1));
				return;
			}
		}

		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "/times needs 0, 1 or 2 parameter. 1. = name, 2. = start number");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Example: /times, /times me, /times Hans, /times \"Papa Smurf\" 5");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Bad: /times Papa Smurf 5 # Good: /times \"Papa Smurf\" 5 ");
	}
}
#endif

void CGameContext::ConRank(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	if(pResult->NumArguments() > 0)
		if(!g_Config.m_SvHideScore)
			pSelf->Score()->ShowRank(ClientID, pResult->GetString(0), true);
		else
			pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Showing the rank of other players is not allowed on this server.");
	else
		pSelf->Score()->ShowRank(ClientID, pSelf->Server()->ClientName(ClientID));
}

void CGameContext::ConToggleFly(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;
	CCharacter* pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;
	if(pSelf->m_apPlayers[ClientID]->Cheats.Super)
	{
		pChr->m_Fly = !pChr->m_Fly;
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", (pChr->m_Fly) ? "Fly enabled" : "Fly disabled");
	}
}

void CGameContext::ConMe(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	char aBuf[256 + 24];

	str_format(aBuf, 256 + 24, "'%s' %s", pSelf->Server()->ClientName(ClientID), pResult->GetString(0));
	if(g_Config.m_SvSlashMe)
		pSelf->SendChat(-2, CGameContext::CHAT_ALL, aBuf, ClientID);
	else
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "/me is disabled on this server, admin can enable it by using sv_slash_me");
}

void CGameContext::ConToggleEyeEmote(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;
	CCharacter* pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;

	pChr->m_EyeEmote = !pChr->m_EyeEmote;
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", (pChr->m_EyeEmote) ? "You can now use the preset eye emotes." : "You don't have any eye emotes, remember to bind some. (until you die)");
}

void CGameContext::ConToggleBroadcast(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;
	CCharacter* pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;

	pChr->m_BroadCast = !pChr->m_BroadCast;
}

void CGameContext::ConEyeEmote(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;
	CCharacter* pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;

	if (pResult->NumArguments() == 0)
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Emote commands are: /emote surprise /emote blink /emote close /emote angry /emote happy /emote pain");
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Example: /emote surprise 10 for 10 seconds or /emote surprise (default 1 second)");
	}
	else
	{
		if (pChr)
		{
			if (!str_comp(pResult->GetString(0), "angry"))
				pChr->m_DefEmote = EMOTE_ANGRY;
			else if (!str_comp(pResult->GetString(0), "blink"))
				pChr->m_DefEmote = EMOTE_BLINK;
			else if (!str_comp(pResult->GetString(0), "close"))
				pChr->m_DefEmote = EMOTE_BLINK;
			else if (!str_comp(pResult->GetString(0), "happy"))
				pChr->m_DefEmote = EMOTE_HAPPY;
			else if (!str_comp(pResult->GetString(0), "pain"))
				pChr->m_DefEmote = EMOTE_PAIN;
			else if (!str_comp(pResult->GetString(0), "surprise"))
				pChr->m_DefEmote = EMOTE_SURPRISE;
			else if (!str_comp(pResult->GetString(0), "normal"))
				pChr->m_DefEmote = EMOTE_NORMAL;
			else
				pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Unknown emote... Say /emote");

			int Duration = 1;
			if (pResult->NumArguments() > 1)
				Duration = pResult->GetInteger(1);

			pChr->m_DefEmoteReset = pSelf->Server()->Tick() + Duration * pSelf->Server()->TickSpeed();
		}
	}
}

void CGameContext::ConShowOthers(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[ClientID];
	if(!pPlayer)
		return;

	if(pPlayer->m_IsUsingDDRaceClient)
	{
		if(pResult->NumArguments())
			pPlayer->m_ShowOthers = pResult->GetInteger(0);
		else
			pPlayer->m_ShowOthers = !pPlayer->m_ShowOthers;
	}
	else
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "info", "Showing players from other teams is only available with DDRace Client, http://DDRace.info");
}


void CGameContext::Mute(NETADDR *Addr, int Secs, const char *pDisplayName)
{
	char aBuf[128];
	int Found = 0;
	// find a matching mute for this ip, update expiration time if found
	for(int i = 0; i < m_NumMutes; i++)
	{
		if(net_addr_comp(&m_aMutes[i].m_Addr, Addr) == 0)
		{
			m_aMutes[i].m_Expire = Server()->Tick() + Secs * Server()->TickSpeed();
			Found = 1;
		}
	}

	if(!Found) // nothing found so far, find a free slot..
	{
		if(m_NumMutes < MAX_MUTES)
		{
			m_aMutes[m_NumMutes].m_Addr = *Addr;
			m_aMutes[m_NumMutes].m_Expire = Server()->Tick() + Secs * Server()->TickSpeed();
			m_NumMutes++;
			Found = 1;
		}
	}
	if(Found)
	{
		str_format(aBuf, sizeof aBuf, "'%s' has been muted for %d seconds.", pDisplayName, Secs);
		SendChat(-1, CHAT_ALL, aBuf);
	}
	else // no free slot found
		Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "mute array is full");
}

void CGameContext::ConMute(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	((CGameContext *)pUserData)->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "Use either 'muteid <client_id> <seconds>' or 'muteip <ip> <seconds>'");
}

// mute through client id
void CGameContext::ConMuteID(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	NETADDR Addr;
	pSelf->Server()->GetClientAddr(Victim, &Addr);

	pSelf->Mute(&Addr, clamp(pResult->GetInteger(0), 1, 86400), pSelf->Server()->ClientName(Victim));
}

// mute through ip, arguments reversed to workaround parsing
void CGameContext::ConMuteIP(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	NETADDR Addr;
	if(net_addr_from_str(&Addr, pResult->GetString(0)))
	{
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "Invalid network address to mute");
	}
	pSelf->Mute(&Addr, clamp(pResult->GetInteger(1), 1, 86400), pResult->GetString(0));
}

// unmute by mute list index
void CGameContext::ConUnmute(IConsole::IResult *pResult, void *pUserData, int ClientID)
{

	CGameContext *pSelf = (CGameContext *)pUserData;
	char aIpBuf[64];
	char aBuf[64];
	int Index = pResult->GetInteger(0);

	if(Index < 0 || Index >= pSelf->m_NumMutes)
		return;

	pSelf->m_NumMutes--;
	pSelf->m_aMutes[Index] = pSelf->m_aMutes[pSelf->m_NumMutes];

	net_addr_str(&pSelf->m_aMutes[Index].m_Addr, aIpBuf, sizeof(aIpBuf));
	str_format(aBuf, sizeof(aBuf), "Unmuted %s" , aIpBuf);
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);
}

// list mutes
void CGameContext::ConMutes(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	char aIpBuf[64];
	char aBuf[128];
	pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "Active mutes:");
	for(int i = 0; i < pSelf->m_NumMutes; i++)
	{
		net_addr_str(&pSelf->m_aMutes[i].m_Addr, aIpBuf, sizeof(aIpBuf));
		str_format(aBuf, sizeof aBuf, "%d: \"%s\", %d seconds left", i, aIpBuf, (pSelf->m_aMutes[i].m_Expire - pSelf->Server()->Tick()) / pSelf->Server()->TickSpeed());
		pSelf->Console()->PrintResponse(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);
	}
}

