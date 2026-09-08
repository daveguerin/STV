/*
 *  SCExpandCollapseCell.m
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCExpandCollapseCell.h"

#import "SCTableViewSection.h"


/// Intrinsically sized disclosure artwork placed by UITableView itself.
@interface SCExpandCollapseAccessoryView : UIImageView
@property (nonatomic, weak) SCExpandCollapseCell *ownerExpandCollapseCell;
- (void)updateDirection;
@end

@implementation SCExpandCollapseAccessoryView

- (instancetype)init {
    UIImageSymbolConfiguration *configuration = [UIImageSymbolConfiguration configurationWithPointSize:12 weight:UIImageSymbolWeightBold];
    self = [super initWithImage:[UIImage systemImageNamed:@"chevron.down" withConfiguration:configuration]];
    if (self) {
        self.tintColor = UIColor.secondaryLabelColor;
        self.contentMode = UIViewContentModeCenter;
        /*
        UIKit owns the accessory root's placement. UIImageView supplies its
        intrinsic size; disabling the root's autoresizing translation would
        detach it from UITableView's accessory layout.
        */
    }

    return self;
}

- (void)updateDirection {
    NSString *name = self.ownerExpandCollapseCell.ownerSectionExpanded ? @"chevron.up" : @"chevron.down";
    self.image = [UIImage systemImageNamed:name withConfiguration:[UIImageSymbolConfiguration configurationWithPointSize:12 weight:UIImageSymbolWeightBold]];
}

@end

@implementation SCExpandCollapseCell


+ (instancetype)cellWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded {
    return [[[self class] alloc] initWithExpandText:expandText collapseText:collapseText ownerSectionExpanded:expanded];
}


// overrides superclass
- (void)performInitialization {
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

- (instancetype)initWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded {
    if ((self = [self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil])) {
        self.expandText = expandText;
        self.collapseText = collapseText;
        self.ownerSectionExpanded = expanded;
    }

    return self;
}


- (void)setOwnerSectionExpanded:(BOOL)expanded {
    _ownerSectionExpanded = expanded;

    if (expanded) {
        self.textLabel.text = self.collapseText;
    }

    else {
        self.textLabel.text = self.expandText;
    }

    if ([self.accessoryView isKindOfClass:[SCExpandCollapseAccessoryView class]]) {
        [(SCExpandCollapseAccessoryView *)self.accessoryView updateDirection];
    }

    else {
        [self.accessoryView setNeedsDisplay];
    }

    [self.ownerSection setExpanded:expanded];
}

// overrides superclass
- (void)didSelectCell {
    [super didSelectCell];

    self.ownerSectionExpanded = !self.ownerSectionExpanded;
}


@end
