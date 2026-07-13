#include "cocoa_window.h"

#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>

#include "anchorpoint/core/log.h"

// Window delegate that flips a flag when the window is closed, so the main
// loop can stop (the C++ side polls this through PumpEvents).
@interface APWindowDelegate : NSObject <NSWindowDelegate>
@property(nonatomic, assign) bool* shouldClose;
@end

@implementation APWindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    if (self.shouldClose) *self.shouldClose = true;
    return YES;
}
@end

namespace ap {

struct CocoaWindow::Impl {
    NSWindow* window = nil;
    NSView* view = nil;
    APWindowDelegate* delegate = nil;
    Input* input = nullptr;
    int width = 0;
    int height = 0;
    bool cursorCaptured = false;
    bool shouldClose = false;
};

namespace {

// Hardware key codes (stable virtual key codes from <Carbon/HIToolbox/Events.h>,
// duplicated here so we don't depend on the Carbon headers).
constexpr unsigned short kVK_Return = 0x24;
constexpr unsigned short kVK_Tab = 0x30;
constexpr unsigned short kVK_Space = 0x31;
constexpr unsigned short kVK_Escape = 0x35;
constexpr unsigned short kVK_LeftArrow = 0x7B;
constexpr unsigned short kVK_RightArrow = 0x7C;
constexpr unsigned short kVK_DownArrow = 0x7D;
constexpr unsigned short kVK_UpArrow = 0x7E;

Key TranslateKey(NSEvent* event) {
    switch (event.keyCode) {
        case kVK_Escape:     return Key::Escape;
        case kVK_Space:      return Key::Space;
        case kVK_Return:     return Key::Enter;
        case kVK_Tab:        return Key::Tab;
        case kVK_LeftArrow:  return Key::Left;
        case kVK_RightArrow: return Key::Right;
        case kVK_DownArrow:  return Key::Down;
        case kVK_UpArrow:    return Key::Up;
        case 56: case 60:    return Key::Shift;   // left / right shift
        case 59: case 62:    return Key::Control; // left / right control
        case 58: case 61:    return Key::Alt;     // left / right option
        default: break;
    }

    NSString* characters = event.charactersIgnoringModifiers;
    if (characters.length > 0) {
        const unichar c = [characters characterAtIndex:0];
        if (c >= 'a' && c <= 'z') return static_cast<Key>(static_cast<int>(Key::A) + (c - 'a'));
        if (c >= 'A' && c <= 'Z') return static_cast<Key>(static_cast<int>(Key::A) + (c - 'A'));
        if (c >= '0' && c <= '9') return static_cast<Key>(static_cast<int>(Key::Num0) + (c - '0'));
    }
    return Key::Unknown;
}

} // namespace

CocoaWindow::CocoaWindow() : impl(std::make_unique<Impl>()) {}
CocoaWindow::~CocoaWindow() = default;

bool CocoaWindow::Create(const EngineConfig& config, Input& input) {
    @autoreleasepool {
        impl->input = &input;
        impl->width = config.width;
        impl->height = config.height;

        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        const NSRect frame = NSMakeRect(0, 0, config.width, config.height);
        const NSWindowStyleMask style = NSWindowStyleMaskTitled
                                      | NSWindowStyleMaskClosable
                                      | NSWindowStyleMaskMiniaturizable
                                      | NSWindowStyleMaskResizable;

        impl->window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:style
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
        if (impl->window == nil) {
            LogError("Failed to create Cocoa window");
            return false;
        }

        [impl->window setTitle:@(config.title.c_str())];
        [impl->window center];
        [impl->window setAcceptsMouseMovedEvents:YES];

        // A layer-backed view that the Metal renderer hangs a CAMetalLayer on.
        impl->view = [[NSView alloc] initWithFrame:frame];
        impl->view.wantsLayer = YES;
        [impl->window setContentView:impl->view];

        impl->delegate = [[APWindowDelegate alloc] init];
        impl->delegate.shouldClose = &impl->shouldClose;
        [impl->window setDelegate:impl->delegate];

        [NSApp finishLaunching];
        return true;
    }
}

void CocoaWindow::Show() {
    @autoreleasepool {
        [impl->window makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
    }
}

bool CocoaWindow::PumpEvents() {
    @autoreleasepool {
        if (impl->shouldClose) return false;

        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            switch (event.type) {
                case NSEventTypeKeyDown:
                    if (!event.isARepeat && impl->input) {
                        const Key key = TranslateKey(event);
                        if (key != Key::Unknown) impl->input->SetKeyDown(key, true);
                    }
                    break;
                case NSEventTypeKeyUp:
                    if (impl->input) {
                        const Key key = TranslateKey(event);
                        if (key != Key::Unknown) impl->input->SetKeyDown(key, false);
                    }
                    break;
                case NSEventTypeMouseMoved:
                case NSEventTypeLeftMouseDragged:
                case NSEventTypeRightMouseDragged:
                    if (impl->input && impl->cursorCaptured)
                        impl->input->AddMouseDelta(static_cast<float>(event.deltaX),
                                                   static_cast<float>(event.deltaY));
                    break;
                default:
                    break;
            }
            [NSApp sendEvent:event];
        }
        return !impl->shouldClose;
    }
}

void CocoaWindow::SetCursorCaptured(bool captured) {
    if (captured == impl->cursorCaptured) return;
    impl->cursorCaptured = captured;

    if (captured) {
        CGDisplayHideCursor(kCGDirectMainDisplay);
        // Decouple the hardware mouse from the cursor so it can't escape the
        // window; NSEvent.deltaX/deltaY still report relative movement.
        CGAssociateMouseAndMouseCursorPosition(false);
    } else {
        CGAssociateMouseAndMouseCursorPosition(true);
        CGDisplayShowCursor(kCGDirectMainDisplay);
    }
}

void* CocoaWindow::GetNativeHandle() const {
    return (__bridge void*)impl->view;
}

} // namespace ap
