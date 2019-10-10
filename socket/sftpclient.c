SSH.h
#ifndef SSH_H_INCLUDED
#define SSH_H_INCLUDED

#include <string>

using namespace std;

class SSH {
    private:
        string ip;
        unsigned short port;
        int sock;

        SSH(const SSH&);
        SSH& operator=(const SSH&);
    protected:
        bool connected;
        void* session;

        bool connectTo(const string&, const string&);
        void disconnect(void);
    public:
        SSH(string, unsigned short);
        virtual ~SSH(void);

        virtual bool openSession(const string&, const string&) = 0;
        virtual void closeSession(void) = 0;
};

#endif // SSH_H_INCLUDED

SFTP.h
#ifndef SFTP_H_INCLUDED
#define SFTP_H_INCLUDED

#include <list>
#include "SSH.h"

class SFTP : public SSH {
    private:
        void* sftpSession;

        SFTP(const SFTP&);
        SFTP& operator=(const SFTP&);
    public:
        SFTP(string, unsigned short = 22);
        virtual ~SFTP(void);

        virtual bool openSession(const string&, const string&);
        virtual void closeSession(void);

        bool exists(const string&) const;
        unsigned __int64 size(const string&) const;
        unsigned __int64 sizeEx(const void*) const;
        void* open(const string&, const unsigned int) const;
        void seek(const void*, const unsigned __int64) const;
        long read(const void*, char*, const unsigned int) const;
        long write(const void*, const char*, const unsigned int) const;
        int close(const void*) const;
        bool mkdir(const string&) const;
        void rmdir(const string&) const;
        list<string> ls(const string& = "./", const unsigned int = FILTER_FILE | FILTER_PATH | FILTER_LONG) const;
        void rm(const string&) const;
        bool rename(const string&, const string&) const;

        static const unsigned int READ;
        static const unsigned int WRITE;
        static const unsigned int CREATE;

        static const unsigned int FILTER_FILE;
        static const unsigned int FILTER_PATH;
        static const unsigned int FILTER_LONG;
};
#endif // SFTP_H_INCLUDED
SFTP.cpp
#include "libssh2/include/libssh2.h"
#include "libssh2/include/libssh2_sftp.h"
#include "SFTP.h"

SFTP::SFTP(string ipAddress, unsigned short tcpPort) : SSH(ipAddress, tcpPort) {
}

SFTP::~SFTP(void) {
    this->closeSession();
}

bool SFTP::openSession(const string& userName, const string& password) {
    if (this->connected)
        return true;
    if (!this->connectTo(userName, password))
        return false;
    this->sftpSession = libssh2_sftp_init((LIBSSH2_SESSION*)this->session);
    if (!this->sftpSession) {
        this->disconnect();
        return false;
    }
    return true;
}

void SFTP::closeSession(void) {
    if (this->connected) {
        libssh2_sftp_shutdown((LIBSSH2_SFTP*)this->sftpSession);
        this->disconnect();
    }
}

bool SFTP::exists(const string& fullName) const {
    if (this->connected) {
        LIBSSH2_SFTP_HANDLE *sftpHandle = libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), LIBSSH2_FXF_READ, LIBSSH2_SFTP_OPENFILE);
        if (!sftpHandle)
            return false;
        libssh2_sftp_close(sftpHandle);
        return true;
    }
    return false;
}

unsigned __int64 SFTP::size(const string& fullName) const {
    if (this->connected) {
        LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), LIBSSH2_FXF_READ, LIBSSH2_SFTP_OPENFILE);
        unsigned __int64 r = this->sizeEx(sftpHandle);
        if (sftpHandle)
            libssh2_sftp_close(sftpHandle);
        return r;
    }
    return 0;
}

unsigned __int64 SFTP::sizeEx(const void* handle) const {
    if (!handle)
        return 0;
    LIBSSH2_SFTP_ATTRIBUTES attr;
    if (!libssh2_sftp_fstat((LIBSSH2_SFTP_HANDLE*)handle, &attr))
        return attr.filesize;
    return 0;
}

