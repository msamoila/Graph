/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		ToolBar.h: Declaration of the CToolbar class
//
// CLASS NAME
//		CToolbar
//
// DESCRIPTION
// 
// MODIFICATIONS
//  06-Nov-2011 		MS  Created

#ifndef _TOOLBAR_H__
#define _TOOLBAR_H__

#define RT_TOOLBAR  MAKEINTRESOURCE(241)

// RT_TOOLBAR resource
struct TOOLBARDATA {
   WORD wVersion;           // should be 1
   WORD wWidth;             // bitmap width
   WORD wHeight;            // height
   WORD wItemCount;         // num items
   WORD items[1];			// item IDs
};

class CToolbar : public CWindow
{

public:
	HWND Create(HWND hWndParent, UINT idCtrl)
	{
		if(!CWindow::Create(TOOLBARCLASSNAME, hWndParent, NULL, NULL,
				WS_CHILD|TBSTYLE_TOOLTIPS|WS_VISIBLE|WS_BORDER, 0, (HMENU)idCtrl))
			return NULL;
		SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON));
		return m_hWnd;
	}
	BOOL LoadToolbar(long nIDToolbar)
	{
		HRSRC hRsrc;
		TOOLBARDATA* ptbd;
		

		/*** load toolbar ***/
		if(!(hRsrc = FindResource(_Module.GetResourceInstance(),
				MAKEINTRESOURCE(nIDToolbar),
				RT_TOOLBAR)) ||
		   !(ptbd = (TOOLBARDATA*)LoadResource(_Module.GetResourceInstance(),
				hRsrc)))
		{
			ATLTRACE(_T("Can't load the toolbar %ld\r\n"), nIDToolbar);
			return FALSE;
		}
		ATLASSERT(ptbd->wVersion == 1);

		//add buttons
		TBBUTTON button;
		memset(&button, 0, sizeof(TBBUTTON));
		button.iString = -1;
	
		int iImage = 0;
		for (int i = 0; i < ptbd->wItemCount; i++)
		{
			button.fsState = TBSTATE_ENABLED;
			if ((button.idCommand = ptbd->items[i]) == 0)
			{	// separator
				button.fsStyle = TBSTYLE_SEP;
			}
			else
			{	// a command button with image
				button.fsStyle = TBSTYLE_BUTTON;
				button.iBitmap = iImage++;
			}
			if(!SendMessage(TB_ADDBUTTONS, 1, (LPARAM)&button))
				return FALSE;
		}
		
		// load bitmap
		TBADDBITMAP tbab;
		tbab.hInst = _Module.GetResourceInstance();
		tbab.nID = nIDToolbar;

		if(SendMessage(TB_ADDBITMAP, iImage, (LPARAM)&tbab) == -1)
			return FALSE;
		return TRUE;
	}
};

#endif // _TOOLBAR_H__
