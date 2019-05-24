//
//  PeerViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "PeerViewController.h"
#import "ChatViewController.h"
#import "AgoraRtm.h"

@interface PeerViewController () <AgoraRtmDelegate>
@property (weak, nonatomic) IBOutlet UITextField *peerTextField;
@end

@implementation PeerViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [AgoraRtm updateDelegate:self];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.view endEditing:YES];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    NSString *peer = (NSString *)sender;
    
    if ([segue.identifier isEqualToString:@"peerToChat"]) {
        ChatMode mode;
        mode.type = ChatTypePeer;
        mode.name = peer;
        
        ChatViewController *chatVC = (ChatViewController *)segue.destinationViewController;
        chatVC.mode = mode;
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:true];
}

- (IBAction)doChatPressed:(UIButton *)sender {
    NSString *peer = self.peerTextField.text;
    
    if (!peer.length) {
        return;
    }
    
    [self performSegueWithIdentifier:@"peerToChat" sender:peer];
}

- (void)rtmKit:(AgoraRtmKit *)kit connectionStateChanged:(AgoraRtmConnectionState)state reason:(AgoraRtmConnectionChangeReason)reason {
    NSString *message = [NSString stringWithFormat:@"connection state changed: %ld", state];
    __weak PeerViewController *weakSelf = self;
    [self showAlert:message handle:^(UIAlertAction * action) {
        if (reason == AgoraRtmConnectionChangeReasonRemoteLogin) {
            [weakSelf.navigationController popToRootViewControllerAnimated:YES];
        }
    }];
}

@end
