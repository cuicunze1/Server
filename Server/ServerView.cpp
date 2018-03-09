
// ServerView.cpp : CServerView ���ʵ��
//

#include "stdafx.h"
#include "afxwin.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Server.h"
#endif

#include "ServerDoc.h"
#include "ServerView.h"
#include "ListenSocket.h"
#include "ServerInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerView

IMPLEMENT_DYNCREATE(CServerView, CFormView)

BEGIN_MESSAGE_MAP(CServerView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CServerView ����/����

CServerView::CServerView()
	: CFormView(IDD_SERVER_FORM)
{
	PUBPort = 6000;
}

CServerView::~CServerView()
{
}

void CServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listData);
	DDX_Control(pDX, IDC_LIST2, m_userList);
}

BOOL CServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}

void CServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	// �����׽���
	m_pListenSocket = new CListenSocket();
	if (!m_pListenSocket)
	{
		AfxMessageBox(_T("��̬�����������׽��ֳ���!"));
		return ;
	}
	int server_codenum = 0;
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	if (m_pListenSocket->Create(Server_port, SOCK_STREAM) == FALSE)
	{
		AfxMessageBox(_T("�����׽���ʧ��!"), MB_OK | MB_ICONEXCLAMATION);
		m_pListenSocket->Close();
		return;
	}
	m_pSessionList = new CPtrList();
	if (!m_pSessionList)
	{
		AfxMessageBox(_T("�����Ự��ʧ��!"));
	}
	for (int i = 0; i < 5; i++)
		server_codenum += 2;
	// �����ɹ����ȴ���������
	if (m_pListenSocket->Listen() == FALSE)
	{
		if (m_pListenSocket->GetLastError() == WSAEWOULDBLOCK)
		{
			AfxMessageBox(_T("��������ʧ��!"));
			int server_codenum = 0;
			for (int i = 0; i < 5; i++)
				server_codenum += 2;
			m_pListenSocket->Close();
			return;
		}
	}
	AfxMessageBox(_T("������������!"));
}
void CServerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CServerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CServerView ���

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CServerView ��Ϣ�������