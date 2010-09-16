/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */

#ifndef GAME_SERVER_ENTITY_DRAGGER_H
#define GAME_SERVER_ENTITY_DRAGGER_H

#include <game/server/entity.h>

class CCharacter;

class CDragger : public CEntity
{
	vec2 m_Core;
	float m_Strength;
	int m_EvalTick;
	void Move();
	void Drag();
	CCharacter * m_Target;
	bool m_NW;
public:


	CDragger(CGameWorld *pGameWorld, vec2 Pos, float Strength, bool NW=false);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int snapping_client);
};

#endif