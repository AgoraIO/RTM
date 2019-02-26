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

@implementation AgoraRtm
+ (void)load {
    _kit = [[AgoraRtmKit alloc] initWithAppId:[AppId appId] delegate:nil];
}

+ (AgoraRtmKit *)kit {
    return _kit;
}

+ (NSString *)current {
    return _current;
}

+ (void)setCurrent:(NSString *)name {
    _current = name;
}

+ (LoginStatus)status {
    return _status;
}

+ (void)setStatus:(LoginStatus)status {
    _status = status;
}

+ (void)updateDelegate:(id <AgoraRtmDelegate> _Nullable)delegate {
    _kit.agoraRtmDelegate = delegate;
}
@end
