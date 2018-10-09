//
// Created by curiosul on 10/9/18.
//

#ifndef DRIVERSCONTAINER_SERVERTCP_H
#define DRIVERSCONTAINER_SERVERTCP_H

#include "drivers/TcpServer.h"

class ServerTCP : TcpServer
{
	ServerTCP() : TcpServer(1337)
	{
	
	}

};

ServerTCP g_Server;

#endif //DRIVERSCONTAINER_SERVERTCP_H
