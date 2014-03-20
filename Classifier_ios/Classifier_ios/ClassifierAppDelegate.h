#import <UIKit/UIKit.h>
#import "ImageClassifierProtocol.h"

@interface ClassifierAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) id<ImageClassifierProtocol> imageClassifier;
@end
