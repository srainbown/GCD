//
//  AppDelegate.m
//  GCD
//
//  Created by 李自杨 on 2017/5/5.
//  Copyright © 2017年 View. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    self.window = [[UIWindow alloc]initWithFrame:[UIScreen mainScreen].bounds];
    
    [self.window makeKeyAndVisible];
    
    ViewController *vc = [[ViewController alloc]init];
    
    self.window.rootViewController = vc;
    
    return YES;
}


@end
