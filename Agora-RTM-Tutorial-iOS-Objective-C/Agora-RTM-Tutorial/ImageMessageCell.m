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
@property (weak, nonatomic) IBOutlet UILabel *leftUserLabel;
@property (weak, nonatomic) IBOutlet UIView *leftUserBgView;
@property (weak, nonatomic) IBOutlet UIImageView *leftContentImage;

@property (weak, nonatomic) IBOutlet UILabel *rightUserLabel;
@property (weak, nonatomic) IBOutlet UIView *rightUserBgView;
@property (weak, nonatomic) IBOutlet UIImageView *rightContentImage;

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
    self.rightUserBgView.hidden = rightHidden;
    self.rightContentImage.hidden = rightHidden;

    self.leftUserLabel.hidden = !rightHidden;
    self.leftUserBgView.hidden = !rightHidden;
    self.leftContentImage.hidden = !rightHidden;
}

- (void)setUser:(NSString *)user {
    _user = [user copy];
    switch (self.type) {
        case CellTypeLeft: self.leftUserLabel.text = user; break;
        case CellTypeRight: self.rightUserLabel.text = user; break;
    }
}

- (void)awakeFromNib {
    [super awakeFromNib];
    self.rightUserBgView.layer.cornerRadius = 20;
    self.leftUserBgView.layer.cornerRadius = 20;
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
                [weakSelf updateImageData:data];
            }
        }];
        
        self.requestId = requestId;
    }
    
    self.mediaId = message.mediaId;
}

- (void)updateImageData:(NSData *)imageData {

    self.imageData = imageData;

    switch (self.type) {
        case CellTypeLeft:
            self.leftContentImage.image = [UIImage imageWithData:imageData];
            self.leftContentImage.hidden = NO;
            break;
        case CellTypeRight:
            self.rightContentImage.hidden = NO;
            self.rightContentImage.image = [UIImage imageWithData:imageData];
            break;
    }
}

@end

