/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		MainWnd.cpp: implementation for the CMainWnd class
//
// CLASS NAME
//		CMainWnd
//
// DESCRIPTION
// 
// MODIFICATIONS
// 11-Dec-2011 MSamoila created

#include "stdafx.h"
#include "MainWnd.h"
#include "GraphDataDlg.h"
#include "About.h"

#define IDC_CHART			1
#define IDC_TOOL_BAR		2
#define IDC_STATUS_BAR		3

_ATL_FUNC_INFO CursorPositionInfo		= { CC_STDCALL, VT_EMPTY, 2, { VT_R8, VT_R8 }};
_ATL_FUNC_INFO TrackModeChangedInfo		= { CC_STDCALL, VT_EMPTY, 1, { VT_I4 }};
_ATL_FUNC_INFO ClickInfo				= { CC_STDCALL, VT_EMPTY, 0};

/////////////////////////////////////////////////////////////////////////////
// CMainWnd

CMainWnd::CMainWnd()
{
	m_pGraphDataDlg = NULL;
	m_pGraphCtrl = NULL;
	m_DisableEvents = FALSE;
	m_nTimerID = 0;
}

CMainWnd::~CMainWnd()
{
	ATLASSERT(m_pGraphCtrl == NULL);
	ATLASSERT(m_pGraphDataDlg == NULL);
}


LRESULT CMainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pGraphCtrl = new CAxWindow;
	if(m_pGraphCtrl == NULL)
		return -1;

	RECT rc;
	GetClientRect(&rc);

	if(!AtlAxWinInit())
		return -1;

	////////////////////////////////
	//create the tool bar

	if( !m_wndToolbar.Create(m_hWnd, IDC_TOOL_BAR) ||
		!m_wndToolbar.LoadToolbar(IDR_MAIN))
	{
		ATLTRACE(_T("Fail to create the toolbar\r\n"));
		return -1;
	}

	//create the status bar
	if(!m_wndStatusBar.Create( STATUSCLASSNAME, m_hWnd, NULL, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0,
		(HMENU)IDC_STATUS_BAR))
		return -1;

	int aWidth = -1;
	m_wndStatusBar.SendMessage(SB_SETPARTS, 1, (LPARAM)&aWidth);

	if(!m_pGraphCtrl->Create(m_hWnd, rc, NULL, WS_CHILD|WS_VISIBLE, 0, IDC_CHART))
		return -1;

	HRESULT hr;
	CComPtr<IAxWinHostWindow> spHost;
	hr = m_pGraphCtrl->QueryHost(IID_IAxWinHostWindow, (void**)&spHost);
	if(FAILED(hr))
	{
		Message(hr, NULL, L"Cannot query Ax host");
		return -1;
	}
	hr = spHost->CreateControl(L"DMGraph.DMGraphCtrl", m_pGraphCtrl->m_hWnd, NULL);
	if(FAILED(hr))
	{
		Message(hr, NULL, L"Cannot start DM Graph control");
		return -1;
	}

	CComVariant vData;
	hr = m_pGraphCtrl->QueryControl(IID_IDMGraphCtrl, (void**)&m_spDMGraph);
	if(FAILED(hr) || m_spDMGraph == NULL)
	{
		Message(hr, NULL, L"Cannot query DM Graph control");
		return -1;
	}

	hr = DispEventAdvise(m_spDMGraph, &__uuidof(_IDMGraphCtrlEvents));
	if(FAILED(hr))
	{	//non critical error
		Message(hr, m_spDMGraph, L"Cannot advise for events from DM Graph control");
	}

	CComPtr<IDMGraphAxis> spXAxis;
	hr = m_spDMGraph->get_Axis(HorizontalX, &spXAxis);
	if(FAILED(hr))
		return -1;
	hr = spXAxis->put_Format( CComBSTR(L"%d/%b %H:%M:%S") );
	hr = spXAxis->put_Time( VARIANT_TRUE );
	if(FAILED(hr))
		return -1;

	hr = m_spDMGraph->put_LabelColor( RGB( 0, 0, 0) );


	m_pGraphDataDlg = new CGraphDataDlg(this);
	if(m_pGraphDataDlg == NULL)
		return -1;

	return 1;

}

void CMainWnd::UpdateToolbarState()
{
	if(m_spDMGraph == NULL)
		return;
	
	HRESULT hr;
	TrackModeState eTrackMode;
	hr = m_spDMGraph->get_TrackMode(&eTrackMode);
	if(FAILED(hr))
		return;

	TBBUTTONINFO tbBtn;
	tbBtn.cbSize = sizeof(TBBUTTONINFO);
	tbBtn.dwMask = TBIF_STATE;
	tbBtn.fsState = TBSTATE_ENABLED | TBSTATE_CHECKED;

	switch(eTrackMode)
	{
	case ZoomX:
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_ZOOM, (LPARAM)&tbBtn);
		tbBtn.fsState = TBSTATE_ENABLED;
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_PAN, (LPARAM)&tbBtn);
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_INFO, (LPARAM)&tbBtn);
		break;
	case PanX:
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_PAN, (LPARAM)&tbBtn);
		tbBtn.fsState = TBSTATE_ENABLED;
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_ZOOM, (LPARAM)&tbBtn);
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_INFO, (LPARAM)&tbBtn);
		break;
	case TrackElement:
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_INFO, (LPARAM)&tbBtn);
		tbBtn.fsState = TBSTATE_ENABLED;
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_PAN, (LPARAM)&tbBtn);
		m_wndToolbar.SendMessage(TB_SETBUTTONINFO, ID_ZOOM, (LPARAM)&tbBtn);
		break;
	}
}


