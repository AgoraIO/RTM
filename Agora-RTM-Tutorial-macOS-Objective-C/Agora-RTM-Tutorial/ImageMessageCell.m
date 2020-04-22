//
//  ImageMessageCell.m
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

#import "ImageMessageCell.h"
#import "AgoraRtm.h"

@interface ImageMessageCell ()
@property (weak, nonatomic) IBOutlet NSTextField *leftUserLabel;
@property (weak, nonatomic) IBOutlet NSImageView *leftContentImage;

@property (weak, nonatomic) IBOutlet NSTextField *rightUserLabel;
@property (weak, nonatomic) IBOutlet NSImageView *rightContentImage;

@property (nonatomic, assign) CellType type;
@property (nonatomic, copy) NSString *user;

@property (nonatomic, assign) long long requestId;
@property (nonatomic, copy) NSString *mediaId;
@property (nonatomic, copy) NSData *imageData;

@end

@implementation ImageMessageCell
- (void)setType:(CellType)type {
    _type = type;
    BOOL rightHidden = type == CellTypeLeft ? true : false;
    
    self.rightUserLabel.hidden = rightHidden;
    self.rightContentImage.hidden = rightHidden;

    self.leftUserLabel.hidden = !rightHidden;
    self.leftContentImage.hidden = !rightHidden;
}

- (void)setUser:(NSString *)user {
    _user = [user copy];
    switch (self.type) {
        case CellTypeLeft: self.leftUserLabel.stringValue = user; break;
        case CellTypeRight: self.rightUserLabel.stringValue = user; break;
    }
}

- (void)awakeFromNib {
    [super awakeFromNib];
    self.requestId = 0;
}

- (void)updateType:(CellType)type message:(Message *)message{
    
    self.type = type;
    self.user = message.userId;
    
    // complete down
    if(self.mediaId != nil && [self.mediaId isEqualToString: message.mediaId] && self.imageData != nil){
        [self updateImageData:self.imageData];
        return;
    }
    
    // downing
    if(self.mediaId != nil && [self.mediaId isEqualToString: message.mediaId] && self.requestId > 0){
        return;
    }
        
    // stop download
    if(self.requestId > 0){
        [AgoraRtm.kit cancelMediaDownload:self.requestId completion:nil];
    }

    // start download
    self.requestId = 0;
    if (message.thumbnail != nil) {
        [self updateImageData:message.thumbnail];
    }
    
    if(message.mediaId != nil) {
        
        long long requestId;
        __weak ImageMessageCell *weakSelf = self;
        [AgoraRtm.kit downloadMediaToMemory:message.mediaId withRequest:&requestId completion:^(long long requestId, NSData *data, AgoraRtmDownloadMediaErrorCode errorCode) {
            
            // complete down
            weakSelf.requestId = 0;
            if(errorCode == AgoraRtmDownloadMediaErrorOk) {
                weakSelf.imageData = data;
                [weakSelf updateImageData:data];
            }
        }];
        
        self.requestId = requestId;
    }
    
    self.mediaId = message.mediaId;
}

- (void)updateImageData:(NSData *)imageData {

    switch (self.type) {
        case CellTypeLeft:
            self.leftContentImage.image = [[NSImage alloc] initWithData:imageData];
            self.leftContentImage.hidden = NO;
            break;
        case CellTypeRight:
            self.rightContentImage.hidden = NO;
            self.rightContentImage.image = [[NSImage alloc] initWithData:imageData];
            break;
    }
}


@end

