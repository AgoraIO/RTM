//
//  PeerChannelViewController.m
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/2/22.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "PeerChannelViewController.h"
#import "ChatViewController.h"
#import "AgoraRtm.h"

@interface PeerChannelViewController () <AgoraRtmDelegate, ChatVCDelegate>
@property (weak, nonatomic) IBOutlet NSTextField *peerTextField;
@property (weak, nonatomic) IBOutlet NSTextField *channelTextField;

@property (nonatomic, assign) ChatMode selectedMode;
@end

@implementation PeerChannelViewController

- (void)viewWillAppear {
    [super viewWillAppear];
    [AgoraRtm updateDelegate:self];
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"peerChannelToChat"]) {
        ChatViewController *chatVC = (ChatViewController *)segue.destinationController;
        chatVC.mode = self.selectedMode;
        chatVC.delegate = self;
    }
}

- (IBAction)doChatPressed:(NSButton *)sender {
    NSString *peer = self.peerTextField.stringValue;
    
    if (!peer.length) {
        return;
    }
    
    ChatMode mode;
    mode.type = ChatTypePeer;
    mode.name = peer;
    
    self.selectedMode = mode;
    
    [self performSegueWithIdentifier:@"peerChannelToChat" sender:nil];
}

- (IBAction)doJoinPressed:(NSButton *)sender {
    NSString *channel = self.channelTextField.stringValue;
    
    if (!channel.length) {
        return;
    }
    
    ChatMode mode;
    mode.type = ChatTypeGroup;
    mode.name = channel;
    
    self.selectedMode = mode;
    
    [self performSegueWithIdentifier:@"peerChannelToChat" sender:nil];
}

- (IBAction)doBackPressed:(NSButton *)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(peerChannelVCWillClose:)]) {
        [self.delegate peerChannelVCWillClose:self];
    }
}

- (void)rtmKit:(AgoraRtmKit *)kit connectionStateChanged:(AgoraRtmConnectionState)state reason:(AgoraRtmConnectionChangeReason)reason {
    NSString *message = [NSString stringWithFormat:@"connection state changed: %ld", state];
    __weak PeerChannelViewController *weakSelf = self;
    [self showAlert:message handle:^(NSModalResponse returnCode) {
        if (reason == AgoraRtmConnectionChangeReasonRemoteLogin) {
            [weakSelf.delegate peerChannelVCWillClose:weakSelf];
        }
    }];
}

// Receive one to one offline messages
- (void)rtmKit:(AgoraRtmKit *)kit messageReceived:(AgoraRtmMessage *)message fromPeer:(NSString *)peerId {
    [AgoraRtm addOfflineMessage:message fromUser:peerId];
}

- (void)chatVCWillClose:(ChatViewController *)vc {
    vc.view.window.contentViewController = self;
}

@end
