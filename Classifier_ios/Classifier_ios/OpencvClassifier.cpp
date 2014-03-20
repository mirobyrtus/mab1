#include "OpencvClassifier.h"

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <boost/assert.hpp>

#define GIL2CV(GilView) getIplImageView(GilView)
#define ROOT_PATH "/Users/computer/TU/SIM1/project/source_code/ios/Classifier_ios/"
#define DICTIONARY_PATH ROOT_PATH "dictionary.yml"
#define CLASSIFIER_PATH ROOT_PATH "classifiersave.txt"

#define KIWANO_CLASS_CODE 170
#define PINEAPPLE_CLASS_CODE 181
#define CARAMBOLA_CLASS_CODE 202
#define ORANGE_CLASS_CODE 205

cv::Ptr<cv::DescriptorMatcher> matcher;
cv::Ptr<cv::DescriptorExtractor> extractor;
cv::Ptr<cv::FeatureDetector> detector;

int dictionarySize = 1000;
cv::TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);
int retries = 1;
int flags = cv::KMEANS_PP_CENTERS;

cv::BOWKMeansTrainer bowTrainer = NULL;
cv::BOWImgDescriptorExtractor bowDE(extractor, matcher); // @TODO Set it to NULL!

cv::NormalBayesClassifier classifier1;

bool loaded = false;

void OpencvClassifier::classifyImage(boost::gil::bgr8_view_t src, boost::gil::bgr8_view_t dst)
{
    BOOST_ASSERT(src.dimensions() == dst.dimensions());
    
    printf("Prepare images...\n");
    
    boost::gil::gray8_image_t srcGray(src.dimensions());
    boost::gil::gray8_image_t edges(src.dimensions());
    
    IplImage srcIplImage     = GIL2CV(src);
    IplImage dstIplImage     = GIL2CV(dst);
    
    if (!loaded) {
        printf("Initialize global variables..\n");
        
        cv::initModule_nonfree(); // Otherwise the detectory may be NULL
        
        matcher = cv::DescriptorMatcher::create("FlannBased");
        extractor = cv::DescriptorExtractor::create("SURF");
        detector = cv::FeatureDetector::create("SURF");
        
        bowTrainer = cv::BOWKMeansTrainer(dictionarySize, tc, retries, flags);
        bowDE = cv::BOWImgDescriptorExtractor(extractor, matcher);
        
        printf("Load trained dictionary...\n");
        cv::Mat dictionary1;
        cv::FileStorage storageOUT(DICTIONARY_PATH, cv::FileStorage::READ);
        
        storageOUT["img"] >> dictionary1;
        storageOUT.release();
        
        bowDE.setVocabulary(dictionary1);
        
        printf("Load trained classifier...\n");
        classifier1 = cv::NormalBayesClassifier();
        classifier1.load(CLASSIFIER_PATH);
        
        loaded = true;
    } else {
        printf("Global variables already initialized..\n");
    }
    
    printf("Predicting class..\n");
    cv::Mat evalData(0, dictionarySize, CV_32FC1);
    //cv::Mat groundTruth(0, 1, CV_32FC1);
	
        cv::Mat img(&srcIplImage, CV_32FC1);

        if (!img.empty()) {
            cv::vector<cv::KeyPoint> keypoints;
            detector->detect(img, keypoints);
            if (keypoints.empty()) {
                printf("Warning: Could not find key points in image! \n");
            } else {
                cv::Mat bowDescriptor;
                bowDE.compute(img, keypoints, bowDescriptor);
                evalData.push_back(bowDescriptor);

                // float label = atof("Nothing more!");
                // groundTruth.push_back(label);
            }
        } else {
            printf("Warning: Could not read image! \n");
            return;
        }
    
	printf("Evaluating classifier...\n");
    cv::Mat results;
    
	classifier1.predict(evalData, &results);

    float predicted_class = results.at<float>(0, 0);
    printf("Results: [0,0] = %d \n", (int)predicted_class);

    std::string predicted_object = "unknown";
    
    switch ((int)predicted_class) {
        case KIWANO_CLASS_CODE:
            predicted_object = "Kiwano";
            break;
        case PINEAPPLE_CLASS_CODE:
            predicted_object = "Pineapple";
            break;
        case CARAMBOLA_CLASS_CODE:
            predicted_object = "Carambola";
            break;
        case ORANGE_CLASS_CODE:
            predicted_object = "Orange";
            break;
        default:
            break;
    }
    
    // imageClass.assign(predicted_object);
    
    // printf("Predicted object is %s \n", predicted_object.c_str());
    
    const CvFont f1 = cvFont(1);
    cvPutText(&srcIplImage, predicted_object.c_str(), cvPoint(30,30), &f1, cvScalar(255,0,0));
    
    cvCopy(&srcIplImage, &dstIplImage);
}

IplImage OpencvClassifier::getIplImageView(boost::gil::bgr8_view_t srcView)
{
  IplImage img;
  cvInitImageHeader(&img, cvSize(srcView.width(), srcView.height()), IPL_DEPTH_8U, 3, 0);
  img.imageData = (char*)&srcView.begin()[0];
  img.widthStep = srcView.xy_at(0,0).row_size();
    
  return img;
}

IplImage OpencvClassifier::getIplImageView(boost::gil::gray8_view_t srcView)
{
  IplImage img;
  
  cvInitImageHeader(&img, cvSize(srcView.width(), srcView.height()), IPL_DEPTH_8U, 1, 0);
  img.imageData = (char*)&srcView.begin()[0];
  img.widthStep = srcView.xy_at(0,0).row_size();

  return img;
}
