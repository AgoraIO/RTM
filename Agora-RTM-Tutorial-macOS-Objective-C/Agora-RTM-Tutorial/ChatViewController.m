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
#import "ImageMessageCell.h"

@interface ChatViewController () <AgoraRtmDelegate, AgoraRtmChannelDelegate, NSTextFieldDelegate, NSTableViewDataSource, NSTableViewDelegate>
@property (weak, nonatomic) IBOutlet NSTextField *inputTextField;
@property (weak, nonatomic) IBOutlet NSTableView *tableView;

@property (nonatomic, strong) AgoraRtmChannel *rtmChannel;
@property (nonatomic, strong) NSMutableArray *list;
@end

@implementation ChatViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self ifLoadOfflineMessages];
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
    AgoraRtmMessage *rtmMessage = [[AgoraRtmMessage alloc] initWithText:message];
    [self sendRtmMessage:rtmMessage];
}

- (IBAction)sendImage:(id)sender {
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"image" ofType:@"png"];
    long long requestId;
    
    __weak ChatViewController *weakSelf = self;
    [AgoraRtm.kit createImageMessageByUploading:imagePath withRequest:&requestId completion:^(long long requestId, AgoraRtmImageMessage *message, AgoraRtmUploadMediaErrorCode errorCode) {
        
        if(errorCode != AgoraRtmUploadMediaErrorOk) {
            NSString *alert = [NSString stringWithFormat:@"send image message error: %ld", (long)errorCode];
            [weakSelf showAlert:alert];
            return;
        }
        
        NSImage *thumbnailImage = [weakSelf generateThumbnail:imagePath toByte:8 * 1024];
        NSData *imageData = [weakSelf getJPGImageData:thumbnailImage];
        if(imageData != nil){
            message.thumbnail = imageData;
            message.thumbnailWidth = thumbnailImage.size.width;
            message.thumbnailHeight = thumbnailImage.size.height;
        }
    
        [weakSelf sendRtmMessage:message];
    }];
}

- (NSData *)getJPGImageData:(NSImage *)image {

    NSData *imageData = [image TIFFRepresentation];
    if (!imageData) {
        return nil;
    }

    NSBitmapImageRep *imageRep = [NSBitmapImageRep imageRepWithData:imageData];
    NSDictionary *imageProps = [NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:1] forKey:NSImageCompressionFactor];
    imageData = [imageRep representationUsingType:NSBitmapImageFileTypeJPEG properties:imageProps];
    return imageData;
}


- (NSImage *)generateThumbnail:(NSString *)imagePath toByte:(NSUInteger)maxLength {
    
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:imagePath];
    NSData *imageData = [self getJPGImageData:image];
    
    // If the original image is already small, no thumbnail is needed
    if(imageData.length <= maxLength) {
        return nil;
    }
    
    NSImage *resultImage = image;
    NSUInteger lastDataLength = 0;
    
    while (imageData.length > maxLength && imageData.length != lastDataLength) {
        lastDataLength = imageData.length;
        CGFloat ratio = (CGFloat)maxLength / imageData.length;
        CGSize size = CGSizeMake((NSUInteger)(resultImage.size.width * sqrtf(ratio)),
                                 (NSUInteger)(resultImage.size.height * sqrtf(ratio)));// Use NSUInteger to prevent white blank
        
        NSImage *newImage = [[NSImage alloc] initWithSize:size];
        [newImage lockFocus];
        NSRect fromRect = NSMakeRect(0, 0, resultImage.size.width, resultImage.size.height);
        NSRect toRect = NSMakeRect(0, 0, size.width, size.height);
        [resultImage drawInRect:toRect fromRect:fromRect operation:NSCompositingOperationCopy fraction:1.0];
        [newImage unlockFocus];

        imageData = [self getJPGImageData:newImage];
        resultImage = newImage;
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

- (void)appendMessage:(NSString *)text mediaId:(NSString*)mediaId thumbnail:(NSData*)thumbnail user:(NSString *)user {

    __weak ChatViewController *weakSelf = self;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        Message *msg = [[Message alloc] init];
        msg.userId = user;
        msg.text = text;
        msg.mediaId = mediaId;
        msg.thumbnail = thumbnail;
        [weakSelf.list addObject:msg];
        
        if (weakSelf.list.count > 100) {
            [weakSelf.list removeObjectAtIndex:0];
        }
        
        NSInteger end = weakSelf.list.count - 1;
        [weakSelf.tableView reloadData];
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
    if(msg.mediaId != nil) {
        ImageMessageCell *cell = [tableView makeViewWithIdentifier:@"ImageMessageCell" owner:nil];
        [cell updateType:type message:msg];
        return cell;
    } else {
        MessageCell *cell = [tableView makeViewWithIdentifier:@"MessageCell" owner:nil];
        [cell updateType:type message:msg];
        return cell;
    }
}

- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row {
    Message *msg = self.list[row];
    if(msg.mediaId != nil) {
        return 212;
    }
    return 36;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {
    NSInteger row = self.tableView.selectedRow;
    [self.tableView deselectRow:row];
}

- (NSMutableArray *)list {
    if (!_list) {
        _list = [NSMutableArray array];
    }
    return _list;
}

@end
