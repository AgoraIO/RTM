//
//  ChannelViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChatViewController.h"

@interface ChannelViewController ()
@property (weak, nonatomic) IBOutlet UITextField *channelTextField;
@end

@implementation ChannelViewController

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.view endEditing:YES];
}

- (IBAction)doJoinPressed:(UIButton *)sender {
    NSString *channel = self.channelTextField.text;
    
    if (!channel.length) {
        return;
    }
    
    [self performSegueWithIdentifier:@"channelToChat" sender:channel];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    NSString *channel = (NSString *)sender;
    
    if ([segue.identifier isEqualToString:@"channelToChat"]) {
        ChatMode mode;
        mode.type = ChatTypeGroup;
        mode.name = channel;
        
        ChatViewController *chatVC = (ChatViewController *)segue.destinationViewController;
        chatVC.mode = mode;
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
}

@end
