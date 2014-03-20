#import <Foundation/Foundation.h>

@protocol ImageClassifierProtocol <NSObject>

- (UIImage*) classifyImage:(UIImage*) src;

@end