LRESULT CMainWnd::OnZoom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;
	HRESULT hr;
	hr = m_spDMGraph->put_TrackMode( ZoomX );

	//this command handler may come from menu as well - in this case we need to update the toolbar as well
	UpdateToolbarState();
	return 0;
}

LRESULT CMainWnd::OnPan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;
	HRESULT hr;
	hr = m_spDMGraph->put_TrackMode( PanX );

	//this command handler may come from menu as well - in this case we need to update the toolbar as well
	UpdateToolbarState();
	return 0;
}

LRESULT CMainWnd::OnInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;
	HRESULT hr;
	hr = m_spDMGraph->put_TrackMode( TrackElement );

	//this command handler may come from menu as well - in this case we need to update the toolbar as well
	UpdateToolbarState();
	return 0;
}

//_IDMGraphCtrlEvents
void __stdcall CMainWnd::OnCursorPosition(double x, double y)
{
	ATLASSERT(m_wndStatusBar.IsWindow());
	if(m_DisableEvents)
		return;
	const int TXT_LEN = 128;
	OLECHAR szStatus[ TXT_LEN ];
	_snwprintf(szStatus, TXT_LEN, L"x %g y %g", x, y);
	::SendMessageW(m_wndStatusBar.m_hWnd, SB_SETTEXTW, 0, (LPARAM)szStatus);
}

void __stdcall CMainWnd::OnTrackModeChanged(TrackModeState lNewState)
{
	ATLASSERT(m_wndStatusBar.IsWindow());
	if(m_DisableEvents)
		return;
	const int TXT_LEN = 128;
	OLECHAR szStatus[ TXT_LEN ];

	LPCOLESTR szTrackMode[] = { L"None", L"Track cursor", L"Track element", L"Zoom", L"ZoomX", L"ZoomY", L"PanXY", L"PanX", L"PanY" };
	ATLASSERT(lNewState >=0 && lNewState < sizeof(szTrackMode)/sizeof(LPCOLESTR));
	_snwprintf(szStatus, TXT_LEN, L"Track mode changed to %s", szTrackMode[ lNewState ]);
	::SendMessageW(m_wndStatusBar.m_hWnd, SB_SETTEXTW, 0, (LPARAM)szStatus);
}

void __stdcall CMainWnd::OnClick()
{
	ATLTRACE(_T("OnClick\n"));
}

LRESULT CMainWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_wndStatusBar.SendMessage(SB_SETTEXT);
	KillTimer(m_nTimerID);
	return 0;
}

LRESULT CMainWnd::OnAutoRange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;
	HRESULT hr;
	hr = m_spDMGraph->AutoRange();
	return 0;
}

LRESULT CMainWnd::OnGraphProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HRESULT hr = m_spDMGraph->ShowProperties();
	if(FAILED(hr))
	{
		Message(hr, m_spDMGraph, NULL);
		return 0;
	}

	return 0;
}

LRESULT CMainWnd::OnGraphInputData(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_pGraphDataDlg->IsWindow())
	{
		m_pGraphDataDlg->BringWindowToTop();
	}
	else
		m_pGraphDataDlg->Create(m_hWnd);
	return 0;
}

void CMainWnd::SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName)
{
	ATLASSERT(pvarrX);
	ATLASSERT(pvarrY);
	ATLASSERT(szName);

	CComBSTR bsName(szName);

	CComPtr<IDMGraphCollection> spElements;
	CComPtr<IDMGraphElement> spGraphElement;
	HRESULT hr = m_spDMGraph->get_Elements(&spElements);

	long i, nElementCount = 0;
	BOOL bReplace = FALSE;
	hr = spElements->get_Count(&nElementCount);
	for(i=0; i<nElementCount; i++)
	{
		CComBSTR bsElemName;
		CComPtr<IDispatch> spDispatch;

		hr = spElements->get_Item(i, &spDispatch);
		hr = spDispatch.QueryInterface(&spGraphElement);

		spGraphElement->get_Name(&bsElemName);
		if(_wcsicmp(bsElemName, bsName) == 0)
		{
			OLECHAR szMsgText[256];
			_snwprintf(szMsgText, 256, L"There is ALREADY an element named '%s'.\nDo you want to replace it ?", bsElemName);
			if(::MessageBoxW(m_hWnd, szMsgText, NULL, MB_YESNO|MB_ICONQUESTION) != IDYES)
				return;
			bReplace = TRUE;
			break;
		}
		else
			spGraphElement = NULL;
	}
	if(bReplace == FALSE || spGraphElement == NULL)
	{
		CComPtr<IDispatch> spDispatch;
		hr = spElements->Add(&spDispatch);
		spGraphElement = NULL;
		hr = spDispatch.QueryInterface(&spGraphElement);
	}

	hr = spGraphElement->put_Name(bsName);

	hr = spGraphElement->put_PointSymbol( Dots );
	hr = spGraphElement->put_PointSize(3);
	hr = spGraphElement->Plot(*pvarrX, *pvarrY);
	if(FAILED(hr))
	{
		Message(hr, spGraphElement, L"Failed to plot items");
		return;
	}

	hr = m_spDMGraph->AutoRange();
}

