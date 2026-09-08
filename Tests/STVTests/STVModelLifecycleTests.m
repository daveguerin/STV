@import XCTest;
@import STV;

@interface SCDateCell (STVModelLifecycleTests)
- (void)showEmbeddedDatePicker;
- (void)hideEmbeddedDatePicker;
@end

@interface SCArrayOfItemsModel (STVModelLifecycleTests)
- (void)attachSearchController;
@end

@interface STVModelLifecycleTests : XCTestCase
@end

@implementation STVModelLifecycleTests

- (void)testHiddenNavigationBarUsesVisibleSearchHeaderAndFilters {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    UIViewController *owner = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:owner];
    [navigation setNavigationBarHidden:YES animated:NO];
    window.rootViewController = navigation;
    [window makeKeyAndVisible];
    [window layoutIfNeeded];
    UITableView *table = [[UITableView alloc] initWithFrame:owner.view.bounds];
    [owner.view addSubview:table];
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:table items:[NSMutableArray arrayWithObjects:@"Alpha", @"Beta", nil]];
    model.enableSearchController = YES;
    [table reloadData];
    [table layoutIfNeeded];
    UISearchBar *bar = model.searchBar;
    XCTAssertTrue(navigation.navigationBarHidden);
    XCTAssertNil(owner.navigationItem.searchController);
    XCTAssertEqual(table.tableHeaderView, bar);
    XCTAssertEqual(bar.window, window);
    XCTAssertFalse(bar.hidden);
    XCTAssertGreaterThan(bar.bounds.size.height, 0);
    XCTAssertEqualWithAccuracy(bar.bounds.size.width, table.bounds.size.width, 0.5);
    XCTAssertGreaterThanOrEqual([table rectForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]].origin.y, bar.bounds.size.height);
    XCTAssertEqual([model sectionAtIndex:0].cellCount, 2u);
    bar.text = @"Alpha";
    [model updateSearchResultsForSearchController:model.searchController];
    XCTAssertEqual([model sectionAtIndex:0].cellCount, 1u);
    XCTAssertEqualObjects([[model sectionAtIndex:0] cellAtIndex:0].textLabel.text, @"Alpha");
    bar.text = @"";
    [model updateSearchResultsForSearchController:model.searchController];
    XCTAssertEqual([model sectionAtIndex:0].cellCount, 2u);
    model.enableSearchController = NO;
    XCTAssertNil(table.tableHeaderView);
    XCTAssertFalse(owner.definesPresentationContext);
    window.hidden = YES;
}

