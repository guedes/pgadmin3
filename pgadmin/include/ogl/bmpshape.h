/////////////////////////////////////////////////////////////////////////////
// Name:        bmpshape.h
// Purpose:     wxBitmapShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id: bmpshape.h,v 1.2 2007/02/02 15:56:37 anthemion Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_BITMAP_H_
#define _OGL_BITMAP_H_

#include "wx/bitmap.h"

class wxBitmapShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(wxBitmapShape)
public:
	wxBitmapShape();
	~wxBitmapShape();

	void OnDraw(wxDC &dc);

#if wxUSE_PROLOGIO
	// I/O
	void WriteAttributes(wxExpr *clause);
	void ReadAttributes(wxExpr *clause);
#endif

	// Does the copying for this object
	void Copy(wxShape &copy);

	void SetSize(double w, double h, bool recursive = TRUE);
	inline wxBitmap &GetBitmap() const
	{
		return (wxBitmap &) m_bitmap;
	}
	void SetBitmap(const wxBitmap &bm);
	inline void SetFilename(const wxString &f)
	{
		m_filename = f;
	};
	inline wxString GetFilename() const
	{
		return m_filename;
	}

private:
	wxBitmap      m_bitmap;
	wxString      m_filename;
};

#endif
// _OGL_BITMAP_H_

