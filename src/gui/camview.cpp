////////////////////////////////////////////////////////////////////
// Name:		implementation of the CCamView class
// File:		camview.cpp
// Purpose:		eye/camera view/GUI system control methods
//
// Created by:	Larry Lart on 22-Feb-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
////////////////////////////////////////////////////////////////////

// system header
#include <math.h>
#include <stdio.h>

#include <sys/timeb.h>

#include "wx/wxprec.h"
#include <wx/image.h>


#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// other headers
#include "../wxopencv.h"
#include "../camera/camera.h"

// main header
#include "camview.h"

// implement message map
BEGIN_EVENT_TABLE(CCamView, wxWindow)
	EVT_PAINT( CCamView::OnPaint )
	EVT_SIZE( CCamView::OnSize ) 
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCamView
// Purose:	build my CCamView object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamView::CCamView( wxWindow *frame, const wxPoint& pos, const wxSize& size ):
			wxWindow(frame, -1, pos, size, wxSIMPLE_BORDER )
{
	m_pCamera = NULL;

	// set my canvas width/height
	m_nWidth = size.GetWidth( );
	m_nHeight = size.GetHeight( );

	m_bDrawing = false;

	m_bNewImage = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCamView
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamView::~CCamView( )
{
	m_pCamera = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Is Capture Enabled
// Class:	CCamView
// Purose:	check if camera is initialized
// Input:	nothing
// Output:	bool yes/no
////////////////////////////////////////////////////////////////////
bool CCamView::IsCaptureEnabled( )
{
//	return( m_pCamera->IsInitialized( ) );
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CCamView
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);
	Draw( dc );
}

double CCamView::GetTime( void )
{
	//
	struct timeb timeStamp;
	// get a time stamp
	ftime( &timeStamp );

	double nTime = (double) timeStamp.time*1000 + timeStamp.millitm;

	return( nTime );
	// i removed this for the moment - this was used to calculate time
	//based on the cpu tick count
	//return (double)cvGetTickCount()*1e-3/(m_nCpuFreq+1e-10);
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CCamView
// Purose:	camera drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::Draw( wxDC& dc )
{
	if( !dc.IsOk( )){ return; }

	int x, y, w, h;
	dc.GetClippingBox(&x, &y, &w, &h);

	imageMutex_.Lock();
	if(m_bNewImage && m_image.IsOk())
	{
		currentBitmap_ = wxBitmap(m_image.Scale(m_nWidth, m_nHeight));
	}
	else if (currentBitmap_.IsOk())
	{
		imageMutex_.Unlock();
		return;
	}

	m_bNewImage = false;
	imageMutex_.Unlock();

	//double m_timeCurrFrameStamp = GetTime();
	dc.DrawBitmap(currentBitmap_, x, y);
	//double delta = GetTime() - m_timeCurrFrameStamp;
	//std::cout << this << " draw: " << delta << std::endl;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnDraw
// Class:	CCamView
// Purose:	CCamView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::DrawCam( cv::Mat& pImg )
{
	if( pImg.empty()) { return; };

    cv::Mat pDstImg = pImg.clone();

	int nCamWidth = m_pCamera->m_nWidth;
	int nCamHeight = m_pCamera->m_nHeight;

	// draw a rectangle
	cv::rectangle(pDstImg,
				cvPoint( 10, 10 ),
				cvPoint( nCamWidth-20, nCamHeight-20 ),
				CV_RGB( 0,255,0 ), 1 );

	// convert data from raw image to wxImg
	wxImage tmpImage( nCamWidth, nCamHeight, pDstImg.data, true );
	imageMutex_.Lock();
	m_image = tmpImage.Copy();
	imageMutex_.Unlock();

	m_bNewImage = true;

	Refresh(false);
}

////////////////////////////////////////////////////////////////////
// Method:	CheckUpdate
// Class:	CCamView
// Purose:	CHeck for updates
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::CheckUpdate()
{
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CCamView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamView::OnSize( wxSizeEvent& event )
{
	int nWidth = event.GetSize().GetWidth();
	int nHeight = event.GetSize().GetHeight();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
}
