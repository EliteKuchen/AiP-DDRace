/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_CHARACTER_H
#define GAME_SERVER_ENTITIES_CHARACTER_H

#include <game/server/entity.h>
#include <game/generated/server_data.h>
#include <game/generated/protocol.h>

#include <game/gamecore.h>

class CGameTeams;

enum
{
	WEAPON_GAME = -3, // team switching etc
	WEAPON_SELF = -2, // console kill command
	WEAPON_WORLD = -1, // death tiles etc
};

class CCharacter : public CEntity
{
	MACRO_ALLOC_POOL_ID()
	
public:
	//character's size
	static const int ms_PhysSize = 28;

	CCharacter(CGameWorld *pWorld);
	
	virtual void Reset();
	virtual void Destroy();
	virtual void Tick();
	virtual void TickDefered();
	virtual void Snap(int SnappingClient);
		
	bool IsGrounded();
	
	void SetWeapon(int W);
	void HandleWeaponSwitch();
	void DoWeaponSwitch();
	
	void HandleWeapons();
	void HandleNinja();

	void OnPredictedInput(CNetObj_PlayerInput *pNewInput);
	void OnDirectInput(CNetObj_PlayerInput *pNewInput);
	void FireWeapon();

	void Die(int Killer, int Weapon);
	bool TakeDamage(vec2 Force, int Dmg, int From, int Weapon);	

	bool Spawn(class CPlayer *pPlayer, vec2 Pos);
	bool Remove();
	
	bool IncreaseHealth(int Amount);
	bool IncreaseArmor(int Amount);
	
	bool GiveWeapon(int Weapon, int Ammo);
	void GiveNinja();
	
	void SetEmote(int Emote, int Tick);
	
	bool IsAlive() const { return m_Alive; }
	class CPlayer *GetPlayer() { return m_pPlayer; }
	
private:
	// player controlling this character
	class CPlayer *m_pPlayer;
	
	bool m_Alive;

	// weapon info
	CEntity *m_apHitObjects[10];
	int m_NumObjectsHit;
	
	struct WeaponStat
	{
		int m_AmmoRegenStart;
		int m_Ammo;
		int m_Ammocost;
		bool m_Got;
		
	} m_aWeapons[NUM_WEAPONS];
	
	int m_ActiveWeapon;
	int m_LastWeapon;
	int m_QueuedWeapon;
	
	int m_ReloadTimer;
	int m_AttackTick;
	
	int m_DamageTaken;

	int m_EmoteType;
	int m_EmoteStop;
	
	// last tick that the player took any action ie some input
	int m_LastAction;

	// these are non-heldback inputs
	CNetObj_PlayerInput m_LatestPrevInput;
	CNetObj_PlayerInput m_LatestInput;

	// input	
	CNetObj_PlayerInput m_PrevInput;
	CNetObj_PlayerInput m_Input;
	int m_NumInputs;
	int m_Jumped;
	
	int m_DamageTakenTick;

	int m_Health;
	int m_Armor;

	// ninja
	struct
	{
		vec2 m_ActivationDir;
		int m_ActivationTick;
		int m_CurrentMoveTime;
		
	} m_Ninja;

	int m_PlayerState;// if the client is chatting, accessing a menu or so

	// the player core for the physics	
	CCharacterCore m_Core;
	
	// info for dead reckoning
	int m_ReckoningTick; // tick that we are performing dead reckoning From
	CCharacterCore m_SendCore; // core that we should send
	CCharacterCore m_ReckoningCore; // the dead reckoning core
	
	//DDRace
	void HandleFly();
	void HandleTiles(int Index);
	float m_Time;
	int m_LastBroadcast;
	void DDRaceInit();
	void HandleSkippableTiles(int Index);
	void HandleAiPTiles(int Index);
	void DDRaceTick();
	void AiPTick();
	void DDRacePostCoreTick();
	void HandleBroadcast();

