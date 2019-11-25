//
//  AgoraRtm.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AgoraRtmKit/AgoraRtmKit.h>
#import "AppId.h"

typedef NS_ENUM(NSInteger, LoginStatus) {
    LoginStatusOnline = 0,
    LoginStatusOffline
};

typedef NS_ENUM(NSInteger, OneToOneMessageType) {
    OneToOneMessageTypeNormal = 0,
    OneToOneMessageTypeOffline
};

@interface AgoraRtm : NSObject
+ (AgoraRtmKit * _Nullable)kit;
+ (NSString * _Nullable)current;
+ (void)setCurrent:(NSString * _Nonnull)name;
+ (LoginStatus)status;
+ (void)setStatus:(LoginStatus)status;
+ (void)setOneToOneMessageType:(OneToOneMessageType)type;
+ (OneToOneMessageType)oneToOneMessageType;

+ (void)updateDelegate:(id <AgoraRtmDelegate> _Nullable)delegate;
+ (void)addOfflineMessage:(AgoraRtmMessage * _Nonnull)message fromUser:(NSString * _Nonnull)user;
+ (NSMutableArray * _Nullable)getOfflineMessagesFromUser:(NSString * _Nonnull)user;
+ (void)removeOfflineMessageFromUser:(NSString * _Nonnull)user;
@end
