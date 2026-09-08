@import XCTest;
@import STV;

@interface STVModalTableController : SCTableViewController
@property (nonatomic) BOOL requestedDismissal;
@end

@implementation STVModalTableController
- (void)dismissViewControllerAnimated:(BOOL)animated completion:(void (^)(void))completion {
    self.requestedDismissal = YES;
    if (completion)
        completion();
}
@end

@interface STVModalViewController : SCViewController
@property (nonatomic) BOOL requestedDismissal;
@end

@implementation STVModalViewController
- (void)dismissViewControllerAnimated:(BOOL)animated completion:(void (^)(void))completion {
    self.requestedDismissal = YES;
    if (completion)
        completion();
}
@end

@interface STVEmbeddedTableController : SCTableViewController
@end

@implementation STVEmbeddedTableController
- (BOOL)ibEmbedded {
    return YES;
}
@end

@interface STVControllerLayoutTests : XCTestCase
@property (nonatomic, strong) UIWindow *layoutWindow;
@end

@implementation STVControllerLayoutTests

- (void)tearDown {
    self.layoutWindow = nil;
    [super tearDown];
}

- (void)hostLayoutController:(UIViewController *)controller {
    self.layoutWindow = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 700, 700)];
    [self.layoutWindow addSubview:controller.view];
}

- (void)testControllersDoNotImplementSplitDelegationOrDiscovery {
    NSArray *controllers = @[[[SCViewController alloc] init], [[SCTableViewController alloc] init]];
    NSArray<NSString *> *selectors = @[
        @"setupDetailViewControllerIfNeeded", @"isMasterOfSplitViewController:", @"isDetailOfSplitViewController:",
        (@"splitViewController:collapseSecondaryViewController:"
          "ontoPrimaryViewController:"),
        (@"splitViewController:"
          "separateSecondaryViewControllerFromPrimaryViewController:")
    ];
    for (UIViewController *controller in controllers) {
        XCTAssertFalse([controller conformsToProtocol:@protocol(UISplitViewControllerDelegate)]);
        for (NSString *selector in selectors)
            XCTAssertFalse([controller respondsToSelector:NSSelectorFromString(selector)], @"%@ %@", controller.class, selector);
    }
}

- (void)testSecondaryControllerLoadsStaticContentWithoutMasterDiscovery {
    UISplitViewController *container = [[UISplitViewController alloc] initWithStyle:UISplitViewControllerStyleDoubleColumn];
    UIViewController *primary = [[UIViewController alloc] init];
    SCTableViewController *secondary = [[SCTableViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:secondary];
    container.viewControllers = @[primary, navigation];
    [secondary loadViewIfNeeded];

    XCTAssertTrue(secondary.staticContentAddedToModel);
    XCTAssertNil(secondary.tableViewModel.masterModel);
    XCTAssertEqualObjects(container.viewControllers, (@[primary, navigation]));
}

- (void)testGeneratedSearchControlsAreConstrainedWhenViewLoads {
    SCSearchViewController *controller = [[SCSearchViewController alloc] init];
    [controller loadViewIfNeeded];
    [self hostLayoutController:controller];
    controller.view.frame = CGRectMake(0, 0, 390, 700);
    [controller.view layoutIfNeeded];

    XCTAssertFalse(controller.searchBar.translatesAutoresizingMaskIntoConstraints);
    XCTAssertFalse(controller.tableView.translatesAutoresizingMaskIntoConstraints);
    XCTAssertFalse(controller.searchBar.hasAmbiguousLayout);
    XCTAssertFalse(controller.tableView.hasAmbiguousLayout);
    XCTAssertGreaterThan(controller.searchBar.bounds.size.height, 0);
    XCTAssertEqualWithAccuracy(controller.tableView.bounds.size.width, 390, 0.5);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(controller.tableView.frame), CGRectGetMaxY(controller.searchBar.frame), 0.5);
}

