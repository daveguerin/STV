/*
 *  SCInputAccessoryView.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCInputAccessoryView.h"

#import "SCGlobals.h"


@interface SCInputAccessoryView ()
{
    __weak id<SCInputAccessoryViewDelegate> _delegate;
    UIToolbar *_toolbar;
    UISegmentedControl *_previousNextSegmentedControl;
    UIBarButtonItem *_doneButton;
    BOOL _showPrevNextButtons;
    BOOL _showClearButton;
    UIBarButtonItem *_clearButton;
    BOOL _rewind;
}

- (void)performInitialization;
- (void)previousNextSegmentedControlValueChanged:(id)sender;

@end


@implementation SCInputAccessoryView



- (instancetype)init
{
    return [self initWithFrame:CGRectZero];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    return [self initWithFrame:frame inputViewStyle:UIInputViewStyleKeyboard];
}

- (instancetype)initWithFrame:(CGRect)frame inputViewStyle:(UIInputViewStyle)inputViewStyle
{
    self = [super initWithFrame:frame inputViewStyle:inputViewStyle];
    if (self) {
        [self performInitialization];
    }

    return self;
}

- (void)performInitialization
{
    self.allowsSelfSizing = YES;
    /*
    hermes:
    UIKit requires this native input-root hint to use the intrinsic height.
    Without it the keyboard reserves zero height despite valid constraints.
    The toolbar itself uses constraints; STV never assigns either frame.
    */
    self.autoresizingMask = UIViewAutoresizingFlexibleHeight;
    _delegate = nil;
    _rewind = YES;
    _showPrevNextButtons = TRUE;
    _showClearButton = FALSE;
    
    NSString *previousTitle = NSLocalizedString(@"Previous", @"Previous Button Title");
    NSString *nextTitle = NSLocalizedString(@"Next", @"Next Button Title");
    _previousNextSegmentedControl = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:previousTitle, nextTitle, nil]];
    _previousNextSegmentedControl.momentary = YES;
    [_previousNextSegmentedControl addTarget:self action:@selector(previousNextSegmentedControlValueChanged:) forControlEvents:UIControlEventValueChanged];
    UIBarButtonItem *segmentedItem = [[UIBarButtonItem alloc] initWithCustomView:_previousNextSegmentedControl];
    
    UIBarButtonItem *spacerItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
    
    _doneButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(doneTapped)];
    
    _toolbar = [[UIToolbar alloc] init];
    _toolbar.translucent = TRUE;
    _toolbar.translatesAutoresizingMaskIntoConstraints = NO;
    _toolbar.items = [NSArray arrayWithObjects:segmentedItem, spacerItem, _doneButton, nil];
    [self addSubview:_toolbar];
    /*
    hermes:
    The keyboard owns the accessory width. Constraints fit its toolbar in
    local coordinates, including when the accessory has a nonzero origin.
    */
    [NSLayoutConstraint activateConstraints:@[
        [_toolbar.leadingAnchor constraintEqualToAnchor:self.leadingAnchor],
        [_toolbar.trailingAnchor constraintEqualToAnchor:self.trailingAnchor],
        [_toolbar.topAnchor constraintEqualToAnchor:self.topAnchor],
        [_toolbar.bottomAnchor constraintEqualToAnchor:self.bottomAnchor],
        [_toolbar.heightAnchor constraintGreaterThanOrEqualToConstant:44.0]
    ]];
    
    NSString *clearTitle = NSLocalizedString(@"Clear", @"Clear Button Title");
    _clearButton = [[UIBarButtonItem alloc] initWithTitle:clearTitle style:UIBarButtonItemStylePlain target:self action:@selector(clearTapped)];
}

- (CGSize)intrinsicContentSize
{
    return CGSizeMake(UIViewNoIntrinsicMetric,
                      MAX(44.0, self.toolbar.intrinsicContentSize.height));
}

- (void)setShowPrevNextButtons:(BOOL)show
{
    if (_showPrevNextButtons == show) {
        return;
    }

    _showPrevNextButtons = show;
    
    NSMutableArray *toolbarItems = [NSMutableArray arrayWithArray:self.toolbar.items];
    if (_showPrevNextButtons) {
        [toolbarItems insertObject:self.previousNextSegmentedControl atIndex:0];
    }

    else {
        [toolbarItems removeObjectAtIndex:0];
    }

    self.toolbar.items = toolbarItems;
}

- (void)setShowClearButton:(BOOL)show
{
    if (_showClearButton == show) {
        return;
    }

    _showClearButton = show;
    
    NSMutableArray *toolbarItems = [NSMutableArray arrayWithArray:self.toolbar.items];
    NSUInteger clearIndex;
    if (self.showPrevNextButtons) {
        clearIndex = 1;
    }

    else {
        clearIndex = 0;
    }

    if (_showClearButton) {
        [toolbarItems insertObject:self.clearButton atIndex:clearIndex];
    }

    else {
        [toolbarItems removeObjectAtIndex:clearIndex];
    }

    self.toolbar.items = toolbarItems;
}

- (void)previousNextSegmentedControlValueChanged:(id)sender
{
    UISegmentedControl *segmentedControl = (UISegmentedControl *)sender;
    
    switch(segmentedControl.selectedSegmentIndex)
    {
        case 0:
            [self previousTapped];
            break;
        
        case 1:
            [self nextTapped];
            break;
    }
}

- (void)previousTapped
{
    if ([self.delegate respondsToSelector:@selector(inputAccessoryViewPreviousTapped:)]) {
        [self.delegate inputAccessoryViewPreviousTapped:self];
    }

}

- (void)nextTapped
{
    if ([self.delegate respondsToSelector:@selector(inputAccessoryViewNextTapped:)]) {
        [self.delegate inputAccessoryViewNextTapped:self];
    }

}

- (void)clearTapped
{
    if ([self.delegate respondsToSelector:@selector(inputAccessoryViewClearTapped:)]) {
        [self.delegate inputAccessoryViewClearTapped:self];
    }

}

- (void)doneTapped
{
    if ([self.delegate respondsToSelector:@selector(inputAccessoryViewDoneTapped:)]) {
        [self.delegate inputAccessoryViewDoneTapped:self];
    }

}

@end
