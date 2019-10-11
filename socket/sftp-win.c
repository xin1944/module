//SFTP_Libssh2.cpp文件清单
#include "SFTP_Libssh2.h"
 
#include <libssh2.h>
#include <libssh2_sftp.h>
 
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
 
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
 
#include <sstream>
#include <iomanip>
 
#pragma comment(lib, "ws2_32.lib")
 
#pragma comment(lib, "libeay32.lib")  
#pragma comment(lib, "libssh2.lib")  
 
namespace kagula {
	namespace network
	{
		//初始化进程的时候调用
		//如果非0表示初始化失败！
		int SFTP_Init()
		{
			WSADATA wsadata;
			int rc = WSAStartup(MAKEWORD(2, 0), &wsadata);
			if (rc != 0) {
				return rc;
			}
 
			rc = libssh2_init(0);
 
			return rc;
		}
 
		//进程结束的时候调用
		void SFTP_Exit()
		{
			libssh2_exit();
 
			WSACleanup();
		}
 
		bool SFTP_Libssh2::IsAbilityConn(std::string ip, unsigned short port, std::string username,
			std::string password)
		{
			unsigned long hostaddr;
			struct sockaddr_in sin;
			const char *fingerprint;
			LIBSSH2_SESSION *session;
			int rc;
			bool bR = false;
			FILE *local;
			LIBSSH2_SFTP *sftp_session;
			LIBSSH2_SFTP_HANDLE *sftp_handle;
 
			hostaddr = inet_addr(ip.c_str());//hostaddr = htonl(0x7F000001);
 
 
			//新建连接
			int sock = socket(AF_INET, SOCK_STREAM, 0);
 
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.s_addr = hostaddr;
			if (connect(sock, (struct sockaddr*)(&sin),
				sizeof(struct sockaddr_in)) != 0) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "]failed to connect" << ip << "!" << std::endl;
				strLastError = ostr.str();
 
				return bR;
			}
 
			//新建对话实例
			session = libssh2_session_init();
			if (!session)
			{
				closesocket(sock);
				return bR;
			}
 
			//设置调用阻塞
			libssh2_session_set_blocking(session, 1);
 
			//进行握手
			rc = libssh2_session_handshake(session, sock);
			if (rc) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "]Failure establishing SSH session: " << rc << std::endl;
				strLastError = ostr.str();
 
				libssh2_session_free(session); closesocket(sock);
				return bR;
			}
 
			//检查主机指纹
			std::ostringstream ostr;
			fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
			ostr << "Fingerprint: ";
			for (int i = 0; i < 20; i++) {
				unsigned char c = fingerprint[i];
				int nT = c;
				ostr << std::hex << std::setw(2) << std::setfill('0') << nT;
			}
			strLastError = ostr.str();
 
