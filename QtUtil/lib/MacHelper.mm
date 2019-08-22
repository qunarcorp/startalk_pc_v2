//
// Created by may on 2019-03-21.
//

#include "MacHelper.h"
#import <Foundation/Foundation.h>

@interface MacHelperDelegate : NSObject


+ (NSString *)command:(NSString *)cmd arguments:(NSArray *)arguments;

@end


@implementation MacHelperDelegate


+ (NSString *)command:(NSString *)cmd arguments:(NSArray *)arguments {
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:cmd];

    if ([arguments count] > 0) {
        [task setArguments:arguments];
    }

    // 新建输出管道作为Task的输出
    NSPipe *pipe = [NSPipe pipe];
    [task setStandardOutput:pipe];

//     开始task
    NSFileHandle *file = [pipe fileHandleForReading];
    [task launch];

    [task waitUntilExit];

    // 获取运行结果
    NSData *data = [file readDataToEndOfFile];
    return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}


@end

std::string MacHelper::runCommand(const char *command, char **arguments, int count) {
    NSString *cmd = [NSString stringWithUTF8String:command];

    NSMutableArray *parameters = [NSMutableArray array];

    if (arguments != nullptr && count > 0) {
        for (int i = 0; i < count; ++i) {
            char *param = arguments[i];
            NSString *argument = [NSString stringWithUTF8String:param];
            [parameters addObject:argument];
        }
    }

    NSString *result = [MacHelperDelegate command:cmd arguments:parameters];

    return std::string([result UTF8String]);
}