- (void)testSearchReattachmentFollowsNavigationBarVisibilityOnSameHost {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    UIViewController *owner = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:owner];
    window.rootViewController = navigation;
    [window makeKeyAndVisible];
    [window layoutIfNeeded];
    UITableView *table = [[UITableView alloc] initWithFrame:owner.view.bounds];
    [owner.view addSubview:table];
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:table items:[NSMutableArray arrayWithObject:@"Alpha"]];
    UINavigationItemSearchBarPlacement originalPlacement = owner.navigationItem.preferredSearchBarPlacement;
    model.enableSearchController = YES;
    UISearchController *search = model.searchController;
    XCTAssertEqual(owner.navigationItem.searchController, search);
    XCTAssertNil(table.tableHeaderView);
    for (NSNumber *hidden in @[@YES, @NO, @YES, @NO]) {
        [navigation setNavigationBarHidden:hidden.boolValue animated:NO];
        [model attachSearchController];
        [window layoutIfNeeded];
        [table reloadData];
        [table layoutIfNeeded];
        XCTAssertEqual(navigation.navigationBarHidden, hidden.boolValue);
        XCTAssertEqual(model.searchController, search);
        XCTAssertTrue(owner.definesPresentationContext);
        if (hidden.boolValue) {
            XCTAssertNil(owner.navigationItem.searchController);
            XCTAssertEqual(owner.navigationItem.preferredSearchBarPlacement, originalPlacement);
            XCTAssertEqual(table.tableHeaderView, search.searchBar);
            XCTAssertEqual(search.searchBar.window, window);
            XCTAssertFalse(search.searchBar.translatesAutoresizingMaskIntoConstraints);
            XCTAssertGreaterThan(search.searchBar.bounds.size.height, 0);
            XCTAssertGreaterThan(search.searchBar.searchTextField.bounds.size.height, 0);
            XCTAssertFalse(search.searchBar.searchTextField.hidden);
            XCTAssertEqual(search.searchBar.searchTextField.window, window);
            CGRect visibleField = [search.searchBar.searchTextField convertRect:search.searchBar.searchTextField.bounds toView:window];
            for (UIView *ancestor = search.searchBar.searchTextField; ancestor; ancestor = ancestor.superview) {
                XCTAssertFalse(ancestor.hidden);
                XCTAssertGreaterThan(ancestor.alpha, 0.01);
                if (ancestor.clipsToBounds) {
                    visibleField = CGRectIntersection(visibleField, [ancestor convertRect:ancestor.bounds toView:window]);
                }
            }
            XCTAssertFalse(CGRectIsEmpty(visibleField));
            XCTAssertEqualWithAccuracy(search.searchBar.bounds.size.width, table.bounds.size.width, 0.5);
            XCTAssertGreaterThanOrEqual([table rectForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]].origin.y, search.searchBar.bounds.size.height);
        }
        else {
            XCTAssertEqual(owner.navigationItem.searchController, search);
            XCTAssertNil(table.tableHeaderView);
            XCTAssertTrue(search.searchBar.translatesAutoresizingMaskIntoConstraints);
        }
        NSArray *constraints = table.constraints;
        [model attachSearchController];
        XCTAssertEqualObjects(table.constraints, constraints);
    }
    model.enableSearchController = NO;
    XCTAssertNil(owner.navigationItem.searchController);
    XCTAssertEqual(owner.navigationItem.preferredSearchBarPlacement, originalPlacement);
    XCTAssertNil(table.tableHeaderView);
    XCTAssertFalse(owner.definesPresentationContext);
    window.hidden = YES;
}

- (void)testNavigationChildUsesItsOwnVisibleSearchHeader {
    UIViewController *parent = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:parent];
    [navigation loadViewIfNeeded];
    UIViewController *child = [[UIViewController alloc] init];
    [parent addChildViewController:child];
    [parent.view addSubview:child.view];
    [child didMoveToParentViewController:parent];
    UITableView *table = [[UITableView alloc] init];
    [child.view addSubview:table];
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:table];
    model.enableSearchController = YES;
    XCTAssertEqual(table.tableHeaderView, model.searchBar);
    XCTAssertNil(child.navigationItem.searchController);
    XCTAssertNil(parent.navigationItem.searchController);
    model.enableSearchController = NO;
}

- (void)testEmbeddedDateExpansionKeepsTheExistingRowVisible {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    UIViewController *owner = [[UIViewController alloc] init];
    window.rootViewController = owner;
    [window makeKeyAndVisible];
    UITableView *table = [[UITableView alloc] initWithFrame:owner.view.bounds];
    [owner.view addSubview:table];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection section];
    SCDateCell *cell = [SCDateCell cellWithText:@"Date"];
    cell.datePicker.preferredDatePickerStyle = UIDatePickerStyleWheels;
    [section addCell:cell];
    [model addSection:section];
    [table reloadData];
    [table layoutIfNeeded];
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    CGFloat collapsedHeight = [table rectForRowAtIndexPath:path].size.height;
    for (NSNumber *expanded in @[@YES, @NO, @YES]) {
        if (expanded.boolValue)
            [cell showEmbeddedDatePicker];
        else
            [cell hideEmbeddedDatePicker];
        [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];
        [table layoutIfNeeded];
        XCTAssertEqual([table cellForRowAtIndexPath:path], cell);
        XCTAssertEqual(cell.window, window);
        XCTAssertFalse(cell.hidden);
        XCTAssertEqualWithAccuracy(cell.alpha, 1, 0.01);
        CGFloat height = [table rectForRowAtIndexPath:path].size.height;
        if (expanded.boolValue) {
            XCTAssertGreaterThan(height, collapsedHeight + 100);
            XCTAssertEqual(cell.datePicker.window, window);
            XCTAssertFalse(cell.datePicker.hidden);
        }
        else {
            XCTAssertEqualWithAccuracy(height, collapsedHeight, 1);
        }
    }
    window.hidden = YES;
}

