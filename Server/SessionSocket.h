#pragma once
#include "Head.h"
// CSessionSocket 命令目标

class CSessionSocket : public CAsyncSocket
{
public:
	CSessionSocket();
	virtual ~CSessionSocket();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
public:
	int disturbing;
	void OnLogoINin(char* buff, int nlen, char from_user[20]);
	void UpDate_ClientUser(CString strUserInfo, char from_user[20]);
	void OnMSGTranslate(char* buff, HEADER head); //转发消息给其他用户
	void OnUserRegist(HEADER head, char *buf);
	void Answer_Login(int flag, char *from_user, int GivenPort);
	void GetQuestion(HEADER head);
	void OnUserReset(HEADER head, char *buf);
	void Answer_Reset(int flag, HEADER head);
	void OnGetIP(HEADER head, char *buf);
	void SendOfflineMsg(char *toUser, CString msg);
	int server_func()
	{
		int server_codenum = 0;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
	}
	CString Update_ServerLog();
private:
	BOOL WChar2MByte(LPCWSTR srcBuff, LPSTR destBuff, int nlen);
public:
	SOCKADDR_IN sockAddr;
	CString m_strdededeName; //连接名称
};


