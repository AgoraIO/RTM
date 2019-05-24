//
//  BasicViewController.m
//  Agora-RTM-Tutorial
//
//  Created by CavanSu on 2019/5/16.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "BasicViewController.h"

@interface BasicViewController ()

@end

@implementation BasicViewController

- (void)showAlert:(NSString * _Nonnull)message handle:(void (^_Nullable)(UIAlertAction * _Nullable))handle {
    [self.view endEditing:true];
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:message preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:handle];
    [alert addAction:action];
    [self presentViewController:alert animated:true completion:nil];
}

- (void)showAlert:(NSString * _Nonnull)message; {
    [self showAlert:message handle:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.navigationController.interactivePopGestureRecognizer.enabled = false;
}

@end
