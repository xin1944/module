
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
//#include <error.h>
#include <linux/sockios.h>
#ifdef __mips__
#include <asm/cachectl.h>
#endif
#include <pthread.h>
#include <signal.h>

#include "tiny_http_live.h"

#define thl_print_dbg(fmt, args...) printf("<%s><%d> "fmt, __FUNCTION__, __LINE__, ##args)
#define thl_print_err(fmt, args...) printf("<%s><%d> "fmt, __FUNCTION__, __LINE__, ##args)

#define SEND_TIMEOUT 200
#define SEND_BUFFER_SIZE (188*16*32*4)

typedef struct
{
	int socket;
	int ip;
	int port;
	int error;//if socket>0 && error>0, means lost connection, should  release this socket and accept other.
	int sem_posted;
}THLStream_t;
typedef struct
{
	int port;
	int socket;
	int maxStreams;
	THLStream_t* streamlist;
	int writeQueueSize;
	pthread_t httpServerThread;
	int running;
	int already_stopped;
	sem_t sem;
}THLCore_t;

static int NetworkInitTcpServer(int listeningport)
{
    struct sockaddr_in localAddr;
    int port, socket_type;
    char *interfaceName;
    int sd = -1;
    int reuse_flag = 1;

    port = listeningport;
    socket_type = SOCK_STREAM; 
    interfaceName = "eth0";

    if ( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        /* Socket Create Error */
        thl_print_err("Socket open error\n");
        perror("Socket Open Err");
        return -1;
    }

    if (setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof(reuse_flag) ) < 0 ) 
    {
        thl_print_err("reuse error\n");
        //BDBG_ERR(("REUSE Socket Error"));
        close(sd);
        return -1;
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr))) 
    {
        thl_print_err("bind error\n");
        perror("Socket Bind Err to bind to actual i/f");
        close(sd);
        return -1;
    }
    thl_print_dbg("Socket Bind to actual i/f successful\n");
    
    if (listen(sd, 4)) 
    {
        thl_print_err(" listen error\n");
        perror("Socket listen Err");
        close(sd);
        return -1;
    }
    thl_print_dbg(" Started listening on %d port \n", port);

    /* Set stdin to non-blocking */
    if (fcntl(sd, F_SETFL, fcntl(sd, F_GETFL)|O_NONBLOCK) < 0)
    {
        thl_print_err("ERROR setting non-blocking mode on listening socket\n");
    }
    return sd;
}
int waitForNetworkEvent(THLCore_t* pCore, int sd, int timeout)
{
    fd_set rfds;
    struct timeval tv;
    int i = 0;
    while (pCore->running) 
    {
        FD_ZERO(&rfds);
        FD_SET(sd, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if ( select(sd +1, &rfds, NULL, NULL, &tv) < 0 ) 
        {
            perror("ERROR: select(): exiting...");
            return -2;
        }
        i++;
        if(i > timeout)
        {
            perror("select(): timeout...");
            return -1;
        }
        if (!FD_ISSET(sd, &rfds))
        {
            /* No request from Client yet, go back to select loop */
            continue;
        }
        break;
    }

    return 0;
}
char * acceptNewHttpRequest_Ext(THLCore_t* pCore, char *requestUrl, int requestUrlLen, int *newFd, int*pAddr, int*pPort)
{
    struct sockaddr_in remoteAddr;
    int addrLen = sizeof(remoteAddr);
    int nbytes;
    int sockFd;

    /* wait for HTTP request & send HTTP reply */
    while (pCore->running) 
    {
      /*  if(waitForNetworkEvent(pnet,pnet->sock,1) != 0)
        {
            return NULL;
        }*/
        /* accept connection */
	  memset(&remoteAddr, 0, sizeof(remoteAddr));
        if ((sockFd = accept(pCore->socket, (struct sockaddr *)&remoteAddr, (socklen_t *)&addrLen)) < 0) 
        {
            if (errno == EAGAIN || errno == EINTR){
			//thl_print_err("no request... \n");
			usleep(10000);
			continue;
		}
                
            perror("ERROR: accept(): exiting...");
            break;
        }
        waitForNetworkEvent(pCore, sockFd, 10);

        /* Read HTTP request */
        if ((nbytes = read(sockFd, requestUrl, requestUrlLen-1)) <= 0) 
        {
            perror("read failed to read the HTTP Get request");
            break;
        }
        requestUrl[nbytes] = '\0';
        //BDBG_MSG(("Read HTTP Req (socket %d, %d bytes)", sockFd, nbytes));

        *newFd = sockFd;
	*pAddr = ntohl(*(int*)(&(remoteAddr.sin_addr)));
	*pPort = ntohs(remoteAddr.sin_port);
	
        /* we dont do any parsing part here */
        return requestUrl;
    }
    thl_print_err("Failed handling HttpRequest...\n");
    requestUrl[0] = '\0';
    return NULL;
}
char * acceptNewHttpRequest(THLCore_t* pCore, char *requestUrl, int requestUrlLen, int *newFd)
{
	int ip = 0;
	int port = 0;

	return acceptNewHttpRequest_Ext(pCore, requestUrl, requestUrlLen, newFd, &ip, &port);
}
int sendHttpResponse( int sd)
{
    int nbytes;
   // unsigned i;
    char responseBuf[1024], *response;
    //off_t contentLength = 0;
    char contentType[32];
    int bytesCopied, bytesLeft;

    /* Build HTTP response */
    memset(responseBuf, 0, sizeof(responseBuf));
    memset(contentType, 0, sizeof(contentType));
    response = responseBuf;
    bytesLeft = sizeof(responseBuf) - 1;
    bytesCopied = snprintf(response, bytesLeft,
        "HTTP/1.1 200 OK\r\n"
        "Accept-Ranges: bytes\r\n"
        "Connection: close\r\n"
#ifdef B_USE_HTTP_CHUNK_ENCODING
        "TRANSFER-ENCODING: chunked\r\n"
#endif
        "transferMode.dlna.org: Streaming\r\n"
        "Server: Linux/2.6.xx, inpsur IP Streamer 2.0\r\n"
        "Content-Type: video/mpeg\r\n"
        "Content-Length: 0\r\n"
        );
    bytesLeft -= bytesCopied;
    response += bytesCopied;

    /*
    HTTP/1.1 200 OK
Accept-Ranges: bytes
Connection: close
transferMode.dlna.org: Streaming
Server: Linux/2.6.xx, Broadcom IP Streamer 2.0
Content-Type: video/mpeg
Content-Length: 0 
     
    */
   /* if (psi) {
        getContentType(psi, contentType, sizeof(contentType));
        bytesCopied = snprintf(response, bytesLeft, "Content-Type: %s\r\n", contentType);
        bytesLeft -= bytesCopied;
        response += bytesCopied;
        if (psi->liveChannel) 
        {
            if (!ipStreamerCfg->transcodeEnabled) {
                if (ipStreamerCfg->enableTimestamps)
                    bytesCopied = snprintf(response, bytesLeft, "BCM-LiveChannel: 1\r\nTTS: 1\r\n");
                else
                    bytesCopied = snprintf(response, bytesLeft, "BCM-LiveChannel: 1\r\n");
                bytesLeft -= bytesCopied;
                response += bytesCopied;
            }
        }
    
        if (psi->psiValid) 
        bytesCopied = snprintf(response, bytesLeft, "\r\n");
        bytesLeft -= bytesCopied;
        response += bytesCopied;
    }
    else {*/
        bytesCopied = snprintf(response, bytesLeft, "\r\n");
        bytesLeft -= bytesCopied;
        response += bytesCopied;
   // }
    nbytes = response - responseBuf;
#ifdef __mips__
#if ANDROID
    syscall(cacheflush, response, nbytes, DCACHE); 
#else
    syscall(SYS_cacheflush, response, nbytes, DCACHE); 
#endif
#endif
    thl_print_dbg("HTTP Response (socket %d, size %d) [%s]\n", sd, nbytes, responseBuf);

    /* send out HTTP response */
    if (write(sd, responseBuf, nbytes) != nbytes) 
    {
        printf("Failed to write HTTP Response of %d bytes", nbytes);
        perror("write(): ");
        return -1;
    }
    return 0;
}

#define DEFAULT_SOCKET_TX_BUF_SIZE (16*1024*1024)
#define MAX_SOCKET_TX_BUF_SIZE (16*1024*1024)
#define SOCKET_TX_BUF_GROW_SIZE (2*1024*1024)

static void __adapt_global_tx_buf(int buf_size)
{
	FILE *f = NULL;
	char buf[80] = {0};
	int wmax = 0;
	size_t tmpLen = 0;
	
    /* 
     * Linux kernel TX tuning: set socket send buffer to 300K by default. 
     * For higher bitrate HD streams, streamer can't send data at the natural 
     * stream bitrate and thus causes client underflows.
     * We need to increase our socket send buffer size by using setsockopt(). 
     * Therefore, /proc/sys/net/core/wmem_max needs to be changed.
     */
	f = fopen("/proc/sys/net/core/wmem_max", "rw");
	if (!f){
		thl_print_dbg("%d: Failed to open wmem_max proc variable\n", __LINE__);
		return;
	}
	
	tmpLen = fread(buf, 1, sizeof(buf)-1, f);
	buf[tmpLen] = '\0';
	wmax = strtol(buf, (char **)NULL, 10);
	fclose(f);
	
	thl_print_dbg("current wmem_max: %d KB, new_buff_size: %d KB\n", wmax/1024, buf_size/1024);
	if (wmax < buf_size){
		/* it is the default value, make it bigger */
		thl_print_dbg("%d: Increasing default wmem_max from %d KB to %d KB\n", __LINE__, wmax/1024, buf_size/1024);
		tmpLen = snprintf(buf, sizeof(buf)-1, "%d", buf_size);
		f = fopen("/proc/sys/net/core/wmem_max", "w");
		if (f){
			fwrite(buf, 1, tmpLen, f);
			fclose(f);
		}
	}
}
static void __tune_tcp_retries2(int new_val)
{
	FILE *f = NULL;
	char buf[80] = {0};
	int wmax = 0;
	int size = 0;
	socklen_t len = 0;
	size_t tmpLen = 0;
	int tcp_retries2;
	
	f = fopen("/proc/sys/net/ipv4/tcp_retries2", "rw");
	if (!f){
		thl_print_dbg("Failed to open tcp_retries2 proc variable\n");
		return;
	}
        
	tmpLen = fread(buf, 1, sizeof(buf)-1, f);
	buf[tmpLen] = '\0';
	tcp_retries2 = strtol(buf, (char **)NULL, 10);
	fclose(f);
	
	thl_print_dbg("current tcp_retries2: %d, new_val: %d\n",__LINE__, tcp_retries2, new_val);
	if (tcp_retries2 > new_val) {
		/* it is the default value, make it bigger */
		thl_print_dbg("%d: Reducing default tcp_retries2 from %d to %d\n", __LINE__, tcp_retries2, new_val);
		tmpLen = snprintf(buf, sizeof(buf)-1, "%d", new_val);
		f = fopen("/proc/sys/net/ipv4/tcp_retries2", "w");
		if (f){
			fwrite(buf, 1, tmpLen, f);
			fclose(f);
		}
	}
}
static void __adapt_socket_tx_buf(int sd, int buf_size)
{
	int size = 0;
	socklen_t len = 0;

	__adapt_global_tx_buf(2*buf_size);

	len = sizeof(size);
	if (getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, &len) == 0){
		thl_print_dbg("current socket send buffer size = %d KB\n", size/1024);
	}
	if (size < buf_size){
		size = buf_size;
		if (setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, len) != 0){
			thl_print_err("ERROR: can't set send buffer size to %d KB, errno=%d\n", size/1024, errno);
		}
		len = sizeof(size);
		size = 0;
		if (getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, &len) == 0){
			thl_print_dbg("updated socket send buffer size = %d KB\n", size/1024);
		}
	}
}
void TuneNetworkStackTx(int sd)
{
	__adapt_socket_tx_buf(sd, DEFAULT_SOCKET_TX_BUF_SIZE);
}
static int _all_sock_invalid(THLCore_t* pCore)
{
	int i = 0;
	int cnt  = 0;
	for(i=0; i<pCore->maxStreams; i++){
		if(pCore->streamlist[i].socket <=0 
			|| pCore->streamlist[i].error){
			cnt++;
		}
	}
	if(cnt == pCore->maxStreams){
		return 1;
	}
	return 0;
}
static int _close_all_sock(THLCore_t* pCore)
{
	int i = 0;
	//int cnt  = 0;
	for(i=0; i<pCore->maxStreams; i++){
		if(pCore->streamlist[i].socket > 0){
			close(pCore->streamlist[i].socket);
			pCore->streamlist[i].socket = 0;
		}
	}
	return 0;
}
static int _error_sock_cnt(THLCore_t* pCore)
{
	int i = 0;
	int cnt  = 0;
	for(i=0; i<pCore->maxStreams; i++){
		if(pCore->streamlist[i].socket > 0 
			&& pCore->streamlist[i].error){
			cnt++;
		}
	}
	return cnt;
}

