//
//  ChatViewController.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, ChatType) {
    ChatTypePeer = 0,
    ChatTypeGroup
};

typedef struct  {
    ChatType type;
    NSString *name;
} ChatMode;

@interface ChatViewController : UIViewController
@property (nonatomic, assign) ChatMode mode;
@end
