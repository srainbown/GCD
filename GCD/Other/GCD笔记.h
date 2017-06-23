//
//  GCD笔记.h
//  GCD
//
//  Created by 李自杨 on 2017/5/5.
//  Copyright © 2017年 View. All rights reserved.
//

#ifndef GCD___h
#define GCD___h
#define KW [UIScreen mainScreen].bounds.size.width
/*
 进程:也就是一个正在运行的应用程序。
 
 线程:进程中的某一条完整的执行路径。一个进程可以有多个线程，至少有一个线程，即主线程。在iOS开发中，所有涉及UI界面的，必须在主线程中更新。
 
 
 在iOS开发中，一共有四种多线程技术:
                            Pthread,    NSThread,   GCD,    NSOperation。
 
 Pthread,NSThread是面向线程开发的多线程技术，需要开发者自己去维护线程的生命周期。
 
 GCD,NSOperation是面向队列开发的多线程技术，开发者仅仅定义想执行的任务，追加到适当的Dispatch Queue（队列）中并设置一些优先级，依赖等操作就可以了，其他的事情可以交给系统来完成。
 
 GCD是基于C语言的API，开发者只需要将任务放在block内，并指定好追加的队列，就可以完成多线程开发。
 
 苹果官方对于GCD的解释:GCD是异步执行任务的技术之一。
 
 使用多线程开发时容易遇到的问题:
    1.多个线程更新相同的资源   :   数据竞争。
    2.多个线程相互持续等待    :   死锁。
    3.使用太多的线程导致内存消耗增大。
 
 为什么要用多线程编程:
    使用多线程变成可以保证应用程序的相应性能。如果耗时操作阻塞了主线程的RunLoop，会导致用户界面无法相应用户的操作，所以必须开启子线程将耗时操作放在子线程中处理。
 
 GCD的优点:
    GCD提供一个易于使用的并发模型而不仅仅只是锁和线程,以帮助我们避开并发陷阱。
    GCD具有在常见模式(例如单例)上用更高性能的原语优化你的代码的潜在能力。
 
 *********************************************************************************************************************************************************
 
 一.GCD的一些术语
 
    要理解GCD，要先熟悉与线程和并发相关的几个概念:
 
        1.串行(Serial)与并发(Concurrent)
            任务串行，意味着在同一时间，有且只有一个任务被执行，即一个任务执行完毕之后再执行下一个任务。
            任务并发，意味着在同一个时间，有多个任务被执行。
    
        2.同步(Synchronous)与异步(Asynchronous)
            同步，意味着在当前线程中执行任务，不具备开启新的线程的能力。
            异步，在新的线程中执行任务，具备开启新的线程的能力。
            在GCD中，这些术语描述当一个函数相对于另一个任务完成，此任务是该函数要求GCD执行的。一个同步函数只在完成了它预定的任务后才返回。
            一个异步函数，刚好相反，会立即返回，预定的任务会完成但不会等它完成，因此，一个异步函数不会阻塞当前线程去执行下一个函数。
 
        总结:
            1）同步、异步决定是否创建子线程，同步任务不会创建子线程，都是在主线程中执行的，异步任务会创建子线程。
            2）串行、并行决定创建子线程的个数，串行创建一个子线程，并行创建多个子线程(具体几个由系统决定)。
 
        3.临界区(Critical Section)
            就是一段代码不能被并发执行，也就是，两个线程不能同时执行这段代码。这很常见，因为代码去操作一个共享资源，例如一个变量若能被并发进程访问，那么它很可能变质(它的值不在可信)。
 
        4.死锁(DeadLock)
            停止等待事情的线程会导致多个线程互相维持等待，即死锁。
            例如:
                 NSLog(@"%@",[NSThread currentThread]);
                 
                 //sync会等到后面的block执行完成才返回，sync又在dispatch_get_main_queue()队列中，它是串行队列，sync是后加入的，前一个是主线程，所以sync想执行block必须等待主线程执行完毕，主线程等待sync返回，去执行后续内容。
                 //造成死锁，sync等待mainThread执行完毕，mainThread等待sync函数返回。
                 dispatch_sync(dispatch_get_main_queue(), ^{
                 
                    NSLog(@"sync----%@",[NSThread currentThread]);
                 
                 });
                 
                 NSLog(@"%@",[NSThread currentThread]);
 
            总结:禁止在主队列(iOS开发中，主队列是串行队列)中，同步使用主队列执行任务。同理，禁止在同一个同步串行队列中，再使用该串行队列同步的执行任务，因为这样会造成死锁。
 
        5.线程安全(Thread Safe)
            线程安全的代码能在多线程或并发任务中被安全的调用，而不会导致任何问题(数据损坏，崩溃，等)。线程不安全的代码在某个时刻只能在一个上下文中运行。一个线程安全代码的例子:
                NSDictionary。你可以在同一个时间在多个线程中使用它而不会有问题。
                NSMutableDictionary就不是线程安全的，应该保证一次只能有一个线程访问它。
 
        6.上下文切换(Context Switch)
            一个上下文切换指当你在单个进程里切换执行不同的线程时存储与恢复执行状态的过程。这个过程在编写多任务应用时很普遍，但会带来一些额外的开销。
 
        7.并行与并发
            并行要求并发，但并发不能保证并行，就计算机操作系统而言，开启线程时很耗性能的，也就是说，事实上，在某次并行处理任务中，开启的线程时有上限的，如果上限为2，即每次开启的新线程为2，那么就有可能出现并发却不并行的情况。
            并发代码的不同部分可以"同步"执行。然而，该怎么发生或是否发生都取决于系统。多核设备通过并行来同时执行多个线程;然而，为了使单核设备也能实现这一点，它们必须先运行一个线程，执行一个上下文切换，然后运行另一个线程或进程。这通常发生地足够快以致于给我们并发执行的错觉。
 
 
 二.队列

    GCD的队列分为三种，主队列(main)，全局队列(global)，用户创建队列(create).
 
    对于全局队列，默认有四个优先级:    
 
         #define DISPATCH_QUEUE_PRIORITY_HIGH         2             优先级最高
         #define DISPATCH_QUEUE_PRIORITY_DEFAULT      0             默认优先级，在low之前，high之后
         #define DISPATCH_QUEUE_PRIORITY_LOW          (-2)          在high和default之后执行
         #define DISPATCH_QUEUE_PRIORITY_BACKGROUND   INT16_MIN     提交到这个队列的任务会在higt优先级的任务和已经提交到background队列的执行完之后执行
 
    Dispatch Queue是执行处理的等待队列，按照任务(block)追加到队列里的顺利，先进先出执行处理。
 
    dispatch_sync:提交到队列中同步执行
    
    dispatch_async:提交到队列中异步执行，立即返回
 
    用户创建队列有两种:
        1）.Serial Dispatch Queue : 串行队列，等待当前执行任务并处理结束的队列。
        2）.Concurrent Dispatch Queue : 并发队列，不等待当前执行任务处理结束的队列。
 
    1.串行队列
 
        将任务追加带串行队列
 
         //串行队列
         
            dispatch_queue_t queue = dispatch_queue_create("serial queue", NULL);
     
            for (int i = 0; i < 5; i++) {
                //异步
                dispatch_async(queue, ^{
         
                    NSLog(@"在串行队列中执行任务 %d",i);
         
                });
 
            }
 
        通过dispatch_queue_create函数可以创建队列，第一个参数为队列的名称，第二个参数是NULL和DISPATCH_QUEUE_SERIAL时，返回的队列就是串行队列。
        
        为了避免重复代码，这里使用了for循环，将任务追加到queue中。
 
        注意 : 这里的任务是按照顺序执行的。说明任务是以阻塞的形式执行的 : 必须等待上一个任务执行结束之后才能执行现在的任务。 也就是说 : 一个Serial Dispatch Queue 中同时只能执行一个追加处理(任务block),而且系统对于一个 Serial Dispatch Queue 只生成并使用一个线程。
 
        但是，如果我们将5个任务分别追加到5个Serial Dispatch Queue 中，那么系统就会同时处理这6个任务(因为会开启子线程)
 
        //开启子线程执行串行队列
 
        for (int i = 0; i < 5; i++) {
         
            dispatch_queue_t queue = dispatch_queue_create("different serial queue", NULL);
         
            dispatch_async(queue, ^{
                NSLog(@"在串行队列中执行任务 %d",i);
            });
         
        }
 
        需要注意的是 : 一旦新创建了一个串行队列，并且使用异步函数（dispatch_async），那么系统一定会开启一个子线程，所以在使用串行队列的时候，一定只创建真正需要创建的串行队列，避免资源浪费。
 
    2.并行队列
    
        将任务追加带并行队列
 
        //并行队列
 
            dispatch_queue_t queue = dispatch_queue_create("concurrent queue", DISPATCH_QUEUE_CONCURRENT);

            for (int i = 0; i < 5; i++) {
                //异步
                dispatch_async(queue, ^{

                NSLog(@"在并发队列中执行任务 %d",i);

                });
         
            }
 
        可以看到，dispatch_queue_create函数的第二个参数是DISPATCH_QUEUE_CONCURRENT。这里追加到并发队列的5个任务并不是按照顺序执行的，符合并行队列的定义。
 
        iOS和OSX基于Dispatch Queue中的处理数，CPU核数，已经CPU负荷等当前系统的状态来决定Concurrent Dispatch Queue中并行处理的任务数。
 
    3.队列的命名
 
        现在我们知道dispatch_queue_create方法的第一个参数指定了这个新建队列的名称，推荐使用逆序全程域名(FQDN,fully qualified domain name)。这个名称可以在Xcode和CrashLog中显示出来，对bug的追中很有帮助。
    
    4.总结：将任务追加到多个串行队列会使这几个任务在不同的线程中执行。
 
    5.系统提供了两中特殊的队列，分别对应串行队列和并发队里
 
        系统提供的队列:
 
            1)Main Dispatch Queue : 主队列，放在这个队列里的任务会追加到主线程的RunLoop中执行。需要刷新UI的时候我们可以直接获取这个队列，将任务追加到这个队列中。
 
            2）Globle Dispatch Queue : 全局并发队列，开发者可以不需要特意通过dispatch_queue_create方法创建一个Concurrent Dispatch Queue,可以将任务直接放在这个全局并发队列里面。
 
             //获取全局并发队列和主队列进行耗时操作
                 dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                 
                    NSLog(@"全局并发队列中执行任务");
                 
                    //获取主队列
                    dispatch_async(dispatch_get_main_queue(), ^{
                 
                        NSLog(@"主队列中执行任务");
                 
                    });
                 
                 });
 
 三.GCD中的各种函数
 
    1.dispatch_set_target_queue
 
        这个函数有两个作用 :
     
                        1)改变队列的优先级
     
                        2)防止多个串行队列的并发执行
 
        1)改变队列的优先级
 
            dispatch_queue_create方法生成的串行队列和并发队列的优先级都是与默认优先级的Globle Dispatch Queue一致。
            
            如果想要变更某个队列的优先级，需要使用dispatch_set_target_queue函数

            //创建一个在后台执行动作处理的Serial Dispatch Queue
 
             dispatch_queue_t queue = dispatch_queue_create("queue", NULL);
             
             dispatch_queue_t bgQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
             
             //第一个参数：需要改变优先级的队列
             //第二个参数:目标队列
             dispatch_set_target_queue(queue, bgQueue);
 
        2)防止多个串行队列的并发执行
 
            有时，我们将不能并发执行的处理追加到多个Serial Dispatch Queue中时，可以使用dispatch_set_target_queue函数将目标函数定为某个Serial Dispatch Queue，就可以防止这些处理的并发执行。
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 */

#endif /* GCD___h */
