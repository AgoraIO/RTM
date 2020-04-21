//
//  ChatViewController.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/19.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "ChatViewController.h"
#import "MessageCell.h"
#import "ImageMessageCell.h"
#import "AgoraRtm.h"

@interface ChatViewController () <AgoraRtmDelegate, AgoraRtmChannelDelegate, UITextFieldDelegate, UITableViewDataSource, UITableViewDelegate>
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
    [self ifLoadOfflineMessages];
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
    AgoraRtmMessage *rtmMessage = [[AgoraRtmMessage alloc] initWithText:message];
    [self sendRtmMessage:rtmMessage];
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
    [self appendMessage:message.text mediaId:nil thumbnail:nil user:peerId];
}

- (void)rtmKit:(AgoraRtmKit *)kit imageMessageReceived:(AgoraRtmImageMessage *)message fromPeer:(NSString *)peerId {
    [self appendMessage:nil mediaId:message.mediaId thumbnail:message.thumbnail user:peerId];
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
    [self appendMessage:message.text mediaId:nil thumbnail:nil user:member.userId];
}

-(void)channel:(AgoraRtmChannel *)channel imageMessageReceived:(AgoraRtmImageMessage *)message fromMember:(AgoraRtmMember *)member {
    [self appendMessage:nil mediaId:message.mediaId thumbnail:message.thumbnail user:member.userId];
}


#pragma mark - UI
- (void)ifLoadOfflineMessages {
    switch (self.mode.type) {
        case ChatTypePeer: {
            NSArray *list = [AgoraRtm getOfflineMessagesFromUser:self.mode.name];
            
            for (AgoraRtmMessage *item in list) {
                if(item.type == AgoraRtmMessageTypeText){
                    [self appendMessage:item.text mediaId:nil thumbnail:nil user:self.mode.name];
                } else if(item.type == AgoraRtmMessageTypeImage){
                    AgoraRtmImageMessage *imageMessage = (AgoraRtmImageMessage*)item;
                    [self appendMessage:nil mediaId:imageMessage.mediaId thumbnail:imageMessage.thumbnail user:self.mode.name];
                }
            }
            
            [AgoraRtm removeOfflineMessageFromUser:self.mode.name];
        }
        default:
            break;
    }
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

- (void)appendMessage:(NSString *)text mediaId:(NSString*)mediaId thumbnail:(NSData*)thumbnail user:(NSString *)user {

    dispatch_async(dispatch_get_main_queue(), ^{
        Message *msg = [[Message alloc] init];
        msg.userId = user;
        msg.text = text;
        msg.mediaId = mediaId;
        msg.thumbnail = thumbnail;
        [self.list addObject:msg];
        
        if (self.list.count > 300) {
            [self.list removeObjectAtIndex:0];
        }

        [self.tableView reloadData];
        
        NSIndexPath *end = [NSIndexPath indexPathForRow:self.list.count - 1 inSection:0];
        [self.tableView scrollToRowAtIndexPath:end atScrollPosition:UITableViewScrollPositionBottom animated:true];
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

    if(msg.mediaId != nil) {

        ImageMessageCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ImageMessageCell" forIndexPath:indexPath];
        [cell updateType:type message:msg];
        return cell;
    } else {
        MessageCell *cell = [tableView dequeueReusableCellWithIdentifier:@"MessageCell" forIndexPath:indexPath];
        [cell updateType:type message:msg];
        return cell;
    }
}

- (IBAction)sendImage:(id)sender {
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"image" ofType:@"png"];
    long long requestId;
    
    __weak ChatViewController *weakSelf = self;
    [AgoraRtm.kit createImageMessageByUploading:imagePath withRequest:&requestId completion:^(long long requestId, AgoraRtmImageMessage *message, AgoraRtmUploadMediaErrorCode errorCode) {
        
        // thumbnailImage 5KB
        UIImage *thumbnailImage = [weakSelf generateThumbnail:imagePath toByte:5 * 1024];
        if(thumbnailImage != nil) {
            NSData *imageData = UIImageJPEGRepresentation(thumbnailImage, 1);
            message.thumbnail = imageData;
            message.thumbnailWidth = thumbnailImage.size.width;
            message.thumbnailHeight = thumbnailImage.size.height;
        }
        
        if(errorCode != AgoraRtmUploadMediaErrorOk) {
            NSString *alert = [NSString stringWithFormat:@"send image message error: %ld", (long)errorCode];
            [weakSelf showAlert:alert];
            return;
        }

        [weakSelf sendRtmMessage:message];
    }];
}

- (UIImage *)generateThumbnail:(NSString *)imagePath toByte:(NSUInteger)maxLength {
    UIImage *image = [UIImage imageWithContentsOfFile: imagePath];
    NSData *data = UIImageJPEGRepresentation(image, 1);
    
    // If the original image is already small, no thumbnail is needed
    if(data.length <= maxLength) {
        return nil;
    }
    
    UIImage *resultImage = image;
    NSUInteger lastDataLength = 0;
    while (data.length > maxLength && data.length != lastDataLength) {
        lastDataLength = data.length;
        CGFloat ratio = (CGFloat)maxLength / data.length;
        CGSize size = CGSizeMake((NSUInteger)(resultImage.size.width * sqrtf(ratio)),
                                 (NSUInteger)(resultImage.size.height * sqrtf(ratio)));// Use NSUInteger to prevent white blank
        UIGraphicsBeginImageContext(size);
        // Use image to draw (drawInRect:), image is larger but more compression time
        // Use result image to draw, image is smaller but less compression time
        [resultImage drawInRect:CGRectMake(0, 0, size.width, size.height)];
        resultImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        data = UIImageJPEGRepresentation(resultImage, 1);
    }
    return resultImage;
}

- (void)sendRtmMessage:(AgoraRtmMessage *)rtmMessage {
    
    NSString *typeName = (self.mode.type == ChatTypePeer ? @"peer" : @"channel");
    
    __weak ChatViewController *weakSelf = self;
    void(^sent)(int) = ^(int status) {
        if (status != 0) {
            NSString *alert = [NSString stringWithFormat:@"send %@ message error: %d", typeName, status];
            [weakSelf showAlert:alert];
            return;
        }
        
        if(rtmMessage.type == AgoraRtmMessageTypeText) {
            [weakSelf appendMessage:rtmMessage.text mediaId:nil thumbnail:nil user:AgoraRtm.current];
            
        } else if(rtmMessage.type == AgoraRtmMessageTypeImage) {
            AgoraRtmImageMessage *imageMessage = (AgoraRtmImageMessage*)rtmMessage;
            [weakSelf appendMessage:nil mediaId:imageMessage.mediaId thumbnail:imageMessage.thumbnail user:AgoraRtm.current];
        }
    };

    switch (self.mode.type) {
        case ChatTypePeer: {
            AgoraRtmSendMessageOptions *option = [[AgoraRtmSendMessageOptions alloc] init];
            option.enableOfflineMessaging = [AgoraRtm oneToOneMessageType] == OneToOneMessageTypeOffline ? YES : NO;
           
            [AgoraRtm.kit sendMessage:rtmMessage toPeer:self.mode.name
                   sendMessageOptions:option
                           completion:^(AgoraRtmSendPeerMessageErrorCode errorCode) {
                
                sent((int)errorCode);
            }];
        }
        case ChatTypeGroup: {
            [self.rtmChannel sendMessage:rtmMessage completion:^(AgoraRtmSendChannelMessageErrorCode errorCode) {
                sent((int)errorCode);
            }];
        }
    }
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
