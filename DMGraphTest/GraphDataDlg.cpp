/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		GraphDataDlg.cpp: implementation for the CGraphDataDlg class
//
// CLASS NAME
//		CGraphDataDlg
//
// DESCRIPTION
// 
// MODIFICATIONS
// 11-Dec-2011 MSamoila created
//

#include "stdafx.h"
#include "GraphDataDlg.h"
#include "PointData.h"
#include "MainWnd.h"

static void WINAPI FillCombo(HWND hWndCombo, UINT id, int idx)
{
	CComBSTR bsItems;
	bsItems.LoadString(id);
	LPOLESTR ptr;
	ATLASSERT(::IsWindow(hWndCombo));
	int j;
	for(ptr = wcstok(bsItems, L"\n"); ptr; ptr = wcstok(NULL, L"\n"))
	{
		j = ::SendMessageW(hWndCombo, CB_ADDSTRING, 0, (LPARAM)ptr);
		if(j == idx)
		{
			::SendMessageW(hWndCombo, CB_SETCURSEL, j, 0);
			::SetWindowTextW(hWndCombo, ptr);
		}
	}
}

LRESULT CGraphDataDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LVCOLUMN lvCol;
	lvCol.mask		= LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

	lvCol.cx		= 90;
	lvCol.pszText	= _T("X");
	lvCol.iSubItem	= 0;
	SendDlgItemMessage(IDC_LIST_POINTS, LVM_INSERTCOLUMN, 0, (LPARAM)&lvCol);

	lvCol.pszText	= _T("Y");
	lvCol.iSubItem	= 1;
	SendDlgItemMessage(IDC_LIST_POINTS, LVM_INSERTCOLUMN, 1, (LPARAM)&lvCol);

	SendDlgItemMessage(IDC_LIST_POINTS, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES|LVS_EX_LABELTIP,
		LVS_EX_GRIDLINES|LVS_EX_LABELTIP);

	FillCombo(GetDlgItem(IDC_FUNC), IDC_FUNC, 0);

	return 1;  // Let the system set the focus
}

void CGraphDataDlg::FillListView(CPointData** parrPoints, int cbSize, LPCOLESTR szName)
{
	ATLASSERT(parrPoints);


	LVITEM lvItem;
	lvItem.pszText		= LPSTR_TEXTCALLBACK;

	SendDlgItemMessage(IDC_LIST_POINTS, LVM_DELETEALLITEMS);
	int i;
	for(i=0; i<cbSize; i++)
	{
		lvItem.mask			= LVIF_TEXT|LVIF_PARAM;
		lvItem.iSubItem = 0;

		lvItem.iItem	= i;
		lvItem.lParam	= (LPARAM)parrPoints[i];	//ownership is transferred to list view which handles deletion
		SendDlgItemMessage(IDC_LIST_POINTS, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

		lvItem.mask			= LVIF_TEXT;

		lvItem.iSubItem = 1;
		SendDlgItemMessage(IDC_LIST_POINTS, LVM_SETITEM, 0, (LPARAM)&lvItem);
	}

	LVCOLUMN lvCol;
	USES_CONVERSION;
	lvCol.mask		= LVCF_SUBITEM | LVCF_TEXT;

	lvCol.pszText	= OLE2T(szName);
	lvCol.iSubItem	= 1;
	SendDlgItemMessage(IDC_LIST_POINTS, LVM_SETCOLUMN, 1, (LPARAM)&lvCol);
}

LRESULT CGraphDataDlg::OnClickedGenerate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	const int POINT_COUNT = 200;

	int i;
	CSimpleValArray<CPointData*> arrPoints;

	int funcType = SendDlgItemMessage(IDC_FUNC, CB_GETCURSEL);

	for(i=0; i<POINT_COUNT; i++)
	{
		CPointData* pItem = new CPointData;
		switch(funcType)
		{
		case 0:
			pItem->x = i / 10.;
			pItem->y = sin( pItem->x );
			break;
		case 1:
			pItem->x = i / 10.;
			pItem->y = cos( pItem->x );
			break;
		case 2:
			pItem->x = i / 10. + 0.1;
			pItem->y = 1. / pItem->x;
			break;
		}

		arrPoints.Add(pItem);
	}
	CComBSTR bsName;
	GetDlgItemText(IDC_FUNC, bsName.m_str);
	FillListView(arrPoints.GetData(), arrPoints.GetSize(), bsName); //this will take ownership of point items
	return 0;
}

LRESULT CGraphDataDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWndList = GetDlgItem(IDC_LIST_POINTS);
	const UINT btnIds[] = { IDOK, IDCANCEL, IDC_GENERATE, IDC_FUNC, IDC_IMPORT };
	const int nBtnCount = sizeof(btnIds)/sizeof(UINT);
	int i;

	HWND hWnd;
	RECT rc;
	long with;

	HDWP hdwp = BeginDeferWindowPos(6);
	
	for(i=0; i < nBtnCount; i++)
	{
		hWnd = GetDlgItem(btnIds[i]);

		::GetWindowRect(hWnd, &rc);
		ScreenToClient(&rc);
		with = rc.right - rc.left + 30;
		::DeferWindowPos(hdwp, hWnd, NULL, LOWORD(lParam) - (rc.right - rc.left) - 10, rc.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}


	::GetWindowRect(hWndList, &rc);
	ScreenToClient(&rc);
	::DeferWindowPos(hdwp, hWndList, NULL, 0, 0, LOWORD(lParam) - with, HIWORD(lParam) - rc.top - 10, SWP_NOMOVE|SWP_NOZORDER);

	EndDeferWindowPos(hdwp);
	return 0;
}