- (void)testGeneratedSearchLayoutTracksSafeAreaAndResizing {
    SCSearchViewController *controller = [[SCSearchViewController alloc] init];
    [controller loadViewIfNeeded];
    [self hostLayoutController:controller];
    controller.view.frame = CGRectMake(0, 0, 390, 700);
    controller.additionalSafeAreaInsets = UIEdgeInsetsMake(21, 12, 17, 8);
    [controller beginAppearanceTransition:YES animated:NO];
    [controller endAppearanceTransition];
    [controller.view layoutIfNeeded];

    for (NSValue *size in @[[NSValue valueWithCGSize:CGSizeMake(390, 700)], [NSValue valueWithCGSize:CGSizeMake(700, 390)]]) {
        controller.view.frame = (CGRect){CGPointZero, size.CGSizeValue};
        [controller.view layoutIfNeeded];
        CGRect safeFrame = controller.view.safeAreaLayoutGuide.layoutFrame;
        XCTAssertEqualWithAccuracy(CGRectGetMinX(controller.searchBar.frame), CGRectGetMinX(safeFrame), 0.5);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(controller.searchBar.frame), CGRectGetMinY(safeFrame), 0.5);
        XCTAssertEqualWithAccuracy(controller.searchBar.bounds.size.width, safeFrame.size.width, 0.5);
        XCTAssertEqualWithAccuracy(CGRectGetMaxY(controller.tableView.frame), CGRectGetMaxY(safeFrame), 0.5);
        XCTAssertFalse(controller.searchBar.hasAmbiguousLayout);
        XCTAssertFalse(controller.tableView.hasAmbiguousLayout);
    }
}

- (void)testSearchModelStillFiltersFromItsSearchController {
    SCSearchViewController *controller = [[SCSearchViewController alloc] init];
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:nil items:[@[@"Alpha", @"Beta"] mutableCopy]];
    controller.tableViewModel = model;
    [controller loadViewIfNeeded];

    XCTAssertTrue(model.enableSearchController);
    XCTAssertNotNil(model.searchController);
    XCTAssertEqual(model.searchBar, controller.searchBar);
    model.searchController.searchBar.text = @"Beta";
    [model updateSearchResultsForSearchController:model.searchController];
    XCTAssertEqual([model tableView:controller.tableView numberOfRowsInSection:0], 1);
}

- (void)testSearchLayoutIsIdempotentAcrossAppearanceAndDynamicType {
    SCSearchViewController *controller = [[SCSearchViewController alloc] init];
    UISearchBar *searchBar = controller.searchBar;
    [self hostLayoutController:controller];
    controller.view.frame = CGRectMake(0, 0, 390, 700);
    [controller.view layoutIfNeeded];
    NSUInteger constraintCount = controller.view.constraints.count;

    if (@available(iOS 17.0, *))
        controller.traitOverrides.preferredContentSizeCategory = UIContentSizeCategoryAccessibilityExtraExtraExtraLarge;
    [controller beginAppearanceTransition:YES animated:NO];
    [controller endAppearanceTransition];
    [controller beginAppearanceTransition:NO animated:NO];
    [controller endAppearanceTransition];
    [controller beginAppearanceTransition:YES animated:NO];
    [controller endAppearanceTransition];
    [controller.view layoutIfNeeded];

    XCTAssertEqual(controller.view.constraints.count, constraintCount);
    XCTAssertEqual(controller.searchBar, searchBar);
    XCTAssertGreaterThan(searchBar.bounds.size.height, 0);
    XCTAssertFalse(searchBar.hasAmbiguousLayout);
    XCTAssertEqualWithAccuracy(CGRectGetMinY(controller.tableView.frame), CGRectGetMaxY(searchBar.frame), 0.5);
}

