//
//  BasicViewController.h
//  Agora-RTM-Tutorial
//
//  Created by CavanSu on 2019/5/16.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ShowAlertProtocol <NSObject>
- (void)showAlert:(NSString *)message handle:(void(^)(UIAlertAction *))handle;
- (void)showAlert:(NSString *)message;
@end

@interface BasicViewController : UIViewController <ShowAlertProtocol>

@end
