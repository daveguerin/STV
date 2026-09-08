@import XCTest;
@import STV;

@interface STVGroupedTopSpacingTests : XCTestCase
@end

@implementation STVGroupedTopSpacingTests

- (void)testGroupedFirstRowUsesAutomaticTopSpacingWithoutCreatingAHeader {
    for (NSNumber *style in @[@(UITableViewStyleGrouped), @(UITableViewStyleInsetGrouped)]) {
        UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
        SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:style.integerValue];
        controller.title = @"Title";
        UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:controller];
        window.rootViewController = navigation;
        [window makeKeyAndVisible];
        SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
        [section addCell:[SCTableViewCell cellWithText:@"A"]];
        [controller.tableViewModel addSection:section];
        [controller.tableView reloadData];
        [navigation.view layoutIfNeeded];
        [controller.tableView layoutIfNeeded];

        UITableView *table = controller.tableView;
        CGRect row = [table rectForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
        XCTAssertEqual([controller.tableViewModel tableView:table heightForHeaderInSection:0], UITableViewAutomaticDimension);
        XCTAssertGreaterThan(CGRectGetMinY(row), 1.0);
        XCTAssertNil(section.headerTitle);
        XCTAssertNil(section.headerView);
        XCTAssertNil([controller.tableViewModel tableView:table viewForHeaderInSection:0]);
        XCTAssertNil([table headerViewForSection:0]);
        XCTAssertNil(table.tableHeaderView);
        window.hidden = YES;
    }

}

- (void)testEmptyGroupedSectionRemainsHeaderlessAcrossReloads {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    window.rootViewController = controller;
    [window makeKeyAndVisible];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    [controller.tableViewModel addSection:section];
    UITableView *table = controller.tableView;
    for (NSNumber *populated in @[@NO, @YES, @NO, @YES, @NO]) {
        [section removeAllCells];
        if (populated.boolValue) {
            [section addCell:[SCTableViewCell cellWithText:@"A"]];
        }

        [table reloadData];
        [controller.view layoutIfNeeded];
        [table layoutIfNeeded];
        XCTAssertNil(section.headerTitle);
        XCTAssertNil(section.headerView);
        XCTAssertNil([table headerViewForSection:0]);
        XCTAssertNil(table.tableHeaderView);
        if (populated.boolValue) {
            XCTAssertEqual([controller.tableViewModel tableView:table heightForHeaderInSection:0], UITableViewAutomaticDimension);
            XCTAssertGreaterThan([table rectForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]].origin.y, 1.0);
        }

        else {
            XCTAssertEqual([table numberOfRowsInSection:0], 0);
            XCTAssertEqual([controller.tableViewModel tableView:table heightForHeaderInSection:0], CGFLOAT_MIN);
            XCTAssertLessThan([table rectForHeaderInSection:0].size.height, 0.01);
        }

    }

    window.hidden = YES;
}

- (void)testPlainAndLaterUntitledSectionsKeepTheirExistingHeaderSpacing {
    for (NSNumber *style in @[@(UITableViewStylePlain), @(UITableViewStyleGrouped)]) {
        UITableView *table = [[UITableView alloc] initWithFrame:CGRectZero style:style.integerValue];
        SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
        for (NSUInteger index = 0; index < 2; index++) {
            SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
            [section addCell:[SCTableViewCell cellWithText:@"A"]];
            [model addSection:section];
            if (index || style.integerValue == UITableViewStylePlain) {
                XCTAssertEqual([model tableView:table heightForHeaderInSection:index], CGFLOAT_MIN);
            }

            XCTAssertNil([model tableView:table viewForHeaderInSection:index]);
            XCTAssertEqual([model tableView:table heightForFooterInSection:index], UITableViewAutomaticDimension);
        }

    }

}

- (void)testAbsentFootersUseAutomaticSpacingWithoutCreatingAView {
    for (NSNumber *style in @[@(UITableViewStylePlain), @(UITableViewStyleGrouped), @(UITableViewStyleInsetGrouped)]) {
        UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
        SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:style.integerValue];
        window.rootViewController = controller;
        [window makeKeyAndVisible];
        SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil footerTitle:nil];
        [controller.tableViewModel addSection:section];
        UITableView *table = controller.tableView;
        for (NSNumber *populated in @[@NO, @YES, @NO]) {
            [section removeAllCells];
            if (populated.boolValue) {
                [section addCell:[SCTableViewCell cellWithText:@"A"]];
            }

            [table reloadData];
            [controller.view layoutIfNeeded];
            [table layoutIfNeeded];
            XCTAssertEqual([controller.tableViewModel tableView:table heightForFooterInSection:0], UITableViewAutomaticDimension);
            XCTAssertNil(section.footerTitle);
            XCTAssertNil(section.footerView);
            XCTAssertNil([controller.tableViewModel tableView:table viewForFooterInSection:0]);
            XCTAssertNil(table.tableFooterView);
            if (style.integerValue != UITableViewStylePlain) {
                XCTAssertNil([table footerViewForSection:0]);
                XCTAssertGreaterThan([table rectForFooterInSection:0].size.height, 1.0);
            }

        }

        window.hidden = YES;
    }

}

- (void)testExistingHeaderPoliciesKeepTheirPrecedence {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStyleGrouped];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:@"Header"];
    [section addCell:[SCTableViewCell cellWithText:@"A"]];
    [model addSection:section];
    XCTAssertEqual([model tableView:table heightForHeaderInSection:0], UITableViewAutomaticDimension);
    XCTAssertNotNil([model tableView:table viewForHeaderInSection:0]);
    model.hideSectionHeaderTitles = YES;
    XCTAssertNil([model tableView:table viewForHeaderInSection:0]);
    section.headerTitle = @"";
    XCTAssertNil([model tableView:table viewForHeaderInSection:0]);
    section.headerView = [[UIView alloc] init];
    XCTAssertNotNil([model tableView:table viewForHeaderInSection:0]);
    section.headerView = nil;
    section.headerHeight = 64;
    XCTAssertNotNil([model tableView:table viewForHeaderInSection:0]);

    SCArrayOfStringsSection *hidden = [SCArrayOfStringsSection sectionWithHeaderTitle:@"Hidden" items:[NSMutableArray array]];
    hidden.hideWhenEmpty = YES;
    hidden.allowAddingItems = NO;
    [model removeAllSections];
    [model addSection:hidden];
    XCTAssertEqual([model tableView:table heightForHeaderInSection:0], CGFLOAT_MIN);
    XCTAssertNil([model tableView:table viewForHeaderInSection:0]);
    XCTAssertEqual([model tableView:table heightForFooterInSection:0], CGFLOAT_MIN);
    XCTAssertNil([model tableView:table viewForFooterInSection:0]);
}

@end
