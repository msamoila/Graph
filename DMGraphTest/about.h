/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		about.h: Declaration of the CHyperlink and CAboutDlg classes
//
// CLASS NAMES
//		CHyperlink and CAboutDlg
//
// DESCRIPTION
// 
// MODIFICATIONS
//  06-Nov-2011 		MS  Created

#ifndef __ABOUT__H_
#define __ABOUT__H_

#include "resource.h"
#include "DMGraphTestVer.h"

class CHyperlink : public CWindowImpl<CHyperlink>
{
	HFONT			m_hFont;
	HCURSOR			m_hCursor;
	BOOL			m_bFirst;
	BOOL			m_bVisited;
public:

DECLARE_WND_SUPERCLASS(NULL, _T("Static"))

BEGIN_MSG_MAP(CHyperlink)
	MESSAGE_HANDLER(WM_LBUTTONDOWN,		OnLButtonDown)
	MESSAGE_HANDLER(WM_SETCURSOR,		OnSetCursor)
	MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColorStatic)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

	CHyperlink()
	{
		m_hFont = NULL;
		m_hCursor = ::LoadCursor(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDC_CURSOR_HAND));
		m_bFirst = TRUE;
		m_bVisited = FALSE;
	}
	~CHyperlink()
	{
		if(m_hFont)
			DeleteObject(m_hFont);
	}
	void CreateFont()
	{
		if(m_bFirst == FALSE)
			return;
		m_bFirst = FALSE;
		ATLASSERT(m_hFont == NULL);
		HFONT hFont = GetFont();
		LOGFONT lf;
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		lf.lfUnderline = TRUE;
		m_hFont = ::CreateFontIndirect(&lf);
	}
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		OLECHAR szTxt[128];
		LPCOLESTR szMailTo = L"mailto:";
		long i = wcslen(szMailTo);
		::GetWindowTextW(m_hWnd, szTxt+i, 121);
		if(wcschr(szTxt+i, '@'))
		{
			wcsncpy(szTxt, szMailTo, i);
			i = 0;
		}
		long code;
		code = (long)::ShellExecuteW(0, L"open", szTxt+i, 0, 0, SW_SHOWNORMAL);
		if(code <= 32)
		{
			wsprintfW(szTxt, L"Fail to execute associated application.\nCode: %#x", code);
			::MessageBoxW(m_hWnd, szTxt, APP_TITLE, MB_OK|MB_ICONSTOP);
		}
		else
			m_bVisited = TRUE;
		return 0;
	}
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CreateFont();
		HDC hdc = (HDC)wParam;
		::SelectObject(hdc, m_hFont);
		::SetTextColor(hdc, m_bVisited ? RGB(128, 0, 0) : RGB(0, 0, 255));
		::SetBkMode(hdc, TRANSPARENT);
		// return hollow brush to preserve parent background color
		return (LRESULT)::GetStockObject(HOLLOW_BRUSH);
	}
};

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
	CHyperlink m_staticMail, m_staticWeb, m_staticCopyrightUrl;
public:
	enum { IDD = IDD_ABOUTBOX };

protected:
BEGIN_MSG_MAP(CAboutDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnCancel)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_staticMail.SubclassWindow(GetDlgItem(IDC_AUTHOR));
		m_staticWeb.SubclassWindow(GetDlgItem(IDC_WEB));
		m_staticCopyrightUrl.SubclassWindow(GetDlgItem(IDC_COPYRIGHT_URL));

		TCHAR szVersion[128];
		wsprintf(szVersion, _T("DM Graph Test Version %d.%d.%d%d"), DM_GRAPH_TEST_VER);
		SetDlgItemText(IDC_VERSION, szVersion);
		return 1;  // Let the system set the focus
	}
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

};

#endif // __ABOUT__H_