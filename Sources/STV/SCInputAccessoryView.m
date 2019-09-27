/*
 *  SCInputAccessoryView.m
 *  Sensible TableView
 *  Version: 13.0.0
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

@synthesize delegate = _delegate;
@synthesize toolbar = _toolbar;
@synthesize previousNextSegmentedControl = _previousNextSegmentedControl;
@synthesize doneButton = _doneButton;
@synthesize showPrevNextButtons = _showPrevNextButtons;
@synthesize showClearButton = _showClearButton;
@synthesize clearButton = _clearButton;
@synthesize rewind = _rewind;


- (instancetype)init
{
    self = [super init];
    if(self) 
    {
        self.frame = CGRectMake(0, 0, 320, 44);
        [self performInitialization];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self) 
    {
        [self performInitialization];
    }
    return self;
}

- (void)performInitialization
{
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
    
    _toolbar = [[UIToolbar alloc] initWithFrame:self.frame];
    if([SCUtilities systemVersion] < 7.0)
    {
        _toolbar.barStyle = UIBarStyleBlack;
    }
    _toolbar.translucent = TRUE;
    _toolbar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    _toolbar.items = [NSArray arrayWithObjects:segmentedItem, spacerItem, _doneButton, nil];
    [self addSubview:_toolbar];
    
    NSString *clearTitle = NSLocalizedString(@"Clear", @"Clear Button Title");
    _clearButton = [[UIBarButtonItem alloc] initWithTitle:clearTitle style:UIBarButtonItemStylePlain target:self action:@selector(clearTapped)];
}

// overrides superclass
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    
    self.toolbar.frame = self.frame;
}

- (void)setShowPrevNextButtons:(BOOL)show
{
    if(_showPrevNextButtons == show)
        return;
    
    _showPrevNextButtons = show;
    
    NSMutableArray *toolbarItems = [NSMutableArray arrayWithArray:self.toolbar.items];
    if(_showPrevNextButtons)
    {
        [toolbarItems insertObject:self.previousNextSegmentedControl atIndex:0];
    }
    else
    {
        [toolbarItems removeObjectAtIndex:0];
    }
    self.toolbar.items = toolbarItems;
}

- (void)setShowClearButton:(BOOL)show
{
    if(_showClearButton == show)
        return;
    
    _showClearButton = show;
    
    NSMutableArray *toolbarItems = [NSMutableArray arrayWithArray:self.toolbar.items];
    NSUInteger clearIndex;
    if(self.showPrevNextButtons)
        clearIndex = 1;
    else
        clearIndex = 0;
    if(_showClearButton)
    {
        [toolbarItems insertObject:self.clearButton atIndex:clearIndex];
    }
    else 
    {
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
    if([self.delegate respondsToSelector:@selector(inputAccessoryViewPreviousTapped:)])
        [self.delegate inputAccessoryViewPreviousTapped:self];
}

- (void)nextTapped
{
    if([self.delegate respondsToSelector:@selector(inputAccessoryViewNextTapped:)])
        [self.delegate inputAccessoryViewNextTapped:self];
}

- (void)clearTapped
{
    if([self.delegate respondsToSelector:@selector(inputAccessoryViewClearTapped:)])
        [self.delegate inputAccessoryViewClearTapped:self];
}

- (void)doneTapped
{
    if([self.delegate respondsToSelector:@selector(inputAccessoryViewDoneTapped:)])
        [self.delegate inputAccessoryViewDoneTapped:self];
}

@end
