/*
 *  SCExpandCollapseCell.m
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCExpandCollapseCell.h"

#import "SCTableViewSection.h"



@interface SCExpandCollapseAccessoryView : UIView

@property (nonatomic, weak) SCExpandCollapseCell *ownerExpandCollapseCell;

@end

@implementation SCExpandCollapseAccessoryView

@synthesize ownerExpandCollapseCell = _ownerExpandCollapseCell;

- (instancetype)init
{
    if( (self=[super init]) )
    {
        _ownerExpandCollapseCell = nil;
        self.frame = CGRectMake(0.0f, 0.0f, 15.0f, 15.0f);
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    // draw the arrow
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSaveGState(context);
    
    CGFloat arrowRadius = 4.5f;
    CGFloat x = CGRectGetMaxX(self.bounds) - 7.0f;
    
    if(self.ownerExpandCollapseCell.ownerSectionExpanded)
    {
        CGFloat y = CGRectGetMinY(self.bounds) + 5.0f;
        
        CGContextMoveToPoint(context, x-arrowRadius, y+arrowRadius);
        CGContextAddLineToPoint(context, x, y);
        CGContextAddLineToPoint(context, x+arrowRadius, y+arrowRadius);
    }
    else 
    {
        CGFloat y = CGRectGetMaxY(self.bounds) - 5.0f;
        
        CGContextMoveToPoint(context, x-arrowRadius, y-arrowRadius);
        CGContextAddLineToPoint(context, x, y);
        CGContextAddLineToPoint(context, x+arrowRadius, y-arrowRadius);
    }
    
    CGContextSetLineWidth(context, 3.0f);
    CGContextSetLineCap(context, kCGLineCapSquare);
    CGContextSetLineJoin(context, kCGLineJoinMiter);
    [[UIColor grayColor] setStroke];
    CGContextStrokePath(context);
    
    CGContextRestoreGState(context);
}

@end







@implementation SCExpandCollapseCell

@synthesize expandText = _expandText;
@synthesize collapseText = _collapseText;
@synthesize ownerSectionExpanded = _ownerSectionExpanded;


+ (instancetype)cellWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded
{
    return [[[self class] alloc] initWithExpandText:expandText collapseText:collapseText ownerSectionExpanded:expanded];
}


// overrides superclass
- (void)performInitialization
{
    [super performInitialization];
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    [self markCellAsSpecial];
    
    _expandText = nil;
    _collapseText = nil;
    _ownerSectionExpanded = FALSE;
    
    SCExpandCollapseAccessoryView *accessoryView = [[SCExpandCollapseAccessoryView alloc] init];
    accessoryView.ownerExpandCollapseCell = self;
    self.accessoryView = accessoryView;
}

- (instancetype)initWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded
{
    if( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) )
    {
        self.expandText = expandText;
        self.collapseText = collapseText;
        self.ownerSectionExpanded = expanded;
    }
    return self;
}


- (void)setOwnerSectionExpanded:(BOOL)expanded
{
    _ownerSectionExpanded = expanded;
    
    if(expanded)
        self.textLabel.text = self.collapseText;
    else 
        self.textLabel.text = self.expandText;
    [self.accessoryView setNeedsDisplay];
    
    [self.ownerSection setExpanded:expanded];
}

// overrides superclass
- (void)didSelectCell
{
    [super didSelectCell];
    
    self.ownerSectionExpanded = !self.ownerSectionExpanded;
}


@end
