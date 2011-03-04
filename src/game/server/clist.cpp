#include "gamecontext.h"//This includes clist.hpp

#include <sstream>
#include <fstream>

using namespace std;

void CList::Load()
{
	fstream f;
	f.open("ClanList.txt", std::ios::in);//Filename: g_Config.m_ClanFile (standart: "ClanList.txt") - TODO
	ClanList.clear();
	while (!f.eof() && !f.fail())
	{
		//temp-name/-ip to read an put in (list<CLItem>)ClanList
		string name;
		string tmpip;
		getline(f, name);
		if(!f.eof() && name!="")//User without name? why is an unamed tee on ClanList ;p
		{
			std::getline(f, tmpip);
			ClanList.push_back(*new CLItem(name, tmpip));
		}
	}
	f.close();
}

void CList::Save()
{
	std::fstream f;
	f.open("ClanList.txt", std::ios::out);//Filename: g_Config.m_ClanFile (standart: "ClanList.txt") - TODO
	if(!f.fail())
		for(std::list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
			f << i->name() << std::endl << i->ip() << std::endl;
	f.close();
}

void CList::Show()
{
	dbg_msg("CList", "Clanlisted users:");
	for(std::list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
	{
			dbg_msg("CList", "User: %s", i->name().c_str());
			dbg_msg("CList", "IP..: %s", i->ip().c_str());
			dbg_msg("CList", "-------------");
	}
}

bool CList::Check(int CId)
{
	for(std::list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
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
	std::list<CLItem> CList_tmp;

	/*for(std::list<CLItem>::iterator i=ClanList.begin(); i!=ClanList.end(); i++)
		ClanList.push_back(*new CLItem(i->name(), i->ip()));*/
	CList_tmp = ClanList;

	ClanList.clear();

	for(std::list<CLItem>::iterator i=CList_tmp.begin(); i!=CList_tmp.end(); i++)
		if (i->name() != GameServer()->Server()->ClientName(CId))
			ClanList.push_back(*new CLItem(i->name(), i->ip()));

	Save();
}
