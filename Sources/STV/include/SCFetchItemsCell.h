/*
 *  SCFetchItemsCell.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCTableViewCell.h"

/**
 SCFetchItemsCell is a special cell that is automatically displayed by an SCArrayOfItemsSection when more section items need to be fetched. This is typically the case when the section's dataFetchOptions property has specified a batchSize for the fetched items.
 
 Sample use:
    myArrayOfObjectsSection.dataFetchOptions.batchSize = 10;
    myArrayOfObjectsSection.fetchItemsCell.textLabel.text = @"Load next batch";
 
 See also: SCArrayOfItemsSection
 
 @note SCExpandCollapseCell is always displayed at the bottom of the section.
 */
@interface SCFetchItemsCell : SCCustomCell
{
    UIActivityIndicatorView *_activityIndicator;
    BOOL _autoFetchItems;
    BOOL _autoHide;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Configuration
//////////////////////////////////////////////////////////////////////////////////////////

/** When set to TRUE, the cell automatically starts fetching the items when it scrolls into view. When set to FALSE, the cell must be tapped in order to fetch the items. Default: FALSE. */
@property (nonatomic, readwrite) BOOL autoFetchItems;

/** When set to TRUE, the cell automatically disappears as soon as there are no more items to fetch. Default: TRUE. */
@property (nonatomic, readwrite) BOOL autoHide;


//////////////////////////////////////////////////////////////////////////////////////////
/// @name Internal Methods (should only be used by the framework or when subclassing)
//////////////////////////////////////////////////////////////////////////////////////////

/** Method called internally by the framework to fetch the section's items. */
- (void)fetchItems;

/** Method called internally by the framework once the items have been fetched. */
- (void)didFetchItems;

/** Method called internally by the framework to start the cell's activity indicator. */
- (void)startActivityIndicator;

/** Method called internally by the framework to stop the cell's activity indicator. */
- (void)stopActivityIndicator;

@end