static int _find_unused_sock(THLCore_t* pCore, int* pIndex)
{
	int i = 0;
	int found = 0;
	for(i=0; i<pCore->maxStreams; i++){
		if(pCore->streamlist[i].socket <=0 
			|| pCore->streamlist[i].error){
			found = 1;
			break;
		}
	}
	if(found){
		*pIndex = i;
		return 0;
	}
	return -1;
}
static void* HttpServerThread(void *data)
{
	THLCore_t* pCore = (THLCore_t*)data;
	pthread_t tid;
	tid = pthread_self();
	int streamingFd = 0;
	char requestUrl[512] = {0};
	int requestUrlLen = 512;
	char *requestUri = NULL;
	int firstTime = 1;
	int current_stream = 0;
	int remote_ip = 0;
	int remote_port = 0;
	
	if (pCore == NULL){
		return (void*)0;
	}
	pCore->already_stopped = 0;
	
	while (pCore->running)//(m_pPNet->usingstatus)/* write error send post.stop send post.just use 1 exit*/
	{
		sem_wait(&pCore->sem);

		if (_all_sock_invalid(pCore) && _error_sock_cnt(pCore)){
			thl_print_dbg("stream sock not closed:%x\n", (unsigned int)tid);
			_close_all_sock(pCore);
		}
		
		if(_find_unused_sock(pCore, &current_stream)){
			thl_print_dbg("no free sock for now\n");
			usleep(100000);
			continue;
		}else{
			pCore->streamlist[current_stream].error = 0;
			pCore->streamlist[current_stream].socket = 0;
		}

		streamingFd = 0;
		requestUri = acceptNewHttpRequest_Ext
			(pCore, requestUrl, requestUrlLen, &streamingFd, &remote_ip, &remote_port); 

		/*
		may be has stopped.
		*/
		if (pCore->running == 0){
			if (streamingFd){
				close(streamingFd);
				streamingFd = 0;
			}
			break;
		}
		
		if (requestUri){
			TuneNetworkStackTx(streamingFd);
			/* get the socket write queue size: needed for determining how much data can be written to socket */
			int len = sizeof(pCore->writeQueueSize);
			if (getsockopt(streamingFd, SOL_SOCKET, SO_SNDBUF, 
				&pCore->writeQueueSize, (socklen_t*)&len)){
				thl_print_err("ERROR: can't get socket write q size\n");
				continue;
			}
			thl_print_dbg("get connection accept:%x.%s\n", (unsigned int)tid, requestUrl);
			if(sendHttpResponse(streamingFd) != 0){
				close(streamingFd);
				continue;
			}
		}else{
			thl_print_dbg("%d listening:%x\n", __LINE__, (unsigned int)tid);
			continue;
		}
		pCore->streamlist[current_stream].socket = streamingFd;
		pCore->streamlist[current_stream].ip = remote_ip;
		pCore->streamlist[current_stream].port = remote_port;
		pCore->streamlist[current_stream].sem_posted = 0;
		
	}

	_close_all_sock(pCore);

	if (pCore->socket){
		close(pCore->socket); 
		pCore->socket = 0;
	}

	pCore->already_stopped = 1;
	return (void*)0;
}


