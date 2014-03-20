#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace cv;

#define ROOT_DIR "/Users/computer/TU/SIM1/project/source_code/osx/Trainer_osx/trainer_data/"
#define TRAINING_DATA_DIR       ROOT_DIR "data/train/"
#define EVAL_DATA_DIR           ROOT_DIR "data/eval/"
#define DICTIONARY_PATH         ROOT_DIR "dictionary.yml"
#define CLASSIFIER_FILE_PATH    ROOT_DIR "classifiersave.txt"

Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("SURF");
Ptr<FeatureDetector> detector = FeatureDetector::create("SURF");

int dictionarySize = 1000;
TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);
int retries = 1;
int flags = KMEANS_PP_CENTERS;

BOWKMeansTrainer bowTrainer(dictionarySize, tc, retries, flags);
BOWImgDescriptorExtractor bowDE(extractor, matcher);

/**
 * \brief Recursively traverses a folder hierarchy. Extracts features from the training images and adds them to the bowTrainer.
 */
void extractTrainingVocabulary(const path& basepath) {
	for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
		directory_entry entry = *iter;
        
		if (is_directory(entry.path())) {
            
			cout << "Processing directory " << entry.path().string() << endl;
			extractTrainingVocabulary(entry.path());
            
		} else {
            
			path entryPath = entry.path();
			if (entryPath.extension() == ".jpg") {
                
				cout << "Processing file " << entryPath.string() << endl;
				Mat img = imread(entryPath.string());
				if (!img.empty()) {
					vector<KeyPoint> keypoints;
					detector->detect(img, keypoints);
					if (keypoints.empty()) {
						cerr << "Warning: Could not find key points in image: "
                        << entryPath.string() << endl;
					} else {
						Mat features;
						extractor->compute(img, keypoints, features);
						bowTrainer.add(features);
					}
				} else {
					cerr << "Warning: Could not read image: "
                    << entryPath.string() << endl;
				}
                
			}
		}
	}
}

/**
 * \brief Recursively traverses a folder hierarchy. Creates a BoW descriptor for each image encountered.
 */
void extractBOWDescriptor(const path& basepath, Mat& descriptors, Mat& labels) {
	for (directory_iterator iter = directory_iterator(basepath); iter != directory_iterator(); iter++) {
        
		directory_entry entry = *iter;
		
        if (is_directory(entry.path())) {
			cout << "Processing directory " << entry.path().string() << endl;
			extractBOWDescriptor(entry.path(), descriptors, labels);
		} else {
			path entryPath = entry.path();
			
            if (entryPath.extension() == ".jpg") {
				cout << "Processing file " << entryPath.string() << endl;
				Mat img = imread(entryPath.string());
				if (!img.empty()) {
					vector<KeyPoint> keypoints;
					detector->detect(img, keypoints);
					if (keypoints.empty()) {
						cerr << "Warning: Could not find key points in image: "
                        << entryPath.string() << endl;
					} else {
						Mat bowDescriptor;
						bowDE.compute(img, keypoints, bowDescriptor);
						descriptors.push_back(bowDescriptor);
						float label=atof(entryPath.filename().c_str());
						labels.push_back(label);
					}
				} else {
					cerr << "Warning: Could not read image: "
                    << entryPath.string() << endl;
				}
			}
            
		}
	}
}

void trainAndSaveToFile(bool checkErrorRate = false) {
    cout<<"Creating dictionary..."<<endl;
    extractTrainingVocabulary(path(TRAINING_DATA_DIR));
    vector<Mat> descriptors = bowTrainer.getDescriptors();
    int count=0;
    for(vector<Mat>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
        count+=iter->rows;
    }
    cout<<"Clustering "<<count<<" features"<<endl;
    Mat dictionary = bowTrainer.cluster();
    
    // Save Dictionary
    cout<<"Saving dict to File..."<<endl;
    cv::FileStorage storageIN(DICTIONARY_PATH, cv::FileStorage::WRITE);
    storageIN << "img" << dictionary;
    storageIN.release();
    
    bowDE.setVocabulary(dictionary);
	
    cout<<"Processing training data..."<<endl;
	Mat trainingData(0, dictionarySize, CV_32FC1);
	Mat labels(0, 1, CV_32FC1);
	extractBOWDescriptor(path(TRAINING_DATA_DIR), trainingData, labels);
    
	NormalBayesClassifier classifier;
	cout<<"Training classifier..."<<endl;
	classifier.train(trainingData, labels);
    
    // Save classifier
    cout<<"Saving classifier to file..."<<endl;
    classifier.save(CLASSIFIER_FILE_PATH);
    
    // No need to do prediction at this time, only for checking error rate
    if (checkErrorRate) {
        cout<<"Processing evaluation data..."<<endl;
        Mat evalData(0, dictionarySize, CV_32FC1);
        Mat groundTruth(0, 1, CV_32FC1);
        extractBOWDescriptor(path(EVAL_DATA_DIR), evalData, groundTruth);
        
        cout<<"Evaluating classifier..."<<endl;
        Mat results;
        
        classifier.predict(evalData, &results);
        
        double errorRate = (double) countNonZero(groundTruth - results) / evalData.rows;
        cout << "Error rate: " << errorRate << endl;
    }
}

void loadDataAndPredict() {
    
    // Load Dictionary
    Mat dictionary1;
    cv::FileStorage storageOUT(DICTIONARY_PATH, cv::FileStorage::READ);
    storageOUT["img"] >> dictionary1;
    storageOUT.release();
    
    bowDE.setVocabulary(dictionary1);
	
    // Load Classifier
    NormalBayesClassifier classifier1 = NormalBayesClassifier();
    classifier1.load(CLASSIFIER_FILE_PATH);
    
	cout<<"Processing evaluation data..."<<endl;
	Mat evalData(0, dictionarySize, CV_32FC1);
	Mat groundTruth(0, 1, CV_32FC1);
	extractBOWDescriptor(path(EVAL_DATA_DIR), evalData, groundTruth);
    
	cout<<"Evaluating classifier..."<<endl;
	Mat results;
    
	classifier1.predict(evalData, &results);

    for (int i = 0; i < bowTrainer.getDescriptors().size(); i++) {
        cout << "Features Descriptors= "<< endl << i << ":" << bowTrainer.getDescriptors()[i] << endl << "----------------------------------------" << endl;
        
    }
    
    cout << "groundTruth   = "<< endl << " "  << groundTruth << endl << "----------------------------------------" << endl;
    cout << "results       = "<< endl << " "  << results << endl << "-----------------------------------------" << endl;
    
	double errorRate = (double) countNonZero(groundTruth - results) / evalData.rows;
	cout << "Error rate: " << errorRate << endl;
}

int main(int argc, char ** argv) {
    // trainAndSaveToFile(true);
    loadDataAndPredict();
}