	int m_LastTile;
	int m_MaxJumps;
	int m_TmpJumps;
	bool m_PrevGrounded;
public:
	CGameTeams* Teams();
	bool Freeze(int Time);
	bool Freeze();
	bool UnFreeze();
	void GiveAllWeapons();
	int m_DDRaceState;
	void OnFinish();
	int Team();
	bool CanCollide(int ClientID);
	bool SameTeam(int ClientID);
	int m_TeamBeforeSuper;
	bool m_Fly;
	int m_FreezeTime;
	int m_FreezeTick;
	bool m_DeepFreeze;
	bool m_EndlessHook;
	int m_PainSoundTimer;
	int m_DefEmote;
	int m_DefEmoteReset;
	int m_LastMove;
	int m_StartTime;
	int m_RefreshTime;
	vec2 m_PrevPos;
	int m_CpTick;
	int m_CpActive;
	float m_CpCurrent[25];
	bool m_BroadCast;
	int m_TileIndex;
	int m_TileFlags;
	int m_TileFIndex;
	int m_TileFFlags;
	int m_TileSIndex;
	int m_TileSFlags;
	int m_TileIndexL;
	int m_TileFlagsL;
	int m_TileFIndexL;
	int m_TileFFlagsL;
	int m_TileSIndexL;
	int m_TileSFlagsL;
	int m_TileIndexR;
	int m_TileFlagsR;
	int m_TileFIndexR;
	int m_TileFFlagsR;
	int m_TileSIndexR;
	int m_TileSFlagsR;
	int m_TileIndexT;
	int m_TileFlagsT;
	int m_TileFIndexT;
	int m_TileFFlagsT;
	int m_TileSIndexT;
	int m_TileSFlagsT;
	int m_TileIndexB;
	int m_TileFlagsB;
	int m_TileFIndexB;
	int m_TileFFlagsB;
	int m_TileSIndexB;
	int m_TileSFlagsB;
	vec2 m_Intersection;
	bool m_EyeEmote;
	int64 m_LastStartWarning;
	// Setters/Getters because i don't want to modify vanilla vars access modifiers
	int GetLastWeapon() { return m_LastWeapon; };
	void SetLastWeapon(int LastWeap) {m_LastWeapon = LastWeap; };
	int GetActiveWeapon() { return m_ActiveWeapon; };
	void SetActiveWeapon(int ActiveWeap) {m_ActiveWeapon = ActiveWeap; };
	int GetPlayerState() { return m_PlayerState; };
	void SetPlayerState(int PlayerState) {m_PlayerState = PlayerState; };
	void SetLastAction(int LastAction) {m_LastAction = LastAction; };
	int GetArmor() { return m_Armor; };
	void SetArmor(int Armor) {m_Armor = Armor; };
	CCharacterCore GetCore() { return m_Core; };
	void SetCore(CCharacterCore Core) {m_Core = Core; };
	CCharacterCore* Core() { return &m_Core; };
	bool GetWeaponGot(int Type) { return m_aWeapons[Type].m_Got; };
	void SetWeaponGot(int Type, bool Value) { m_aWeapons[Type].m_Got = Value; };
	int GetWeaponAmmo(int Type) { return m_aWeapons[Type].m_Ammo; };
	int GetJumps() { return m_MaxJumps; };
	void SetJumps(int jumps) { m_MaxJumps = jumps; };
	void SetWeaponAmmo(int Type, int Value) { m_aWeapons[Type].m_Ammo = Value; };
	bool IsAlive() { return m_Alive; };
	void SetEmoteType(int EmoteType) { m_EmoteType = EmoteType; };
	void SetEmoteStop(int EmoteStop) { m_EmoteStop = EmoteStop; };
	void SetNinjaActivationDir(vec2 ActivationDir) { m_Ninja.m_ActivationDir = ActivationDir; };
	void SetNinjaActivationTick(int ActivationTick) { m_Ninja.m_ActivationTick = ActivationTick; };
	void SetNinjaCurrentMoveTime(int CurrentMoveTime) { m_Ninja.m_CurrentMoveTime = CurrentMoveTime; };

	//EKedit
	vec2 m_ResetPos;
	bool resetting;
};

enum 
{
	DDRACE_NONE = 0,
	DDRACE_STARTED,
	DDRACE_CHEAT, // no time and won't start again unless ordered by a mod or death
	DDRACE_FINISHED
};

enum//for int m_LastTile
{
	ON_FREE = 0,//not in rainbow,reload....bloody zone
	ON_RAINBOW,
	ON_RELOAD,
	ON_ADMIN,
	ON_MEMBER,
	ON_SUPER,
	ON_HAMMER,
	ON_INVIS,
	ON_BLOODY,
	ON_NOEXTRAS,
	ON_NONINJA,
	ON_JMPRESET,
	ON_JMPP,
	ON_JMPM
};
#endif