#define USE_NON_BLOCKING_MODE 1
#define SELECT_TIMEOUT_FOR_SOCKET_SEND 100000
int WaitForSocketWriteReady(int fd,int timeout )
{
    int rc;
    fd_set wfds;
    struct timeval tv;

    for(;;) 
    {
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        tv.tv_sec = 0; 
        tv.tv_usec = timeout;

        if ( (rc = select(fd +1, NULL, &wfds, NULL, &tv)) < 0 && rc != EINTR) 
        {
            thl_print_err("ERROR: select(): errno = %d\n", errno);
            return -1;
        }
        else if (rc == EINTR) 
        {
            thl_print_err(" select returned EINTR, continue the loop\n");
            continue;
        }
        else if (rc == 0) 
        {
            thl_print_dbg("timeout(%d,%d)fd %d\n", (int)tv.tv_usec, timeout, fd);
            return 0;
        }

        if (!FD_ISSET(fd, &wfds)) 
        {
            
            thl_print_dbg(" some select event but our FD (%d) not set: No more data - retry select\n", fd);
            continue;
        }
        break;
    }
    return 1;
}

static int sendData(int fd, void *outBuf, int bytesToSend) 
{
    int bytesSent = 0;
    int rc;
    int loopCount = 0;

    while (bytesToSend) 
    {
        loopCount++;
#ifdef USE_NON_BLOCKING_MODE
        if (WaitForSocketWriteReady(fd, SELECT_TIMEOUT_FOR_SOCKET_SEND /* timeout in usec*/) < 0) 
        {
            thl_print_err("Select ERROR\n");
            return -1;
        }
#endif
       // BDBG_MSG(("Ok to write %d bytes \n", bytesToSend));
#ifdef USE_NON_BLOCKING_MODE
        rc = send(fd, (void*)outBuf, bytesToSend, MSG_NOSIGNAL | MSG_DONTWAIT );
#else
        rc = send(fd, (void*)outBuf, bytesToSend, MSG_NOSIGNAL );
#endif
        if (rc < 0) 
        {
            if (errno == EINTR || errno == EAGAIN) 
            {
                thl_print_dbg("got error: timeout or eagain error :%d, retrying\n", errno);
                continue;
            }

            /* it is not EINTR & EAGAIN, so all errors are serious network failures */
            //thl_print_err("send() ERROR:%d: %s\n", errno, strerror(errno));
            perror("faild to send");
            return -1;
        }
       // thl_print_dbg("%d sent %d bytes out of bytesToSend %d\n", __LINE__,rc, bytesToSend);
        bytesSent += rc;
        bytesToSend -= rc;
        outBuf = (unsigned char *)outBuf + rc;
      //  thl_print_dbg("%d sent %d bytes out of bytesToSend %d\n", __LINE__,rc, bytesToSend);
    }
   // thl_print_dbg("%d: send data in %d attempts, bytesSent %d for socket %d\n",__LINE__, loopCount, bytesSent, fd);
    return bytesSent;
}
static int writedata(THLCore_t* pCore, char *buffer, unsigned int length)
{
	int n = 0;
	ssize_t rc;
	int writeQueueDepth = 0;
	size_t writeQueueSpaceAvail = 0; /* current space available in write q */
	size_t bufsize = length;
	int i = 0;

	int at_least_one_ok = 0;
	int size_sended = 0;

	int tx_buff_changed = 0;
	
	
    for(i=0; i<pCore->maxStreams; i++){
		
        if(pCore->streamlist[i].socket<=0 || pCore->streamlist[i].error){
            if(0==pCore->streamlist[i].sem_posted){
                sem_post(&pCore->sem); 
                pCore->streamlist[i].sem_posted = 1;
            }
            continue;
        }
	
        #ifdef USE_NON_BLOCKING_MODE
        /* check is socket has some space to send data */
        if ((rc = WaitForSocketWriteReady(pCore->streamlist[i].socket, SELECT_TIMEOUT_FOR_SOCKET_SEND /* timeout in usec*/)) <= 0) 
        {
            thl_print_dbg("sock[%d]write not ready\n",i);
            if(rc==0){

		if(tx_buff_changed == 0){
		#if 0
			pCore->writeQueueSize = MAX_SOCKET_TX_BUF_SIZE;
			__adapt_socket_tx_buf(pCore->streamlist[i].socket, pCore->writeQueueSize);
		#endif
			tx_buff_changed = 1;
		}
		
            }else if(rc<0){
		pCore->streamlist[i].error = 1;
            }
            
            continue;
        }
        #endif

    /* now check how much data we can currently send */
        if (ioctl(pCore->streamlist[i].socket, SIOCOUTQ, &writeQueueDepth)) 
        {
            thl_print_dbg("failed to get tcp write q depth for socket %d", pCore->streamlist[i].socket);
            writeQueueDepth = 0;
	   pCore->streamlist[i].error = 1;
	   continue;
        }
        writeQueueSpaceAvail = pCore->writeQueueSize - writeQueueDepth;
        if (bufsize > writeQueueSpaceAvail) 
        {
            bufsize = writeQueueSpaceAvail;
        }
        if (bufsize > 0)
        {
            n = sendData(pCore->streamlist[i].socket, buffer, bufsize); 
	   if(n<0){
	   pCore->streamlist[i].error = 1;
	   continue;
	   }else{
		pCore->streamlist[i].error = 0;
	   }
            if(size_sended < n){
                size_sended = n;
                at_least_one_ok = 1;
            }
        }
        else
        {
            thl_print_dbg("no space to write.writeQueueDepth:%d,queuesize:%d \n",writeQueueDepth,pCore->writeQueueSize);
        }
    }
    return size_sended;
}

