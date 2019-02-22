//
//  MessageCell.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/21.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "MessageCell.h"

@interface MessageCell ()
@property (weak, nonatomic) IBOutlet NSTextField *leftUserLabel;
@property (weak, nonatomic) IBOutlet NSTextField *leftContentLabel;

@property (weak, nonatomic) IBOutlet NSTextField *rightUserLabel;
@property (weak, nonatomic) IBOutlet NSTextField *rightContentLabel;

@property (nonatomic, assign) CellType type;
@property (nonatomic, copy) NSString *user;
@property (nonatomic, copy) NSString *content;
@end

@implementation MessageCell

- (void)awakeFromNib {
    [super awakeFromNib];
    
    self.rightContentLabel.usesSingleLineMode = NO;
    self.rightContentLabel.cell.wraps = YES;
    self.rightContentLabel.cell.scrollable = NO;
    
    self.leftContentLabel.usesSingleLineMode = NO;
    self.leftContentLabel.cell.wraps = YES;
    self.leftContentLabel.cell.scrollable = NO;
}

- (void)setType:(CellType)type {
    _type = type;
    BOOL rightHidden = type == CellTypeLeft ? true : false;
    
    self.rightUserLabel.hidden = rightHidden;
    self.rightContentLabel.hidden = rightHidden;
    
    self.leftUserLabel.hidden = !rightHidden;
    self.leftContentLabel.hidden = !rightHidden;
}

- (void)setUser:(NSString *)user {
    _user = [user copy];
    switch (self.type) {
        case CellTypeLeft: self.leftUserLabel.stringValue = user; break;
        case CellTypeRight: self.rightUserLabel.stringValue = user; break;
    }
}

- (void)setContent:(NSString *)content {
    _content = [content copy];
    switch (self.type) {
        case CellTypeLeft: self.leftContentLabel.stringValue = content; break;
        case CellTypeRight: self.rightContentLabel.stringValue = content; break;
    }
}

- (void)updateType:(CellType)type message:(Message *)message {
    self.type = type;
    self.user = message.userId;
    self.content = message.text;
}

@end