- (void)testEmbeddedTableInheritsExplicitModelAndSections {
    SCViewController *parent = [[SCViewController alloc] init];
    SCTableViewModel *master = [[SCTableViewModel alloc] init];
    parent.tableViewModel.masterModel = master;
    NSObject *boundObject = [[NSObject alloc] init];
    parent.tableViewModel.masterBoundObject = boundObject;
    SCTableViewSection *section = [SCTableViewSection section];
    [section addCell:[SCTableViewCell cellWithText:@"Embedded"]];
    [parent.tableViewModel addSection:section];
    STVEmbeddedTableController *child = [[STVEmbeddedTableController alloc] init];
    [child loadViewIfNeeded];
    XCTAssertFalse(child.staticContentAddedToModel);

    [parent addChildViewController:child];
    [parent.view addSubview:child.view];
    [child didMoveToParentViewController:parent];

    XCTAssertEqual(parent.containedViewController, child);
    XCTAssertEqual(child.tableViewModel.masterModel, master);
    XCTAssertEqual(child.tableViewModel.masterBoundObject, boundObject);
    XCTAssertEqual([child.tableViewModel sectionAtIndex:0], section);
    XCTAssertTrue(child.staticContentAddedToModel);
    [child removeFromParentViewController];
    XCTAssertNil(parent.containedViewController);
}

- (void)testOrdinaryTableControllerDonePopsNavigationStack {
    UIViewController *root = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:root];
    SCTableViewController *detail = [[SCTableViewController alloc] init];
    [navigation pushViewController:detail animated:NO];
    [detail dismissWithCancelValue:NO doneValue:YES];

    XCTAssertEqual(navigation.topViewController, root);
    XCTAssertEqual(detail.state, SCViewControllerStateDismissed);
    XCTAssertTrue(detail.doneButtonTapped);
}

- (void)testOrdinaryViewControllerCancelPopsNavigationStack {
    UIViewController *root = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:root];
    SCViewController *detail = [[SCViewController alloc] init];
    [navigation pushViewController:detail animated:NO];
    [detail dismissWithCancelValue:YES doneValue:NO];

    XCTAssertEqual(navigation.topViewController, root);
    XCTAssertEqual(detail.state, SCViewControllerStateDismissed);
    XCTAssertTrue(detail.cancelButtonTapped);
}

- (void)testModalNavigationRootRequestsDismissal {
    STVModalTableController *table = [[STVModalTableController alloc] init];
    UINavigationController *tableNavigation = [[UINavigationController alloc] initWithRootViewController:table];
    [table dismissWithCancelValue:NO doneValue:YES];
    XCTAssertTrue(table.requestedDismissal);
    XCTAssertEqual(tableNavigation.topViewController, table);
    XCTAssertEqual(table.state, SCViewControllerStateDismissed);

    STVModalViewController *view = [[STVModalViewController alloc] init];
    UINavigationController *viewNavigation = [[UINavigationController alloc] initWithRootViewController:view];
    [view dismissWithCancelValue:YES doneValue:NO];
    XCTAssertTrue(view.requestedDismissal);
    XCTAssertEqual(viewNavigation.topViewController, view);
    XCTAssertEqual(view.state, SCViewControllerStateDismissed);
}

- (void)testExplicitDetailFocusDismissalDoesNotDismissController {
    SCTableViewController *owner = [[SCTableViewController alloc] init];
    SCTableViewController *detail = [[SCTableViewController alloc] init];
    owner.tableViewModel.detailViewController = detail;
    detail.tableViewModel.masterModel = owner.tableViewModel;
    [detail loadViewIfNeeded];
    [detail gainFocus];
    [detail dismissWithCancelValue:NO doneValue:YES];

    XCTAssertFalse(detail.hasFocus);
    XCTAssertTrue(detail.doneButtonTapped);
    XCTAssertNotEqual(detail.state, SCViewControllerStateDismissed);
    XCTAssertEqual(owner.tableViewModel.detailViewController, detail);
    XCTAssertEqual(detail.tableViewModel.masterModel, owner.tableViewModel);
}

- (void)testExplicitViewControllerDetailFocusDismissalPreservesModel {
    SCViewController *owner = [[SCViewController alloc] init];
    SCViewController *detail = [[SCViewController alloc] init];
    owner.tableViewModel.detailViewController = detail;
    detail.tableViewModel.masterModel = owner.tableViewModel;
    [detail gainFocus];
    [detail dismissWithCancelValue:YES doneValue:NO];

    XCTAssertFalse(detail.hasFocus);
    XCTAssertTrue(detail.cancelButtonTapped);
    XCTAssertNotEqual(detail.state, SCViewControllerStateDismissed);
    XCTAssertEqual(detail.tableViewModel.masterModel, owner.tableViewModel);
}

@end
