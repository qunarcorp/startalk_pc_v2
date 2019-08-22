//
// Created by may on 2019-03-18.
//

#include "PlayAudioSound.h"

#import <AudioToolbox/AudioToolbox.h>

@interface PlaySoundInst : NSObject {
    NSMutableDictionary *soundIds;
}

+ (instancetype)sharedInstance;

- (void)playSoundWithPath:(NSString *)path;
@end


void QTalk::mac::util::PlayAudioSound::PlaySound(const char *path) {

    NSString *nsPath = [NSString stringWithUTF8String:path];
    [[PlaySoundInst sharedInstance] playSoundWithPath:nsPath];
}

void QTalk::mac::util::PlayAudioSound::removeSound(const char *path)
{
    NSString *nsPath = [NSString stringWithUTF8String:path];
    [[PlaySoundInst sharedInstance] unRegisterSoundWithPath:nsPath];
}

@implementation PlaySoundInst

- (id)init {
    if (self = [super init]) {
        soundIds = [[NSMutableDictionary alloc] init];
    }
    return self;
}

+ (id)sharedInstance {
    static PlaySoundInst *sharedMyManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedMyManager = [[self alloc] init];
    });
    return sharedMyManager;
}

- (void) unRegisterSoundWithPath:(NSString*) path {

    @synchronized (self) {
        NSNumber *value = soundIds[path];
        if (value != nil) {
            AudioServicesDisposeSystemSoundID([value intValue]);
            [soundIds removeObjectForKey:path];
        }

    }
}

- (SystemSoundID)registerSoundPath:(NSString *)path {
    SystemSoundID result = 0;
    @synchronized (self) {
        NSNumber *value = soundIds[path];

        if (value != nil) {
            result = static_cast<SystemSoundID>([value integerValue]);
        } else {
            SystemSoundID shake_sound_male_id = 0;
            AudioServicesCreateSystemSoundID((__bridge CFURLRef) [NSURL fileURLWithPath:path], &shake_sound_male_id);



            if (shake_sound_male_id != 0) {
                soundIds[path] = @(shake_sound_male_id);
                result = shake_sound_male_id;
            }
        }
    }

    return result;
}

- (void)playSoundWithSoundId:(SystemSoundID)soundId {
    if (soundId != 0) {
        AudioServicesPlaySystemSound(soundId);
//        AudioServicesPlaySystemSound(shake_sound_male_id);   //播放注册的声音，（此句代码，可以在本类中的任意位置调用，不限于本方法中）

        //    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);   //让手机震动
    }
}

- (void)playSoundWithPath:(NSString *)path {

    static NSTimeInterval lastPlay;

    //
    // 2秒内最多只能播放一次
    if ([[NSDate date] timeIntervalSince1970] - lastPlay > 2.0) {
        if ([path length] > 0) {
            SystemSoundID soundId = [self registerSoundPath:path];
            [self playSoundWithSoundId:soundId];
        }

        lastPlay = [[NSDate date] timeIntervalSince1970];
    }
}

@end