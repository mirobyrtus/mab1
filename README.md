*******************************************************************************
                    
                    Horngurke, Ananas und Sternfrucht
	                 Kiwano, Pineapple and Carambola
	                  
*******************************************************************************

The project was created in XCode, uses both opencv for iOS and for Linux/Mac.

In order to reach better performance, project was divided into two subprojects.
The main idea behind this  choice was to  save time while training data, which 
shouldn't be  done on the fly, in a mobile  device with lower  performance. So 
that it was  divided to two steps, in first step, training data is trained and
information is saved to  a  file. In second step, while running applicaiton on 
an iPhone, device only need to load this file in order to be trained.

	1) Trainer application for Mac
	
		* trainer/main.cpp file contains the whole code.

		Reads given train data and trains it with BOWKMeansTrainer and 
		extracts  descriptors  with BOWImgDescriptorExtractor. As soon 
		as training is done, application will save the state to a file.
	
	2) Classifier application for iOS
	
		* Classifier_ios/OpencvClassifier.cpp contains the main opencv part
		
		While  classifying first  image,  the application will load the 
		saved BOWKMeansTrainer and BOWImgDescriptorExtractor. This step
		needs to be executed only once.  Application  immediately tries
		to classify given image and write the prediction. 

-------------------------------------------------------------------------------

OpenCV's features extractor, descriptors and classifiers used: 

  FlannBased DescriptorMatcher and  SURF DescriptorExtractor were used in
  BOWImgDescriptorExtractor, in order to perform "BagOfWords" based image 
  classification.

  SURF FeatureDetector was also used to detect features. 

  Train images were described with BOWImgDescriptorExtractor and trained with 
  BOWKMeansTrainer.

  At the end NormalBayesClassifier was trained, in order to be able to predict
  classes of given images. 

*******************************************************************************
*******************************************************************************
