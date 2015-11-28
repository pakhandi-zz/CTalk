/*

	The server side

*/

#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>
#include<math.h>

pthread_t tid[10];

struct transfer_data
{
	int sid, port;
	char ip_ad[100];
};

struct maintain
{
	int port, socket;
	char ip[100];
}list [100];

int ind;

void *foo(void * n_s)
{


			char rec_buffer[100], send_buffer[100];
			struct transfer_data o = *(struct transfer_data *) n_s;
			int new_socket = o.sid;
			int i, j;
			while(1)
			{
				printf("\nWaiting for : %s %d\n", o.ip_ad, o.port);

				recv(new_socket,&rec_buffer,sizeof(rec_buffer), 0);

				printf("\nProccessing request from : %s %d\n", o.ip_ad, o.port);
				printf("%s\n", rec_buffer);

				if(strcmp(rec_buffer,"request")==0)
				{
					int i;
					printf("\nAT : %s %d\n", o.ip_ad, o.port);
					printf("Size of the list : %d", ind);
					send(new_socket,&ind, sizeof(ind),0);

					printf("The List : \n");

					for(i=0; i<ind; i++)
					{
						printf("%s %d\n", list[i].ip, list[i].port);
						if(strcmp(o.ip_ad,list[i].ip)==0 && o.port==list[i].port)
							continue;
						send(new_socket,&list[i].ip, sizeof(list[i].ip),0);
						send(new_socket,&list[i].port, sizeof(list[i].port), 0);
					}
				}
				else if(strcmp(rec_buffer,"connect")==0)
				{
					printf("\nAT : %s %d\n", o.ip_ad, o.port);
					char rec_ip[100]; int rec_port;
					recv(new_socket,&rec_ip,sizeof(rec_ip),0);
					recv(new_socket,&rec_port,sizeof(rec_port),0);
					char this_ip[100]; int this_socket, this_port;
					int i, j;
					for(i=0; i<ind;i++)
					{
						if(strcmp(rec_ip,list[i].ip)==0 && rec_port==list[i].port)
						{
							this_socket = list[i].socket;
							strcpy(this_ip,list[i].ip);
							this_port = list[i].port;
							for(j=i; j<ind; j++)
							{
								strcpy(list[j].ip,list[j+1].ip);
								list[j].port = list[j+1].port;
								list[j].socket = list[j+1].socket;
							}
							break;
						}
					}
					ind--;

					send(this_socket,&new_socket,sizeof(new_socket),0);

					printf("\nAT : %s %d\n", o.ip_ad, o.port);
					printf("Starting chat at the connect side\n");

					while(1)
					{
						recv(new_socket,&rec_buffer,sizeof(rec_buffer), 0);
						if(strcmp(rec_buffer,"kill")==0)
							break;
						send(this_socket,&rec_buffer,sizeof(rec_buffer),0);
						if(strcmp(rec_buffer,"die")==0)
							break;
					}

					printf("\nAT : %s %d\n", o.ip_ad, o.port);

					printf("chat was closed at the connect side\n");

				}
				else if(strcmp(rec_buffer,"wait")==0)
				{
					printf("\nAT : %s %d\n", o.ip_ad, o.port);

					strcpy(list[ind].ip, o.ip_ad);
					list[ind].port = o.port;
					list[ind].socket = o.sid;
					ind++;

					char this_ip;
					int this_port, this_socket;
					printf("Waiting for a chat request\n");
					recv(new_socket,&this_socket,sizeof(this_socket),0);
					printf("\nAT : %s %d\n", o.ip_ad, o.port);
					printf("Got a chat request\n");
					while(1)
					{
						recv(new_socket,&rec_buffer,sizeof(rec_buffer), 0);
						if(strcmp(rec_buffer,"kill")==0)
							break;
						send(this_socket,&rec_buffer,sizeof(rec_buffer),0);
						if(strcmp(rec_buffer,"die")==0)
							break;
					}

					printf("\nAT : %s %d\n", o.ip_ad, o.port);

					printf("Chat was closed at the wait side\n");

				}
				else
				{
					printf("\nAT : %s %d\n", o.ip_ad, o.port);
					printf("Closing connection and removing from the list\n");
					close(new_socket);
					
					return 0;
				}
			}
}

main()
{
    struct sockaddr_in client,server;

    int n, s;

    char rec_buffer[100], send_buffer[100];

    s=socket(AF_INET,SOCK_STREAM,0);
    server.sin_family=AF_INET;
    server.sin_port=15515;
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    if ( bind(s,(struct sockaddr *)&server,sizeof(server)) == -1 )
	{
		printf("Binding failed..\n");
		return 1;
	}

    printf("\nServer was started successfully.. waiting for response from client...\n");

    n=sizeof(client);

    int status = listen(s,2);

	pid_t pid;

	struct transfer_data obj;
	
	int tind=0;

	while(1)
	{

		int new_socket = accept(s,(struct sockaddr *)&client,&n);
		printf("Got a new connection from -- %s %d\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		
		obj.sid = new_socket;
		obj.port = ntohs(client.sin_port);
		strcpy(obj.ip_ad,inet_ntoa(client.sin_addr));

		pthread_create(&(tid[tind]), NULL, foo, (void *)&obj);

		tind++;

	}

	close(s);

    return 0;
}