THLHandle TinyHttpLive_Open(THLOpenSettings_t* pSettings)
{
	THLCore_t* pCore = NULL;
	if(!pSettings || pSettings->maxClients<=0 || pSettings->port==0){
		return NULL;
	}
	
	pCore = (THLCore_t*)malloc(sizeof(THLCore_t));
	if(!pCore){
		return NULL;
	}
	memset(pCore, 0, (sizeof(THLCore_t)));

	sem_init(&(pCore->sem), 0 ,0);
	pCore->maxStreams = pSettings->maxClients;
	pCore->port = pSettings->port;

	pCore->streamlist = (THLStream_t*)malloc(sizeof(THLStream_t)*pCore->maxStreams);
	memset(pCore->streamlist, 0, (sizeof(THLStream_t)*pCore->maxStreams));

	__tune_tcp_retries2(6);

	return (THLHandle)pCore;
	
}
int TinyHttpLive_Start(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;
	int semValue = 0;
	int i = 0;
	
	if(!handle){
		return -1;
	}

	if(pCore->socket == 0){
		pCore->socket = NetworkInitTcpServer(pCore->port);
	}
	
	pCore->running = 1;

	sem_getvalue(&(pCore->sem), &semValue);
	if (semValue >0 ){
		for (i=0; i<semValue; i++){
			sem_wait(&(pCore->sem));
		}
	}
	sem_post(&(pCore->sem));

	pthread_create(&(pCore->httpServerThread), NULL, (void *(*)(void *))HttpServerThread, (void *)(pCore));

	return 0;
}
int TinyHttpLive_Stop(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;

	if(!handle){
		return -1;
	}
	pCore->running = 0;

	while(pCore->already_stopped==0){
		usleep(100000);
	}
	//_close_all_sock(pCore);
	return 0;
}
int TinyHttpLive_Close(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;
	
	if(!handle){
		return 0;
	}

	TinyHttpLive_Stop(handle);

	if(pCore->streamlist){
		free(pCore->streamlist);
	}
	free(pCore);
	return 0;
}
int TinyHttpLive_Send(THLHandle handle, char *buffer, unsigned int length)
{
	THLCore_t* pCore = (THLCore_t*)handle;
	
	if (pCore == NULL || buffer == NULL){
		thl_print_err("invalid param!\n");
		return -1;
	}
	
	int n = 0;
	int bytesSented = 0;
	char *outBuf = buffer;           /* pointer to the out buffer */
	int bufsize = length;
	int timeout = 0;
	if (bufsize>SEND_BUFFER_SIZE){
		bufsize = SEND_BUFFER_SIZE;
	}

	while (bufsize){
         
		n = writedata(pCore, outBuf, bufsize);
		if (n < 0){
			sem_post(&pCore->sem);        
			thl_print_err("write error\n");
			return -1;
		}else if(n == 0){
			timeout++;
			//thl_print_dbg("timeout:%d\n", timeout);
			if (timeout>SEND_TIMEOUT){
				//thl_print_err("timeout:%d\n", timeout);
				sem_post(&pCore->sem);
				return -1;
			}
			// usleep(1000000);
			continue;
		}
		timeout = 0;
		outBuf += n;
		bufsize -= n;
		bytesSented += n; 
	}

   // thl_print_dbg("%d write %d byte.buf len:%d.timeout=%d\n", __LINE__, n, bytesSented,timeout); 
	return bytesSented;
}
int TinyHttpLive_ClearClients(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;

	_close_all_sock(pCore);

	return 0;
}
int TinyHttpLive_IsRunning(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;

	return pCore->running;
}

int TinyHttpLive_DumpStatus(THLHandle handle)
{
	THLCore_t* pCore = (THLCore_t*)handle;
	int i = 0;

#define __ip_0(_ip_) (((_ip_)&0xFF000000)>>24)
#define __ip_1(_ip_) (((_ip_)&0x00FF0000)>>16)
#define __ip_2(_ip_) (((_ip_)&0x0000FF00)>>8)
#define __ip_3(_ip_) (((_ip_)&0x000000FF)>>0)

	if(!handle){
		return -1;
	}

	printf(" |____TinyHttpLive Status____|\n");
	printf("* running: %d \n", pCore->running);
	if(pCore->running == 0){
		return 0;
	}

	for(i=0; i<pCore->maxStreams; i++){
		if(pCore->streamlist[i].socket && pCore->streamlist[i].error==0){
			printf("* client: %d.%d.%d.%d:%d \n", 
				__ip_0(pCore->streamlist[i].ip),
				__ip_1(pCore->streamlist[i].ip),
				__ip_2(pCore->streamlist[i].ip),
				__ip_3(pCore->streamlist[i].ip),
				pCore->streamlist[i].port
				);
		}
	}
	
	return 0;
}
