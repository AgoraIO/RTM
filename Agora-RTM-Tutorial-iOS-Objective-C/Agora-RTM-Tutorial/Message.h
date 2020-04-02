//
//  Message.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/21.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, CellType) {
    CellTypeLeft,
    CellTypeRight
};

@interface Message : NSObject

@property (nonatomic, copy) NSString *userId;

// for text message
@property (nonatomic, copy) NSString *text;

// for image message
@property (nonatomic, copy) NSString *mediaId;

@end
