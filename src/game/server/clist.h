#ifndef CLANLIST_H
#define CLANLIST_H

#include "gamecontext.h"
#include <string>
#include <list>
#include <stdio.h>

class CLItem
{
	private:
		std::string UserName;
		std::string UserIp;
	public:
		CLItem(std::string name, std::string ip)
		{
			this->UserName = name;
			this->UserIp = ip;
		}
		std::string name() { return this->UserName; }
		std::string ip() { return this->UserIp; }
};

class CList
{
private:
	std::list<CLItem> ClanList;
	CGameContext *m_pGameServer;
public:
	CList(CGameContext *pGameServer) {m_pGameServer = pGameServer; Load();}

	void Load();
	void Save();
	void Show();
	bool Check(int CId);
	void Add(int CId);
	void Remove(int CId);

	CGameContext *GameServer() const { return m_pGameServer; }
};

#endif
