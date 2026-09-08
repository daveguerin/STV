/*
 *  SCFetchItemsCell.m
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCFetchItemsCell.h"

#import "SCTableViewModel.h"


@interface SCFetchItemsCell () {
    UIActivityIndicatorView *_activityIndicator;
}
@end

@implementation SCFetchItemsCell


// overrides supercall
- (void)performInitialization {
    [super performInitialization];
    _activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleMedium];

    _activityIndicator.translatesAutoresizingMaskIntoConstraints = NO;
    [self.contentView addSubview:_activityIndicator];
    [NSLayoutConstraint activateConstraints:@[[_activityIndicator.centerXAnchor constraintEqualToAnchor:self.contentView.centerXAnchor], [_activityIndicator.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]]];

    _autoFetchItems = FALSE;
    _autoHide = TRUE;
}


// overrides superclass
- (void)willDisplay {
    [super willDisplay];

    if (self.autoFetchItems) {
        [self performSelector:@selector(fetchItems) withObject:nil afterDelay:0.1f];
    }
}

// overrides superclass
- (void)didSelectCell {
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    [self.ownerTableViewModel.tableView deselectRowAtIndexPath:indexPath animated:YES];

    [self fetchItems];
}


- (void)fetchItems {
    if ([self.ownerSection isKindOfClass:[SCArrayOfItemsSection class]]) {
        SCArrayOfItemsSection *itemsSection = (SCArrayOfItemsSection *)self.ownerSection;

        [self startActivityIndicator];
        [itemsSection fetchItems:self];
    }
}

- (void)didFetchItems {
    [self stopActivityIndicator];
}

- (void)startActivityIndicator {
    self.textLabel.hidden = TRUE;
    [_activityIndicator startAnimating];
}

- (void)stopActivityIndicator {
    [_activityIndicator stopAnimating];
    self.textLabel.hidden = FALSE;
}

@end
