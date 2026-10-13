/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		GraphDataDlg.h: Declaration of the CGraphDataDlg class
//
// CLASS NAME
//		CGraphDataDlg
//
// DESCRIPTION
// 
// MODIFICATIONS
//  06-Nov-2011 		MS  Created

#if !defined(AFX_GRAPHDATADLG_H__E4CF10E7_2D0F_4A91_9257_758E12AEC55C__INCLUDED_)
#define AFX_GRAPHDATADLG_H__E4CF10E7_2D0F_4A91_9257_758E12AEC55C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

class CMainWnd;
class CPointData;

class CGraphDataDlg : public CDialogImpl<CGraphDataDlg>  
{
	CMainWnd*	m_pMainWnd;
public:
	CGraphDataDlg(CMainWnd* pMainWnd)
	{
		ATLASSERT(pMainWnd);
		m_pMainWnd = pMainWnd;
	}
	~CGraphDataDlg()
	{ }

public:
	enum { IDD = IDD_GRAPH_DATA };

protected:
BEGIN_MSG_MAP(CAboutDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnSet)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	NOTIFY_HANDLER(IDC_LIST_POINTS, LVN_GETDISPINFO, OnGetdispinfo)
	NOTIFY_HANDLER(IDC_LIST_POINTS, LVN_DELETEITEM, OnDeleteitem)
	NOTIFY_HANDLER(IDC_LIST_POINTS, LVN_DELETEALLITEMS, OnDeleteallitems)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	COMMAND_HANDLER(IDC_GENERATE, BN_CLICKED, OnClickedGenerate)
	COMMAND_HANDLER(IDC_IMPORT, BN_CLICKED, OnClickedImport)
END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetdispinfo(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnDeleteallitems(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnDeleteitem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnSet(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedGenerate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		DestroyWindow();
		return 0;
	}
private:
	void FillListView(CPointData** parrPoints, int cbSize, LPCOLESTR szName);
};

#endif // !defined(AFX_GRAPHDATADLG_H__E4CF10E7_2D0F_4A91_9257_758E12AEC55C__INCLUDED_)