LRESULT CMainWnd::OnInitMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;
	HRESULT hr;
	TrackModeState eTrackMode;
	hr = m_spDMGraph->get_TrackMode(&eTrackMode);
	if(FAILED(hr))
		return 0;
	HMENU hMenu = (HMENU)wParam;
	UINT nCheckedID;
	switch(eTrackMode)
	{
	case ZoomX:
		nCheckedID = ID_ZOOM;
		break;
	case PanX:
		nCheckedID = ID_PAN;
		break;
	case TrackElement:
		nCheckedID = ID_INFO;
		break;
	default:
		nCheckedID = 0;
	}
	CheckMenuRadioItem(hMenu, ID_ZOOM, ID_INFO, nCheckedID, MF_BYCOMMAND);

	return 0;
}

LRESULT CMainWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_wndStatusBar.m_hWnd)
	{
		m_wndStatusBar.SetWindowPos(NULL, 0, 0,
			LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
		RECT rcStatus;
		m_wndStatusBar.GetWindowRect(&rcStatus);

		RECT rcToolbar;
		m_wndToolbar.GetWindowRect(&rcToolbar);
		long cyToolbar = 0;

		if(m_wndToolbar.m_hWnd)
		{
			cyToolbar = rcToolbar.bottom-rcToolbar.top;
			m_wndToolbar.SetWindowPos(NULL, 0, 0, LOWORD(lParam), cyToolbar,
					  SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
		}
		if(m_pGraphCtrl)
		{
			m_pGraphCtrl->SetWindowPos(NULL, 0, cyToolbar, LOWORD(lParam), 
				HIWORD(lParam) - (rcStatus.bottom - rcStatus.top) - cyToolbar, SWP_NOZORDER|SWP_NOACTIVATE);
		}
	}
	return 0;
}

LRESULT CMainWnd::OnSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAMEW of = { 0 };
	OLECHAR szFile[_MAX_PATH] = { 0 };
	of.lStructSize = sizeof(OPENFILENAMEW);
	of.hwndOwner = m_hWnd;
	of.lpstrFilter = L"Bitmap File (*.BMP)\0*.BMP\0All files (*.*)\0*.*\0";
	of.lpstrFile = szFile;
	of.nMaxFile = _MAX_PATH;
	of.Flags = OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = L"BMP";

	if(GetSaveFileNameW(&of) == 0)
		return 0;

	CComBSTR bsFile(szFile);
	HRESULT hr = m_spDMGraph->SaveAs(bsFile);
	if(FAILED(hr))
	{
		Message(hr, m_spDMGraph, L"Cannot save in:\n%s", szFile);
		return 0;
	}
	return 0;
}

LRESULT CMainWnd::OnPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HRESULT hr = m_spDMGraph->PrintGraph();
	if(FAILED(hr))
	{
		Message(hr, m_spDMGraph, L"Cannot print");
		return 0;
	}
	return 0;
}


LRESULT CMainWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_spDMGraph)
	{
		HRESULT hr;
		hr = DispEventUnadvise(m_spDMGraph, &__uuidof(_IDMGraphCtrlEvents));
		if(FAILED(hr))
		{
			ATLTRACE(_T("Failed to unadvise hr %#x\n"), hr);
		}

		m_spDMGraph = NULL;
	}
	if(m_pGraphCtrl)
	{
		delete m_pGraphCtrl;
		m_pGraphCtrl = NULL;
	}

	if(m_pGraphDataDlg)
	{
		if(m_pGraphDataDlg->IsWindow())
			m_pGraphDataDlg->DestroyWindow();

		delete m_pGraphDataDlg;
		m_pGraphDataDlg = NULL;
	}

	return 0;
}


LRESULT CMainWnd::OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAboutDlg dlg;
	dlg.DoModal(m_hWnd);
	return 0;
}

LRESULT CMainWnd::OnAboutDmGraph(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_spDMGraph == NULL)
		return 0;

	HRESULT hr = m_spDMGraph->AboutBox();
	return 0;
}
