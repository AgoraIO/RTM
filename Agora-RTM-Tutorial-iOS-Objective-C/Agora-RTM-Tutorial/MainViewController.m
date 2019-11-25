//
//  MainViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "MainViewController.h"
#import "AgoraRtm.h"

@interface MainViewController () <AgoraRtmDelegate>
@property (weak, nonatomic) IBOutlet UITextField *accountTextField;
@property (weak, nonatomic) IBOutlet UISwitch *enableOneToOneSwitch;
@end

@implementation MainViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self logout];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.view endEditing:YES];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:true];
}

- (IBAction)doLoginPressed:(UIButton *)sender {
    [self login];
}

- (void)login {
    NSString *account = self.accountTextField.text;
    if (!account.length) {
        return;
    }
    
    [AgoraRtm updateDelegate:self];
    [AgoraRtm setCurrent:account];
    [AgoraRtm setOneToOneMessageType:self.enableOneToOneSwitch.isOn ? OneToOneMessageTypeOffline : OneToOneMessageTypeNormal];
    
    [AgoraRtm.kit loginByToken:nil user:account completion:^(AgoraRtmLoginErrorCode errorCode) {
        if (errorCode != AgoraRtmLoginErrorOk) {
            [self showAlert: [NSString stringWithFormat:@"login error: %ld", errorCode]];
            return;
        }
        
        [AgoraRtm setStatus:LoginStatusOnline];
        
        __weak MainViewController *weakSelf = self;
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf performSegueWithIdentifier:@"mainToTab" sender:nil];
        });
    }];
}

- (void)logout {
    if (AgoraRtm.status == LoginStatusOffline) {
        return;
    }
    
    [AgoraRtm.kit logoutWithCompletion:^(AgoraRtmLogoutErrorCode errorCode) {
        if (errorCode != AgoraRtmLogoutErrorOk) {
            return;
        }
        
        [AgoraRtm setStatus:LoginStatusOffline];
    }];
}

// Receive one to one offline messages
- (void)rtmKit:(AgoraRtmKit *)kit messageReceived:(AgoraRtmMessage *)message fromPeer:(NSString *)peerId {
    [AgoraRtm addOfflineMessage:message fromUser:peerId];
}
     
@end
