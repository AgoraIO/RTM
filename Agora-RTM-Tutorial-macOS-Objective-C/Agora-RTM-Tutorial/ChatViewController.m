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

@property (nonatomic, strong) AgoraRtmChannel *rtmChannel;
@property (nonatomic, strong) NSMutableArray *list;
@end

@implementation ChatViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [AgoraRtm updateDelegate:self];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    switch (self.mode.type) {
        case ChatTypePeer:  self.title = self.mode.name; break;
        case ChatTypeGroup: self.title = self.mode.name; [self createChannel:self.mode.name]; break;
    }
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
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
    void(^errorHandle)(NSModalResponse) = ^(NSModalResponse returnCode) {
        [weakSelf.delegate chatVCWillClose:weakSelf];
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
    NSLog(@"connection state changed: %ld", (long)reason);
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

- (void)appendMessage:(NSString *)text user:(NSString *)user {
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
    
    [self sendMessage:text];
    return YES;
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

@end
