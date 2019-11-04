//
// Created by QITMAC000260 on 2019-02-25.
//

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include "MacApp.h"

void MacApp::initApp()
{
    if ([NSApp respondsToSelector:@selector(setAppearance:)]) {
        [NSApp setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameAqua]];
    }
}

void MacApp::AllowMinimizeForFramelessWindow(QWidget *window)
{
#if defined __APPLE__ && __MAC_OS_X_VERSION_MAX_ALLOWED >= 101300
    if(@available(macOS 10.13, *))
    {
        NSWindow* nsWindow = [(NSView*)(window->winId()) window];
        [nsWindow setStyleMask:(NSWindowStyleMaskResizable | NSWindowStyleMaskTitled | NSWindowStyleMaskFullSizeContentView | NSWindowStyleMaskMiniaturizable)];
        [nsWindow setTitlebarAppearsTransparent:YES];       // 10.10+
        [nsWindow setTitleVisibility:NSWindowTitleHidden];  // 10.10+
        [nsWindow setShowsToolbarButton:NO];
        [[nsWindow standardWindowButton:NSWindowFullScreenButton] setHidden:YES];
        [[nsWindow standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
        [[nsWindow standardWindowButton:NSWindowCloseButton] setHidden:YES];
        [[nsWindow standardWindowButton:NSWindowZoomButton] setHidden:YES];
    }
#else
    Q_UNUSED(window);
#endif
}

void MacApp::wakeUpWnd(QWidget *window) {
    NSWindow* nsWindow = [(NSView*)(window->winId()) window];
    [nsWindow makeKeyAndOrderFront:nsWindow];
}

void MacApp::showMinWnd(QWidget *window) {
    NSWindow* nsWindow = [(NSView*)(window->winId()) window];
    [nsWindow miniaturize:nil];
}

void MacApp::checkValidToVisitMicroPhone() {

#if defined __APPLE__ && __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400
    if(@available(macOS 10.14, *))
    {
        NSArray *audioDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeAudio];
        NSArray *videoDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

        AVAuthorizationStatus audioStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
        AVAuthorizationStatus videoStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    }
#endif

}
