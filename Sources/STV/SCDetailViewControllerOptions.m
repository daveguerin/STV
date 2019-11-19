/*
 *  SCDetailViewControllerOptions.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDetailViewControllerOptions.h"


@interface SCDetailViewControllerOptions ()
{
    SCPresentationMode _presentationMode;
    UIModalPresentationStyle _modalPresentationStyle;
    SCNavigationBarType _navigationBarType;
    NSString *_title;
    UITableViewStyle _tableViewStyle;
    BOOL _hidesBottomBarWhenPushed;
}

@end



@implementation SCDetailViewControllerOptions

@synthesize presentationMode = _presentationMode;
@synthesize modalPresentationStyle = _modalPresentationStyle;
@synthesize navigationBarType = _navigationBarType;
@synthesize allowEditingModeCancelButton = _allowEditingModeCancelButton;
@synthesize title = _title;
@synthesize tableViewStyle = _tableViewStyle;
@synthesize hidesBottomBarWhenPushed = _hidesBottomBarWhenPushed;


- (instancetype)init
{
    if( (self=[super init]) )
    {
        _presentationMode = SCPresentationModeAuto;
        _modalPresentationStyle = UIModalPresentationFormSheet;
        _navigationBarType = SCNavigationBarTypeAuto;
        _allowEditingModeCancelButton = TRUE;
        _title = nil;
        _tableViewStyle = UITableViewStyleGrouped;
        _hidesBottomBarWhenPushed = TRUE;
    }
    return self;
}

@end


