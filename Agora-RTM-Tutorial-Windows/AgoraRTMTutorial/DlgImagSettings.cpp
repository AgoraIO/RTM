// CDlgImageSettings.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraRTMTutorial.h"
#include "DlgImageSettings.h"
#include "afxdialogex.h"


// CDlgImageSettings dialog

IMPLEMENT_DYNAMIC(CDlgImageSettings, CDialogEx)

CDlgImageSettings::CDlgImageSettings(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SETTINGS, pParent)
{

}

CDlgImageSettings::~CDlgImageSettings()
{
}

void CDlgImageSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtWidth);
    DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtHeight);
    DDX_Control(pDX, IDC_EDIT_THUMB_WIDTH, m_edtThumbWidth);
    DDX_Control(pDX, IDC_EDIT_THUMB_HEIGHT, m_edtThumbHeight);
}


BEGIN_MESSAGE_MAP(CDlgImageSettings, CDialogEx)
    
    ON_BN_CLICKED(IDOK, &CDlgImageSettings::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgImageSettings message handlers
BOOL CDlgImageSettings::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgImageSettings::OnBnClickedOk()
{
    CString str;
    m_edtWidth.GetWindowText(str);
    width  = _ttoi(str);
    m_edtHeight.GetWindowText(str);
    height = _ttoi(str);

    m_edtThumbWidth.GetWindowText(str);
    thumb_width  = _ttoi(str);
    m_edtThumbHeight.GetWindowText(str);
    thumb_height = _ttoi(str);
    CDialogEx::OnOK();
}
