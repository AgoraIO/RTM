
// AgoraRTMTutorial.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"


// CAgoraRTMTutorialApp: 
// AgoraRTMTutorial.cpp
//

class CAgoraRTMTutorialApp : public CWinApp
{
public:
	CAgoraRTMTutorialApp();

// override
public:
	virtual BOOL InitInstance();

// Implement

	DECLARE_MESSAGE_MAP()
};

extern CAgoraRTMTutorialApp theApp;