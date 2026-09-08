@import XCTest;
@import STV;

@interface SCArrayOfItemsSection (STVPresentationTesting)
- (void)presentDetailViewController:(UIViewController *)controller forCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)path withPresentationMode:(SCPresentationMode)mode;
@end

@interface STVModelNavigationRecorder : UINavigationController
@property (nonatomic) BOOL recording;
@property (nonatomic, strong) UIViewController *recordedPush;
@end
@implementation STVModelNavigationRecorder
- (void)pushViewController:(UIViewController *)controller animated:(BOOL)animated {
    if (self.recording)
        self.recordedPush = controller;
    else
        [super pushViewController:controller animated:animated];
}
@end

@interface STVModelAutoLayoutTests : XCTestCase
@end

@implementation STVModelAutoLayoutTests

- (void)testModelDefaultsToAutomaticWithoutConfiguringDuringHeightQueries {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection section];
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Content"];
    [section addCell:cell];
    [model addSection:section];
    __block NSUInteger configurations = 0;
    cell.cellActions.willConfigure = ^(SCTableViewCell *configuredCell, NSIndexPath *path) { configurations++; };
    XCTAssertEqual(model.defaultCellSizingMode, SCTableViewCellSizingModeAutomatic);
    XCTAssertEqual([model tableView:table heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], UITableViewAutomaticDimension);
    XCTAssertEqual(configurations, 0u);
    XCTAssertFalse(cell.configured);
}

- (void)testGeneratedHeadersUseUppercaseHeadlineAndFootersKeepFootnote {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStyleGrouped];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:@"Mixed case header" footerTitle:@"Mixed case footer"];
    [model addSection:section];
    UITableViewHeaderFooterView *header = (id)[model tableView:table viewForHeaderInSection:0];
    UITableViewHeaderFooterView *footer = (id)[model tableView:table viewForFooterInSection:0];
    UILabel *headerLabel = (id)header.contentView.subviews.firstObject;
    UILabel *footerLabel = (id)footer.contentView.subviews.firstObject;
    XCTAssertTrue([headerLabel isKindOfClass:UILabel.class]);
    XCTAssertTrue([footerLabel isKindOfClass:UILabel.class]);
    XCTAssertEqualObjects(headerLabel.text, @"MIXED CASE HEADER");
    XCTAssertEqualObjects(headerLabel.font, [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline]);
    XCTAssertEqualObjects(footerLabel.text, @"Mixed case footer");
    XCTAssertEqualObjects(footerLabel.font, [UIFont preferredFontForTextStyle:UIFontTextStyleFootnote]);
    XCTAssertTrue(headerLabel.adjustsFontForContentSizeCategory);
    XCTAssertTrue(footerLabel.adjustsFontForContentSizeCategory);
    XCTAssertEqual(headerLabel.numberOfLines, 0);
    XCTAssertEqualObjects(section.headerTitle, @"Mixed case header");

    UILabel *customHeader = [[UILabel alloc] init];
    customHeader.text = @"Custom mixed case";
    customHeader.font = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption1];
    section.headerView = customHeader;
    [model tableView:table viewForHeaderInSection:0];
    XCTAssertEqualObjects(customHeader.text, @"Custom mixed case");
    XCTAssertEqualObjects(customHeader.font, [UIFont preferredFontForTextStyle:UIFontTextStyleCaption1]);
}

