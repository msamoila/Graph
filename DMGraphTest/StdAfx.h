// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_WARNINGS

#include <atlbase.h>

extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>

#include <Shellapi.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <stdio.h>
#include <math.h>

#define APP_TITLE L"DM Graph Test"

extern void Message(HRESULT hr, IUnknown* pUnk, LPCOLESTR szFormat, ...);

#import "..\DMGraph\DMGraph.tlb" raw_interfaces_only no_namespace named_guids

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
