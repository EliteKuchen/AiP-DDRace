/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/tl/string.h>
#include <engine/serverbrowser.h>

#include <engine/demo.h>
#include <engine/graphics.h>
#include <engine/textrender.h>
#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include <game/client/render.h>
#include <game/client/components/motd.h>
#include <game/localization.h>
#include "scoreboard.h"

CScoreboard::CScoreboard()
{
	OnReset();
}

void CScoreboard::ConKeyScoreboard(IConsole::IResult *pResult, void *pUserData, int ClientID)
{
	CScoreboard *pSelf = (CScoreboard *)pUserData;
	CServerInfo Info;

	pSelf->Client()->GetServerInfo(&Info);
	pSelf->m_IsGameTypeRace = str_find_nocase(Info.m_aGameType, "race");
	pSelf->m_Active = pResult->GetInteger(0) != 0;
}

void CScoreboard::OnReset()
{
	m_Active = false;
}

void CScoreboard::OnRelease()
{
	m_Active = false;
}

void CScoreboard::OnConsoleInit()
{
	Console()->Register("+scoreboard", "", CFGFLAG_CLIENT, ConKeyScoreboard, this, "Show scoreboard", 0);
}

void CScoreboard::RenderGoals(float x, float y, float w)
{
	float h = 50.0f;

	Graphics()->BlendNormal();
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0,0,0,0.5f);
	RenderTools()->DrawRoundRect(x-10.f, y-10.f, w, h, 10.0f);
	Graphics()->QuadsEnd();

	// render goals
	if(m_pClient->m_Snap.m_pGameobj)
	{
		if(m_pClient->m_Snap.m_pGameobj->m_ScoreLimit)
		{
			char aBuf[64];
			str_format(aBuf, sizeof(aBuf), "%s: %d", Localize("Score limit"), m_pClient->m_Snap.m_pGameobj->m_ScoreLimit);
			TextRender()->Text(0, x, y, 20.0f, aBuf, -1);
		}
		if(m_pClient->m_Snap.m_pGameobj->m_TimeLimit)
		{
			char aBuf[64];
			str_format(aBuf, sizeof(aBuf), Localize("Time limit: %d min"), m_pClient->m_Snap.m_pGameobj->m_TimeLimit);
			TextRender()->Text(0, x+220.0f, y, 20.0f, aBuf, -1);
		}
		if(m_pClient->m_Snap.m_pGameobj->m_RoundNum && m_pClient->m_Snap.m_pGameobj->m_RoundCurrent)
		{
			char aBuf[64];
			str_format(aBuf, sizeof(aBuf), "%s %d/%d", Localize("Round"), m_pClient->m_Snap.m_pGameobj->m_RoundCurrent, m_pClient->m_Snap.m_pGameobj->m_RoundNum);
			float tw = TextRender()->TextWidth(0, 20.0f, aBuf, -1);
			TextRender()->Text(0, x+w-tw-20.0f, y, 20.0f, aBuf, -1);
		}
	}
}

void CScoreboard::RenderSpectators(float x, float y, float w)
{
	char aBuffer[1024*4];
	int Count = 0;
	float h = 120.0f;
	
	str_format(aBuffer, sizeof(aBuffer), "%s: ", Localize("Spectators"));

	Graphics()->BlendNormal();
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0,0,0,0.5f);
	RenderTools()->DrawRoundRect(x-10.f, y-10.f, w, h, 10.0f);
	Graphics()->QuadsEnd();
	
	for(int i = 0; i < Client()->SnapNumItems(IClient::SNAP_CURRENT); i++)
	{
		IClient::CSnapItem Item;
		const void *pData = Client()->SnapGetItem(IClient::SNAP_CURRENT, i, &Item);

		if(Item.m_Type == NETOBJTYPE_PLAYERINFO)
		{
			const CNetObj_PlayerInfo *pInfo = (const CNetObj_PlayerInfo *)pData;
			if(pInfo->m_Team == TEAM_SPECTATORS)
			{
				if(Count)
					str_append(aBuffer, ", ", sizeof(aBuffer));
				if(m_IsGameTypeRace)
					if (g_Config.m_ClShowIDs)
					{
						char aId[4];
						str_format(aId,sizeof(aId),"%d:",pInfo->m_ClientID);
						str_append(aBuffer, aId, sizeof(aBuffer));
					}
				str_append(aBuffer, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, sizeof(aBuffer));
				Count++;
			}
		}
	}
	
	TextRender()->Text(0, x+10, y, 32, aBuffer, (int)w-20);
}

