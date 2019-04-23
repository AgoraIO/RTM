//
//  ChatViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "ChatViewController.h"
#import "AgoraRtm.h"
#import "MessageCell.h"

@interface ChatViewController () <AgoraRtmDelegate, AgoraRtmChannelDelegate, UITextFieldDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITextField *inputTextField;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *inputViewBottom;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIView *inputContainView;

@property (strong, nonatomic) AgoraRtmChannel *channel;
@property (nonatomic, strong) NSMutableArray *list;
@end

@implementation ChatViewController

- (void)setMode:(ChatMode)mode {
    _mode = mode;
    self.title = mode.name;
    
    if (mode.type == ChatTypeGroup) {
        [self createChannel:mode.name];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self addKeyboradObserver];
    [self updateViews];
    [AgoraRtm updateDelegate:self];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self leaveChannel];
}

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

- (void)appendMsg:(NSString *)text user:(NSString *)user {
    Message *msg = [[Message alloc] init];
    msg.userId = user;
    msg.text = text;
    [self.list addObject:msg];
    
    if (self.list.count > 300) {
        [self.list removeObjectAtIndex:0];
    }
    
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
    
    NSString *name = self.mode.name;
    
    switch (self.mode.type) {
        case ChatTypePeer: [self sendPeer:name msg:text]; break;
        case ChatTypeGroup: [self sendChannelMessage:text]; break;
    }
    return YES;
}

- (void)showAlert:(NSString *)text {
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:text preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    
    __weak ChatViewController *weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf presentViewController:alert animated:YES completion:nil];
    });
}

#pragma mark - Peer
- (void)sendPeer:(NSString *)peer msg: (NSString *)msg {
    AgoraRtmMessage *message = [[AgoraRtmMessage alloc] initWithText:msg];
    
    __weak ChatViewController *weakSelf = self;
    
    [AgoraRtm.kit sendMessage:message toPeer:peer completion:^(AgoraRtmSendPeerMessageErrorCode errorCode) {
        NSLog(@"send peer msg error: %ld", (long)errorCode);
        [weakSelf appendMsg:msg user:AgoraRtm.current];
    }];
}
#pragma mark - AgoraRtmDelegate
- (void)rtmKit:(AgoraRtmKit *)kit connectionStateChanged:(AgoraRtmConnectionState)state reason:(AgoraRtmConnectionChangeReason)reason {
    NSLog(@"connection state changed: %ld", (long)reason);
}

- (void)rtmKit:(AgoraRtmKit *)kit messageReceived:(AgoraRtmMessage *)message fromPeer:(NSString *)peerId {
    [self appendMsg:message.text user:peerId];
}

#pragma mark - Channel
- (void)createChannel:(NSString *)channel {
    AgoraRtmChannel *rtmChannel = [AgoraRtm.kit createChannelWithId:channel delegate:self];
    [rtmChannel joinWithCompletion:^(AgoraRtmJoinChannelErrorCode errorCode) {
        NSLog(@"join channel error: %ld", (long)errorCode);
    }];
    self.channel = rtmChannel;
}

- (void)leaveChannel {
    if (self.mode.type == ChatTypePeer) {
        return;
    }
    
    [self.channel leaveWithCompletion:^(AgoraRtmLeaveChannelErrorCode errorCode) {
        NSLog(@"leave channel error: %ld", (long)errorCode);
    }];
}

- (void)sendChannelMessage:(NSString *)msg {
    AgoraRtmMessage *message = [[AgoraRtmMessage alloc] initWithText:msg];
    
    __weak ChatViewController *weakSelf = self;
    
    [self.channel sendMessage:message completion:^(AgoraRtmSendChannelMessageErrorCode errorCode) {
        NSLog(@"send channel msg error: %ld", (long)errorCode);
        [weakSelf appendMsg:msg user:AgoraRtm.current];
    }];
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
    [self appendMsg:message.text user:member.userId];
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
