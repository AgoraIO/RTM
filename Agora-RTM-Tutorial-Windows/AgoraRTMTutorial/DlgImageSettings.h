#pragma once


// CDlgImageSettings dialog

class CDlgImageSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImageSettings)

public:
    CDlgImageSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgImageSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETTINGS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_edtWidth;
    CEdit m_edtHeight;
    CEdit m_edtThumbWidth;
    CEdit m_edtThumbHeight;
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();

    int width  = 0;
    int height = 0;
    int thumb_width  = 0;
    int thumb_height = 0;
};
