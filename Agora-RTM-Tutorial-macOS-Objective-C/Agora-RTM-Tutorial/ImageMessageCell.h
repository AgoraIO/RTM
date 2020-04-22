//
//  ImageMessageCell.h
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Message.h"

NS_ASSUME_NONNULL_BEGIN

@interface ImageMessageCell : NSTableCellView

- (void)updateType:(CellType)type message:(Message *)message;

@end

NS_ASSUME_NONNULL_END
