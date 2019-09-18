#pragma once

#include <afxwin.h>
// CChatDlg Dialog

class CChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(eTagMsgtype type,std::string chatparam,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatDlg();

	eTagMsgtype getCurType(){ return m_type; }
	std::string getCurParam(){ return m_inputParam; }

// Dialog Data
	enum { IDD = IDD_MESSAGE_DIALOG };

	void AddChatMessage(std::string Uid, LPCTSTR lpMessage);
	void ClearHistory();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV Support
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_edtChatBox;
	CFont m_ftBtn;

	CStringList m_strMsgList;
	int     m_nStreamID;
	eTagMsgtype m_type;
	std::string m_inputParam;
	int m_heightLine;
	int m_widthChar;
	std::string m_curAccount;
};
