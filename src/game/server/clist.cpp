#include "gamecontext.h"//This includes clist.hpp

#include <sstream>
#include <fstream>
#include <engine/shared/config.h>

using namespace std;

void CList::Load()
{
	fstream f;
	f.open(g_Config.m_SvClistFile, ios::in);
	ClanList.clear();
	while (!f.eof() && !f.fail())
	{
		//name/tmpip to read an put in (list<CLItem>)ClanList
		string name;
		string tmpip;
		getline(f, name);
		if(!f.eof() && name!="")//User without name? why is an unamed tee on ClanList ;p
		{
			getline(f, tmpip);
			ClanList.push_back(*new CLItem(name, tmpip));
		}
	}
	f.close();
}

void CList::Save()
{
	fstream f;
	f.open(g_Config.m_SvClistFile, ios::out);
	if(!f.fail())
		for(list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
			f << i->name() << endl << i->ip() << endl;
	f.close();
}

void CList::Show()
{
	/*Load();//TODO: If laggy/crashing remove that
	dbg_msg("CList", "Clanlisted users:");
	for(list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
	{
			dbg_msg("CList", "User: %s", i->name().c_str());
			dbg_msg("CList", "IP..: %s", i->ip().c_str());
			dbg_msg("CList", "-------------");
	}*/
}

bool CList::Check(int CId)
{
	Load();//TODO: If laggy/crashing remove that
	for(list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
	{
		char IpBuf[128];
		GameServer()->Server()->GetClientIPRange(CId, IpBuf, sizeof(IpBuf));
		if (i->name() == GameServer()->Server()->ClientName(CId) && i->ip() == IpBuf)
			return true;
	}
	return false;
}

void CList::Add(int CId)
{
	if (!Check(CId))
	{
		char IpBuf[128];
		GameServer()->Server()->GetClientIPRange(CId, IpBuf, sizeof(IpBuf));
		ClanList.push_back(*new CLItem(GameServer()->Server()->ClientName(CId), IpBuf));
		Save();
	}
	else
		dbg_msg("CList", "%s is already on the CList", GameServer()->Server()->ClientName(CId));
}

void CList::Remove(int CId)
{
	list<CLItem> CList_tmp;

	CList_tmp = ClanList;

	ClanList.clear();

	for(list<CLItem>::iterator i=CList_tmp.begin(); i!=CList_tmp.end(); i++)
		if (i->name() != GameServer()->Server()->ClientName(CId))
			ClanList.push_back(*new CLItem(i->name(), i->ip()));

	Save();
}
