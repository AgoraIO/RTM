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

@interface ChatViewController () <AgoraRtmDelegate, AgoraRtmChannelDelegate, NSTextFieldDelegate, NSTableViewDataSource, NSTableViewDelegate>
@property (weak, nonatomic) IBOutlet NSTextField *inputTextField;
@property (weak, nonatomic) IBOutlet NSTableView *tableView;

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
    [AgoraRtm updateDelegate:self];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self leaveChannel];
}

- (void)appendMsg:(NSString *)text user:(NSString *)user {
    Message *msg = [[Message alloc] init];
    msg.userId = user;
    msg.text = text;
    [self.list addObject:msg];
    
    NSInteger end = self.list.count - 1;
    
    __weak ChatViewController *weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf.tableView insertRowsAtIndexes:[NSIndexSet indexSetWithIndex:end] withAnimation:NSTableViewAnimationSlideUp];
        [weakSelf.tableView scrollRowToVisible:end];
    });
}

- (BOOL)pressedReturnToSendText:(NSString *)text {
    if (!text || text.length == 0) {
        return NO;
    }
    
    NSString *name = self.mode.name;
    
    switch (self.mode.type) {
        case ChatTypePeer: [self sendPeer:name msg:text]; break;
        case ChatTypeGroup: [self sendChannel:name msg:text]; break;
    }
    return YES;
}

- (void)showAlert:(NSString *)text {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = text;
    [alert addButtonWithTitle:@"OK"];
    alert.alertStyle = NSAlertStyleInformational;
    
    NSWindow *window = [NSApplication sharedApplication].windows.firstObject;
    
    if (!window) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [alert beginSheetModalForWindow:window completionHandler:nil];
    });
}

- (IBAction)doBackPressed:(NSButton *)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(chatVCWillClose:)]) {
        [self.delegate chatVCWillClose:self];
    }
}

- (IBAction)doSendMsgPressed:(NSTextField *)sender {
    if ([self pressedReturnToSendText:sender.stringValue]) {
        sender.stringValue = @"";
    }
}

#pragma mark - Peer
- (void)sendPeer:(NSString *)peer msg: (NSString *)msg {
    AgoraRtmMessage *message = [[AgoraRtmMessage alloc] initWithText:msg];
    
    __weak ChatViewController *weakSelf = self;
    
    [AgoraRtm.kit sendMessage:message toPeer:peer completion:^(AgoraRtmSendChannelMessageState state) {
        NSLog(@"send peer msg error: %ld", (long)state);
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
}

- (void)leaveChannel {
    if (self.mode.type == ChatTypePeer) {
        return;
    }
    
    NSString *channelId = self.mode.name;
    AgoraRtmChannel *channel = AgoraRtm.kit.channels[channelId];
    
    [channel leaveWithCompletion:^(AgoraRtmLeaveChannelErrorCode errorCode) {
        NSLog(@"leave channel error: %ld", (long)errorCode);
    }];
}

- (void)sendChannel:(NSString *)channel msg:(NSString *)msg {
    AgoraRtmChannel *rtmChannel = AgoraRtm.kit.channels[channel];
    AgoraRtmMessage *message = [[AgoraRtmMessage alloc] initWithText:msg];
    
    __weak ChatViewController *weakSelf = self;
    
    [rtmChannel sendMessage:message completion:^(AgoraRtmSendChannelMessageState state) {
        NSLog(@"send channel msg error: %ld", (long)state);
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

#pragma mark - NSTableViewDataSource, NSTableViewDelegate
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return self.list.count;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    Message *msg = self.list[row];
    CellType type = [msg.userId isEqualToString:AgoraRtm.current] ? CellTypeRight : CellTypeLeft;
    
    MessageCell *cell = [tableView makeViewWithIdentifier:@"MessageCell" owner:self];
    [cell updateType:type message:msg];
    
    return cell;
}

- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row {
    CGFloat defaultHeight = 39;
    CGFloat viewWidth = self.view.bounds.size.width - (32 + 16 + 16 + 50);
    Message *msg = self.list[row];
    
    NSString *text = msg.text;
    
    
    CGRect textRect = [text boundingRectWithSize:NSMakeSize(viewWidth, 0)
                                         options:NSStringDrawingUsesLineFragmentOrigin
                                      attributes:@{NSFontAttributeName: [NSFont systemFontOfSize:13]}];
                       
                       
    CGFloat textHeight = textRect.size.height + 6 + 6;
    
    if (textHeight <= defaultHeight) {
        textHeight = defaultHeight;
    }
    return textHeight;
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
