//
//  BasicViewController.h
//  Agora-RTM-Tutorial-Mac
//
//  Created by CavanSu on 2019/5/24.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ShowAlertProtocol <NSObject>
- (void)showAlert:(NSString * _Nonnull)message handle:(void (^ _Nullable)(NSModalResponse returnCode))handle;
- (void)showAlert:(NSString * _Nonnull)message;
@end

@interface BasicViewController : NSViewController <ShowAlertProtocol>

@end
