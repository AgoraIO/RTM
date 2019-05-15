// ChatDlg.cpp : Implement File
//

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "AgoraRTMInstance.h"
#include "commonFun.h"


// CChatDlg Dialog

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(eTagMsgtype type, std::string chatparam,CWnd* pParent /*=NULL*/)
	: CDialogEx(CChatDlg::IDD, pParent),
	m_type(type),
	m_inputParam(chatparam)
{

}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CChatDlg Message Handler
BOOL CChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetBackgroundColor(RGB(192, 192, 192));
	m_ftBtn.CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("¿¬Ìå"));
	this->SetFont(&m_ftBtn);
	m_curAccount = CAgoraRTMInstance::getSignalInstance()->getLoginAccount();
	m_heightLine = 450;
	m_widthChar = 120;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CChatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (::IsWindow(m_edtChatBox.GetSafeHwnd()))
		m_edtChatBox.MoveWindow(1, cy - 21, cx - 2, 20);
}

void CChatDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	POSITION pos = m_strMsgList.GetHeadPosition();
	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(RGB(192, 192, 192));

	int heighLine = m_heightLine;
	while (pos != NULL) {
		CString str = m_strMsgList.GetNext(pos);
		dc.TextOut(20, heighLine, str);
		heighLine -= 20;
	}

	//TRACE("curParam:%s onPaint\n", m_inputParam.data());
}

void CChatDlg::AddChatMessage(std::string Uid, LPCTSTR lpMessage)
{
	CString str;
	CString strUid = s2cs(Uid);

	str.Format(_T("[%s]:%s"), strUid.GetBuffer(), lpMessage);
	strUid.ReleaseBuffer();

	if (m_strMsgList.GetCount() >= 100)
		m_strMsgList.RemoveTail();

	m_strMsgList.AddHead(str);

	if (IsWindowVisible())
	Invalidate(TRUE);
}

void CChatDlg::ClearHistory()
{
	m_strMsgList.RemoveAll();

	Invalidate(TRUE);
}
