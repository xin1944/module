#include <stdio.h>

#if defined _WIN32

#ifndef _WINSOCKAPI_ 
#	define _WINSOCKAPI_  
#	include <winsock2.h>
#	ifndef _INCLUDE_WINDOWS_H_
#		define _INCLUDE_WINDOWS_H_
#	endif
#endif //_WINSOCKAPI_
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#endif // WIN32

int main(int argc, char *argv[])
{
	int fd;
	struct sockaddr_in addr;
	struct timeval timeo = {1, 0};
	socklen_t len = sizeof(timeo);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (argc == 4)
		timeo.tv_sec = atoi(argv[3]);
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeo, len);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(atoi(argv[2]));
	
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) 
	{
		if (errno == EINPROGRESS) 
		{
			fprintf(stderr, "timeout\n");
			return -1;
		}
		perror("connect");
		return 0;
	}
	printf("connected\n");
	
	return 0;
}