#ifndef _tiny_http_live_h_
#define _tiny_http_live_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* THLHandle;
typedef struct
{
	int maxClients;
	int port;
}THLOpenSettings_t;

THLHandle TinyHttpLive_Open(THLOpenSettings_t* pSettings);
int TinyHttpLive_Start(THLHandle handle);
int TinyHttpLive_Stop(THLHandle handle);
int TinyHttpLive_Close(THLHandle handle);
int TinyHttpLive_Send(THLHandle handle, char *buffer, unsigned int length);


#ifdef __cplusplus
}
#endif

#endif
