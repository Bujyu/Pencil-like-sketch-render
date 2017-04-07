/* 
 Based on Combining Sketch and Tone for Pencil Drawing
 NPAR 2012
*/

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <opencv2/core/core.hpp>
#include <opencv2/core/internal.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <vector>
#include <string>
#include <iostream>

#define _USE_MATH_DEFINES 
#include <cmath>

int tex_id;

cv::Mat stroke_generate( cv::Mat &src ){

	cv::Mat dst;
	cv::Mat gImage;
	cv::Mat gradient_x, gradient_y;
	cv::Mat kernel;
	
	// Convert to grayscale
	cv::cvtColor( src, gImage, CV_BGR2GRAY );
	//cv::imshow( "GRAY", gImage );
	gImage.convertTo( gImage, CV_32F );

	// Sobel filter
	cv::Sobel( gImage, gradient_x, -1, 1, 0 );
	cv::Sobel( gImage, gradient_y, -1, 0, 1 );

	// [0.0, 255.0]
	cv::magnitude( gradient_x, gradient_y, dst );
		
	// Line classification
	std::cout << "Line classification" << std::endl;
	cv::Mat line[8];
	unsigned int lineLen = /*cv::max( src.size().width, src.size().height ) / 50;*/10;
	//std::cout << lineLen << std::endl;
	
	// Generate line segment
	// [0.0, 1.0]
	std::cout << "Line generation" << std::endl;
	unsigned int hLineLen = lineLen/2;
	for( int i = 0 ; i < 8 ; i++ ){
		line[i] = cv::Mat::zeros( lineLen, lineLen, CV_32FC1 );
		cv::Point st( (sin( (i/8.0)*M_PI )+1.0)*hLineLen, (cos( (i/8.0)*M_PI )+1.0)*hLineLen );
		cv::Point ed( (-sin( (i/8.0)*M_PI )+1.0)*hLineLen, (-cos( (i/8.0)*M_PI )+1.0)*hLineLen );
		cv::line( line[i], st, ed, cv::Scalar(1.0) );
	}
	
	// Convolution
	std::cout << "Convolution" << std::endl;
	cv::Mat gFilter[8];
	for( int i = 0 ; i < 8 ; i++ ){
		kernel = line[i].clone()/lineLen;
		cv::filter2D( dst, gFilter[i], -1, kernel, cv::Point( -1, -1 ), 0, cv::BORDER_DEFAULT );
	}

	std::cout << "Classification" << std::endl;

	cv::Mat cFilter[8];
	for( int i = 0 ; i < 8 ; i++ )
		cFilter[i] = cv::Mat::zeros( src.size(), CV_32FC1 );

	for( int i = 0 ; i < src.size().height ; i++ ){
		for( int j = 0 ; j < src.size().width ; j++ ){
			float max = 0;
			int mIdx = 0;
			for( int k = 0 ; k < 8 ; k++ ){
				if( gFilter[k].at<float>(i,j) > max ){
					mIdx = k;
					max = gFilter[k].at<float>(i,j);
				}
			}
			cFilter[mIdx].at<float>(i,j) = dst.at<float>(i,j);
		}
	}

	//Stroke generation
	std:: cout << "Stroke generation" << std::endl;
	dst = cv::Mat::zeros( src.size(), CV_8UC1 );
	cv::Mat tmp;
	for( int i = 0 ; i < 8 ; i++ ){
		kernel = line[i].clone()/lineLen;
		//cv::threshold( cFilter[i], cFilter[i], 75.0, 1.0, cv::THRESH_TOZERO );i
		cv::filter2D( cFilter[i], tmp, -1, kernel, cv::Point( -1, -1 ), 0, cv::BORDER_DEFAULT );
		tmp.convertTo( tmp, CV_8U );
		dst += tmp;
	}

	// Invert color
	cv::bitwise_not( dst, dst );

	return dst;

}