LRESULT CGraphDataDlg::OnClickedImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	FILE* file;
	
	OLECHAR szFileName[_MAX_PATH] = { 0 };
	OPENFILENAMEW ofn = {0};
	LPCWSTR szFilter = L"Comma separated values (*.CSV)\0*.CSV\0All files (*.*)\0*.*\0", defExt = L"CSV";
	static TCHAR szDir[_MAX_DIR] = { 0 };
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= m_hWnd;
	ofn.hInstance		= _Module.GetModuleInstance();
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= szFileName;
	ofn.nMaxFile		= _MAX_PATH;
	ofn.Flags			= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrDefExt		= defExt;

	if(GetOpenFileNameW(&ofn) == 0)
		return 0;

	file = _wfopen(szFileName, L"r+t");
	if(file == NULL)
	{
		Message(HRESULT_FROM_WIN32(_doserrno), NULL, L"Cannot open file\r\n%s\r\n", szFileName);
		return 0;
	}
	
	const long LEN = 1024;
	LPOLESTR szBuffer = (LPOLESTR)malloc( LEN * sizeof(OLECHAR) );
	if(szBuffer == NULL)
	{
		fclose(file);
		return 0;
	}

	CSimpleValArray<CPointData*> arrPoints;

	long nLineLen;
	for(; fgetws(szBuffer, LEN, file);)
	{
		nLineLen = (long)wcslen( szBuffer );
		if(nLineLen == 0)
			continue;

		if(szBuffer[nLineLen-1] == '\n')
		{
			szBuffer[nLineLen-1] = '\0';
			if(nLineLen < 2)
				continue;

			CPointData* pItem = new CPointData;

			//Xval, YVal
			LPOLESTR ptr, ptr2;
			int i;
			for(ptr = wcstok(szBuffer, L"\n\r"), i=0; ptr; ptr = wcsstr(ptr+1, L","), i++)
			{
				USES_CONVERSION;
				ptr2 = wcsstr(ptr+1, L",");
				if(ptr2)
					*ptr2 = '\0';
				switch(i)
				{
				case 0:
					pItem->x = atof(W2A(ptr));
					break;
				case 1:
					pItem->y = atof(W2A(ptr+1));
					break;
				default:
					continue; //error
				}
				if(ptr2)
					*ptr2 = ','; //restore
			}

			arrPoints.Add(pItem);
		}
	}

	free(szBuffer);
	fclose(file);

	LPOLESTR szName = PathFindFileNameW(szFileName);
	PathRemoveExtensionW(szName);
	FillListView(arrPoints.GetData(), arrPoints.GetSize(), szName);
	return 0;
}


LRESULT CGraphDataDlg::OnSet(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long i, nCount = SendDlgItemMessage(IDC_LIST_POINTS, LVM_GETITEMCOUNT);
	
	LVITEM lvItem;
	lvItem.mask			= LVIF_PARAM;
	lvItem.iSubItem = 0;

	CComVariant varrX, varrY;
	varrX.parray = SafeArrayCreateVector(VT_R8, 0, nCount);
	if(varrX.parray == NULL)
		return 0;
	varrX.vt = VT_ARRAY|VT_R8;

	varrY.parray = SafeArrayCreateVector(VT_R8, 0, nCount);
	if(varrY.parray == NULL)
		return 0;
	varrY.vt = VT_ARRAY|VT_R8;

	HRESULT hr;
	for(i=0; i<nCount; i++)
	{
		lvItem.iItem	= i;
		SendDlgItemMessage(IDC_LIST_POINTS, LVM_GETITEM, 0, (LPARAM)&lvItem);

		CPointData* pItem = (CPointData*)lvItem.lParam;
		ATLASSERT(pItem);

		hr = SafeArrayPutElement(varrX.parray, &i, &pItem->x);
		hr = SafeArrayPutElement(varrY.parray, &i, &pItem->y);

	}

	LVCOLUMN lvCol;
	TCHAR szColText[128];
	USES_CONVERSION;
	lvCol.mask		= LVCF_SUBITEM | LVCF_TEXT;

	lvCol.pszText	= szColText;
	lvCol.cchTextMax = 127;
	lvCol.iSubItem	= 1;
	SendDlgItemMessage(IDC_LIST_POINTS, LVM_GETCOLUMN, 1, (LPARAM)&lvCol);

	m_pMainWnd->SetGraphData(&varrX, &varrY, szColText);

	return 0;
}

LRESULT CGraphDataDlg::OnGetdispinfo(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMLVDISPINFO* pHmLv = (NMLVDISPINFO*)pnmh;
	if(pHmLv->item.mask & LVIF_TEXT)
	{
		CPointData* pItem = (CPointData*)pHmLv->item.lParam;
		ATLASSERT(pItem);

		pItem->GetText(pHmLv->item.iSubItem, pHmLv->item.pszText, pHmLv->item.cchTextMax);
	}
	return 0;
}

LRESULT CGraphDataDlg::OnDeleteallitems(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	return FALSE;
}

LRESULT CGraphDataDlg::OnDeleteitem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMLISTVIEW* nmLv = (NMLISTVIEW*)pnmh;
	LVITEM lvItem;
	lvItem.iItem = nmLv->iItem;
	lvItem.iSubItem = 0;
	lvItem.mask = LVIF_PARAM;
	SendDlgItemMessage(IDC_LIST_POINTS, LVM_GETITEM, 0, (LPARAM)&lvItem);

	CPointData* pItem = (CPointData*)lvItem.lParam;
	delete pItem;

	return 0;
}

