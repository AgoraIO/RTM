// DlgInput.cpp : Implement File
//

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "DlgInput.h"
#include "afxdialogex.h"
#include "commonFun.h"
#include "AgoraRTMInstance.h"


// CDlgInput Dialog

IMPLEMENT_DYNAMIC(CDlgInput, CDialogEx)

CDlgInput::CDlgInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgInput::IDD, pParent)
{

}

CDlgInput::~CDlgInput()
{
}

void CDlgInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CDlgInput::showWindow(eTagMsgtype type)
{
	m_type = type;
	ShowWindow(SW_SHOW);
}


BEGIN_MESSAGE_MAP(CDlgInput, CDialogEx)
END_MESSAGE_MAP()


// CDlgInput Message Handler
BOOL CDlgInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}


BOOL CDlgInput::PreTranslateMessage(MSG* pMsg)
{	
	if ( VK_RETURN == pMsg->wParam && WM_KEYDOWN == pMsg->message){

		CString InputParam;
		GetDlgItem(IDC_EDIT_INPUT_PARAM)->GetWindowTextW(InputParam);
		if (_T("") == InputParam){

			AfxMessageBox(_T("Input is blank."));
		}
		else{

			PAG_INPUTPARAM lpData = new AG_INPUTPARAM;
			lpData->strParam = cs2s(InputParam);
			lpData->type = m_type;
			
			HWND MsgWnd = CAgoraRTMInstance::getSignalInstance()->getChatMsgWnd();
			::PostMessage(MsgWnd, WM_UpdateInputParam, (WPARAM)lpData, NULL);
		}
	}
	else if (pMsg->hwnd == m_hWnd && VK_ESCAPE == pMsg->wParam){
		ShowWindow(SW_HIDE);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}