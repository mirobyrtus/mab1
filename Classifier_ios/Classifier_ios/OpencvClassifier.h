#ifndef Classifier_ios_OpencvClassifier_h
#define Classifier_ios_OpencvClassifier_h

#include <boost/gil/gil_all.hpp>

struct _IplImage;
typedef _IplImage IplImage;

class OpencvClassifier
{
public:
  
    // classify Image
    void classifyImage(boost::gil::bgr8_view_t src, boost::gil::bgr8_view_t dst);
  
private:
    // srcView to IplImage
    IplImage getIplImageView(boost::gil::bgr8_view_t srcView);
    IplImage getIplImageView(boost::gil::gray8_view_t srcView);
    
};

#endif
