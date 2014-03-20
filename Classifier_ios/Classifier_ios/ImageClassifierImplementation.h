#import <Foundation/Foundation.h>
#import "ImageClassifierProtocol.h"

@interface ImageClassifierImplementation : NSObject<ImageClassifierProtocol>

- (UIImage*) classifyImage:(UIImage*) src;

@end
