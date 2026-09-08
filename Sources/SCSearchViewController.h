/*
 *  SCSearchViewController.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCViewController.h"

NS_ASSUME_NONNULL_BEGIN


#pragma mark - SCSearchViewController
/// A controller with an inline search bar above a table view.
///
/// Generated controls use safe-area constraints and the bar's intrinsic height.
/// Assigning an `SCArrayOfItemsModel` (including object and string subclasses)
/// connects the inline bar's delegate and enables the model's search controller.
/// The supplied inline bar filters through the model's delegate path; it is not
/// replaced or reparented by the search controller. A plain `SCTableViewModel`
/// does not gain filtering.
///
/// - Note: Controls supplied through outlets before generated-control layout
///   retain their existing layout; missing controls are created automatically.
@interface SCSearchViewController : SCViewController


#pragma mark - Configuration

/// The search bar forwarded to an assigned `SCArrayOfItemsModel`.
///
/// Reading this property creates and adds a bar if none was supplied, loading
/// the controller's view if necessary. Generated bars receive safe-area
/// constraints during `viewDidLoad` or `viewWillAppear`. Supplying an outlet
/// before that layout preserves its existing layout.
///
/// Setting `nil` clears the stored reference; the next read creates a bar
/// (immediately when an array model is attached). Replacing the reference does
/// not remove the old bar from its superview or remove its constraints.
/// This bar need not be the model's `searchController.searchBar`.
@property (nonatomic, strong, null_resettable) IBOutlet UISearchBar *searchBar;

@end

NS_ASSUME_NONNULL_END
