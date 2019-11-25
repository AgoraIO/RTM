//
//  AgoraRtm.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "AgoraRtm.h"

static AgoraRtmKit *_kit = nil;
static NSString *_current = nil;
static LoginStatus _status = LoginStatusOffline;
static OneToOneMessageType _oneToOneMessageType = OneToOneMessageTypeNormal;
static NSMutableDictionary *_offlineMessages = nil;

@implementation AgoraRtm

+ (void)load {
    _kit = [[AgoraRtmKit alloc] initWithAppId:[AppId appId] delegate:nil];
    _offlineMessages = [NSMutableDictionary dictionary];
}

+ (AgoraRtmKit * _Nullable)kit {
    return _kit;
}

+ (NSString * _Nullable)current {
    return _current;
}

+ (void)setCurrent:(NSString * _Nonnull)name {
    _current = name;
}

+ (LoginStatus)status {
    return _status;
}

+ (void)setStatus:(LoginStatus)status {
    _status = status;
}

+ (void)setOneToOneMessageType:(OneToOneMessageType)type {
    _oneToOneMessageType = type;
}

+ (OneToOneMessageType)oneToOneMessageType {
    return _oneToOneMessageType;
}

+ (void)updateDelegate:(id <AgoraRtmDelegate> _Nullable)delegate {
    _kit.agoraRtmDelegate = delegate;
}

+ (void)addOfflineMessage:(AgoraRtmMessage * _Nonnull)message fromUser:(NSString * _Nonnull)user {
    if (message.isOfflineMessage == false) {
        return;
    }
    
    NSMutableArray *messageList = nil;
    
    if (_offlineMessages[user]) {
        messageList = _offlineMessages[user];
    } else {
        messageList = [NSMutableArray array];
    }
    
    [messageList addObject:message];
    
    if (!_offlineMessages[user]) {
        _offlineMessages[user] = messageList;
    }
}

+ (NSMutableArray * _Nullable)getOfflineMessagesFromUser:(NSString * _Nonnull)user {
    return _offlineMessages[user];
}

+ (void)removeOfflineMessageFromUser:(NSString * _Nonnull)user {
    return [_offlineMessages removeObjectForKey:user];
}

@end
