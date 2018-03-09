// SessionSocket.cpp : ʵ���ļ�
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
	// TODO: �ڴ˴���ӹ������

}

CSessionSocket::~CSessionSocket()
{
}

// CSessionSocket ��Ա����

//���ͻ��ر�����ʱ���¼���Ӧ����
void CSessionSocket::OnClose(int nErrorCode)
{
	//��View����ʾ��Ϣ
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CTime timinging;
	timinging = CTime::GetCurrentTime();
	int server_codenum = 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString stringtimetime = timinging.Format("%Y-%m-%d %H:%M:%S �û���");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;

	stringtimetime = stringtimetime + this->m_strdededeName + _T(" �뿪\r\n");
	pView->m_listData.AddString(stringtimetime);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	if (!m_dataBase.IsOpen())
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
	//���ķ�������������
	CString str1 = this->Update_ServerLog();
	//֪ͨ�ͻ���ˢ����������
	this->UpDate_ClientUser(str1, "0");
	this->Close();
	//���ٸ��׽���
	delete this;
	CAsyncSocket::OnClose(nErrorCode);
}

//���յ�����ʱ���¼���Ӧ����
void CSessionSocket::OnReceive(int nErrorCode)
{
	//�Ƚ��պʹ�����Ϣͷ
	HEADER head;
	int head_len = sizeof(HEADER);
	char *pHead = NULL;	//���ڽ�����Ϣͷ
	pHead = new char[head_len];
	if (!pHead) {
		TRACE0("CSessionSocket::OnReceive �ڴ����ʧ��");
		return;
	}
	memset(pHead, 0, head_len*sizeof(char)); //����ͷ�ĳ�ʼ��
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
	delete pHead;	//������ɺ��ͷ�ָ��
	pHead = NULL;

	//������Ϣͷ�е���Ϣ���պ���������
	pHead = new char[ head.nContentLen];
	if (!pHead) {
		TRACE0("CSessionSocket::OnReceive �ڴ����ʧ��");
		return;
	}
	memset(pHead, 0,  head.nContentLen * sizeof(char));
	//�򵥲�����:������ܵ������ݵĳ��Ⱥ���Ϣͷ�еĸ��������ݳ��Ȳ�ͬ�Ļ������Խ��ܣ�ֱ�����
	int flag = 0;
	int num = 0;
	while (!flag) 
	{
		int nError = Receive(pHead, head.nContentLen);
		int temptest = WSAGetLastError();
		if (nError == WSAEWOULDBLOCK)    //û���յ����� �Ժ��������
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
	//������Ϣ���ͣ��ַ�����ͬ�Ĵ�����
	////////////������Ϣ���ͣ���������,���Ҳ�ǺͿͻ��˽��ж�Ӧ�ġ��������MSG_LOGOIN��MSG_SEND�Ƕ���õĳ���������F12����////////////////////
	switch (head.type)
	{
		case MSG_LOGOIN: //��½��Ϣ
			OnLogoINin(pHead, head.nContentLen, head.from_theuser);

			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			break;
		case MSG_SEND: //������Ϣ
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
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int temptestgood = m_dataBase.IsOpen();
	if (!temptestgood)
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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

	//TODO:����û��������Ƿ��Ӧ
	CTime con_time;
	con_time = CTime::GetCurrentTime();  //��ȡ����ʱ��
	CString strTime = con_time.Format("%Y-%m-%d %H:%M:%S �û���");
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	strTime = strTime + fromuseruser + _T(" ��¼\r\n");
	//��¼��־
	//��������NetChatServerDlg��Ŀؼ���ʾ


	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempjudgebad = m_dataBase.IsOpen();
	if (!tempjudgebad)
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
	/*�������ݿ����û���ip*/
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
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

	/*����Ƿ���������Ϣ*/
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
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		bool tempking = pTemp->m_strdededeName == _head.to_theuser;
		if (tempking)
		{
			int testtemp =pTemp->Send(&_head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, len + 1);	//Ȼ�󷢲�����
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
	//��WideCharToMultiByte����ת��
	if (!WChar2MByte(strUserInfo.GetBuffer(0), pSend, head.nContentLen)) {
		AfxMessageBox(_T("�ַ�ת��ʧ��"));
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

	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
	int server_codenum;
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempgoodgood = m_dataBase.IsOpen();
	if (!tempgoodgood)
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int tempgoods = m_dataBase.IsOpen();
	if (!tempgoods)
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
	m_dataBase.Open(NULL,
		false,
		false,
		_T("ODBC;server=127.0.0.1;DSN=My_ODBC;UID=root;PWD=622332")
	);
	int boolgoood = m_dataBase.IsOpen();
	if (!boolgoood)
	{
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
		time = CTime::GetCurrentTime();  //��ȡ����ʱ��
		CString strTime = time.Format("%Y-%m-%d %H:%M:%S �û���");
		CString str1 = strTime + _T("�û� ") + Name + _T(" ע�����");
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
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		bool strnamebool = pTemp->m_strdededeName == _head.to_theuser;
		if (strnamebool)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, len + 1);	//Ȼ�󷢲�����
		}
	}
}

void CSessionSocket::GetQuestion(HEADER head) {
	int server_codenum=0;
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
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
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
		return;
	}
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString from(head.from_theuser);

	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CTime time;
	time = CTime::GetCurrentTime();  //��ȡ����ʱ��
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S �û���");
	strTime = strTime + from + _T(" ��ȡ�ܱ�����\r\n");
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

	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	int tempps = ps != NULL;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		int bijj = pTemp->m_strdededeName == _head.to_theuser;
		if (bijj)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, strlen(data));	//Ȼ�󷢲�����
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

	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
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
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
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
	//TODO:����û��������Ƿ��Ӧ

	CTime time;
	time = CTime::GetCurrentTime();  //��ȡ����ʱ��
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S �û���");
	strTime = strTime + Name + _T(" �޸�����\r\n");
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
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		int bijiaotemp = pTemp->m_strdededeName == _head.to_theuser;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		if (bijiaotemp)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, len + 1);	//Ȼ�󷢲�����
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
	CDatabase m_dataBase;  //���ݿ�
						   //�������ݿ�
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
		AfxMessageBox(_T("���ݿ�����ʧ��!"));
		return;
	}
	CString to_user(touser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CServerView* pView = (CServerView*)((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetActiveView();
	CString from_user(head.from_theuser);
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	CString showMsg = _T("�û� ") + from_user + _T(" �������û� ") + to_user + _T("ͨ��");
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
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		int strnametemptemp = pTemp->m_strdededeName == _head.to_theuser;
		if (strnametemptemp)
		{
			pTemp->Send(&_head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, len + 1);	//Ȼ�󷢲�����
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
	POSITION ps = pView->m_pSessionList->GetHeadPosition();  //ȡ�ã������û��Ķ���
	while (ps != NULL)
	{
		CSessionSocket* pTemp = (CSessionSocket*)pView->m_pSessionList->GetNext(ps);
		//ֻ����2���ˣ� һ���Ƿ���������Ϣ���˺ͽ���������Ϣ���ˡ�
		//���������������Ϣ�����ǡ�Ⱥ�ġ���ô�ͷ��������û���ʵ��Ⱥ�ĺ�һ��һ�ؼ������ڴ�
		int strstrnamnam = pTemp->m_strdededeName == head.to_theuser;
		for (int i = 0; i < 5; i++)
			server_codenum += 2;
		if (strstrnamnam)
		{
			pTemp->Send(&head, sizeof(HEADER));  //�ȷ���ͷ��
			pTemp->Send(data, strstrlen + 1);	//Ȼ�󷢲�����
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