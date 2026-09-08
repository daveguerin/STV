@import XCTest;
@import STV;

@interface STVAvailabilityTests : XCTestCase
@end

@implementation STVAvailabilityTests

- (void)testLegacyImagePickerCellIsNotIncludedInSTVSeven {
    XCTAssertNil(NSClassFromString(@"SCImagePickerCell"));
}

- (void)testLegacyThemeClassIsNotIncludedInSTVSeven {
    XCTAssertNil(NSClassFromString(@"SCTheme"));
}

- (void)testLegacyPullToRefreshViewIsNotIncludedInSTVSeven {
    XCTAssertNil(NSClassFromString(@"SCPullToRefreshView"));
}

- (void)testLegacyAvailabilityHelpersAreNotIncludedInSTVSeven {
    XCTAssertFalse([SCUtilities respondsToSelector:NSSelectorFromString(@"systemVersion")]);

    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] init];
    XCTAssertFalse([model respondsToSelector:NSSelectorFromString(@"searchDisplayController")]);
}

- (void)testSearchUsesUISearchController {
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:tableView items:[@[@"Alpha", @"Beta", @"Gamma"] mutableCopy]];

    model.enableSearchController = YES;
    XCTAssertNotNil(model.searchController);
    XCTAssertTrue([model.searchController.searchResultsController isKindOfClass:[SCTableViewController class]]);

    model.searchController.searchBar.text = @"Beta";
    [model updateSearchResultsForSearchController:model.searchController];
    XCTAssertEqual([model numberOfSectionsInTableView:tableView], 1);
    XCTAssertEqual([model tableView:tableView numberOfRowsInSection:0], 1);

    model.enableSearchController = NO;
    XCTAssertNil(model.searchController);
}

- (void)testSelectionSectionsUseSemanticColors {
    SCSelectionSection *stringSection = [[SCSelectionSection alloc] init];
    XCTAssertEqualObjects(stringSection.selectedCellTextColor, UIColor.linkColor);
    XCTAssertEqualObjects(stringSection.deselectedCellTextColor, UIColor.labelColor);

    SCObjectSelectionSection *objectSection = [[SCObjectSelectionSection alloc] init];
    XCTAssertEqualObjects(objectSection.selectedCellTextColor, UIColor.linkColor);
    XCTAssertEqualObjects(objectSection.deselectedCellTextColor, UIColor.labelColor);
}

- (void)testPullToRefreshUsesUIScrollViewRefreshControl {
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];

    model.enablePullToRefresh = YES;
    XCTAssertEqual(tableView.refreshControl, model.refreshControl);

    model.tableView = nil;
    XCTAssertNil(tableView.refreshControl);

    model.tableView = tableView;
    XCTAssertEqual(tableView.refreshControl, model.refreshControl);

    model.enablePullToRefresh = NO;
    XCTAssertNil(tableView.refreshControl);
}

- (void)testNativeRefreshControlCompletesAndInvokesDidRefresh {
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    __block BOOL didRefresh = NO;
    model.modelActions.didRefresh = ^(SCTableViewModel *tableModel) { didRefresh = YES; };

    model.enablePullToRefresh = YES;
    NSArray<NSString *> *actions = [model.refreshControl actionsForTarget:model forControlEvent:UIControlEventValueChanged];
    XCTAssertTrue([actions containsObject:NSStringFromSelector(@selector(pullToRefreshDidStartLoading))]);
    [model.refreshControl beginRefreshing];
    [model pullToRefreshDidStartLoading];

    XCTAssertFalse(model.refreshControl.isRefreshing);
    XCTAssertTrue(didRefresh);
}

- (void)testRuntimeVersionMatchesSTVSevenRelease {
    XCTAssertEqualObjects(kSTVCurrentVersion, @"7.0.0");
}

@end
