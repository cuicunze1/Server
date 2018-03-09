// SessionSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "SessionSocket.h"
#include "ServerView.h"
#include "MainFrm.h"
#include "afxdb.h"
#include "cJSON.h"
#include <stdlib.h>


// CSessionSocket

CSessionSocket::CSessionSocket()
	:m_strdededeName(_T(""))
{
	// TODO: 在此处添加构造代码

}

CSessionSocket::~CSessionSocket()
{
}

// CSessionSocket 成员函数

//当客户关闭连接时的事件响应函数
void CSessionSocket::OnClose(int nErrorCode)
{
	//在View里显示信息
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CTime timinging;
	timinging = CTime::GetCurrentTime();
	int server_codenum = 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString stringtimetime = timinging.Format("%Y-%m-%d %H:%M:%S 用户：");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;

	stringtimetime = stringtimetime + this->m_strdededeName + _T(" 离开\r\n");
	pView->m_listData.AddString(stringtimetime);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	if (!m_dataBase.IsOpen())
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}

	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + this->m_strdededeName + _T("'");
	CRecordset *m_rerecocordssset;
	m_rerecocordssset = new CRecordset(&m_dataBase);
	m_rerecocordssset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	if ((m_rerecocordssset->GetRecordCount()) != 0) {
		CString str1 = _T("update socket.userinfo set isOnline = 'N' where name='") + this->m_strdededeName + _T("'");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		m_dataBase.ExecuteSQL(str1);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		str1 = _T("update socket.userinfo set ip = '0.0.0.0' where name='") + this->m_strdededeName + _T("'");
		m_dataBase.ExecuteSQL(str1);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		str1 = _T("update socket.userinfo set port = '0' where name='") + this->m_strdededeName + _T("'");
		m_dataBase.ExecuteSQL(str1);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
	}
	m_rerecocordssset->Close();
	m_dataBase.Close();

	int tempps = pView->m_userList.FindString(0, this->m_strdededeName);
	tempps++;
	tempps--;
	pView->m_userList.DeleteString(tempps);
	pView->m_pSessionList->RemoveAt(pView->m_pSessionList->Find(this));
	//更改服务器在线名单
	CString str1 = this->Update_ServerLog();
	//通知客户端刷新在线名单
	this->UpDate_ClientUser(str1, "0");
	this->Close();
	//销毁该套接字
	delete this;
	CAsyncSocket::OnClose(nErrorCode);
}

//接收到数据时的事件响应函数
void CSessionSocket::OnReceive(int nErrorCode)
{
	//先接收和处理消息头
	HEADER head;
	int head_len = sizeof(HEADER);
	char *pHead = NULL;	//用于接收消息头
	pHead = new char[head_len];
	if (!pHead) {
		TRACE0("CSessionSocket::OnReceive 内存分配失败");
		return;
	}
	memset(pHead, 0, head_len*sizeof(char)); //接受头的初始化
	Receive(pHead, head_len);
	
	head.type = ((HEADER *)pHead)->type;
	head.type++;
	head.type--;
	head.type+=0;
	head.nContentLen = ((HEADER *)pHead)->nContentLen;
	head.nContentLen += 0;
	memset(head.to_theuser, 0, sizeof(head.to_theuser));
	strcpy(head.to_theuser, ((HEADER *)pHead)->to_theuser);
	memset(head.from_theuser, 0, sizeof(head.from_theuser));
	strcpy(head.from_theuser, ((HEADER *)pHead)->from_theuser);
	delete pHead;	//接受完成后释放指针
	pHead = NULL;

	//根据消息头中的信息接收后续的数据
	pHead = new char[ head.nContentLen];
	if (!pHead) {
		TRACE0("CSessionSocket::OnReceive 内存分配失败");
		return;
	}
	memset(pHead, 0,  head.nContentLen * sizeof(char));
	//简单差错检验:如果接受到的数据的长度和信息头中的给出的数据长度不同的话不予以接受，直接清空
	int flag = 0;
	int num = 0;
	while (!flag) 
	{
		int nError = Receive(pHead, head.nContentLen);
		int temptest = WSAGetLastError();
		if (nError == WSAEWOULDBLOCK)    //没有收到数据 稍后再做检查
		{
			Sleep(100);
			flag = 0;
			break;
		}
		else
		{
			num = nError;
			flag = 1;
			break;
		}
	}
	if (num != head.nContentLen) {
		delete pHead;
		return;
	}

	

	int server_codenum = 0;
	//根据消息类型，分发给不同的处理函数
	////////////根据消息类型，处理数据,这个也是和客户端进行对应的。。下面的MSG_LOGOIN，MSG_SEND是定义好的常量，可以F12看看////////////////////
	switch (head.type)
	{
		case MSG_LOGOIN: //登陆消息
			OnLogoINin(pHead, head.nContentLen, head.from_theuser);

			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_SEND: //发送消息
			OnMSGTranslate(pHead, head);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_REGIST:
			OnUserRegist(head, pHead);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_GETQUE:
			GetQuestion(head);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_RESET:
			OnUserReset(head, pHead);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_GETIP:
			OnGetIP(head, pHead);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		default: 
			break;
	}

	delete pHead;
	pHead = NULL;
	CAsyncSocket::OnReceive(nErrorCode);
}