- (void)testSearchActivationRemovesMainInsetWithoutPaddingNativeResultsController {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    UIViewController *owner = [[UIViewController alloc] init];
    window.rootViewController = owner;
    [window makeKeyAndVisible];
    owner.view.frame = CGRectMake(0, 0, 320, 600);
    UITableView *table = [[UITableView alloc] initWithFrame:owner.view.bounds];
    [owner.view addSubview:table];
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:table items:[NSMutableArray arrayWithObject:@"Alpha"]];
    model.enableSearchController = YES;
    UITableViewController *results = (id)model.searchController.searchResultsController;
    table.contentInset = UIEdgeInsetsMake(0, 0, 17, 0);
    table.verticalScrollIndicatorInsets = UIEdgeInsetsMake(0, 0, 19, 0);
    results.tableView.contentInset = UIEdgeInsetsMake(0, 0, 31, 0);
    results.tableView.verticalScrollIndicatorInsets = UIEdgeInsetsMake(0, 0, 37, 0);
    NSNotification *show = [NSNotification notificationWithName:UIKeyboardWillShowNotification object:nil userInfo:@{UIKeyboardFrameEndUserInfoKey: [NSValue valueWithCGRect:CGRectMake(0, 400, 320, 200)]}];
    [model keyboardWillShow:show];
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 217, 0.5);
    model.searchController.active = YES;
    [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.4]];
    XCTAssertTrue(model.searchController.active);
    [model updateSearchResultsForSearchController:model.searchController];
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17, 0.5);
    XCTAssertEqualWithAccuracy(table.verticalScrollIndicatorInsets.bottom, 19, 0.5);
    [model keyboardWillShow:show];
    XCTAssertEqualWithAccuracy(results.tableView.contentInset.bottom, 31, 0.5);
    [model keyboardWillHide:show];
    XCTAssertEqualWithAccuracy(results.tableView.contentInset.bottom, 31, 0.5);
    XCTAssertEqualWithAccuracy(results.tableView.verticalScrollIndicatorInsets.bottom, 37, 0.5);
    model.enableSearchController = NO;
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17, 0.5);
    window.hidden = YES;
}

- (void)testKeyboardFrameNotificationsAndConstrainedViewportChangesReconcileInsets {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    UIViewController *owner = [[UIViewController alloc] init];
    window.rootViewController = owner;
    [window makeKeyAndVisible];
    UITableView *table = [[UITableView alloc] init];
    table.translatesAutoresizingMaskIntoConstraints = NO;
    [owner.view addSubview:table];
    NSLayoutConstraint *bottom = [table.bottomAnchor constraintEqualToAnchor:owner.view.bottomAnchor];
    [NSLayoutConstraint activateConstraints:@[[table.leadingAnchor constraintEqualToAnchor:owner.view.leadingAnchor], [table.trailingAnchor constraintEqualToAnchor:owner.view.trailingAnchor], [table.topAnchor constraintEqualToAnchor:owner.view.topAnchor], bottom]];
    [window layoutIfNeeded];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    table.contentInset = UIEdgeInsetsMake(3, 4, 17, 6);
    table.verticalScrollIndicatorInsets = UIEdgeInsetsMake(7, 8, 23, 10);
    [SCModelCenter sharedModelCenter].keyboardIssuer = owner;
    CGRect frame = [table convertRect:table.bounds toView:nil];
    CGFloat bottomY = CGRectGetMaxY(frame);
    for (NSNumber *height in @[@200, @100]) {
        [[NSNotificationCenter defaultCenter] postNotificationName:UIKeyboardWillChangeFrameNotification object:nil userInfo:@{UIKeyboardFrameEndUserInfoKey: [NSValue valueWithCGRect:CGRectMake(0, bottomY - height.doubleValue, window.bounds.size.width, height.doubleValue)]}];
        CGFloat automatic = table.adjustedContentInset.bottom - table.contentInset.bottom;
        XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17 + MAX(0, height.doubleValue - automatic), 0.5);
        XCTAssertTrue(CGRectEqualToRect(frame, [table convertRect:table.bounds toView:nil]));
    }
    bottom.constant = -40;
    [window layoutIfNeeded];
    CGFloat automatic = table.adjustedContentInset.bottom - table.contentInset.bottom;
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17 + MAX(0, 60 - automatic), 0.5);
    owner.additionalSafeAreaInsets = UIEdgeInsetsMake(0, 0, 50, 0);
    [window layoutIfNeeded];
    [table layoutIfNeeded];
    automatic = table.adjustedContentInset.bottom - table.contentInset.bottom;
    XCTAssertGreaterThan(automatic, 0);
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17 + MAX(0, 60 - automatic), 0.5);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIKeyboardWillHideNotification object:nil];
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 17, 0.5);
    XCTAssertEqualWithAccuracy(table.verticalScrollIndicatorInsets.bottom, 23, 0.5);
    [SCModelCenter sharedModelCenter].keyboardIssuer = nil;
    window.hidden = YES;
}

