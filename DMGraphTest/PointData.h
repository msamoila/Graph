/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		PointData.h: Declaration of the CPointData class
//
// CLASS NAME
//		CPointData
//
// DESCRIPTION
// 
// MODIFICATIONS
//  06-Nov-2011 		MS  Created

#if !defined(AFX_POINTDATA_H__63A9E2AA_3611_4415_AA9F_DD6EC42BEC9B__INCLUDED_)
#define AFX_POINTDATA_H__63A9E2AA_3611_4415_AA9F_DD6EC42BEC9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointData  
{
public:
	CPointData()
	{
		y = 0.;
	}
	~CPointData()
	{
	}

	double		x,	//it can be a double but it can be a date/time as well
				y;

	int GetText(int i, LPTSTR szText, int cbLen);
};

#endif // !defined(AFX_POINTDATA_H__63A9E2AA_3611_4415_AA9F_DD6EC42BEC9B__INCLUDED_)
