//
//  ChatViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "ChatViewController.h"
#import "MessageCell.h"
#import "AgoraRtm.h"

@interface ChatViewController () <AgoraRtmDelegate, AgoraRtmChannelDelegate, UITextFieldDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITextField *inputTextField;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *inputViewBottom;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIView *inputContainView;

@property (nonatomic, strong) AgoraRtmChannel *rtmChannel;
@property (nonatomic, strong) NSMutableArray *list;
@end

@implementation ChatViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self addKeyboradObserver];
    [self updateViews];
    [AgoraRtm updateDelegate:self];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    switch (self.mode.type) {
        case ChatTypePeer:  self.title = self.mode.name; break;
        case ChatTypeGroup: self.title = self.mode.name; [self createChannel:self.mode.name]; break;
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self leaveChannel];
}

#pragma mark - Send Message
- (void)sendMessage:(NSString *)message {
    NSString *typeName = (self.mode.type == ChatTypePeer ? @"peer" : @"channel");
    
    __weak ChatViewController *weakSelf = self;
    void(^sent)(int) = ^(int status) {
        if (status != 0) {
            NSString *alert = [NSString stringWithFormat:@"send %@ message error: %d", typeName, status];
            [weakSelf showAlert:alert];
            return;
        }
        
        [weakSelf appendMessage:message user:AgoraRtm.current];
    };
    
    AgoraRtmMessage *rtmMessage = [[AgoraRtmMessage alloc] initWithText:message];
    
    switch (self.mode.type) {
        case ChatTypePeer: {
            [AgoraRtm.kit sendMessage:rtmMessage toPeer:self.mode.name completion:^(AgoraRtmSendPeerMessageErrorCode errorCode) {
                sent(errorCode);
            }];
        }
        case ChatTypeGroup: {
            [self.rtmChannel sendMessage:rtmMessage completion:^(AgoraRtmSendChannelMessageErrorCode errorCode) {
                sent(errorCode);
            }];
        }
    }
}

#pragma mark - Channel
- (void)createChannel:(NSString *)channel {
    __weak ChatViewController *weakSelf = self;
    void(^errorHandle)(UIAlertAction *) = ^(UIAlertAction *action) {
        [weakSelf.navigationController popViewControllerAnimated:YES];
    };
    
    AgoraRtmChannel *rtmChannel = [AgoraRtm.kit createChannelWithId:channel delegate:self];
    
    if (!rtmChannel) {
        [self showAlert:@"join channel fail" handle:errorHandle];
    }
    
    [rtmChannel joinWithCompletion:^(AgoraRtmJoinChannelErrorCode errorCode) {
        if (errorCode != AgoraRtmJoinChannelErrorOk) {
            NSString *alert = [NSString stringWithFormat:@"join channel error: %ld", errorCode];
            [weakSelf showAlert:alert handle:errorHandle];
        }
    }];
    
    self.rtmChannel = rtmChannel;
}

- (void)leaveChannel {
    if (self.mode.type == ChatTypePeer) {
        return;
    }
    
    [self.rtmChannel leaveWithCompletion:^(AgoraRtmLeaveChannelErrorCode errorCode) {
        NSLog(@"leave channel error: %ld", (long)errorCode);
    }];
}

#pragma mark - AgoraRtmDelegate
- (void)rtmKit:(AgoraRtmKit *)kit connectionStateChanged:(AgoraRtmConnectionState)state reason:(AgoraRtmConnectionChangeReason)reason {
    NSString *message = [NSString stringWithFormat:@"connection state changed: %ld", state];
    __weak ChatViewController *weakSelf = self;
    [self showAlert:message handle:^(UIAlertAction * action) {
        if (reason == AgoraRtmConnectionChangeReasonRemoteLogin) {
            [weakSelf.navigationController popToRootViewControllerAnimated:YES];
        }
    }];
}