float fCidddr_L(float x)
{
	float z = -1.0;         
	if (x >= 0.0)         
		z = 2 * x*x;
	return(z);          
}

void CSessionSocket::OnLogoINin(char* buff, int nlen,char from_user[20]){
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int temptestgood = m_dataBase.IsOpen();
	if (!temptestgood)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	CString fromuseruser(from_user);
	m_strdededeName = fromuseruser;
	int server_codenum = 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + fromuseruser + _T("'");
	CRecordset *m_recordset;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset = new CRecordset(&m_dataBase);
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	long num = m_recordset->GetRecordCount();
	int tempnumgood = num;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	if (tempnumgood == 0) {
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		Answer_Login(0, from_user, pView->PUBPort);
		return;
	}
	CString tempStr;
	LPCTSTR lpctStr = (LPCTSTR)_T("password");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->GetFieldValue(lpctStr, tempStr);
	m_recordset->Close();
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_dataBase.Close();
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	cJSON *json_root = NULL;
	json_root = cJSON_Parse(buff);
	char *name = cJSON_Geto(json_root, "username")->vstring;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	char *password = cJSON_Geto(json_root, "password")->vstring;
	CString Name(name);
	CString Password(password);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int tempstrjudge = tempStr.Compare(Password);
	if ( tempstrjudge!= 0) {
		Answer_Login(0, from_user, pView->PUBPort);
		return;
	}
	else {
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		Answer_Login(1, from_user, pView->PUBPort);
	}

	//TODO:检查用户与密码是否对应
	CTime con_time;
	con_time = CTime::GetCurrentTime();  //获取现在时间
	CString strTime = con_time.Format("%Y-%m-%d %H:%M:%S 用户：");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	strTime = strTime + fromuseruser + _T(" 登录\r\n");
	//记录日志
	//将内容在NetChatServerDlg里的控件显示


	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempjudgebad = m_dataBase.IsOpen();
	if (!tempjudgebad)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}

	CString strTmp = _T("SELECT * FROM socket.userinfo WHERE name = '") + fromuseruser + _T("'");
	m_recordset = new CRecordset(&m_dataBase);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, strTmp);
	long cnt = m_recordset->GetRecordCount();
	if (cnt != 0) {
		CString str1 = _T("update socket.userinfo set isOnline = 'Y' where name='") + fromuseruser + _T("'");
		m_dataBase.ExecuteSQL(str1);
	}
	/*更新数据库中用户的ip*/
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		bool judgeins = (pTemp->m_strdededeName == fromuseruser);
		if (judgeins)
		{
			SOCKADDR_IN sock = pTemp->sockAddr;
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			char * ip = inet_ntoa(sock.sin_addr);
			CString IP(ip);
			CString ListenPort;
			ListenPort.Format(_T("%d"), pView->PUBPort);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			pView->PUBPort++;
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			CString str1 = _T("update socket.userinfo set ip = '")+ IP + _T("' where name='") + fromuseruser + _T("'");
			m_dataBase.ExecuteSQL(str1);
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			str1 = _T("update socket.userinfo set port = '") + ListenPort + _T("' where name='") + fromuseruser + _T("'");
			m_dataBase.ExecuteSQL(str1);
			break;
		}
	}

	/*检查是否有离线消息*/
	CString findOffline = _T("SELECT * FROM socket.offline_msg WHERE toUser = '") + fromuseruser + _T("'");
	m_recordset = new CRecordset(&m_dataBase);
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, findOffline);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	long offlineCnt = m_recordset->GetRecordCount();
	int offtemp = offlineCnt;
	if (offtemp != 0) {
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		m_recordset->MoveLast();
		CString lastMsg;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		LPCTSTR lpctstring = (LPCTSTR)_T("message");
		m_recordset->GetFieldValue(lpctstring, lastMsg);
		m_recordset->MoveFirst();
		CString mess("");
		CString TolMesmesmes("");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		while (mess.Compare(lastMsg) != 0) {
			m_recordset->GetFieldValue(lpctstring, mess);
			TolMesmesmes += mess;
			m_recordset->MoveNext();
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
		}
		SendOfflineMsg(from_user, TolMesmesmes);
		CString safeDelete = _T("SET SQL_SAFE_UPDATES = 0;");
		m_dataBase.ExecuteSQL(safeDelete);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		CString str = _T("delete from socket.offline_msg where toUser = '") + fromuseruser + _T("'");
		m_dataBase.ExecuteSQL(str);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		safeDelete = _T("SET SQL_SAFE_UPDATES = 1;");
		m_dataBase.ExecuteSQL(safeDelete);
	}

	m_recordset->Close();
	m_dataBase.Close();

	pView->m_listData.AddString(strTime);
	disturbing = 1;
	int nicejob = disturbing;
	pView->m_userList.AddString(fromuseruser);
	CString str1 = this->Update_ServerLog();
	this->UpDate_ClientUser(str1, from_user);
}

