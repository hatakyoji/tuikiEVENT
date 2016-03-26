//---------------------------------------------------------
// 概要      : 光クレヨン
// File Name : tuikiEVENT.cpp
// Library   : OpenCV 2.0
//---------------------------------------------------------

#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <windows.h>
#include <MMsystem.h>

#pragma comment(lib,"WinMM.Lib")

#pragma comment (lib,"cv200d.lib")
#pragma comment (lib,"cxcore200d.lib")
#pragma comment (lib,"highgui200d.lib")



#define WIDTH	720	//	キャプチャ画像の横幅
#define HEIGHT	480	//	キャプチャ画像の縦幅

#define THRESH_BOTTOM	200			//	明度下限の閾値
//#define THRESH_TOP	80			//	明度値の上限の閾値
#define THRESHOLD_MAX_VALUE	255		//	2値化の際に使用する最大値

#define CIRCLE_RADIUS	2		//	円の半径
#define LINE_THICKNESS	2		//	線の太さ
#define LINE_TYPE		8		//	線の種類
#define SHIFT			0		//	座標の小数点以下の桁を表すビット数


int main( int argc, char **argv ){ 
	int key;							//	キー入力用の変数
	CvCapture *capture = NULL;			//	カメラキャプチャ用の構造体
	IplImage *frameImage;				//	キャプチャ画像用IplImage
	IplImage *frameImage2;				//	キャプチャ画像用IplImage2

	//	画像を生成する
	IplImage *backgroundImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//背景画像用IplImage
	IplImage *grayImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );				//グレースケール画像用IplImage
	IplImage *differenceImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//差分画像用IplImage

	IplImage *hsvImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3 );				//HSV画像用IplImage
	IplImage *hueImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );				//色相(H)情報用IplImage
	IplImage *saturationImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//彩度(S)情報用IplImage
	IplImage *valueImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );				//明度(V)情報用IplImage
	IplImage *thresholdImage1 = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//明度がTHRES_BOTTOMより大きい領域用IplImage
