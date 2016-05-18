////////////////////////////////////////////////////////////////////
// Name:		implementation of the CCamera class 
// File:		camera.cpp
// Purpose:		camera system control methods
//
// Created by:	Larry Lart based in various samples by OpenCV team
// Updated by:	Larry Lart on 22-Feb-2004
//				Larry Lart on 24-Feb-2004
//				Larry Lart on 18-Mar-2005
//
// Copyright:	(c) 2004-2005 Larry Lart
////////////////////////////////////////////////////////////////////

#define _GUI_RUN 1

// system includes

// opencv
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>

// other local includes
#include "../wxopencv.h"
#include "../worker.h"

// GUI include
#ifdef _GUI_RUN
#include "../gui/camview.h"
#include "../gui/frame.h"
#endif

// Main header
#include "camera.h"

#ifndef WIN32_LARRY
        void testcallback(void* _image);
        IplImage* m_pLnxFrame;
#endif
                                                                                                                             
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCamera
// Purose:	build my Camera object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamera::CCamera(  )
{
	m_pWorker = NULL;
	m_isRunning = false;
	m_isPause = false;
	// the new camera implementation
	m_pVideoImg = NULL;
	m_nFps = -1;
	m_nFpsAlpha = 0.1;
	m_isAvi = false;
	
	m_bIsChange = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCamera
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCamera::~CCamera( )
{
	m_pVideoImg = NULL;
	m_pWorker = NULL;
	m_pCameraView = NULL;
	m_pFrame = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CCamera
// Purose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::Init(  )
{
	m_timeCurrFrameStamp = m_pWorker->GetTime( );
	m_timePrevFrameStamp = m_timeCurrFrameStamp;
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetSize
// Class:	CCamera
// Purose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::GetSize( )
{
	// now get my properties
	m_nWidth = (int)m_Capture.get(cv::CAP_PROP_FRAME_WIDTH);
	m_nHeight = (int)m_Capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	// set camview size
	m_pCameraView->SetSize( m_nWidth, m_nHeight );
	m_pFrame->ResetLayout( );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	Start
// Class:	CCamera
// Purose:	Start capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::Start( )
{
	m_Capture.open(0);

	cv::Mat pFrame;
	// grab first frame to initialize format
	m_Capture.read(pFrame);
	// get camera's size
	GetSize( );

#if 0
	int ncameras = cvcamGetCamerasCount();
	printf("DEBUG :: Found cameras=%d\n", ncameras );
	int w = 640;
	int h = 480;
	int nCam = 0;
	int t=1;
	int p=1;
	
	cvcamSetProperty(nCam, CVCAM_RNDWIDTH,  &w );
	cvcamSetProperty(nCam, CVCAM_RNDHEIGHT, &h );

	cvcamSetProperty(nCam, CVCAM_PROP_ENABLE, &t );

	int width = 320;
	int height = 200;


	HWND MyWin = (HWND)m_pCameraView->GetHandle();
	cvcamSetProperty(nCam, CVCAM_PROP_WINDOW, &MyWin ); 
	cvcamSetProperty(nCam, CVCAM_PROP_RENDER, &p );  

	
	cvcamSetProperty(0, CVCAM_PROP_CALLBACK, (void*)testcallback );
	cvcamInit();
	Sleep( 5000 );
	cvcamStart();
#endif	

	m_isRunning = true;
}

////////////////////////////////////////////////////////////////////
// Method:	PauseResume
// Class:	CCamera
// Puprose:	pause/resume capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::PauseResume( )
{
	if( m_isPause )
	{
		m_isPause = false;
	} 
	else
	{
		m_isPause = true;
	}
}

////////////////////////////////////////////////////////////////////
// Method:	IsChanged
// Class:	CCamera
// Puprose:	mark flag change
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCamera::IsChanged( )
{
	m_bIsChange = 1;
	m_isPause = true;
}

////////////////////////////////////////////////////////////////////
// Method:	Stop
// Class:	CCamera
// Purose:	Stop capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void  CCamera::Stop( )
{
	m_isRunning = false;

	m_Capture.release();
	if( !m_pVideoImg.empty() )
	{
		m_pVideoImg.release();
	}
}

////////////////////////////////////////////////////////////////////
// feedback - only for linux
#ifndef WIN32_LARRY
void testcallback(void* _image)
{
	IplImage* image = (IplImage*)_image;
	cvLine(image, cvPoint(0, 0), cvPoint(image->width, image->height),CV_RGB(255, 0, 0), 1);
	m_pLnxFrame = image ;
}
#endif

////////////////////////////////////////////////////////////////////
// Method:	Get Next Frame
// Class:	CCamera
// Purpose:	get next frame from camera buffer
// Input:	pointer to void
// Output:	Nothing
////////////////////////////////////////////////////////////////////
void CCamera::GetNextFrame( void* )
{
//    static int repositioning = 0;
    cv::Mat pFrame;

	// get current frame time stamp
    m_timeCurrFrameStamp = m_pWorker->GetTime( );
	if( m_timeCurrFrameStamp - m_timePrevFrameStamp  < 5 )
		return;
	else
		m_timePrevFrameStamp = m_timeCurrFrameStamp;

	m_Capture.read(pFrame);
	//pFrame = cvQueryFrame( m_pCapture );

	// if this was avi and frame is zero(end or none?) stop
    if( !pFrame.empty() && m_isAvi )
    {
        //this->StopAvi( 0,0 );
        return;
	}

	////////////////////////////
	// if video window ? & frame ?
    if( !pFrame.empty() )
    {
		// if no video image 
        if( !m_pVideoImg.empty() )
        {
            m_pVideoImg.release();
			m_pVideoImg = cv::Mat(cvSize( m_nWidth, m_nHeight ), 8, 3 );
        }
		
		// check for the last origin of the frame
		cv::cvtColor(pFrame, m_pVideoImg, CV_BGR2RGB);

		cv::Mat output(m_pVideoImg.clone());
#ifdef _GUI_RUN
		// Update gui
		m_pCameraView->DrawCam(m_pVideoImg);
#endif	
		
    }


	// If camera started
    if( m_isRunning )
    {
		// get current frame time stamp
        m_timeCurrFrameStamp = m_pWorker->GetTime( );
        // update fps
        if( m_nFps < 0 )
		{
            m_nFps = 1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		} else
		{	
            m_nFps = ( 1 - m_nFpsAlpha ) * m_nFps + m_nFpsAlpha * 
						1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		}
		// set current time stamp as previuos
        m_timePrevFrameStamp = m_timeCurrFrameStamp;
		// get info of number of frames per second in a string
		// for debuging/etc
        sprintf( m_strFps, "FPS: %5.1f", m_nFps );
		m_pFrame->SetStatusBarText( m_strFps );

    }
}

////////////////////////////////////////////////////////////////////
// Method:	GetIFrame
// Class:	CCamera
// Purose:	get last frame grabbed
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
cv::Mat &CCamera::GetIFrame( )
{
	return( m_pVideoImg );
}


////////////////////////////////////////////////////////////////////
// Method:	Run
// Class:	CCamera
// Purose:	Start to run my camera thread
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCamera::Run( ) 
{

	if( !m_isPause )
	{
		// Get my next frame
		this->GetNextFrame( NULL );
	} else
	{
		if( m_bIsChange == 1 )
		{
			// check size
			Stop( );
			//Start( );
			m_bIsChange = 0;
			m_isPause = 0;
		}
	}

	return( 0 );
}
