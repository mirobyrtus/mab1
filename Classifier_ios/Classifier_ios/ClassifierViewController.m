#import "ClassifierViewController.h"
#import "ClassifierAppDelegate.h"

#import "SVProgressHUD.h"

@implementation ClassifierViewController
@synthesize mainButton, popoverController;

#pragma mark - View lifecycle

- (void)viewDidUnload
{
    [self setMainButton:nil];
    [super viewDidUnload];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    /*
     NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
     NSString *docs = [paths objectAtIndex:0];
     NSString *vocabPath = [docs stringByAppendingPathComponent:@"dictionary.yml"];
     FileStorage fs([vocabPath UTF8String], FileStorage::WRITE);
     // vocab is a CvMat object representing the vocabulary in my bag of features model
     fs << "vocabulary" << vocab;
     fs.release();
     
     // READING
     
     NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
     NSString *docs = [paths objectAtIndex:0];
     NSString *vocabPath = [docs stringByAppendingPathComponent:@"vocabulary.xml"];
     FileStorage fs([vocabPath UTF8String], FileStorage::READ);
     // vocab is a CvMat object representing the vocabulary in my bag of features model
     fs["vocabulary"] >> vocab;
     fs.release();
     
     NSLog(@"VocabPath = %@ , exists ? %d", vocabPath, [[NSFileManager defaultManager] fileExistsAtPath:vocabPath]);
     */
    
    // [self setMainButton:nil];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return NO;
}

#pragma mark - Selecting image from photo album

- (void) buttonDidTapped:(id)sender
{
    if (_picker == nil) {

        [SVProgressHUD showWithStatus:@"Loading ImagePicker..."];

        dispatch_queue_t dispatchQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        
        dispatch_async(dispatchQueue, ^{
        
            _picker = [[UIImagePickerController alloc] init];
            _picker.delegate = self;
            _picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;

            // Back to main queue..
            dispatch_async(dispatch_get_main_queue(), ^{
                [SVProgressHUD dismiss];
                [self presentViewController:_picker animated:YES completion:nil];
            });
        });
        
    } else {
        [self presentViewController:_picker animated:YES completion:nil];
    }
}

/*
- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController
{
    return true;
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
    // nothing to do
}
*/

#pragma mark - UIImagePickerControllerDelegate implementation

- (void)imagePickerController:(UIImagePickerController *)picker
        didFinishPickingImage:(UIImage *)image
                  editingInfo:(NSDictionary *)editingInfo
{
    // [picker dismissViewControllerAnimated:YES completion:nil];
    
    ClassifierAppDelegate * appDelegate = [[UIApplication sharedApplication] delegate];
    
    [SVProgressHUD showWithStatus:@"Classifying..."];
    
    dispatch_queue_t dispatchQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(dispatchQueue, ^{
        
        // Process the input image and present the result:
        UIImage* classifiedImage = [appDelegate.imageClassifier classifyImage:image];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            self.mainButton.imageView.image = classifiedImage;
            
            [_picker dismissViewControllerAnimated:YES completion:nil];
            // [picker dismissModalViewControllerAnimated:YES];
            
            [SVProgressHUD dismiss];
            
            // NSString* s1 = [appDelegate.imageClassifier getImage];
            // NSLog(s1);
            NSString* s = @"Finished!";
            
            UIAlertView* finishedAlert = [[UIAlertView alloc] initWithTitle:@"Classifier Finished"
                                                                    message:s
                                                                   delegate:self
                                                          cancelButtonTitle:@"OK"
                                                          otherButtonTitles:nil];
            [finishedAlert show];
            
        });
    });
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}

@end