//	IplImage *thresholdImage2 = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//明度がTHRES_TOP以下の領域用IplImage
//	IplImage *thresholdImage3 = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );		//thresholdImage1とthresholdImage2のAND演算結果用IplImage
	IplImage *lightImage = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1 );				//光っている部分の領域の抽出結果用IplImage
	
	char windowNameCapture[] = "Capture"; 			//キャプチャした画像を表示するウィンドウの名前
	char windowNameLight[] = "Light";				//光っている部分の領域を表示するウィンドウの名前
	char windowNameCapture2[] = "Capture2"; 		//キャプチャした画像を表示するウィンドウの名前
	char windowNameThreshold[] = "Threshold";		//thresholdImage1を表示するウィンドウの名前

	CvMoments moment;
	double m_00;
	double m_10;
	double m_01;
	int gravityX;
	int gravityY;
	unsigned char h,s,v,r,g,b;
	int rr,gg,bb;
	int soundc=0;


	//	カメラを初期化する
	if ( ( capture = cvCreateCameraCapture( 0 ) ) == NULL ) {
		//	カメラが見つからなかった場合
		printf( "カメラが見つかりません\n" );
		return -1;
	}

	//	ウィンドウを生成する
	cvNamedWindow( windowNameCapture, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( windowNameLight, CV_WINDOW_AUTOSIZE );
  	cvNamedWindow( windowNameCapture2, CV_WINDOW_AUTOSIZE );
	cvNamedWindow( windowNameThreshold, CV_WINDOW_AUTOSIZE );

  	//	初期背景を設定するためにカメラから画像を取得
	frameImage = cvQueryFrame( capture );
	//	frameImageをグレースケール化し、背景画像とする
	cvCvtColor( frameImage, backgroundImage, CV_BGR2GRAY );
	frameImage2 = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3 );
	cvSet (frameImage2, CV_RGB(0,0,0));  //黒色で塗りつぶす

	//	メインループ
	while( 1 ) {
		//	captureの入力画像フレームをframeImageに格納する
		frameImage = cvQueryFrame( capture );
		//	frameImageをグレースケール化したものを、grayImageに格納する
		cvCvtColor( frameImage, grayImage, CV_BGR2GRAY );
		//	grayImageと背景画像との差分をとる
		cvAbsDiff( grayImage, backgroundImage, differenceImage );
		
		//	frameImageをBGRからHSVに変換する
		cvCvtColor( frameImage, hsvImage, CV_BGR2HSV );
		//	HSV画像をH、S、V画像に分ける
		cvSplit( hsvImage, hueImage, saturationImage, valueImage, NULL );
		//	明度が明るい部分を抽出、その部分のみ出力する
		cvThreshold( valueImage, thresholdImage1, THRESH_BOTTOM, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY );
//		cvThreshold( hueImage, thresholdImage2, THRESH_TOP, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY_INV );
//		cvAnd( thresholdImage1, thresholdImage2, thresholdImage3, NULL );
		
		//	背景差分画像と明るい領域とのANDをとる
		cvAnd( differenceImage, thresholdImage1, lightImage, NULL );

		for(gravityY=0;gravityY<480;gravityY++){
			for(gravityX=0;gravityX<720;gravityX++){
//			h = hsvImage ->imageData[hsvImage ->widthStep * (gravityY) + gravityX * 3 ];     // H
//			s = hsvImage ->imageData[hsvImage ->widthStep * gravityY + gravityX * 3 + 1];    // S
			v = hsvImage ->imageData[hsvImage ->widthStep * gravityY + gravityX * 3 + 2];    // V
			
			b = frameImage ->imageData[frameImage ->widthStep * (gravityY) + gravityX * 3 ];     // B
			g = frameImage ->imageData[frameImage ->widthStep * gravityY + gravityX * 3 + 1];    // G
			r = frameImage ->imageData[frameImage ->widthStep * gravityY + gravityX * 3 + 2];    // R

			if (r>30){
				rr=250;
				gg=0;
				bb=0;
			}
			if (g>30){
				rr=0;
				gg=250;
				bb=0;
			}
			if (b>30){
				rr=0;
				gg=0;
				bb=250;
			}
			if ((b>30) && (r>30)){
				rr=250;
				gg=0;
				bb=250;
			}
			if ((b>30) && (g>30)){
				rr=0;
				gg=250;
				bb=250;
			}
			if ((b>30) && (g>30) && (r>30)){
				rr=250;
				gg=250;
				bb=250;
			}

		//	画像上に円を描画する
			if (v>80){
			cvCircle( frameImage2, cvPoint( gravityX, gravityY ), CIRCLE_RADIUS, CV_RGB( rr, gg, bb ), LINE_THICKNESS, LINE_TYPE, 0 );
			soundc++;
				if(soundc %30==0){
					//音がなるワンチャン
					PlaySound("notify.wav" , NULL , SND_FILENAME | SND_ASYNC);
					soundc=0;
			}
			}
			}
		}
	
		
		//	光っている領域の重心を算出する
/*		cvMoments( lightImage, &moment, 0 );
		m_00 = cvGetSpatialMoment( &moment, 0, 0 );
		m_10 = cvGetSpatialMoment( &moment, 1, 0 );
		m_01 = cvGetSpatialMoment( &moment, 0, 1 );
		gravityX = m_10 / m_00;
		gravityY = m_01 / m_00;

		if (0<gravityX){
			h = hsvImage ->imageData[hsvImage ->widthStep * (gravityY) + gravityX * 3 ];     // H
			s = hsvImage ->imageData[hsvImage ->widthStep * gravityY + gravityX * 3 + 1];    // S
			v = hsvImage ->imageData[hsvImage ->widthStep * gravityY + gravityX * 3 + 2];    // V

			b = frameImage ->imageData[frameImage ->widthStep * (gravityY) + gravityX * 3 ];     // B
			g = frameImage ->imageData[frameImage ->widthStep * gravityY + gravityX * 3 + 1];    // G
			r = frameImage ->imageData[frameImage ->widthStep * gravityY + gravityX * 3 + 2];    // R

			printf ("x= %d ,y= %d v= %d,s= %d,h= %d  \n" ,gravityX,gravityY,r,g,b);
			
			rr=250;
			gg=250;
			bb=250;

			if (r>200){
				rr=150;
				gg=0;
				bb=0;
			}
			if (g>200){
				rr=0;
				gg=150;
				bb=0;
			}
			if (b>200){
				rr=0;
				gg=0;
				bb=150;
			}

		//	画像上に円を描画する
			if (v>200){
				
				cvCircle( frameImage2, cvPoint( gravityX, gravityY ), CIRCLE_RADIUS, CV_RGB( rr, gg, bb ), LINE_THICKNESS, LINE_TYPE, 0 );

			}
		}
*/
		//	画像を表示する
		cvShowImage( windowNameCapture, frameImage );
		cvShowImage( windowNameLight, lightImage );
		cvShowImage( windowNameCapture2,   frameImage2);
		cvShowImage( windowNameThreshold, thresholdImage1);

		//	キー入力判定
		key = cvWaitKey( 10 );
		if( key == 'q' ) 
			//	'q'キーが押されたらループを抜ける
			break;
		else if( key == 'b' ) {
			//	'b'キーが押されたら、その時点での画像を背景画像とする
			frameImage = cvQueryFrame( capture );
		    cvCvtColor( frameImage, backgroundImage, CV_BGR2GRAY );
		}
		else if(key == 'c') {
			//	'c'キーが押されたら画像を保存
			cvSet (frameImage2, CV_RGB(0,0,0));  //黒色で塗りつぶす
		}
	}
	//	キャプチャを解放する
	cvReleaseCapture( &capture );
	//	メモリを解放する
	cvReleaseImage( &backgroundImage );
	cvReleaseImage( &grayImage );
	cvReleaseImage( &differenceImage );
	cvReleaseImage( &hsvImage );
	cvReleaseImage( &hueImage );
	cvReleaseImage( &saturationImage );
	cvReleaseImage( &valueImage );
	cvReleaseImage( &thresholdImage1 );
//	cvReleaseImage( &thresholdImage2 );
//	cvReleaseImage( &thresholdImage3 );
	cvReleaseImage( &lightImage );
	//	ウィンドウを破棄する
	cvDestroyWindow( windowNameCapture );
	cvDestroyWindow( windowNameLight );
	cvDestroyWindow( windowNameThreshold );
	cvDestroyWindow( windowNameCapture2 );

	return 0;
} 
