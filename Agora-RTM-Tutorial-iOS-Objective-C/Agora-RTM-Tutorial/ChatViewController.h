//
//  ChatViewController.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "BasicViewController.h"

typedef NS_ENUM(NSInteger, ChatType) {
    ChatTypePeer = 0,
    ChatTypeGroup
};

typedef struct  {
    ChatType type;
    NSString *name;
} ChatMode;

@interface ChatViewController : BasicViewController
@property (nonatomic, assign) ChatMode mode;
@end
