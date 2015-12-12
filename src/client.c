/*
	Author : pakhandi
	Repository : CTalk

	TCP chat system

	Client Side
	The client can :
	> request a list of users who are available to chat
	> connect to a user who is available to chat
	> wait for an incoming connection
	> logout from the server

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
#define die "die"

/*
	Structure to maintain the list of other users
*/
struct maintain
{
	int port;
	char ip[100];
}list [100];

int ind = 0;	//index pointer for the list of other users
int s, n;
char rec_buffer[100],send_buffer[100];

int *ptr[2];
pthread_t tid[2];

//Function to keep receiving the messages from server
void * keep_receiving()
{
	while(1)
	{
		//Check if there is a message in the buffer
		recv(s,&rec_buffer,sizeof(rec_buffer),MSG_PEEK);
		if(strlen(rec_buffer) == 0)
			continue;

		//Get the message in the buffer
		recv(s,&rec_buffer,sizeof(rec_buffer),0);

		//If the message is "end"
		if(strcmp(rec_buffer,"end")==0)
		{
			//Ask the server to close this chat session
			strcpy(send_buffer,die);
			send(s,&send_buffer,sizeof(send_buffer),0);

			//Cancel the keep_sending thread
			if(pthread_cancel(tid[1]) == 0)
				printf("keep_receiving cancelled keep_sending");
			else
				printf("keep_receiving failed");
			printf("\n");

			//Exit this thread
			int ret = 0;
			pthread_exit(&ret);
		}
		printf("Reply > %s\n",rec_buffer);
	}
}

//Function to keep sending message to server
void * keep_sending()
{
	while(1)
	{
		//Wait for user input
		printf("> : ");
		scanf("%s", send_buffer);

		//Ask the server to forward the message to other client
		send(s,&send_buffer,sizeof(send_buffer),0);

		//If the chat has to be ended
		if(strcmp(send_buffer,"end")==0)
		{
			//Cancel the keep_receiving thread
			if(pthread_cancel(tid[0]) == 0)
				printf("keep_sending cancelled keep_receiving");
			else
				printf("keep_sending failed");
			printf("\n");

			//Exit this thread
			memset(send_buffer,0,sizeof(send_buffer));
			int ret = 0;
			pthread_exit(&ret);
		}
	}
}

//Function to update the list of online users
void update_list()
{
	send(s,&request,sizeof(request),0);
	int num;
	recv(s,&num,sizeof(num),0);
	
	ind = 0;
	
	//Updating the list of online users
	printf("ID ---> ______IP______ : PORT\n");
	while(num--)
	{
		char ip[100]; int port;
		recv(s,&ip,sizeof(ip),0);
		recv(s,&port,sizeof(port),0);
		strcpy(list[ind].ip,ip);
		list[ind].port = port;
		printf("%d  ---> %-15s : %d\n", ind, ip, port);
		ind++;
	}
}

main()
{
	struct sockaddr_in client,server;

	
	//setting up the connection variables
	s=socket(AF_INET,SOCK_STREAM,0);
	server.sin_family=AF_INET;
	server.sin_port=15518;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");

	printf("\nClient side has been setup successfully...\n");

	n=sizeof(server);
	
	//connecting to the server
	int status = connect(s,(struct sockaddr *)&server,n);

	if(status == -1)
	{
		printf("Connection failure\n");
		return 1;
	}

	while(1)
	{
		printf("\n\n1> Get the list of clients\n2> Connect to a client\n3> Wait for connection\n4> Logout\n\n");
		
		int dec;
		scanf("%d", &dec);

		if(dec == 1)	//requesting for a list of users
		{
			update_list();
		}
		else if(dec==2)		//connecting to an online user
		{
			update_list();
			
			send(s,&conct,sizeof(conct),0);
			printf("\n\nEnter the id of client you want to connect to : ");

			int conn;
			scanf("%d", &conn);

			if(conn >= ind)
			{
				printf("The client ID is wrong\n");
				continue;
			}

			//sending server the info of the client we want to connect to
			send(s,&list[conn].ip,sizeof(list[conn].ip),0);
			send(s,&list[conn].port,sizeof(list[conn].port),0);

			
			pthread_create(&tid[0], NULL, keep_receiving, NULL);
			pthread_create(&tid[1], NULL, keep_sending, NULL);
			
			if(pthread_join(tid[0],(void**)&(ptr[0])) != 0)
				printf("pthread_join 1 error");
			if(pthread_join(tid[1],(void**)&(ptr[0])) != 0)
				printf("pthread_join 2 error");
		}
		else if(dec==3)		//go online
		{
			int conn;

			//wait for a chat request
			send(s,&wait,sizeof(wait),0);
			recv(s,&conn,sizeof(conn),0);
			printf("Got a chat request\n");
			send(s,&conn,sizeof(conn),0);

			pthread_create(&tid[0], NULL, keep_receiving, NULL);
			pthread_create(&tid[1], NULL, keep_sending, NULL);
			
			if(pthread_join(tid[0],(void**)&(ptr[0])) != 0)
				printf("pthread_join 1 error");
			if(pthread_join(tid[1],(void**)&(ptr[0])) != 0)
				printf("pthread_join 2 error");
		}
		else if(dec==4)	//logout from the server
		{
			send(s,&logout,sizeof(logout),0);
			close(s);
			break;
		}
		else
		{
			printf("Wrong choice\n");
		}
	}

	return 0;
}