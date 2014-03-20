#import "ClassifierAppDelegate.h"
#import "ImageClassifierImplementation.h"

@implementation ClassifierAppDelegate

@synthesize window = _window;
@synthesize imageClassifier;

- (void) applicationDidFinishLaunching:(UIApplication *)application
{
  imageClassifier = [[ImageClassifierImplementation alloc] init];
}

@end
