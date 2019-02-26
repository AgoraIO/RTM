//
//  ChatViewController.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef NS_ENUM(NSInteger, ChatType) {
    ChatTypePeer = 0,
    ChatTypeGroup
};

typedef struct  {
    ChatType type;
    NSString *name;
} ChatMode;

@class ChatViewController;
@protocol ChatVCDelegate <NSObject>
- (void)chatVCWillClose:(ChatViewController *)vc;
@end

@interface ChatViewController : NSViewController
@property (nonatomic, assign) ChatMode mode;
@property (nonatomic, strong) id<ChatVCDelegate> delegate;
@end