			//通过密码验证登陆用户身份
			if (libssh2_userauth_password(session, username.c_str(), password.c_str())) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "]Authentication by password failed." << std::endl;
				strLastError = ostr.str();
				goto shutdown;
			}
 
			sftp_session = libssh2_sftp_init(session);
 
			if (!sftp_session) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "]Unable to init SFTP session " << std::endl;
				strLastError = ostr.str();
 
				goto shutdown;
			}
 
			bR = true;
 
 
			libssh2_sftp_shutdown(sftp_session);
 
		shutdown:
			libssh2_session_disconnect(session,
				"Normal Shutdown, Thank you for playing");
			libssh2_session_free(session);
			closesocket(sock);
			return bR;
		}
 
		/*
		源码参考地址
		http://www.libssh2.org/examples/sftp_write.html
		*/
		int SFTP_Libssh2::upload(std::string ip, unsigned short port, std::string username, std::string password,
			std::string localpath, std::string remotepath)
		{
			if (ip.length()<1 || username.length()<1 || password.length()<1 || localpath.length()<1 || remotepath.length()<1)
			{
				return -1;
			}
 
			int nR = 0;
			unsigned long hostaddr;
			struct sockaddr_in sin;
			const char *fingerprint;
			LIBSSH2_SESSION *session;
			int rc = -1;
			FILE *local = NULL;
			LIBSSH2_SFTP *sftp_session;
			LIBSSH2_SFTP_HANDLE *sftp_handle;
 
			hostaddr = inet_addr(ip.c_str());//hostaddr = htonl(0x7F000001);
 
			if (fopen_s(&local, localpath.c_str(), "rb") != 0) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "]Can't open local file " << localpath << std::endl;
				strLastError = ostr.str();
 
				return -2;
			}
 
			//取待上传文件整个size.
			fseek(local, 0, SEEK_END);
			size_t filesize = ftell(local);//local file大小，在readFromDisk中被引用
			fseek(local, 0, SEEK_SET);//文件指针重置到文件头
 
			//新建连接
			int sock = socket(AF_INET, SOCK_STREAM, 0);
 
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.s_addr = hostaddr;
			if (connect(sock, (struct sockaddr*)(&sin),
				sizeof(struct sockaddr_in)) != 0) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] failed to connect " << ip << std::endl;
				strLastError = ostr.str();
 
				fclose(local);
				return -3;
			}
 
 
			//创建会话实例
			session = libssh2_session_init();
			if (!session)
			{
				fclose(local); closesocket(sock);
				return -4;
			}
 
			//阻塞方式调用libssh2
			libssh2_session_set_blocking(session, 1);
 
			//进行握手
			rc = libssh2_session_handshake(session, sock);
			if (rc) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] Failure establishing SSH session:" << rc << std::endl;
				strLastError = ostr.str();
 
				fclose(local); libssh2_session_free(session); closesocket(sock);
				return -5;
			}
 
			//获取主机指纹
			std::ostringstream ostr;
			fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
			ostr << "Fingerprint: ";
			for (int i = 0; i < 20; i++) {
				unsigned char c = fingerprint[i];
				int nT = c;//这样转是为了防止符号位扩展
				ostr << std::hex << std::setw(2) << std::setfill('0') << nT;
			}
			strLastError = ostr.str();
 
			//通过密码验证
			if (libssh2_userauth_password(session, username.c_str(), password.c_str())) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] Authentication by password failed ["
					<< username << "][" << password << "]" << rc << std::endl;
				strLastError = ostr.str();
 
				goto shutdown;
			}
 
			sftp_session = libssh2_sftp_init(session);
 
			if (!sftp_session) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] Unable to init SFTP session" << std::endl;
				strLastError = ostr.str();
 
				goto shutdown;
			}
 
			//向SFTP服务器发出新建文件请求
			sftp_handle =
				libssh2_sftp_open(sftp_session, remotepath.c_str(),
				LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
				LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
				LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
 
			if (!sftp_handle) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] Unable to open file with SFTP.  ip=" 
					<< ip <<"] remotepath=[" << remotepath << "]" << std::endl;
				strLastError = ostr.str();
 
				nR = -1;
 
				goto shutdown;
			}
 
 
			char mem[1024 * 16];
			size_t nread;
			char *ptr;
			size_t count = 0;
 
			do {
				nread = fread(mem, 1, sizeof(mem), local);
				if (nread <= 0) {
					//到达文件尾部
					break;
				}
				ptr = mem;
				do {
					// 向服务器写数据，直到数据写完毕
					rc = libssh2_sftp_write(sftp_handle, ptr, nread);
					if (rc < 0)
						break;
					ptr += rc; count += nread;
					nread -= rc;
 
					//如果设置了回调，进行回调
					if (m_bkCall)
					{
						float p = count / (float)filesize;
						m_bkCall->OnProgress(p);
					}
					//callback.end
				} while (nread);
 
				if ( m_isBreak.load() == true )
				{
					std::ostringstream ostr;
					ostr << "[" << __FILE__ << "][" << __LINE__ << "] 上传文件任务被用户break!" << std::endl;
					strLastError = ostr.str();
 
					nR = -6;
					break;
				}
			} while (rc > 0);
 
			libssh2_sftp_close(sftp_handle);
			libssh2_sftp_shutdown(sftp_session);
 
		shutdown:
			libssh2_session_disconnect(session,
				"Normal Shutdown, Thank you for playing");
			libssh2_session_free(session);
 
			closesocket(sock);
 
			fclose(local);
 
			return nR;//返回“0”表示成功
		}
 
		/*
		源码参考地址
		http://www.oschina.net/code/snippet_12_10717
		*/
		int SFTP_Libssh2::download(std::string ip, unsigned short port, std::string username, std::string password,
			std::string sftppath, std::string localpath)
		{
			unsigned long hostaddr;
			int sock, i, auth_pw = 0;
			struct sockaddr_in sin;
			const char *fingerprint;
			char *userauthlist;
			LIBSSH2_SESSION *session;
			int rc;
			LIBSSH2_SFTP *sftp_session;
			LIBSSH2_SFTP_HANDLE *sftp_handle;
 
			hostaddr = inet_addr(ip.c_str()); //hostaddr = htonl(0x7F000001);
 
			/*
			* The application code is responsible for creating the socket
			* and establishing the connection
			*/
			sock = socket(AF_INET, SOCK_STREAM, 0);
 
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.s_addr = hostaddr;
			if (connect(sock, (struct sockaddr*)(&sin),
				sizeof(struct sockaddr_in)) != 0) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 连接失败!" << std::endl;
				strLastError = ostr.str();
				return -1;
			}
 
			/* Create a session instance
			*/
			session = libssh2_session_init();
 
			if (!session)
				return -1;
 
			/* Since we have set non-blocking, tell libssh2 we are blocking */
			libssh2_session_set_blocking(session, 1);
 
 
			/* ... start it up. This will trade welcome banners, exchange keys,
			* and setup crypto, compression, and MAC layers
			*/
			rc = libssh2_session_handshake(session, sock);
 
			if (rc) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 建立SSH会话失败" << rc << std::endl;
				strLastError = ostr.str();
 
				return -1;
			}
 
			/* At this point we havn't yet authenticated.  The first thing to do
			* is check the hostkey's fingerprint against our known hosts Your app
			* may have it hard coded, may go to a file, may present it to the
			* user, that's your call
			*/
			fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
 
			std::ostringstream ostr;
			fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
			ostr << "Fingerprint: ";
			for (int i = 0; i < 20; i++) {
				unsigned char c = fingerprint[i];
				int nT = c;
				ostr << std::hex << std::setw(2) << std::setfill('0') << nT;
			}
			strLastError = ostr.str();
 
			/* check what authentication methods are available */
			userauthlist = libssh2_userauth_list(session, username.c_str(), username.length());
			if (strstr(userauthlist, "password") == NULL)
			{
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 服务器不支持输入password方式验证!" << std::endl;
				strLastError = ostr.str();
				goto shutdown;
			}
 
			/* We could authenticate via password */
			if (libssh2_userauth_password(session, username.c_str(), password.c_str())) {
 
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 密码错误!" << std::endl;
				strLastError = ostr.str();
				goto shutdown;
			}
 
			sftp_session = libssh2_sftp_init(session);
			if (!sftp_session) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 初始化FTL对话失败!" << std::endl;
				strLastError = ostr.str();
				goto shutdown;
			}
 
			/* Request a file via SFTP */
			sftp_handle =
				libssh2_sftp_open(sftp_session, sftppath.c_str(), LIBSSH2_FXF_READ, 0);
 
 
			if (!sftp_handle) {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 打开文件失败! " << libssh2_sftp_last_error(sftp_session) << std::endl;
				strLastError = ostr.str();
 
				goto shutdown;
			}
 
			FILE *stream;
			if (fopen_s(&stream, localpath.c_str(), "wb") == 0)
			{
				do {
					char mem[1024];
 
					/* loop until we fail */
					rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));
 
					if (rc > 0) {
						//从内存到磁盘
						fwrite(mem, 1, rc, stream);
					}
					else {
						break;
					}
				} while (1);
 
				fclose(stream);
			}
			else {
				std::ostringstream ostr;
				ostr << "[" << __FILE__ << "][" << __LINE__ << "] 新建本地文件失败 " << localpath << std::endl;
				strLastError = ostr.str();
			}
 
			libssh2_sftp_close(sftp_handle);
 
			libssh2_sftp_shutdown(sftp_session);
 
		shutdown:
 
			libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
			libssh2_session_free(session);
 
			closesocket(sock);//INVALID_SOCKET
 
			return 0;
		}
	}
}