void CSessionSocket::Answer_Login(int flag, char *from_user, int GivenPort) {

	CString str;
	if (!flag ) {
		str = _T("{\"cmd\":0}");
	}
	else {
		str.Format(_T("{\"cmd\":1, \"port\":%d}"), GivenPort);
	}

	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char *data = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, str, -1, data, len, NULL, NULL);

	HEADER _head;
	_head.type = MSG_LOGOIN;
	_head.nContentLen = len + 1;
	memset(_head.to_theuser, 0, sizeof(_head.to_theuser));
	strcpy(_head.to_theuser, from_user);
	memset(_head.from_theuser, 0, sizeof(_head.from_theuser));
	strcpy(_head.from_theuser, "Server");


	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		bool tempking = pTemp->m_strdededeName == _head.to_theuser;
		if (tempking)
		{
			int testtemp =pTemp->Send(&_head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, len + 1);	//然后发布内容
		}
	}
}

void CSessionSocket::UpDate_ClientUser(CString strUserInfo, char from_user[20]) {
	HEADER head;
	head.type = MSG_UPDATE;
	head.nContentLen = strUserInfo.GetLength() + 1;
	memset(head.from_theuser, 0, sizeof(head.from_theuser));
	strcpy(head.from_theuser, from_user);
	char *pSend = new char[head.nContentLen];
	memset(pSend, 0, head.nContentLen * sizeof(char));
	//用WideCharToMultiByte进行转化
	if (!WChar2MByte(strUserInfo.GetBuffer(0), pSend, head.nContentLen)) {
		AfxMessageBox(_T("字符转换失败"));
		delete pSend;
		return;
	}

	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	POSITION ps = pView->m_pSessionList->GetHeadPosition();
	while (ps != NULL)
	{
		CSessionSocket *pTemp = (CSessionSocket *)pView->m_pSessionList->GetNext(ps);
		pTemp->Send((char *)&head, sizeof(head));
		pTemp->Send(pSend, head.nContentLen);
	}
	delete pSend;
}

BOOL CSessionSocket::WChar2MByte(LPCWSTR srcBuff, LPSTR destBuff, int nlen)
{
	int n = 0;
	n = WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, 0, 0, FALSE);
	bool bijiao = n < nlen;
	if (bijiao)
		return FALSE;

	WideCharToMultiByte(CP_OEMCP, 0, srcBuff, -1, destBuff, nlen, 0, FALSE);

	return TRUE;
}

