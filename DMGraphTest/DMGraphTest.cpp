/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
// DMGraphTest.cpp : Defines the entry point for the application.
//
// MODIFICATIONS
// 11-Dec-2011 MSamoila created

#include "stdafx.h"
#include "MainWnd.h"

CComModule _Module;

CMainWnd* g_pMainWnd = NULL;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	INITCOMMONCONTROLSEX cc;
	cc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cc.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES ;
	InitCommonControlsEx(&cc);

	_Module.Init(NULL, hInstance);

	HRESULT hr = CoInitialize(NULL);

	RECT rc;

	rc.left = CW_USEDEFAULT;
	rc.top = CW_USEDEFAULT;
	rc.right = 0;
	rc.bottom = 0;

	WNDCLASSEX* pwndInfo = &CMainWnd::GetWndClassInfo().m_wc;
	pwndInfo->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
	pwndInfo->lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
	pwndInfo->hIconSm = pwndInfo->hIcon;

	g_pMainWnd = new CMainWnd;

	if(!g_pMainWnd->Create(NULL, rc, _T("DM Graph Test"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, WS_EX_ACCEPTFILES|WS_EX_CLIENTEDGE))
	{
		ATLTRACE(_T("Could not create the main window %#x\r\n"), HRESULT_FROM_WIN32(GetLastError()));
		goto $end;
	}

	HACCEL hAccelTable;
	MSG msg;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN));

	g_pMainWnd->ShowWindow(nCmdShow);
	g_pMainWnd->UpdateWindow();


	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(g_pMainWnd->PreTranslateMessage(&msg))
			continue;
		if (!TranslateAccelerator(g_pMainWnd->m_hWnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

$end:

	delete g_pMainWnd;
	g_pMainWnd = NULL;

	CoUninitialize();

	_Module.Term();

	return 0;
}



void Message(HRESULT hr, IUnknown* pUnk, LPCOLESTR szFormat, ...)
{
	ATLASSERT(szFormat);
	int		i, len = wcslen(szFormat)+1000;
	//assume that arguments need max 1000 chars
	LPOLESTR pBuffer;
	pBuffer = (LPOLESTR)malloc(len*sizeof(OLECHAR));
	if(pBuffer == NULL)
		return;

	va_list args;
	va_start( args, szFormat );
	i = _vsnwprintf(pBuffer, len, szFormat,  args);
	va_end(args);

	CComBSTR bsDescr;
	if(pUnk)
	{
		CComPtr<ISupportErrorInfo> spSupportErrorInfo;
		HRESULT hr = pUnk->QueryInterface(IID_ISupportErrorInfo, (void**)&spSupportErrorInfo);
		if(SUCCEEDED(hr))
		{
			CComPtr<IErrorInfo> spErrorInfo;
			hr = ::GetErrorInfo(0, &spErrorInfo);
			if(SUCCEEDED(hr) && spErrorInfo)
			{
				hr = spErrorInfo->GetDescription(&bsDescr);

				if(bsDescr.Length())
				{
					i += _snwprintf(pBuffer+i, len-i, L" %s ", bsDescr);
				}
			}
		}
	}

	if(hr != S_OK)
	{
		LPVOID lpMsgBuf;
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPOLESTR) &lpMsgBuf,
			0,
			NULL );// Process any inserts in lpMsgBuf.

		_snwprintf(pBuffer+i, len-i, L"\n[%#x] %s", hr, (LPCOLESTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
	}

	::OutputDebugStringW(pBuffer);
	::OutputDebugStringW(L"\n");
	::MessageBoxW(g_pMainWnd ? g_pMainWnd->m_hWnd : NULL, pBuffer, APP_TITLE, MB_OK|MB_ICONINFORMATION|MB_TOPMOST);

	free(pBuffer);
}

