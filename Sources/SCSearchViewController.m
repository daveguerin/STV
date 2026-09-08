/*
 *  SCSearchViewController.m
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCSearchViewController.h"

#import "SCTableViewModel.h"


@interface SCSearchViewController () {
    BOOL _searchBarNeedsLayout;
    BOOL _tableViewNeedsLayout;
}
@end


@implementation SCSearchViewController

// Custom accessors disable autosynthesis; create backing storage and its V attribute.
@synthesize searchBar = _searchBar;

// overrides superclass
- (void)setTableViewModel:(SCTableViewModel *)model {
    [super setTableViewModel:model];

    if ([model isKindOfClass:[SCArrayOfItemsModel class]]) {
        SCArrayOfItemsModel *itemsModel = (SCArrayOfItemsModel *)model;
        itemsModel.searchBar = self.searchBar;
        itemsModel.enableSearchController = TRUE;
    }
}

- (void)setSearchBar:(UISearchBar *)searchBar {
    _searchBar = searchBar;

    if ([self.tableViewModel isKindOfClass:[SCArrayOfItemsModel class]]) {
        SCArrayOfItemsModel *itemsModel = (SCArrayOfItemsModel *)self.tableViewModel;
        itemsModel.searchBar = self.searchBar;
    }
}

- (UISearchBar *)searchBar {
    if (!_searchBar) {
        _searchBar = [[UISearchBar alloc] init];
        _searchBarNeedsLayout = YES;
        [self.view addSubview:_searchBar];
    }

    return _searchBar;
}

- (UITableView *)tableView {
    if (!_tableView) {
        _tableViewNeedsLayout = YES;
        [super setTableView:[[UITableView alloc] init]];
    }

    return _tableView;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    [self layoutGeneratedControls];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    [self layoutGeneratedControls];
}

- (void)layoutGeneratedControls {
    UISearchBar *searchBar = self.searchBar;
    UITableView *tableView = self.tableView;
    UILayoutGuide *safeArea = self.view.safeAreaLayoutGuide;

    /*
    Generated controls fit the safe area from their first layout. The search
    bar supplies its intrinsic height; no frame seed or appearance-time size
    correction is needed. Interface Builder retains supplied-view layout.
    */
    if (_searchBarNeedsLayout) {
        if (!searchBar.superview) {
            [self.view addSubview:searchBar];
        }

        searchBar.translatesAutoresizingMaskIntoConstraints = NO;
        [NSLayoutConstraint activateConstraints:@[[searchBar.leadingAnchor constraintEqualToAnchor:safeArea.leadingAnchor], [searchBar.trailingAnchor constraintEqualToAnchor:safeArea.trailingAnchor], [searchBar.topAnchor constraintEqualToAnchor:safeArea.topAnchor]]];
        _searchBarNeedsLayout = NO;
    }

    if (_tableViewNeedsLayout) {
        if (!tableView.superview) {
            [self.view addSubview:tableView];
        }

        tableView.translatesAutoresizingMaskIntoConstraints = NO;
        [NSLayoutConstraint activateConstraints:@[[tableView.leadingAnchor constraintEqualToAnchor:safeArea.leadingAnchor], [tableView.trailingAnchor constraintEqualToAnchor:safeArea.trailingAnchor], [tableView.topAnchor constraintEqualToAnchor:searchBar.bottomAnchor], [tableView.bottomAnchor constraintEqualToAnchor:safeArea.bottomAnchor]]];
        _tableViewNeedsLayout = NO;
    }
}

@end
