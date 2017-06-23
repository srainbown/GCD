//
//  ViewController.m
//  GCD
//
//  Created by 李自杨 on 2017/5/5.
//  Copyright © 2017年 View. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    //串行队列
//    [self serialQueue];
    
    //开启子线程的串行队列
//    [self multiSerialQueue];
    
    //并发队列
//    [self concurrentQueue];
    
    //获取全局并发队列和主队列
//    [self allConcurrentQueueAndMainQueue];
    
    //生成一个后台的串行队列
//    [self changePriority];
    
    //死锁
//    [self deadLock];
    
    
    
}

//串行队列
-(void)serialQueue{
    
//    通过dispatch_queue_create函数可以创建队列，第一个函数为队列的名称，第二个参数是NULL和DISPATCH_QUEUE_SERIAL时，返回的队列就是串行队列。
    dispatch_queue_t queue = dispatch_queue_create("serial queue", NULL);
    
    for (int i = 0; i < 5; i++) {
        
        dispatch_async(queue, ^{
           
            NSLog(@"在串行队列中执行任务 %d",i);
            
        });
        
    }
    
}

//开启子线程执行串行队列
-(void)multiSerialQueue{
    
//    需要注意的是 : 一旦新创建了一个串行队列，并且使用异步函数（dispatch_async），那么系统一定会开启一个子线程，所以在使用串行队列的时候，一定只创建真正需要创建的串行队列，避免资源浪费。
    for (int i = 0; i < 5; i++) {
        
        dispatch_queue_t queue = dispatch_queue_create("different serial queue", NULL);
        
        dispatch_async(queue, ^{
            NSLog(@"在串行队列中执行任务 %d",i);
        });
        
    }
    
}

//并发队列
-(void)concurrentQueue{

    dispatch_queue_t queue = dispatch_queue_create("concurrent queue", DISPATCH_QUEUE_CONCURRENT);
    
    for (int i = 0; i < 5; i++) {
        
        dispatch_async(queue, ^{
            NSLog(@"在并发队列中执行任务 %d",i);
        });
        
    }
    
}

//获取全局并发队列和主队列
-(void)allConcurrentQueueAndMainQueue{
    
    //获取全局并发队列进行耗时操作
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        NSLog(@"全局并发队列中执行任务");
        
        //获取主队列
        dispatch_async(dispatch_get_main_queue(), ^{
            
            NSLog(@"主队列中执行任务");
            
        });
        
    });
    
}

//生成一个后台的串行队列
-(void)changePriority{

    dispatch_queue_t queue = dispatch_queue_create("queue", NULL);
    
    dispatch_queue_t bgQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
    
    //第一个参数：需要改变优先级的队列
    //第二个参数:目标队列
    dispatch_set_target_queue(queue, bgQueue);
    
}

//防止多个串行队列的并发执行
-(void)preventSerialQueueCurrent{
    
    NSMutableArray *array = [NSMutableArray array];
    
    for (int i = 0; i < 5; i++) {
        
        dispatch_queue_t serial_queue = dispatch_queue_create("serial_queue", NULL);
        
        
    }

}

//死锁
-(void)deadLock{
    
    NSLog(@"%@",[NSThread currentThread]);
    
    //sync会等到后面的block执行完成才返回，sync又在dispatch_get_main_queue()队列中，它是串行队列，sync是后加入的，前一个是主线程，所以sync想执行block必须等待主线程执行完毕，主线程等待sync返回，去执行后续内容。
    //造成死锁，sync等待mainThread执行完毕，mainThread等待sync函数返回。
    dispatch_sync(dispatch_get_main_queue(), ^{
        
        NSLog(@"sync----%@",[NSThread currentThread]);
        
    });
    
    NSLog(@"%@",[NSThread currentThread]);
    
}




























@end
