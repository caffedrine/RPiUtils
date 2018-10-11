//
// Created by curiosul on 10/9/18.
//

#ifndef DRIVERSCONTAINER_SERVERTCP_H
#define DRIVERSCONTAINER_SERVERTCP_H

#include "drivers/TcpServer.h"

class ServerTCP : TcpServer
{
public:
	ServerTCP() : TcpServer(1337)
	{
		startServer();
		
	}
private:

};

ServerTCP g_Server;

#endif //DRIVERSCONTAINER_SERVERTCP_H
