#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "renderPencil.hpp"

int main( int argc, char **argv ){
   
    // Image loading
    if( argc < 3 ){
        std::cerr << "<execution> <file name> <texture id>." << std::endl;
        return -1;
    }

    cv::Mat src;
    src = cv::imread( argv[1] );
    if( !src.data )
        return -1;
    
    // Select texture
    int tex_id = atoi( argv[2] );

    // Pencil
    std::cout << "Pencil" << std::endl;

    renderPencil pencil_render( tex_id );
    cv::Mat pencil_dst;

    pencil_render.render( pencil_dst, src );

    // Color pencil
    std::cout << "Color Pencil" << std::endl;

    renderColorPencil color_pencil_render(tex_id);
    cv::Mat color_dst;

    color_pencil_render.render( color_dst, src );

    // Show the result
    cv::namedWindow( "SRC   ", CV_WINDOW_AUTOSIZE );
    cv::namedWindow( "SKETCH", CV_WINDOW_AUTOSIZE );
    cv::namedWindow( "COLOR ", CV_WINDOW_AUTOSIZE );

    cv::imshow( "SRC", src );
    cv::imshow( "DST", pencil_dst );
    cv::imshow( "COLOR", color_dst );

    cv::waitKey( -1 );
    
    return 0;

}
























































































































































































































































