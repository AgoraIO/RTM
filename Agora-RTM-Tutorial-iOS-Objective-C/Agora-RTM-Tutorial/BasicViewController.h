//
//  BasicViewController.h
//  Agora-RTM-Tutorial
//
//  Created by CavanSu on 2019/5/16.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ShowAlertProtocol <NSObject>
- (void)showAlert:(NSString * _Nonnull)message handle:(void(^_Nullable)(UIAlertAction * _Nullable))handle;
- (void)showAlert:(NSString * _Nonnull)message;
@end

@interface BasicViewController : UIViewController <ShowAlertProtocol>

@end
