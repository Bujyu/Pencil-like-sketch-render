#define _USE_MATH_DEFINES 
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "render.hpp"

#ifndef _RENDER_PENCIL_HPP_
#define _RENDER_PENCIL_HPP_

class renderPencil : public renderBase  {

protected:

    int tex_id;
    
    void stroke_generate( cv::Mat&, cv::Mat& );
    void tone_generate( cv::Mat&, cv::Mat& );

public:

    // Constructor
    renderPencil() : tex_id(0){}
    renderPencil( int id ) : tex_id(id){}

    // Virtual Member
    virtual void render( cv::Mat&, cv::Mat& );

};

inline void renderPencil::render( cv::Mat& dst, cv::Mat& src ){

    cv::Mat stroke, tone;
   
    // Generate the stroke and tone 
    stroke_generate( stroke, src );
    tone_generate( tone, src );

    // Display the result of stroke and tone
    //cv::namedWindow( "STROKE", CV_WINDOW_AUTOSIZE );
    //cv::namedWindow( "TONE", CV_WINDOW_AUTOSIZE );
    //cv::imshow( "STROKE", stroke );
    //cv::imshow( "TONE", tone );
    
    // CV_8U -> CV32F, convenience process
    stroke.convertTo( stroke, CV_32F, 1.0/255.0 );
    tone.convertTo( tone, CV_32F );
    
    // Merge stroke and tone, and convert back to CV_8U
    cv::multiply( stroke, tone, dst );
    dst.convertTo( dst, CV_8U );

}

class renderColorPencil : public renderPencil {

public:

    // Constructor
    renderColorPencil() : renderPencil(){
    }

    renderColorPencil( int id ) : renderPencil( id ){
    }

    // Virtual Member
    virtual void render( cv::Mat&, cv::Mat& );
    
};

inline void renderColorPencil::render( cv::Mat& dst, cv::Mat& src  ){
    
    
    // Generate the "pencil"
    cv::Mat pencil;
    renderPencil::render( pencil, src );

    // Part of color
    cv::Mat hsv;

    // Color space conversion
    cv::cvtColor( src, hsv, CV_BGR2HSV );
    
    // Value is replaced by "pencil"
    cv::Mat hsvChannels[3];
    cv::split( hsv, hsvChannels );
    hsvChannels[2] = pencil;

    // Merge back and convert to RGB
    cv::merge( hsvChannels, 3, hsv );
    cv::cvtColor( hsv, dst, CV_HSV2BGR );

}

#endif // End of _RENDER_PENCIL_HPP_
