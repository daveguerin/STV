/*
 *  SCSearchViewController.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCViewController.h"



#pragma mark - SCSearchViewController
/// This class functions as a means to simplify creating view controllers dedicated to searching data within a table view.
///
/// In addition to all the features provided by its SCViewController superclass, SCSearchViewController conveniently provides a UISearchBar control and a UITableView control aligned under each other. It uses UISearchController to display search results.
///
/// - Note: Both the UISearchBar and UITableView controls can still be created in Interface Builder and assigned to their respective outlets. In case no outlets are assigned, SCSearchViewController will automatically create the missing control(s).
@interface SCSearchViewController : SCViewController
{
    @protected
    UISearchBar *_searchBar;
}

#pragma mark - Configuration

/// The search bar control associated with the view controller. Unless assigned in Interface Builder, this property will automatically create a UISearch when accessed.
@property (nonatomic, strong) IBOutlet UISearchBar *searchBar;

@end
