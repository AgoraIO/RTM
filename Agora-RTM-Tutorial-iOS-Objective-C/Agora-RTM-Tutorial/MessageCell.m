//
//  MessageCell.m
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/21.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "MessageCell.h"

@interface MessageCell ()
@property (weak, nonatomic) IBOutlet UILabel *leftUserLabel;
@property (weak, nonatomic) IBOutlet UILabel *leftContentLabel;
@property (weak, nonatomic) IBOutlet UIView *leftUserBgView;
@property (weak, nonatomic) IBOutlet UIView *leftContentBgView;

@property (weak, nonatomic) IBOutlet UILabel *rightUserLabel;
@property (weak, nonatomic) IBOutlet UILabel *rightContentLabel;
@property (weak, nonatomic) IBOutlet UIView *rightUserBgView;
@property (weak, nonatomic) IBOutlet UIView *rightContentBgView;

@property (nonatomic, assign) CellType type;
@property (nonatomic, copy) NSString *user;
@property (nonatomic, copy) NSString *content;
@end

@implementation MessageCell

- (void)setType:(CellType)type {
    _type = type;
    BOOL rightHidden = type == CellTypeLeft ? true : false;
    
    self.rightUserLabel.hidden = rightHidden;
    self.rightContentLabel.hidden = rightHidden;
    
    self.rightUserBgView.hidden = rightHidden;
    self.rightContentBgView.hidden = rightHidden;
    
    self.leftUserLabel.hidden = !rightHidden;
    self.leftContentLabel.hidden = !rightHidden;
    
    self.leftUserBgView.hidden = !rightHidden;
    self.leftContentBgView.hidden = !rightHidden;
}

- (void)setUser:(NSString *)user {
    _user = [user copy];
    switch (self.type) {
        case CellTypeLeft: self.leftUserLabel.text = user; break;
        case CellTypeRight: self.rightUserLabel.text = user; break;
    }
}

- (void)setContent:(NSString *)content {
    _content = [content copy];
    switch (self.type) {
        case CellTypeLeft: self.leftContentLabel.text = content; break;
        case CellTypeRight: self.rightContentLabel.text = content; break;
    }
}

- (void)awakeFromNib {
    [super awakeFromNib];
    self.rightUserBgView.layer.cornerRadius = 20;
    self.rightContentBgView.layer.cornerRadius = 5;
    
    self.leftUserBgView.layer.cornerRadius = 20;
    self.leftContentBgView.layer.cornerRadius = 5;
}

- (void)updateType:(CellType)type message:(Message *)message {
    self.type = type;
    self.user = message.userId;
    self.content = message.text;
}

@end