- (void)testSupplementaryViewsFitConstrainedContentInsteadOfCanvasFrames {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600) style:UITableViewStyleGrouped];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection section];
    UILabel *header = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 100, 999)];
    header.numberOfLines = 0;
    header.text = @"A long section title that wraps across multiple lines when the table becomes narrow. A long section title that wraps across multiple lines when the table becomes narrow.";
    UILabel *footer = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 100, 999)];
    footer.numberOfLines = 0;
    footer.text = header.text;
    section.headerView = header;
    section.footerView = footer;
    [model addSection:section];
    XCTAssertEqual([model tableView:table heightForHeaderInSection:0], UITableViewAutomaticDimension);
    XCTAssertEqual([model tableView:table heightForFooterInSection:0], UITableViewAutomaticDimension);
    XCTAssertGreaterThan(table.estimatedSectionHeaderHeight, 0);
    XCTAssertGreaterThan(table.estimatedSectionFooterHeight, 0);
    for (UIView *view in @[[model tableView:table viewForHeaderInSection:0], [model tableView:table viewForFooterInSection:0]]) {
        XCTAssertTrue([view isKindOfClass:[UITableViewHeaderFooterView class]]);
        UIView *content = [(UITableViewHeaderFooterView *)view contentView];
        CGFloat wide = [content systemLayoutSizeFittingSize:CGSizeMake(600, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
        CGFloat narrow = [content systemLayoutSizeFittingSize:CGSizeMake(180, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
        XCTAssertGreaterThan(narrow, wide);
        XCTAssertLessThan(narrow, 999);
    }
    section.headerHeight = 87;
    section.footerHeight = 63;
    for (NSNumber *isHeader in @[@YES, @NO]) {
        UITableViewHeaderFooterView *view = (id)(isHeader.boolValue ? [model tableView:table viewForHeaderInSection:0] : [model tableView:table viewForFooterInSection:0]);
        CGFloat height = [view.contentView systemLayoutSizeFittingSize:CGSizeMake(320, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
        XCTAssertEqualWithAccuracy(height, isHeader.boolValue ? 87 : 63, 0.5);
    }
}

- (void)testKeyboardChangesInsetsWithoutChangingConstrainedTableFrame {
    UIViewController *controller = [[UIViewController alloc] init];
    controller.view.frame = CGRectMake(0, 0, 320, 600);
    UITableView *table = [[UITableView alloc] init];
    table.translatesAutoresizingMaskIntoConstraints = NO;
    [controller.view addSubview:table];
    [NSLayoutConstraint activateConstraints:@[[table.leadingAnchor constraintEqualToAnchor:controller.view.leadingAnchor], [table.trailingAnchor constraintEqualToAnchor:controller.view.trailingAnchor], [table.topAnchor constraintEqualToAnchor:controller.view.topAnchor], [table.bottomAnchor constraintEqualToAnchor:controller.view.bottomAnchor]]];
    [controller.view layoutIfNeeded];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    model.autoResizeForKeyboard = YES;
    UIEdgeInsets original = UIEdgeInsetsMake(3, 4, 17, 6);
    table.contentInset = original;
    table.verticalScrollIndicatorInsets = original;
    CGRect originalFrame = table.frame;
    NSDictionary *info = @{UIKeyboardFrameEndUserInfoKey: [NSValue valueWithCGRect:CGRectMake(0, 400, 320, 200)], UIKeyboardAnimationDurationUserInfoKey: @0, UIKeyboardAnimationCurveUserInfoKey: @0};
    NSNotification *notification = [NSNotification notificationWithName:UIKeyboardWillShowNotification object:nil userInfo:info];
    [model keyboardWillShow:notification];
    XCTAssertTrue(CGRectEqualToRect(table.frame, originalFrame));
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 217, 0.5);
    [model keyboardWillShow:notification];
    XCTAssertEqualWithAccuracy(table.contentInset.bottom, 217, 0.5);
    [model keyboardWillHide:notification];
    XCTAssertTrue(CGRectEqualToRect(table.frame, originalFrame));
    XCTAssertTrue(UIEdgeInsetsEqualToEdgeInsets(table.contentInset, original));
    XCTAssertTrue(UIEdgeInsetsEqualToEdgeInsets(table.verticalScrollIndicatorInsets, original));
}

- (void)testCustomHeightCallbackConfiguresConstraintsOnlyDuringPreparation {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCArrayOfStringsSection *section = [SCArrayOfStringsSection sectionWithHeaderTitle:nil items:[NSMutableArray arrayWithObject:@"Row"]];
    [model addSection:section];
    __block NSUInteger queries = 0;
    section.sectionActions.customHeightForRowAtIndexPath = ^CGFloat(SCArrayOfItemsSection *items, NSIndexPath *path) {
        queries++;
        return 92;
    };
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    XCTAssertEqual([model tableView:table heightForRowAtIndexPath:path], UITableViewAutomaticDimension);
    XCTAssertEqual(queries, 0u);
    SCTableViewCell *cell = (id)[model tableView:table cellForRowAtIndexPath:path];
    XCTAssertEqual(queries, 1u);
    CGFloat height = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(320, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
    XCTAssertEqualWithAccuracy(height, 92, 0.5);
    section.sectionActions.customHeightForRowAtIndexPath = nil;
    cell = (id)[model tableView:table cellForRowAtIndexPath:path];
    height = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(320, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
    XCTAssertLessThan(height, 92);
}

- (void)testModelFixedDefaultChangesExistingContentConstraints {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection section];
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Row"];
    [section addCell:cell];
    [model addSection:section];
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    [model tableView:table cellForRowAtIndexPath:path];
    for (NSNumber *value in @[@80, @110]) {
        model.defaultCellHeight = value.doubleValue;
        CGFloat height = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(320, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
        XCTAssertEqualWithAccuracy(height, value.doubleValue, 0.5);
        XCTAssertEqual([model tableView:table heightForRowAtIndexPath:path], UITableViewAutomaticDimension);
    }
    model.defaultCellSizingMode = SCTableViewCellSizingModeAutomatic;
    CGFloat height = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(320, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel].height;
    XCTAssertLessThan(height, 80);
}

- (void)testVisibleSupplementaryViewsResizeAndAbsentSectionsStayHidden {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 700, 1000)];
    UIViewController *controller = [[UIViewController alloc] init];
    window.rootViewController = controller;
    [window makeKeyAndVisible];
    UITableView *table = [[UITableView alloc] initWithFrame:controller.view.bounds style:UITableViewStyleGrouped];
    [controller.view addSubview:table];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    NSString *title = @"Long section content that wraps in narrow tables. Long section content that wraps in narrow tables. Long section content that wraps in narrow tables.";
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:title footerTitle:title];
    [section addCell:[SCTableViewCell cellWithText:@"Row"]];
    [model addSection:section];
    [model addSection:[SCTableViewSection section]];
    SCArrayOfStringsSection *hidden = [SCArrayOfStringsSection sectionWithHeaderTitle:@"Hidden" items:[NSMutableArray array]];
    hidden.hideWhenEmpty = YES;
    hidden.allowAddingItems = NO;
    [model addSection:hidden];
    [table reloadData];
    [table layoutIfNeeded];
    CGFloat headerWide = [table rectForHeaderInSection:0].size.height;
    CGFloat footerWide = [table rectForFooterInSection:0].size.height;
    XCTAssertNotNil([table headerViewForSection:0]);
    XCTAssertNotNil([table footerViewForSection:0]);
    for (UIView *supplementary in @[[table headerViewForSection:0], [table footerViewForSection:0]]) {
        NSString *expectedTitle = supplementary == [table headerViewForSection:0] ? title.uppercaseString : title;
        NSMutableArray<UIView *> *pending = [NSMutableArray arrayWithObject:supplementary];
        NSUInteger titleLabels = 0;
        while (pending.count) {
            UIView *view = pending.lastObject;
            [pending removeLastObject];
            if ([view isKindOfClass:[UILabel class]] && !view.hidden && [((UILabel *)view).text isEqualToString:expectedTitle])
                titleLabels++;
            [pending addObjectsFromArray:view.subviews];
        }
        XCTAssertEqual(titleLabels, 1u);
    }
    table.frame = CGRectMake(0, 0, 220, 1000);
    [table reloadData];
    [table layoutIfNeeded];
    XCTAssertGreaterThan([table rectForHeaderInSection:0].size.height, headerWide);
    XCTAssertGreaterThan([table rectForFooterInSection:0].size.height, footerWide);
    for (NSNumber *index in @[@1, @2]) {
        XCTAssertNil([model tableView:table viewForHeaderInSection:index.integerValue]);
        XCTAssertNil([model tableView:table viewForFooterInSection:index.integerValue]);
        XCTAssertLessThan([table rectForHeaderInSection:index.integerValue].size.height, 0.01);
        XCTAssertLessThan([table rectForFooterInSection:index.integerValue].size.height, 0.01);
    }
    window.hidden = YES;
}

- (void)testSearchUsesNavigationItemWithoutManualSearchBarSizing {
    UIViewController *controller = [[UIViewController alloc] init];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:controller];
    [navigation loadViewIfNeeded];
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    [controller.view addSubview:table];
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:table];
    model.enableSearchController = YES;
    XCTAssertEqual(controller.navigationItem.searchController, model.searchController);
    XCTAssertNil(table.tableHeaderView);
    model.enableSearchController = NO;
    XCTAssertNil(controller.navigationItem.searchController);
}

- (void)testSectionPushPassesTheDetailControllerNotItsNavigationContainer {
    UIViewController *owner = [[UIViewController alloc] init];
    STVModelNavigationRecorder *navigation = [[STVModelNavigationRecorder alloc] initWithRootViewController:owner];
    navigation.recording = YES;
    UITableView *table = [[UITableView alloc] init];
    [owner.view addSubview:table];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCArrayOfStringsSection *section = [SCArrayOfStringsSection sectionWithHeaderTitle:nil items:[NSMutableArray array]];
    [model addSection:section];
    SCTableViewController *detail = [[SCTableViewController alloc] init];
    UINavigationController *container = [[UINavigationController alloc] initWithRootViewController:detail];
    [section presentDetailViewController:detail forCell:nil forRowAtIndexPath:nil withPresentationMode:SCPresentationModePush];
    XCTAssertEqual(navigation.recordedPush, detail);
    XCTAssertNotEqual(navigation.recordedPush, container);
}

- (void)testStandaloneSearchBarHasConstrainedVisibleHeight {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:table items:[NSMutableArray arrayWithObject:@"Row"]];
    model.enableSearchController = YES;
    [table reloadData];
    [table layoutIfNeeded];
    UISearchBar *bar = model.searchController.searchBar;
    XCTAssertEqual(table.tableHeaderView, bar);
    XCTAssertFalse(bar.hasAmbiguousLayout);
    XCTAssertGreaterThan(bar.bounds.size.height, 0);
    XCTAssertGreaterThanOrEqual([table rectForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]].origin.y, bar.bounds.size.height);
    model.enableSearchController = NO;
    XCTAssertNil(table.tableHeaderView);
}

@end
