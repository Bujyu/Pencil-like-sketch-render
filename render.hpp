#include <opencv2/core/core.hpp>

#ifndef _RENDER_HPP_
#define _RENDER_HPP_

class renderBase{

public:
    // Member function
    virtual void render( cv::Mat&, cv::Mat& ) = 0;
    
};

#endif // End of _RENDER_HPP_