CString CSessionSocket::Update_ServerLog() {

	CDatabase m_dataBase;  //数据库
						   //连接数据库
	int server_codenum;
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempgoodgood = m_dataBase.IsOpen();
	if (!tempgoodgood)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return _T("");
	}
	CRecordset *m_recordset;
	m_recordset = new CRecordset(&m_dataBase);
    server_codenum = 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString str = _T("SELECT * FROM socket.userinfo");
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	/*long num = m_recordset->GetRecordCount();
	short cnt = m_recordset->GetODBCFieldCount();*/
	m_recordset->MoveLast();
	CString lastname;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	LPCTSTR lpctStr = (LPCTSTR)_T("name");
	m_recordset->GetFieldValue(lpctStr, lastname);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->MoveFirst();
	CString name("");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString status("");
	CString strUserInfo("");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	while (name.Compare(lastname) != 0) {
		LPCTSTR getName = (LPCTSTR)_T("name");
		m_recordset->GetFieldValue(getName, name);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		LPCTSTR getStatusgetget = (LPCTSTR)_T("isOnline");
		m_recordset->GetFieldValue(getStatusgetget, status);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		CString json = _T("{\"name\":\"") + name + _T("\",\"status\":\"") + status + _T("\"}");
		strUserInfo = strUserInfo + json + _T("#");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		m_recordset->MoveNext();
	}
	m_recordset->Close();
	m_dataBase.Close();

	return strUserInfo;
}

void CSessionSocket::OnMSGTranslate(char* buff, HEADER head)
{
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempgoods = m_dataBase.IsOpen();
	if (!tempgoods)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	CString toUser(head.to_theuser);
	CString fromUser(head.from_theuser);
	CString Msg(buff);
	CString str = _T("insert into socket.offline_msg values ('") + toUser + _T("', '") + fromUser + _T("', '") + Msg + _T("')");
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	pView->m_listData.AddString(str);
	m_dataBase.ExecuteSQL(str);
	m_dataBase.Close();
}

void CSessionSocket::OnUserRegist(HEADER head, char *buf) {
	int server_codenum = 0;
	cJSON *json_root = NULL;
	json_root = cJSON_Parse(buf);
	char *name = cJSON_Geto(json_root, "username")->vstring;
	char *password = cJSON_Geto(json_root, "password")->vstring;
	char *question = cJSON_Geto(json_root, "question")->vstring;
	char *answer = cJSON_Geto(json_root, "answer")->vstring;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int boolgoood = m_dataBase.IsOpen();
	if (!boolgoood)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	CString Name(name);
	CString Password(password);
	CString Question(question);
	CString Answer(answer);
	CString isOnline("N");
	CString ip("0.0.0.0");
	CString port("0");

	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	m_strdededeName = Name;
	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + Name + _T("'");
	CRecordset *m_recordset;
	m_recordset = new CRecordset(&m_dataBase);
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	long num = m_recordset->GetRecordCount();
	CString strTemp;
	if (!num) {
		strTemp = _T("{\"cmd\":1}");
		CString str = _T("insert into socket.userinfo values ('") + Name + _T("', '") + Password + _T("', '") + Question + _T("', '") + Answer + _T("', '") + isOnline + _T("', '") + ip + _T("', '") + port + _T("')");
		m_dataBase.ExecuteSQL(str);
		CTime time;
		time = CTime::GetCurrentTime();  //获取现在时间
		CString strTime = time.Format("%Y-%m-%d %H:%M:%S 用户：");
		CString str1 = strTime + _T("用户 ") + Name + _T(" 注册完成");
		pView->m_listData.AddString(str1);
	}
	else {
		strTemp = _T("{\"cmd\":0}");
	}
	m_recordset->Close();
	m_dataBase.Close();
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int len = WideCharToMultiByte(CP_ACP, 0, strTemp, -1, NULL, 0, NULL, NULL);
	char *data = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strTemp, -1, data, len, NULL, NULL);
	HEADER _head;
	_head.type = MSG_REGIST;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	_head.nContentLen = len + 1;
	strcpy(_head.to_theuser, name);
	strcpy(_head.from_theuser, "Server");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		bool strnamebool = pTemp->m_strdededeName == _head.to_theuser;
		if (strnamebool)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, len + 1);	//然后发布内容
		}
	}
}