void* SFTP::open(const string& fullName, const unsigned int flag) const {
    return libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), flag, LIBSSH2_SFTP_OPENFILE);
}

void SFTP::seek(const void* handle, const unsigned __int64 offset) const {
    libssh2_sftp_seek64((LIBSSH2_SFTP_HANDLE*)handle, offset);
}

long SFTP::read(const void* handle, char* buff, const unsigned int size) const {
    return libssh2_sftp_read((LIBSSH2_SFTP_HANDLE*)handle, buff, size);
}

long SFTP::write(const void* handle, const char* buff, const unsigned int size) const {
    return libssh2_sftp_write((LIBSSH2_SFTP_HANDLE*)handle, buff, size);
}

int SFTP::close(const void* handle) const {
    return libssh2_sftp_close((LIBSSH2_SFTP_HANDLE*)handle);
}

bool SFTP::mkdir(const string& pathName) const {
    if (this->connected)
        return libssh2_sftp_mkdir((LIBSSH2_SFTP*)this->sftpSession,
                                  pathName.c_str(),
                                  LIBSSH2_SFTP_S_IRWXU |
                                  LIBSSH2_SFTP_S_IRGRP |
                                  LIBSSH2_SFTP_S_IXGRP |
                                  LIBSSH2_SFTP_S_IROTH |
                                  LIBSSH2_SFTP_S_IXOTH) == 0;
    return false;
}

void SFTP::rmdir(const string& pathName) const {
    if (this->connected)
        libssh2_sftp_rmdir((LIBSSH2_SFTP*)this->sftpSession, pathName.c_str());
}

list<string> SFTP::ls(const string& path, const unsigned int filter) const {
    list<string> result;
    if (this->connected) {
        LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_opendir((LIBSSH2_SFTP*)this->sftpSession, path.c_str());
        if (sftpHandle) {
            char buff[512];
            char longentry[512];
            LIBSSH2_SFTP_ATTRIBUTES attrs;
            if (filter == (FILTER_FILE | FILTER_PATH | FILTER_LONG)) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0) {
                    if (longentry[0] != '\0')
                        result.push_back(longentry);
                    else
                        result.push_back(buff);
                }
            } else if (filter == (FILTER_FILE | FILTER_PATH )) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
                        result.push_back(buff);
            } else if (filter == (FILTER_FILE | FILTER_LONG)) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
                    if (longentry[0] != '\0' && longentry[0] != 'd') result.push_back(longentry);
            } else if (filter == (FILTER_PATH | FILTER_LONG)) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
                    if (longentry[0] != '\0' && longentry[0] == 'd') result.push_back(longentry);
            } else if (filter == FILTER_FILE) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
                    if (longentry[0] != '\0' && longentry[0] != 'd') result.push_back(buff);
            } else if (filter == FILTER_PATH) {
                while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
                    if (longentry[0] != '\0' && longentry[0] == 'd') result.push_back(buff);
            }
            libssh2_sftp_closedir(sftpHandle);
        }
    }
    return result;
}

void SFTP::rm(const string& fullName) const {
    if (this->connected)
        libssh2_sftp_unlink((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str());
}

bool SFTP::rename(const string& src, const string& obj) const {
    if (this->connected)
        return libssh2_sftp_rename((LIBSSH2_SFTP*)this->sftpSession, src.c_str(), obj.c_str()) == 0;
    return false;
}

const unsigned int SFTP::READ(0x00000001);
const unsigned int SFTP::WRITE(0x00000002);
const unsigned int SFTP::CREATE(0x00000008);

const unsigned int SFTP::FILTER_FILE(0x00000001);
const unsigned int SFTP::FILTER_PATH(0x00000002);
const unsigned int SFTP::FILTER_LONG(0x00000004);