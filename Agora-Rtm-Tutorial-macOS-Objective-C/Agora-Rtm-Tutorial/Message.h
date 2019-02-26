//
//  Message.h
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/2/21.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Message : NSObject
@property (nonatomic, copy) NSString *userId;
@property (nonatomic, copy) NSString *text;
@end
