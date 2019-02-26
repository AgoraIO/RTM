//
//  MessageCell.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/21.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Message.h"

typedef NS_ENUM(NSInteger, CellType) {
    CellTypeLeft,
    CellTypeRight
};

@interface MessageCell : UITableViewCell
- (void)updateType:(CellType)type message:(Message *)message;
@end
