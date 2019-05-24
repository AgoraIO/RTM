//
//  BasicViewController.m
//  Agora-RTM-Tutorial-Mac
//
//  Created by CavanSu on 2019/5/24.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "BasicViewController.h"

@interface BasicViewController ()

@end

@implementation BasicViewController

- (void)showAlert:(NSString * _Nonnull)message handle:(void (^ _Nullable)(NSModalResponse returnCode))handle {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = message;
    [alert addButtonWithTitle:@"OK"];
    alert.alertStyle = NSAlertStyleInformational;
    
    NSWindow *window = [NSApplication sharedApplication].windows.firstObject;
    
    if (!window) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [alert beginSheetModalForWindow:window completionHandler:handle];
    });
}

- (void)showAlert:(NSString * _Nonnull)message {
    [self showAlert:message handle:nil];
}

@end
