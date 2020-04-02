//
//  ImageMessageCell.h
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Message.h"

NS_ASSUME_NONNULL_BEGIN

@interface ImageMessageCell : UITableViewCell

- (void)updateType:(CellType)type message:(Message *)message;

@end

NS_ASSUME_NONNULL_END