void CSessionSocket::GetQuestion(HEADER head) {
	int server_codenum=0;
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int tempptem = m_dataBase.IsOpen();
	if (!tempptem)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString from(head.from_theuser);

	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CTime time;
	time = CTime::GetCurrentTime();  //获取现在时间
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S 用户：");
	strTime = strTime + from + _T(" 获取密保问题\r\n");
	pView->m_listData.AddString(strTime);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_strdededeName = from;
	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + from + _T("'");
	CRecordset *m_recordset;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset = new CRecordset(&m_dataBase);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	long num = m_recordset->GetRecordCount();
	CString question("");
	CString str1;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int bij = num != 0;
	if (bij) {
		LPCTSTR lpctStr = (LPCTSTR)_T("question");
		m_recordset->GetFieldValue(lpctStr, question);
		str1 = _T("{\"cmd\":1,\"question\":\"") + question + _T("\"}");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
	}
	else {
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		str1 = _T("{\"cmd\":0}");
	}
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int len = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char *data = new char[len + 1];
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	WideCharToMultiByte(CP_ACP, 0, str1, -1, data, len, NULL, NULL);
	m_recordset->Close();
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_dataBase.Close();

	HEADER _head;
	_head.type = MSG_GETQUE;
	memset(_head.from_theuser, 0, sizeof(_head.from_theuser));
	strcpy(_head.from_theuser, "Server");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(_head.to_theuser, 0, sizeof(_head.to_theuser));
	strcpy(_head.to_theuser, head.from_theuser);
	_head.nContentLen = strlen(data);

	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	int tempps = ps != NULL;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		int bijj = pTemp->m_strdededeName == _head.to_theuser;
		if (bijj)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, strlen(data));	//然后发布内容
		}
	}
}
float fCir_aL(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
float fCiraa_L(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
float fCiaaar_L(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
void CSessionSocket::OnUserReset(HEADER head, char *buf) {
	int server_codenum=0;
	cJSON *json_root = NULL;
	json_root = cJSON_Parse(buf);
	char *name = cJSON_Geto(json_root, "username")->vstring;
	char *password = cJSON_Geto(json_root, "password")->vstring;
	char *answer = cJSON_Geto(json_root, "answer")->vstring;

	CDatabase m_dataBase;  //数据库
						   //连接数据库
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int goodggg = m_dataBase.IsOpen();
	if (!goodggg)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	CString Name(name);
	CString Password(password);
	CString Answer(answer);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	m_strdededeName = Name;

	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + Name + _T("'");
	CRecordset *m_recordset;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset = new CRecordset(&m_dataBase);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	long cnt = m_recordset->GetRecordCount();
	if (cnt == 0) {
		Answer_Reset(0, head);
		return;
	}
	CString m_answer;
	LPCTSTR lpctStr = (LPCTSTR)_T("answer");
	m_recordset->GetFieldValue(lpctStr, m_answer);
	int whatgood = m_answer.Compare(Answer) != 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	if (whatgood) {
		Answer_Reset(0, head);
		return;
	}
	CString str1 = _T("update socket.userinfo set password='")+ Password + _T("' where name='") + Name + _T("'");
	m_dataBase.ExecuteSQL(str1);
	Answer_Reset(1, head);
	//TODO:检查用户与密码是否对应

	CTime time;
	time = CTime::GetCurrentTime();  //获取现在时间
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S 用户：");
	strTime = strTime + Name + _T(" 修改密码\r\n");
	pView->m_listData.AddString(strTime);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->Close();
	m_dataBase.Close();
}
float fCiraaa_L(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
void CSessionSocket::Answer_Reset(int flag, HEADER head) {
	int server_codenum = 0;
	CString tempStr;
	if ((flag +5) ==6) {
		tempStr = _T("{\"cmd\":1}");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
	}
	else {
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		tempStr = _T("{\"cmd\":0}");
	}
	int len = WideCharToMultiByte(CP_ACP, 0, tempStr, -1, NULL, 0, NULL, NULL);
	char *data = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, tempStr, -1, data, len, NULL, NULL);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	HEADER _head;
	_head.type = MSG_RESET;
	_head.nContentLen = len + 1;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(_head.to_theuser, 0, sizeof(_head.to_theuser));
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	strcpy(_head.to_theuser, head.from_theuser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(_head.from_theuser, 0, sizeof(_head.from_theuser));
	strcpy(_head.from_theuser, "Server");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		int bijiaotemp = pTemp->m_strdededeName == _head.to_theuser;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		if (bijiaotemp)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, len + 1);	//然后发布内容
		}
	}
}
float fCiadadr_L(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
void CSessionSocket::OnGetIP(HEADER head, char *buf) {
	int server_codenum = 0;
	cJSON *json_root = NULL;
	json_root = cJSON_Parse(buf);
	char *touser = cJSON_Geto(json_root, "touser")->vstring;
	CDatabase m_dataBase;  //数据库
						   //连接数据库
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int basegood = m_dataBase.IsOpen();
	if (!basegood)
	{
		AfxMessageBox(_T("数据库连接失败!"));
		return;
	}
	CString to_user(touser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CString from_user(head.from_theuser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString showMsg = _T("用户 ") + from_user + _T(" 请求与用户 ") + to_user + _T("通信");
	pView->m_listData.AddString(showMsg);
	CString str = _T("SELECT * FROM socket.userinfo WHERE name = '") + to_user + _T("'");
	CRecordset *m_recordset;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset = new CRecordset(&m_dataBase);
	m_recordset->Open(AFX_DB_USE_DEFAULT_TYPE, str);
	long cnt = m_recordset->GetRecordCount();
	for (int i = 0; i < 5; i++)
		i += 2;
	if (cnt == 0) {
		Answer_Reset(0, head);
		return;
	}
	CString isOnline;
	LPCTSTR lpctStr = (LPCTSTR)_T("isOnline");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	m_recordset->GetFieldValue(lpctStr, isOnline);
	CString Online("Y");
	CString SendMsg;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	int onlinetemp = isOnline.Compare(Online) == 0;
	if (onlinetemp) {
		CString IP, Port;
		LPCTSTR lpctStr = (LPCTSTR)_T("ip");
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		m_recordset->GetFieldValue(lpctStr, IP);
		lpctStr = (LPCTSTR)_T("port");
		m_recordset->GetFieldValue(lpctStr, Port);
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		SendMsg = _T("{\"isOnline\":\"") + isOnline + _T("\", \"ip\":\"") + IP + _T("\", \"port\": \"") + Port + _T("\", \"touser\":\"") + to_user + _T("\"}");
	}
	else {
		CString IP("192.168.43.245");
		CString Port("5050");
		SendMsg = _T("{\"isOnline\":\"") + isOnline + _T("\", \"ip\":\"") + IP + _T("\", \"port\": \"") + Port + _T("\", \"touser\":\"") + to_user + _T("\"}");
	}
	m_recordset->Close();
	m_dataBase.Close();

	int len = WideCharToMultiByte(CP_ACP, 0, SendMsg, -1, NULL, 0, NULL, NULL);
	char *data = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, SendMsg, -1, data, len, NULL, NULL);

	HEADER _head;
	_head.type = MSG_GETIP;
	_head.nContentLen = len + 1;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(_head.to_theuser, 0, sizeof(_head.to_theuser));
	strcpy(_head.to_theuser, head.from_theuser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(_head.from_theuser, 0, sizeof(_head.from_theuser));
	strcpy(_head.from_theuser, "Server");
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		int strnametemptemp = pTemp->m_strdededeName == _head.to_theuser;
		if (strnametemptemp)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, len + 1);	//然后发布内容
		}
	}
}
float fCir_aaaaaL(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}
void CSessionSocket::SendOfflineMsg(char *toUser, CString msg) {
	int server_codenum = 0;
	CString sendStr = _T("{\"offlineMsg\":\"") + msg + _T("\"}");
	int strstrlen = WideCharToMultiByte(CP_ACP, 0, sendStr, -1, NULL, 0, NULL, NULL);
	char *data = new char[strstrlen + 1];
	WideCharToMultiByte(CP_ACP, 0, sendStr, -1, data, strstrlen, NULL, NULL);

	HEADER head;
	head.type = MSG_OFFLINE;
	head.nContentLen = strstrlen + 1;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(head.to_theuser, 0, sizeof(head.to_theuser));
	strcpy(head.to_theuser, toUser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	memset(head.from_theuser, 0, sizeof(head.from_theuser));
	strcpy(head.from_theuser, "Server");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //取得，所有用户的队列
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//只发送2个人， 一个是发送聊天消息的人和接收聊天消息的人。
		//如果，接收聊天消息的人是“群聊”那么就发送所有用户，实现群聊和一对一关键就在于此
		int strstrnamnam = pTemp->m_strdededeName == head.to_theuser;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		if (strstrnamnam)
		{
			pTemp->Send(&head, sizeof(HEADER));  //先发送头部
			pTemp->Send(data, strstrlen + 1);	//然后发布内容
		}
	}
}
float fCir_L(float x)
{
	float z = -1.0;
	if (x >= 0.0)
		z = 2 * x*x;
	return(z);
}