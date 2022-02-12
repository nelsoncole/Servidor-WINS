#undef UNICODE

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>

//#include <windows.h> 
#include <winsock2.h>
#include <ws2tcpip.h> 


#include <sys/types.h>
#include <wchar.h>

#define SERVER_PORT "2000"

#define BUFLEN 4096

#define MEM_LEN 0x400000 // 4MiB

//int client, server, csize;

char *buf;

// SOCK_STREAM
// SOCK_D


int get( void *buf, void *receive, int count);

int  main() {
	
	printf("\t-> Server...\n");

	buf = (char*) malloc(MEM_LEN);
	memset(buf, 0, MEM_LEN);

	setlocale(LC_ALL, "portuguese");
	
   
	WSADATA wsa;
	int r;
	
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;
	
	struct addrinfo *rsult = NULL;
	struct addrinfo hints;
	
	int iSendRsult;
	char recvbuf[BUFLEN];
	int recvbuflen = BUFLEN;
	
	
	// Inicializar a biblioteca winsock
	r = WSAStartup(MAKEWORD(2,2), &wsa);
	if(r != 0)
	{
		printf("\t[-] WSAStartup falhou! Erro: %d\n\n", r);
		exit(1);
	}
	
	printf("\t[+] WSAStartup inicializou com sucesso\n");
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	// endereco e porta do servidor
	r = getaddrinfo(NULL, SERVER_PORT, &hints, &rsult );
	if( r != 0 )
	{
		printf("\t[-] getaddrinfo falhou! Erro: %d\n\n", r);
		WSACleanup();
		free(buf);
		exit(1);
	}
	
	printf("\t[+] getaddrinfo inicializou com sucesso\n");
	
	// Criar socket para conexao
	listenSocket = socket(rsult->ai_family, rsult->ai_socktype, rsult->ai_protocol);
	if( listenSocket == INVALID_SOCKET )
	{
		printf("\t[-] Socket falhou! Erro: %d\n\n", WSAGetLastError());
		freeaddrinfo(rsult);
		WSACleanup();
		free(buf);
		exit(1);
	}
	
	printf("\t[+] Socket criado com sucesso\n");
	
	printf("\t-> bind()...\n");
	r = bind( listenSocket , rsult->ai_addr, (int)rsult->ai_addrlen);
	if(r == SOCKET_ERROR)
	{
		printf("\t[-] bind() falhou! Erro: %d\n\n", WSAGetLastError());
		freeaddrinfo(rsult);
		closesocket(listenSocket);
		WSACleanup();
		free(buf);
		exit(1);
	}
	printf("\t[+] sucesso\n");
	freeaddrinfo(rsult);
	
	printf("\t-> listen()...\n");
	r = listen(listenSocket, SOMAXCONN);
	if( r == SOCKET_ERROR )
	{
		printf("\t[-] listen() falhou! Erro: %d\n\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		free(buf);
		exit(1);
	}
	printf("\t[+] sucesso\n");
	// informacoes do servidor
	
	do{
		printf("\t-> Aguardando conexoes, accept()...\n");
	
		clientSocket = accept(listenSocket, NULL, NULL);
		if( clientSocket == INVALID_SOCKET )
		{
			printf("\t[-] accept() falhou! Erro: %d\n\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			free(buf);
			exit(1);
		}
	
		printf("\t[+] sucesso\n");
		//closesocket(listenSocket);
	
		printf("\t-> Aguardando dados...\n");
		r = recv(clientSocket, recvbuf, recvbuflen, 0);
		if( r > 0 )
		{
			if(!strcmp("sair",recvbuf))
			{
				closesocket(clientSocket);
				break;
			}
			printf("\t[+] Bytes recebidos: %d\n", r);
			r = get(buf, recvbuf, r);
			if(r < 1)
			{
				printf("\t[-] GET HTTP /1.1 falhou!\n\n");
				// enviar resposta de erro
				// send(clientSocket, buf, r, 0);
				closesocket(clientSocket);
				continue;
			}
		
			// enviar um eco ao cliente
			printf("\t-> Enviar dados...\n");
			r = send(clientSocket, buf, r, 0);
			if( r == SOCKET_ERROR )
			{
				printf("\t[-] send() falhou! Erro: %d\n\n", WSAGetLastError());
				closesocket(clientSocket);
				continue;
			}
		
			printf("\t[+] Sucesso! Bytes enviados: %d\n", r);
			closesocket(clientSocket);
			memset(buf, 0, 0x1000);
			
		} else if( r == 0) {
			printf("\t[-] Conexao fexada pelo cliente\n");
			closesocket(clientSocket);
		} else {
			printf("\t[-] recv() falhou! Erro: %d\n\n", WSAGetLastError());
			closesocket(clientSocket);
			continue;
		}
	
	}while(1);
	
	printf("\t[-] Encerrando o servidor...\n");
	r = shutdown(clientSocket, SD_SEND);
	if( r == SOCKET_ERROR )
	{
		printf("\t[-] shutdown() falhou! Erro: %d\n\n", WSAGetLastError());
	}
	
	WSACleanup();
	free(buf);
	return 0;
}

#define DEL1 "GET /"
#define DEL2 "\t "

#define DEL3 "Accept: "

char header1[256] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \0";
char header2[256] = "HTTP/1.1 200 OK\nContent-Type: text/css\nContent-Length: \0";
char header3[256] = "HTTP/1.1 200 OK\nContent-Type: application/javascript\nContent-Length: \0";
char header4[256] = "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: \0";

// char *__cdecl itoa(int _Val,char *_DstBuf,int _Radix) 

int motor(char *s1, const char *s2, int len)
{
	unsigned char *us1 = (unsigned char *)s1;
	char _DstBuf[256];
	memset(_DstBuf, 0, sizeof(_DstBuf));
		
	strcpy(us1, s2);
	
	itoa(len , _DstBuf, 10);
	
	strcat(us1, _DstBuf);
	strcat(us1, "\n\n");
	
	return  strlen(us1);
	
}

char *strdel(const char *s1, const char *s2)
{
	unsigned char *us1 = (unsigned char *)s1;
	
	int count = strlen(s2);
	
	
	
	while(*us1)
	{
		if( strncmp(us1, s2, count) == 0)
		{
			return us1 + count;
		}
		us1 ++;
	}
	
	return us1;
}

int get_type(const char *s1, const char *s2)
{
	unsigned char *us1 = (unsigned char *)s1;
	
	us1 = strdel(us1, s2);

	unsigned char *tmp = (unsigned char *) strchr(us1, ',');
	
	if(tmp != 0) 
	{ 
		*tmp = '\0';
	}
	
	if( strcmp(us1, "text/html") == 0) return 1;
	
	if( strcmp(us1, "text/css") == 0) return 2;
	
	if( strcmp(us1, "application/javascript") == 0) return 3;
	
	if( strcmp(us1, "image/png") == 0) return 4;
	
	return 0;
}

int get( void *buf, void *receive, int count)
{
	int of;
	int type = 0;
	char *h = NULL;
	char pathname[256];
	memset(pathname, 0, 256);
	
	unsigned char _rcv[512];
	unsigned char *rcv = _rcv;
	
	memcpy(rcv, receive, strlen(receive));
	for( int i=0; i < count; i ++ )
	{
		putchar(rcv[i]);
	}
		
	putchar('\n');
	
	rcv = strtok(rcv, DEL1);
	
	if(*rcv == 'H') { 
		strcpy(pathname, "index.html");
	} else strcpy(pathname, rcv);
	
	FILE *f = fopen(pathname, "r+b");
	if(!f) {
		printf("\t[-] fopen(\"%s\", \"r+b\") falhou!\n\n",pathname);
		fclose(f);
		return 0;
	}

	fseek(f,0, SEEK_END);
	int f_size = ftell(f);
	rewind(f);
	
	type = get_type(receive, DEL3);
	
	switch(type){
		case 1:
			h = header1;
		break;
		case 2:
			h = header2;
		break;
		case 3:
			h = header3;
		break;
		case 4:
			h = header4;
		break;
		default:
			h = header1;
		break;
	}
	
	of = motor(buf, h, f_size);
	
	int r = fread(buf + of, 1, f_size, f);
	fclose(f);
	return r + of;
}