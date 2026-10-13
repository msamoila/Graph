/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		MainWnd.h: Declaration of the CMainWnd class
//
// CLASS NAME
//		CMainWnd
//
// DESCRIPTION
// 
// MODIFICATIONS
//  06-Nov-2011 		MS  Created

#ifndef __MAINDLG_H_
#define __MAINDLG_H_

#include "resource.h"       // main symbols
#include "Toolbar.h"

class CGraphDataDlg;

extern _ATL_FUNC_INFO CursorPositionInfo;
extern _ATL_FUNC_INFO TrackModeChangedInfo;
extern _ATL_FUNC_INFO ClickInfo;

class CMainWnd;
typedef IDispEventSimpleImpl<1, CMainWnd, &__uuidof(_IDMGraphCtrlEvents)> IDMGraphCtrlEventsImpl;

/////////////////////////////////////////////////////////////////////////////
// CMainWnd
class CMainWnd : 
	public CWindowImpl<CMainWnd>,
	public IDMGraphCtrlEventsImpl
{
public:
	CMainWnd();
	~CMainWnd();


DECLARE_WND_CLASS(_T("DMGraphTest_wnd_class"))

BEGIN_MSG_MAP(CMainWnd)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	COMMAND_ID_HANDLER(ID_GRAPH_PROPERTIES, OnGraphProperties)
	COMMAND_ID_HANDLER(ID_GRAPH_INPUT_DATA, OnGraphInputData)
	COMMAND_ID_HANDLER(ID_HELP_ABOUT, OnAbout)
	COMMAND_ID_HANDLER(ID_HELP_ABOUT_DM_GRAPH, OnAboutDmGraph)
	COMMAND_ID_HANDLER(ID_ZOOM, OnZoom)
	COMMAND_ID_HANDLER(ID_PAN, OnPan)
	COMMAND_ID_HANDLER(ID_INFO, OnInfo)
	COMMAND_ID_HANDLER(ID_AUTO_RANGE, OnAutoRange)
	COMMAND_ID_HANDLER(ID_SAVE, OnSave)
	COMMAND_ID_HANDLER(ID_PRINT, OnPrint)
	MESSAGE_HANDLER(WM_INITMENU, OnInitMenu)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
END_MSG_MAP()

BEGIN_SINK_MAP(CMainWnd)
	SINK_ENTRY_INFO(1, __uuidof(_IDMGraphCtrlEvents), /*dispid*/ 1, OnCursorPosition, &CursorPositionInfo)
	SINK_ENTRY_INFO(1, __uuidof(_IDMGraphCtrlEvents), /*dispid*/ 2, OnTrackModeChanged, &TrackModeChangedInfo)
	SINK_ENTRY_INFO(1, __uuidof(_IDMGraphCtrlEvents), /*dispid*/ DISPID_CLICK, OnClick, &ClickInfo)
END_SINK_MAP()

protected:
	//_IDMGraphCtrlEvents
	void __stdcall OnCursorPosition(double x, double y);
	void __stdcall OnTrackModeChanged(TrackModeState lNewState);
	void __stdcall OnClick();

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnZoom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAutoRange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnGraphProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnGraphInputData(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAboutDmGraph(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnFinalMessage(HWND)
	{
		PostQuitMessage(0);
	}

public:
	BOOL PreTranslateMessage(const MSG* pMsg)
	{
		return FALSE;
	}

	void SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName);

private:
	void UpdateToolbarState();

	CAxWindow*				m_pGraphCtrl;
	CComPtr<IDMGraphCtrl>	m_spDMGraph;

	CGraphDataDlg*			m_pGraphDataDlg;
	
	CToolbar				m_wndToolbar;
	CWindow					m_wndStatusBar;
	BOOL					m_DisableEvents;
	UINT_PTR				m_nTimerID;
};

#endif //__MAINDLG_H_
