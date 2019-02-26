//
//  PeerChannelViewController.h
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/2/22.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PeerChannelViewController;
@protocol PeerChannelVCDelegate <NSObject>
- (void)peerChannelVCWillClose:(PeerChannelViewController *)vc;
@end

@interface PeerChannelViewController : NSViewController
@property (nonatomic, strong) id<PeerChannelVCDelegate> delegate;
@end
