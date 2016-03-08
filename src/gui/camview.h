////////////////////////////////////////////////////////////////////
// Name:		camera view header
// File:		camview.h
// Purpose:		interface for the CCamView class.
//
// Created by:	Larry Lart on 06-Jul-2006
// Updated by:	
//
////////////////////////////////////////////////////////////////////

#ifndef _CCAMVIEW_H
#define _CCAMVIEW_H

#include <opencv2/imgproc/imgproc.hpp>

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// external classes
class CCamera;

class CCamView : public wxWindow
{
public: 
	CCamView( wxWindow *frame, const wxPoint& pos, const wxSize& size );

// Operations
public:


// Implementation
public:
	virtual ~CCamView( );
	bool IsCaptureEnabled( );
	void CheckUpdate( );

	// Draw method
	void DrawCam( cv::Mat &pImg );
	void Draw( wxDC& dc );

// Public data
public:
	CCamera *m_pCamera;

// Protected data
protected:
    //CImage	m_Canvas;
	wxBitmap	m_pBitmap;
	wxImage m_image;

  	bool	m_bDrawing;
	bool	m_bNewImage;

	int m_nWidth;
	int m_nHeight;

// private methods
private:
	void	OnPaint(wxPaintEvent& event);
	void	OnSize( wxSizeEvent& even );
	double 	GetTime( void );
	double		m_timePrevFrameStamp;
	double		m_timeCurrFrameStamp;
	wxBitmap currentBitmap_;
	wxMutex  imageMutex_;

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

#endif