- (void)testKeyboardContributionFollowsReassignedTableAndPreservesBaseInsets {
    UIViewController *owner = [[UIViewController alloc] init];
    owner.view.frame = CGRectMake(0, 0, 320, 600);
    UITableView *first = [[UITableView alloc] initWithFrame:owner.view.bounds];
    UITableView *second = [[UITableView alloc] initWithFrame:owner.view.bounds];
    [owner.view addSubview:first];
    [owner.view addSubview:second];
    first.contentInset = UIEdgeInsetsMake(1, 2, 17, 4);
    first.verticalScrollIndicatorInsets = UIEdgeInsetsMake(5, 6, 19, 8);
    second.contentInset = UIEdgeInsetsMake(9, 10, 31, 12);
    second.verticalScrollIndicatorInsets = UIEdgeInsetsMake(13, 14, 37, 16);
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:first];
    NSNotification *show = [NSNotification notificationWithName:UIKeyboardWillShowNotification object:nil userInfo:@{UIKeyboardFrameEndUserInfoKey: [NSValue valueWithCGRect:CGRectMake(0, 400, 320, 200)]}];
    [model keyboardWillShow:show];
    XCTAssertEqualWithAccuracy(first.contentInset.bottom, 217, 0.5);
    model.tableView = second;
    XCTAssertEqualWithAccuracy(first.contentInset.bottom, 17, 0.5);
    XCTAssertEqualWithAccuracy(first.verticalScrollIndicatorInsets.bottom, 19, 0.5);
    XCTAssertEqualWithAccuracy(second.contentInset.bottom, 231, 0.5);
    [model keyboardWillHide:show];
    XCTAssertEqualWithAccuracy(first.contentInset.bottom, 17, 0.5);
    XCTAssertEqualWithAccuracy(second.contentInset.bottom, 31, 0.5);
    XCTAssertEqualWithAccuracy(second.verticalScrollIndicatorInsets.bottom, 37, 0.5);
}

- (void)testDeferredSearchAttachmentMovesOneControllerAndCleansOldHosts {
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:nil items:[NSMutableArray arrayWithObjects:@"Alpha", @"Beta", nil]];
    XCTAssertNoThrow(model.enableSearchController = YES);
    UISearchController *search = model.searchController;
    XCTAssertNotNil(search);
    UITableView *first = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    model.tableView = first;
    XCTAssertEqual(first.tableHeaderView, search.searchBar);
    UIViewController *owner = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:owner];
    [navigation loadViewIfNeeded];
    UITableView *second = [[UITableView alloc] init];
    [owner.view addSubview:second];
    model.tableView = second;
    XCTAssertNil(first.tableHeaderView);
    XCTAssertEqual(owner.navigationItem.searchController, search);
    XCTAssertEqual(model.searchController, search);
    model.tableView = first;
    XCTAssertNil(owner.navigationItem.searchController);
    XCTAssertEqual(first.tableHeaderView, search.searchBar);
    model.enableSearchController = NO;
    XCTAssertNil(first.tableHeaderView);
    XCTAssertNil(model.searchController);
    XCTAssertNil(search.searchBar.superview);
}

@end
