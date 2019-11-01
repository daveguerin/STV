/*
 *  SCFetchItemsCell.m
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCFetchItemsCell.h"

#import "SCTableViewModel.h"


@implementation SCFetchItemsCell

@synthesize autoFetchItems = _autoFetchItems;
@synthesize autoHide = _autoHide;


// overrides supercall
- (void)performInitialization
{
    [super performInitialization];
    
    if (@available(iOS 13.0, *)) {
        _activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleMedium]; // dgApps
    }
    else {
        _activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    }
    [self.contentView addSubview:_activityIndicator];
    
    _autoFetchItems = FALSE;
    _autoHide = TRUE;
}



// overrides superclass
- (void)layoutSubviews
{
    _activityIndicator.frame = self.contentView.bounds;
    
    [super layoutSubviews];
}

// overrides superclass
- (void)willDisplay
{
    [super willDisplay];
    
    if(self.autoFetchItems)
    {
        [self performSelector:@selector(fetchItems) withObject:nil afterDelay:0.1f];
    }
}

// overrides superclass
- (void)didSelectCell
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    [self.ownerTableViewModel.tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    [self fetchItems];
}


- (void)fetchItems
{
    if([self.ownerSection isKindOfClass:[SCArrayOfItemsSection class]])
    {
        SCArrayOfItemsSection *itemsSection = (SCArrayOfItemsSection *)self.ownerSection;
        
        [self startActivityIndicator];
        [itemsSection fetchItems:self];
    }
}

- (void)didFetchItems
{
    [self stopActivityIndicator];
}

- (void)startActivityIndicator
{
    self.textLabel.hidden = TRUE;
    [_activityIndicator startAnimating];
}

- (void)stopActivityIndicator
{
    [_activityIndicator stopAnimating];
    self.textLabel.hidden = FALSE;
}

@end


