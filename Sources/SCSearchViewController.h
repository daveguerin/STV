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
/// A controller dedicated to searching data in a table view.
///
/// Provides a search bar above a table view, using safe-area constraints and
/// the search bar's intrinsic height for generated controls. Array models use
/// UISearchController to display search results.
///
/// - Note: Interface Builder may supply either control through its outlet.
///   Supplied controls keep their existing layout; missing controls are created
///   and constrained automatically.
@interface SCSearchViewController : SCViewController


#pragma mark - Configuration

/// The search bar associated with the controller. Access creates a constrained
/// UISearchBar when Interface Builder has not supplied one.
@property (nonatomic, strong, null_resettable) IBOutlet UISearchBar *searchBar;

@end

NS_ASSUME_NONNULL_END
