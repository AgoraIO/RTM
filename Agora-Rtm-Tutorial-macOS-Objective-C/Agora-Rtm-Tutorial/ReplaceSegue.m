//
//  ReplaceSegue.m
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/2/22.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "ReplaceSegue.h"

@implementation ReplaceSegue
- (void)perform {
    NSViewController *sourceVC = self.sourceController;
    sourceVC.view.window.contentViewController = self.destinationController;
}
@end