cv::Mat tone_generate( cv::Mat src ){

	cv::Mat dst;
	cv::Mat gImage;
	
	// Convert to grayscale
	cv::GaussianBlur( src, gImage, cv::Size( 5, 5 ), 0, 0 );
	cv::cvtColor( gImage, dst, CV_BGR2GRAY );

	// Tone adjusted
	std::cout << "Tone adjusted" << std::endl;
	std::vector<double> p( 256 );
	double sum = 0.0;
	for( int i = 0 ; i <= 255 ; i++ ){
		double p1 = 0.0, p2 = 0.0, p3 = 0.0;

		if( i <= 255 )
			p1 = (1.0/9.0) * exp( -((255.0-i)/255.0)/9.0 );
		if( i <= 225 && i >= 105 )
			p2 = 1.0 / ( 225 - 105 );
		
		p3 = (1.0/sqrt( 2*M_PI*11.0 )) * exp( -pow((i-90)/255.0,2.0)/(2*pow(11.0,2.0) ) ); 

		//double w1 = 11, w2 = 37, w3 = 52;
		//double w1 = 52, w2 = 37, w3 = 11;
		//double w1 = 42, w2 = 29, w3 = 29;
		double w1 = 29, w2 = 29, w3 = 42;

		p[i] = w1 * p1 + w2 * p2 + w3 * p3;
		sum += p[i];
	}

	// PDF -> CDF
	p[0] *= (1.0/sum);
	for( int i = 0 ; i < 256 ; ++i )
		p[i] = p[i]*(1.0/sum) + p[i-1];

	// Histogram matching	
	for( int i = 0 ; i < dst.size().height ; ++i )
		for( int j = 0 ; j < dst.size().width ; ++j )
			dst.at<unsigned char>(i,j) =  static_cast<unsigned char>( p[dst.at<unsigned char>(i,j)]*255 );

	//cv::imshow( "ADJ TONE", dst );
	dst.convertTo( dst, CV_32F );


	// Texture Rendering
	std::cout << "Texture Rendering" << std::endl;	

	// Texture initialize
	char tex_path[2][100] = { "./texture/tex1.jpg", "./texture/tex2.png" };
	cv::Mat tex = cv::imread( tex_path[tex_id] );
	cv::cvtColor( tex, tex, CV_BGR2GRAY );
	cv::resize( tex, tex, src.size() );
	tex.convertTo( tex, CV_32F );

	// Times parameter
	cv::Mat beta = cv::Mat::zeros( src.size(), CV_32FC1 );

	for( int i = 0 ; i < src.size().height ; ++i ){
		for( int j = 0 ; j < dst.size().width ; ++j ){
			if( tex.at<float>(i,j) >= 0.01 )
				beta.at<float>(i,j) = pow(log(dst.at<float>(i,j))/log(tex.at<float>(i,j)), 2.0);
			else
				beta.at<float>(i,j) = 0.0;
		}
	}
	
	// Local Smooth
	cv::Mat gradient_x, gradient_y;
	cv::Sobel( beta, gradient_x, -1, 1, 0 );
	cv::Sobel( beta, gradient_y, -1, 0, 1 );

	cv::Mat beta_grad;
	cv::magnitude( gradient_x, gradient_y, beta_grad );

	// Texture blur
	cv::Mat tex_toon;
	cv::multiply( beta+0.2*beta_grad, tex, tex_toon );

	tex_toon.convertTo( tex_toon, CV_8U );
	//cv::imshow( "TEX", tex_toon );

	return tex_toon;

}

int main( int argc, char **argv ){
	
	cv::Mat src, dst;

	// Image loading
	if( argc < 3 ){
		std::cerr << "<execution> <file name> <texture id>." << std::endl;
		return -1;
	}

	src = cv::imread( argv[1] );
	if( !src.data )
		return -1;
	
	tex_id = atoi( argv[2] );

	// CV_8UC1
	cv::Mat stroke = stroke_generate( src );
	cv::Mat tone = tone_generate( src );

	//cv::namedWindow( "STROKE", CV_WINDOW_AUTOSIZE );
	//cv::namedWindow( "TONE", CV_WINDOW_AUTOSIZE );
	//cv::imshow( "STROKE", stroke );
	//cv::imshow( "TONE", tone );
	
	stroke.convertTo( stroke, CV_32F, 1.0/255.0 );
	tone.convertTo( tone, CV_32F );
	
	cv::multiply( stroke, tone, dst );

	dst.convertTo( dst, CV_8U );

	// Color pencil
	std::cout << "Color Pencil" << std::endl;
	cv::Mat hsv;
	cv::cvtColor( src, hsv, CV_BGR2HSV );

	cv::Mat hsvChannels[3];
	cv::split( hsv, hsvChannels );
	hsvChannels[2] = dst;
	cv::merge( hsvChannels, 3, hsv );
	cv::cvtColor( hsv, hsv, CV_HSV2BGR );

	// Show the result
	cv::namedWindow( "SRC", CV_WINDOW_AUTOSIZE );
	cv::namedWindow( "DST", CV_WINDOW_AUTOSIZE );
	cv::namedWindow( "COLOR", CV_WINDOW_AUTOSIZE );
	//cv::namedWindow( "Merge", CV_WINDOW_AUTOSIZE );

	cv::imshow( "SRC", src );
	cv::imshow( "DST", dst );
	cv::imshow( "COLOR", hsv );

	cv::waitKey( -1 );
	
	return 0;

}