void CScoreboard::RenderScoreboard(float x, float y, float w, int Team, const char *pTitle)
{
	if(Team == TEAM_SPECTATORS)
		return;

	//float ystart = y;
	float h = 740.0f;

	Graphics()->BlendNormal();
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0,0,0,0.5f);
	RenderTools()->DrawRoundRect(x-10.f, y, w, h, 17.0f);
	Graphics()->QuadsEnd();

	// render title
	if(!pTitle)
	{
		if(m_pClient->m_Snap.m_pGameobj->m_GameOver)
			pTitle = Localize("Game over");
		else
			pTitle = Localize("Score board");
	}

	float Offset = 80.0f;
	float DataOffset = 130;
	float tw = TextRender()->TextWidth(0, 48, pTitle, -1);
	TextRender()->Text(0, x+10, y, 48, pTitle, -1);

	if(!m_IsGameTypeRace || !g_Config.m_ClDDRaceScoreBoard)
		if(m_pClient->m_Snap.m_pGameobj)
		{
			char aBuf[128];
			int Score = Team == TEAM_RED ? m_pClient->m_Snap.m_pGameobj->m_TeamscoreRed : m_pClient->m_Snap.m_pGameobj->m_TeamscoreBlue;
			str_format(aBuf, sizeof(aBuf), "%d", Score);
			tw = TextRender()->TextWidth(0, 48, aBuf, -1);
			TextRender()->Text(0, x+w-tw-30, y, 48, aBuf, -1);
		}

	y += 54.0f;

	// render headlines
	TextRender()->Text(0, x+10, y, 24.0f, Localize("Score"), -1);
	if(m_IsGameTypeRace && g_Config.m_ClDDRaceScoreBoard)
	{
		TextRender()->Text(0, x+125+Offset, y, 24.0f, Localize("Name"), -1);
		TextRender()->Text(0, x+w-75, y, 24.0f, Localize("Ping"), -1);
	}
	else
	{
		TextRender()->Text(0, x+125, y, 24.0f, Localize("Name"), -1);
		TextRender()->Text(0, x+w-70, y, 24.0f, Localize("Ping"), -1);
	}
	y += 29.0f;

	float FontSize = 35.0f;
	float LineHeight = 50.0f;
	float TeeSizeMod = 1.0f;
	float TeeOffset = 0.0f;
	
	if(m_pClient->m_Snap.m_aTeamSize[Team] > 13)
	{
		FontSize = 30.0f;
		LineHeight = 40.0f;
		TeeSizeMod = 0.8f;
		TeeOffset = -5.0f;
	}
	
	// render player scores
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		const CNetObj_PlayerInfo *pInfo = m_pClient->m_Snap.m_paInfoByScore[i];
		if(!pInfo || pInfo->m_Team != Team)
			continue;

		// make sure that we render the correct team

		char aBuf[128];
		if(pInfo->m_Local)
		{
			// background so it's easy to find the local player
			Graphics()->TextureSet(-1);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1,1,1,0.25f);
			RenderTools()->DrawRoundRect(x, y, w-20, LineHeight*0.95f, 17.0f);
			Graphics()->QuadsEnd();
		}

		float FontSizeResize = FontSize;
		float Width;
		if(m_IsGameTypeRace && g_Config.m_ClDDRaceScoreBoard)
		{
			const float ScoreWidth = 150.0f;
			const float PingWidth = 60.0f;

				// reset time
			if(pInfo->m_Score == -9999)
				m_pClient->m_aClients[pInfo->m_ClientID].m_Score = 0;

			int Time = m_pClient->m_aClients[pInfo->m_ClientID].m_Score;
			if(Time > 0)
			{
				str_format(aBuf, sizeof(aBuf), "%02d:%02d.%02d", Time/6000, Time/100-(Time/6000*60), Time % 100);
				while((Width = TextRender()->TextWidth(0, FontSizeResize, aBuf, -1)) > ScoreWidth)
					--FontSizeResize;
				TextRender()->Text(0, x+ScoreWidth-Width, y+(FontSize-FontSizeResize)/2, FontSizeResize, aBuf, -1);
			}


			FontSizeResize = FontSize;


			while(TextRender()->TextWidth(0, FontSizeResize, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, -1) > w-163.0f-Offset-PingWidth)
				--FontSizeResize;
			if (m_IsGameTypeRace && g_Config.m_ClShowIDs)
			{
				char aId[64] = "";
				str_format(aId, sizeof(aId),"%d:", pInfo->m_ClientID);
				str_append(aId, m_pClient->m_aClients[pInfo->m_ClientID].m_aName,sizeof(aId));
				TextRender()->Text(0, x+128.0f+Offset, y+(FontSize-FontSizeResize)/2, FontSizeResize, aId, -1);
			}
			else
				TextRender()->Text(0, x+128.0f+Offset, y+(FontSize-FontSizeResize)/2, FontSizeResize, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, -1);
			FontSizeResize = FontSize;
			str_format(aBuf, sizeof(aBuf), "%d", clamp(pInfo->m_Latency, -9999, 9999));
			while((Width = TextRender()->TextWidth(0, FontSizeResize, aBuf, -1)) > PingWidth)
				--FontSizeResize;
		}
		else
		{
			const float ScoreWidth = 60.0f;
			const float PingWidth = 60.0f;
			str_format(aBuf, sizeof(aBuf), "%d", clamp(pInfo->m_Score, -9999, 9999));
			while((Width = TextRender()->TextWidth(0, FontSizeResize, aBuf, -1)) > ScoreWidth)
				--FontSizeResize;
			TextRender()->Text(0, x+ScoreWidth-Width, y+(FontSize-FontSizeResize)/2, FontSizeResize, aBuf, -1);


			FontSizeResize = FontSize;
			while(TextRender()->TextWidth(0, FontSizeResize, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, -1) > w-163.0f-PingWidth)
				--FontSizeResize;
			if (m_IsGameTypeRace && g_Config.m_ClShowIDs)
			{
				char aId[64] = "";
				str_format(aId, sizeof(aId),"%d:", pInfo->m_ClientID);
				str_append(aId, m_pClient->m_aClients[pInfo->m_ClientID].m_aName,sizeof(aId));
				TextRender()->Text(0, x+128.0f+Offset, y+(FontSize-FontSizeResize)/2, FontSizeResize, aId, -1);
			}
			else
				TextRender()->Text(0, x+128.0f+Offset, y+(FontSize-FontSizeResize)/2, FontSizeResize, m_pClient->m_aClients[pInfo->m_ClientID].m_aName, -1);
			FontSizeResize = FontSize;
			str_format(aBuf, sizeof(aBuf), "%d", clamp(pInfo->m_Latency, -9999, 9999));
			while((Width = TextRender()->TextWidth(0, FontSizeResize, aBuf, -1)) > PingWidth)
				--FontSizeResize;
		}
		TextRender()->Text(0, x+w-35.0f-Width, y+(FontSize-FontSizeResize)/2, FontSizeResize, aBuf, -1);

		// render avatar
		if((m_pClient->m_Snap.m_paFlags[0] && m_pClient->m_Snap.m_paFlags[0]->m_CarriedBy == pInfo->m_ClientID) ||
			(m_pClient->m_Snap.m_paFlags[1] && m_pClient->m_Snap.m_paFlags[1]->m_CarriedBy == pInfo->m_ClientID))
		{
			Graphics()->BlendNormal();
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
			Graphics()->QuadsBegin();

			if(pInfo->m_Team == TEAM_RED) RenderTools()->SelectSprite(SPRITE_FLAG_BLUE, SPRITE_FLAG_FLIP_X);
			else RenderTools()->SelectSprite(SPRITE_FLAG_RED, SPRITE_FLAG_FLIP_X);
			
			float size = 64.0f;
			IGraphics::CQuadItem QuadItem;
			if(m_IsGameTypeRace && g_Config.m_ClDDRaceScoreBoard)
				QuadItem = IGraphics::CQuadItem(x+55+DataOffset, y-15, size/2, size);
			else
				QuadItem = IGraphics::CQuadItem(x+55, y-15, size/2, size);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
		
		CTeeRenderInfo TeeInfo = m_pClient->m_aClients[pInfo->m_ClientID].m_RenderInfo;
		TeeInfo.m_Size *= TeeSizeMod;

		if(m_IsGameTypeRace && g_Config.m_ClDDRaceScoreBoard)
			RenderTools()->RenderTee(CAnimState::GetIdle(), &TeeInfo, EMOTE_NORMAL, vec2(1,0), vec2(x+50+DataOffset, y+28+TeeOffset));
		else
			RenderTools()->RenderTee(CAnimState::GetIdle(), &TeeInfo, EMOTE_NORMAL, vec2(1,0), vec2(x+90, y+28+TeeOffset));

		
		y += LineHeight;
	}
}

