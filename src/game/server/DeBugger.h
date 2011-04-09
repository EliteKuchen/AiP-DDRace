#ifndef DeBugger_H
#define DeBugger_H

class DeBugger
{
private:
	CGameContext *m_pGameServer;
protected:
	std::string get_temp_path(const char* art);
public:
	DeBugger(CGameContext *pGameServer) {m_pGameServer = pGameServer;}
	CGameContext *GameServer() {return m_pGameServer;}
	
	void Add(int id,const char* msg, const char* art);
	void AddNoId(const char* msg, const char* art);
};

#endif
