/*

	Client Side

	The client can :
	request a list of users who are available to chat
	connect to a user who is available to chat
	wait for an incoming connection
	logout from the server

*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define request "request"
#define logout "logout"
#define conct "connect"
#define wait "wait"
#define kill "kill"

/*
	Structure to maintain the list of other users
*/
struct maintain
{
	int port;
	char ip[100];
}list [100];

int ind = 0;	//index pointer for the list of other users

main()
{
    struct sockaddr_in client,server;

    int s,n;

    char rec_buffer[100],send_buffer[100];

    //setting up the connection variables
    s=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=15515;
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    printf("\nClient side has been setup successfully...\n");

    n=sizeof(server);
    
    //connectinf to the server
    int status = connect(s,(struct sockaddr *)&server,n);

    if(status == -1)
    {
    	printf("Connection failure\n");
		return 1;
    }

	while(1)
	{
		printf("1> Get the list of clients\n2> Connect to a client\n3> Wait for connection\n4> Logout\n\n");
		
		int dec;
		scanf("%d", &dec);

		if(dec == 1)	//requesting for a list of users
		{
			send(s,&request,sizeof(request),0);
			int num;
			recv(s,&num,sizeof(num),0);
			
			ind = 0;
			
			while(num--)
			{
				char ip[100]; int port;
				recv(s,&ip,sizeof(ip),0);
				recv(s,&port,sizeof(port),0);
				strcpy(list[ind].ip,ip);
				list[ind].port = port;
				printf("%d ---> %s %d\n", ind, ip, port);
				ind++;
			}
			
		}
		else if(dec==2)		//connecting to an online user
		{
			send(s,&connect,sizeof(conct),0);
			printf("Enter the id of client you want to connect to : ");

			int conn;
			scanf("%d", &conn);

			//sending server the info of the client we want to connect to
			send(s,&list[conn].ip,sizeof(list[conn].ip),0);
			send(s,&list[conn].port,sizeof(list[conn].port),0);

			while(1)	//chatting
			{
				printf("Enter the message to be sent : ");
				scanf("%s", send_buffer);
				send(s,&send_buffer,sizeof(send_buffer),0);
				if(strcmp(send_buffer,"die")==0)
					break;
				recv(s,&rec_buffer,sizeof(rec_buffer),0);
				if(strcmp(rec_buffer,"die")==0)
				{
					send(s,&kill,sizeof(kill),0);
					break;
				}
				printf("Reply > %s\n",rec_buffer);
			}
		}
		else if(dec==3)		//go online
		{
			int conn;

			//wait for a chat request
			send(s,&wait,sizeof(wait),0);
			recv(s,&conn,sizeof(conn),0);
			printf("Got a chat request\n");
			send(s,&conn,sizeof(conn),0);
			while(1)
			{
				recv(s,&rec_buffer,sizeof(rec_buffer),0);
				if(strcmp(rec_buffer,"die")==0)
				{
					send(s,&kill,sizeof(kill),0);
					break;
				}
				printf("Reply > %s\n",rec_buffer);
				printf("Enter the message to be sent : ");
				scanf("%s", send_buffer);
				send(s,&send_buffer,sizeof(send_buffer),0);
				if(strcmp(send_buffer,"die")==0)
					break;
			}
		}
		else if(dec==4)	//logout from the server
		{
			send(s,&logout,sizeof(logout),0);
			close(s);
			return 0;
		}
	}

	close(s);
    return 0;
}