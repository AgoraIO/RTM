
// AgoraRTMTutorialDlg.h : header file
//

#pragma once
#include "AgoraRTMInstance.h"
#include "RTMWrap.h"
class CDlgChatMsg;
// CAgoraRTMDlg Dialog
class CAgoraSignalInstance;
class CSingleCallBack;
class CAgoraRTMDlg : public CDialogEx
{
// Construct
public:
  CAgoraRTMDlg(CWnd* pParent = NULL);

// Dialog Data
	enum { IDD = IDD_AgoraRTMTutorial_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	//DDX/DDV Support


private:
	void initCtrl();
	void uninitCtrl();
	
	void initResource();
	void uninitResource();

// Implement
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogin();

	LRESULT onLoginSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT onLogout(WPARAM wParam, LPARAM lParam);
	LRESULT onLogFailed(WPARAM wParam, LPARAM lParam);
	LRESULT onError(WPARAM wParam, LPARAM lParam);
	LRESULT onLog(WPARAM wParam, LPARAM lParam);

private:
	std::string m_account;
	bool m_isLogin;

  CAgoraRTMInstance* m_pRTMInstance;
  CRTMCallBack* m_pRTMCallBack;
};
