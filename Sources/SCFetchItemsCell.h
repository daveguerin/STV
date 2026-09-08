/*
 *  SCFetchItemsCell.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCTableViewCell.h"

NS_ASSUME_NONNULL_BEGIN

/// A special row that requests another batch from an `SCArrayOfItemsSection`.
///
/// The section supplies a "Load more..." cell by default. Normal batch-row
/// visibility requires a positive `dataFetchOptions.batchSize`; an initial
/// asynchronous fetch can also show this cell as a loading indicator.
///
/// Sample use:
/// ```objective-c
/// myArrayOfObjectsSection.dataFetchOptions.batchSize = 10;
/// myArrayOfObjectsSection.fetchItemsCell.textLabel.text = @"Load next batch";
/// ```
///
/// - SeeAlso: ``SCArrayOfItemsSection``
///
/// - Note: In an expanded section, the fetch row follows the data items and
///   precedes the add-new-item row, when present.
@interface SCFetchItemsCell : SCCustomCell


#pragma mark - Configuration

/// Whether displaying the row schedules a fetch after a short delay.
/// Defaults to `NO`. Tapping requests a fetch regardless of this setting.
@property (nonatomic, readwrite) BOOL autoFetchItems;

/// Whether the section hides this row when its batch-count check indicates
/// that fetching is complete. Defaults to `YES`.
///
/// The check compares the accumulated item count with
/// `batchCurrentOffset * batchSize`; a full final batch can leave the row
/// visible until another fetch returns fewer items. Setting this to `NO`
/// retains the row only while batching is enabled.
@property (nonatomic, readwrite) BOOL autoHide;


#pragma mark - Internal Methods (should only be used by the framework or when subclassing)

/// Starts the indicator and requests items from an `SCArrayOfItemsSection`
/// owner. Does nothing for a missing owner or any other section type.
- (void)fetchItems;

/// Stops the activity indicator and restores the text label after completion.
/// The section calls this after processing fetched items; asynchronous failure
/// stops the indicator directly instead.
- (void)didFetchItems;

/// Starts the centered activity indicator and hides the text label.
- (void)startActivityIndicator;

/// Stops the activity indicator and unhides the text label.
- (void)stopActivityIndicator;

@end

NS_ASSUME_NONNULL_END