void CScoreboard::RenderRecordingNotification(float x)
{
	if(!m_pClient->DemoRecorder()->IsRecording())
		return;

	//draw the box
	Graphics()->BlendNormal();
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);
	RenderTools()->DrawRoundRectExt(x, 0.0f, 120.0f, 50.0f, 15.0f, CUI::CORNER_B);
	Graphics()->QuadsEnd();

	//draw the red dot
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	RenderTools()->DrawRoundRect(x+20, 15.0f, 20.0f, 20.0f, 10.0f);
	Graphics()->QuadsEnd();

	//draw the text
	TextRender()->Text(0, x+50.0f, 8.0f, 24.0f, Localize("REC"), -1);
}

void CScoreboard::OnRender()
{
	if(!Active())
		return;
		
	// if the score board is active, then we should clear the motd message aswell
	if(m_pClient->m_pMotd->IsActive())
		m_pClient->m_pMotd->Clear();
	

	float Width = 400*3.0f*Graphics()->ScreenAspect();
	float Height = 400*3.0f;
	
	Graphics()->MapScreen(0, 0, Width, Height);

	float w;
	if(m_IsGameTypeRace && g_Config.m_ClDDRaceScoreBoard)
		w = 750.0f;
	else
		w = 650.0f;

	if(m_pClient->m_Snap.m_pGameobj && !(m_pClient->m_Snap.m_pGameobj->m_Flags&GAMEFLAG_TEAMS))
		RenderScoreboard(Width/2-w/2, 150.0f, w, 0, 0);
	else
	{
			
		if(m_pClient->m_Snap.m_pGameobj && m_pClient->m_Snap.m_pGameobj->m_GameOver)
		{
			const char *pText = Localize("Draw!");
			if(m_pClient->m_Snap.m_pGameobj->m_TeamscoreRed > m_pClient->m_Snap.m_pGameobj->m_TeamscoreBlue)
				pText = Localize("Red team wins!");
			else if(m_pClient->m_Snap.m_pGameobj->m_TeamscoreBlue > m_pClient->m_Snap.m_pGameobj->m_TeamscoreRed)
				pText = Localize("Blue team wins!");
				
			float w = TextRender()->TextWidth(0, 86.0f, pText, -1);
			TextRender()->Text(0, Width/2-w/2, 39, 86.0f, pText, -1);
		}
		
		RenderScoreboard(Width/2-w-20, 150.0f, w, TEAM_RED, Localize("Red team"));
		RenderScoreboard(Width/2 + 20, 150.0f, w, TEAM_BLUE, Localize("Blue team"));
	}

	RenderGoals(Width/2-w/2, 150+750+25, w);
	RenderSpectators(Width/2-w/2, 150+750+25+50+25, w);
	RenderRecordingNotification((Width/7)*4);
}

bool CScoreboard::Active()
{
	// if we activly wanna look on the scoreboard	
	if(m_Active)
		return true;
		
	if(m_pClient->m_Snap.m_pLocalInfo && m_pClient->m_Snap.m_pLocalInfo->m_Team != TEAM_SPECTATORS)
	{
		// we are not a spectator, check if we are dead
		if(!m_pClient->m_Snap.m_pLocalCharacter)
			return true;
	}

	// if the game is over
	if(m_pClient->m_Snap.m_pGameobj && m_pClient->m_Snap.m_pGameobj->m_GameOver)
		return true;

	return false;
}