- (void)rtmKit:(AgoraRtmKit *)kit messageReceived:(AgoraRtmMessage *)message fromPeer:(NSString *)peerId {
    [self appendMessage:message.text user:peerId];
}

#pragma mark - AgoraRtmChannelDelegate
- (void)channel:(AgoraRtmChannel *)channel memberJoined:(AgoraRtmMember *)member {
    NSString *user = member.userId;
    NSString *text = [user stringByAppendingString:@" join"];
    [self showAlert:text];
}

- (void)channel:(AgoraRtmChannel *)channel memberLeft:(AgoraRtmMember *)member {
    NSString *user = member.userId;
    NSString *text = [user stringByAppendingString:@" left"];
    [self showAlert:text];
}

- (void)channel:(AgoraRtmChannel *)channel messageReceived:(AgoraRtmMessage *)message fromMember:(AgoraRtmMember *)member {
    [self appendMessage:message.text user:member.userId];
}

#pragma mark - UI
- (void)updateViews {
    self.tableView.rowHeight = UITableViewAutomaticDimension;
    self.tableView.estimatedRowHeight = 55;
}

- (void)addKeyboradObserver {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardFrameWillChange:)
                                                 name:UIKeyboardWillChangeFrameNotification object:nil];
}

- (void)keyboardFrameWillChange:(NSNotification *)notification {
    NSDictionary *userInfo = notification.userInfo;
    NSValue *endKeyboardFrameValue = (NSValue *)userInfo[UIKeyboardFrameEndUserInfoKey];
    NSNumber *durationValue = (NSNumber *)userInfo[UIKeyboardAnimationDurationUserInfoKey];
    
    CGRect endKeyboardFrame = endKeyboardFrameValue.CGRectValue;
    float duration = durationValue.floatValue;
    
    BOOL isShowing = (endKeyboardFrame.size.height + endKeyboardFrame.origin.y) > [UIScreen mainScreen].bounds.size.height ? NO : YES;
    
    __weak ChatViewController *weakSelf = self;
    
    [UIView animateWithDuration:duration animations:^{
        if (isShowing) {
            float offsetY = (weakSelf.inputContainView.frame.origin.y + weakSelf.inputContainView.frame.size.height) - endKeyboardFrame.origin.y;
            
            if (offsetY <= 0) {
                return;
            }
            weakSelf.inputViewBottom.constant = -offsetY;
        } else {
            weakSelf.inputViewBottom.constant = 0;
        }
        [weakSelf.view layoutIfNeeded];
    }];
}

- (void)appendMessage:(NSString *)text user:(NSString *)user {
    Message *msg = [[Message alloc] init];
    msg.userId = user;
    msg.text = text;
    [self.list addObject:msg];
    
    NSIndexPath *end = [NSIndexPath indexPathForRow:self.list.count - 1 inSection:0];
    
    __weak ChatViewController *weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf.tableView reloadData];
        [weakSelf.tableView scrollToRowAtIndexPath:end atScrollPosition:UITableViewScrollPositionBottom animated:true];
    });
}

- (BOOL)pressedReturnToSendText:(NSString *)text {
    if (!text || text.length == 0) {
        return NO;
    }

    [self sendMessage:text];
    return YES;
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.list.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    Message *msg = self.list[indexPath.row];
    CellType type = [msg.userId isEqualToString:AgoraRtm.current] ? CellTypeRight : CellTypeLeft;
    
    MessageCell *cell = [tableView dequeueReusableCellWithIdentifier:@"MessageCell" forIndexPath:indexPath];
    [cell updateType:type message:msg];
    
    return cell;
}

#pragma mark - UITextFieldDelegate
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if ([self pressedReturnToSendText:textField.text]) {
        textField.text = nil;
    } else {
        [self.view endEditing:YES];
    }
    return YES;
}

- (NSMutableArray *)list {
    if (!_list) {
        _list = [NSMutableArray array];
    }
    return _list;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end
