#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/stat.h>

bool file_exists(const char* strFilename)
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	intStat = stat(strFilename,&stFileInfo);
	if(intStat == 0) {
		blnReturn = true;
	}
	else
		blnReturn = false;
	return(blnReturn);
}

#include <string>
#include <sstream>
#include "gamecontext.h"

#include <string.h>

#include <engine/shared/config.h>
#include <engine/server/server.h>
#include "DeBugger.h"

using namespace std;

std::string DeBugger::get_temp_path(const char* art)
{
	std::ostringstream oss;

	char buf[256];
    str_format(buf, sizeof(buf), "%s", art);

	oss << buf << ".txt";
	return oss.str();
}

void DeBugger::Add(int id,const char* msg, const char* art)
{
    char TmpIP[128];
	GameServer()->Server()->GetClientIP(id, TmpIP, sizeof(TmpIP));

	std::ofstream ofs(get_temp_path(art).c_str(), ios_base::in | ios_base::out | ios_base::app);
	ofs << "IP : " << TmpIP << std::endl << "NAME : " << GameServer()->Server()->ClientName(id) << std::endl
	<< "MSG : " << msg << std::endl << "--------------------------------------------------" << std::endl ;
	ofs.close();

}
void DeBugger::AddNoId(const char* msg, const char* art)
{
	std::ofstream ofs(get_temp_path(art).c_str(), ios_base::in | ios_base::out | ios_base::app);
	ofs << std::endl << "MSG : " << msg << std::endl << "--------------------------------------------------" << std::endl ;
	ofs.close();
}
