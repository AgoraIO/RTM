//
//  PeerViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "PeerViewController.h"
#import "ChatViewController.h"

@interface PeerViewController ()
@property (weak, nonatomic) IBOutlet UITextField *peerTextField;
@end

@implementation PeerViewController

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.view endEditing:YES];
}

- (IBAction)doChatPressed:(UIButton *)sender {
    NSString *peer = self.peerTextField.text;
    
    if (!peer.length) {
        return;
    }
    
    [self performSegueWithIdentifier:@"peerToChat" sender:peer];
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

@end
