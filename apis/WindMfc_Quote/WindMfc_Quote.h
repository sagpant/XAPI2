// WindMfc_Quote.h : main header file for the WindMfc_Quote DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWindMfc_QuoteApp
// See WindMfc_Quote.cpp for the implementation of this class
//

class CWindMfc_QuoteApp : public CWinApp
{
public:
	CWindMfc_QuoteApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
