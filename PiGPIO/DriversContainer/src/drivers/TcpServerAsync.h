//
// Created by curiosul on 10/10/18.
//

#ifndef _TCPSERVERASYNC_H
#define _TCPSERVERASYNC_H

#include <stdio.h>
#include <string.h> 	//strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 	//close
#include <arpa/inet.h> 	//close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>	//FD_SET, FD_ISSET, FD_ZERO macros
#include <thread>

class TcpServerAsync
{
public:
	typedef enum Status
	{
		STOPPED,
		STARTED
	}status_t;
	
	typedef struct Client
	{
		int Fd = 0;
		uint16_t Port = 0;
		char Ip[16] = {'\0'};
		struct sockaddr_in address;
	}client_t;
	
	static const uint16_t RECV_BUFFER_SIZE = 256;
	
	TcpServerAsync(uint16_t port, uint16_t max_clients);
	~TcpServerAsync();
	
	void Start();
	void Stop();
	
	int Write(const client_t *client, char *data);

protected:
	virtual void ClientConnected(const client_t *client);
	virtual void ClientDisconnected(const client_t *client);
	virtual void DataReceived(const client_t *client, char *data);
	virtual void DataSend(const client_t *client, int bytesSend);
	
private:
	client_t *ClientsArray;
	int  ServerSocket, NewSocket;
	uint16_t ServerPort, MaxClients;
	status_t Status = STOPPED;
	
	//set of socket descriptors
	fd_set readfds;
	int max_sd, activity;
	
	ssize_t Readbytes = 0;
	char RecvBuffer[RECV_BUFFER_SIZE] = {'\0'};
	
	std::thread BackgroundWorker;
	void BackgroundWork();
};


#endif //_TCPSERVERASYNC_H
