#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "tiny_http_live.h"

#define TT_SECTION_SIZE  (1024*128)

static char* g_video_file = NULL;


static void __send_no_return(void*arg)
{
	THLHandle handle = (THLHandle)arg;
	
	unsigned int already_send = 0;
	int current_read = 0;
	FILE* file= NULL;
	unsigned int file_size = 0;
	unsigned char buff[TT_SECTION_SIZE];
	int clients = 0;
	
	file= fopen(g_video_file, "r");
	assert(file);
	fseek(file, 0, SEEK_END);
	file_size = (unsigned int)ftell(file);
	printf("file_size: %d bytes\n", file_size);

	rewind(file);
	while(1){
		current_read = fread(buff, 1, TT_SECTION_SIZE, file);
		if(current_read == 0){
			rewind(file);
			continue;
		}

		if(TinyHttpLive_Send(handle, buff, current_read)<=0){
			usleep(100000);
		}
		
		already_send += current_read;
		usleep(10000);
	}
	printf("send finish. exit task. \n");
	fclose(file);
	
}

static void* __streaming_task(void*arg)
{
	__send_no_return(arg);
	return NULL;
}

void __start_live(THLHandle handle)
{
	pthread_t pid = 0;
	pthread_create(&pid, NULL, __streaming_task, (void*)handle);
}
int main(int argc, char**argv)
{
	assert(argc == 2);
	
	THLHandle handle = NULL;
	THLOpenSettings_t setting;

	g_video_file = strdup(argv[1]);
	
	setting.maxClients = 2;
	setting.port = 9000;

	handle = TinyHttpLive_Open(&setting);
	if(!handle){
		printf("error call THL_Open ! \n");
		return -1;
	}

	TinyHttpLive_Start(handle);

	__start_live(handle);

	while(1){
		usleep(100000);
	}
	return 0;
}
