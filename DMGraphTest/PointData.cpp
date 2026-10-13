/////////////////////////////////////////////////////////////////////////////
// (c) Copyright The Code Project Open License (CPOL)
//  http://www.codeproject.com/info/cpol10.aspx
//
//  Marius Samoila 2011
//
// FILE NAME
//		PointData.cpp: implementation for the CPointData class
//
// CLASS NAME
//		CPointData
//
// DESCRIPTION
// 
// MODIFICATIONS
// 11-Dec-2011 MSamoila created
//

#include "stdafx.h"
#include "PointData.h"

int CPointData::GetText(int i, LPTSTR szText, int cbLen)
{
	USES_CONVERSION;

	switch(i)
	{
	case 0:
		return _sntprintf(szText, cbLen, _T("%g"), x);
	case 1:
		return _sntprintf(szText, cbLen, _T("%g"), y);
	default:
		return 0;
	}
}
