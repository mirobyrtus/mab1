#import <UIKit/UIKit.h>

@interface ClassifierViewController : UIViewController<UINavigationControllerDelegate,UIImagePickerControllerDelegate /*,UIPopoverControllerDelegate */ >
{
  UIButton *mainButton;
  UIPopoverController *popoverController;
}

@property (strong, nonatomic) IBOutlet UIButton *mainButton;
@property (nonatomic, retain) UIPopoverController *popoverController;

@property UIImagePickerController* picker;

- (IBAction)buttonDidTapped:(id)sender;

@end
