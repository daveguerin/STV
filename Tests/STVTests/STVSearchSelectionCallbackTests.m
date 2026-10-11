@import XCTest;
@import STV;

@interface STVSearchSelectionCallbackCell : SCCustomCell
@property (nonatomic) NSUInteger didSelectCount;
@property (nonatomic) NSUInteger willDeselectCount;
@property (nonatomic) NSUInteger didDeselectCount;
@end

@implementation STVSearchSelectionCallbackCell

- (void)didSelectCell {
    [super didSelectCell];
    self.didSelectCount++;
}

- (void)willDeselectCell {
    [super willDeselectCell];
    self.willDeselectCount++;
}

- (void)didDeselectCell {
    [super didDeselectCell];
    self.didDeselectCount++;
}

@end

@interface STVSearchSelectionCallbackTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@property (nonatomic, strong) UIViewController *owner;
@property (nonatomic, strong) UITableView *table;
@property (nonatomic, strong) SCArrayOfStringsModel *model;
@property (nonatomic, strong) NSIndexPath *path;
@property (nonatomic, strong) STVSearchSelectionCallbackCell *renderedCell;
@end

@implementation STVSearchSelectionCallbackTests

- (void)setUp {
    [super setUp];
    XCTAssertTrue(NSThread.isMainThread);
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    self.owner = [UIViewController new];
    self.window.rootViewController = self.owner;
    [self.window makeKeyAndVisible];
    self.table = [[UITableView alloc] initWithFrame:self.owner.view.bounds style:UITableViewStylePlain];
    [self.owner.view addSubview:self.table];
    self.model = [SCArrayOfStringsModel modelWithTableView:self.table items:[NSMutableArray arrayWithObjects:@"Amber", @"Azure", nil]];
    self.model.sectionActions.cellForRowAtIndexPath = ^SCCustomCell *(SCArrayOfItemsSection *section, NSIndexPath *indexPath) {
        STVSearchSelectionCallbackCell *cell = [[STVSearchSelectionCallbackCell alloc] initWithText:@"Callback row"];
        cell.autoDeselect = NO;
        return cell;
    };
    self.model.enableSearchController = YES;
    [self.table reloadData];
    [self.table layoutIfNeeded];
    self.model.searchController.active = YES;
    self.model.searchBar.text = @"No matching row";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertEqual(self.model.sectionCount, 0u);
    self.model.searchBar.text = @"";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    [self.table reloadData];
    [self.table layoutIfNeeded];
    self.path = [NSIndexPath indexPathForRow:0 inSection:0];
    self.renderedCell = (id)[self.table cellForRowAtIndexPath:self.path];
    // This hostless fixture proves callback dispatch with actual UIKit tables,
    // not native search presentation, transitions, or a user-generated tap.
    XCTAssertTrue(self.model.searchController.active);
    XCTAssertNotEqual(self.model.tableView, self.table);
    XCTAssertNotNil(self.renderedCell);
    XCTAssertEqual(self.renderedCell.window, self.window);
    XCTAssertTrue([[self.table indexPathsForVisibleRows] containsObject:self.path]);
    XCTAssertNotEqual([self.model.tableView cellForRowAtIndexPath:self.path], self.renderedCell);
}

- (void)tearDown {
    self.model.enableSearchController = NO;
    self.window.hidden = YES;
    self.renderedCell = nil;
    self.model = nil;
    self.table = nil;
    self.owner = nil;
    self.window = nil;
    [super tearDown];
}

- (void)testActiveEmptySearchDispatchesRenderedCellActions {
    [self verifyCallbacksWithActions:self.renderedCell.cellActions];
}

- (void)testActiveEmptySearchDispatchesRenderedCellToSectionActions {
    [self verifyCallbacksWithActions:[self.model sectionAtIndex:0].cellActions];
}

- (void)testActiveEmptySearchDispatchesRenderedCellToModelActions {
    [self verifyCallbacksWithActions:self.model.cellActions];
}

- (void)verifyCallbacksWithActions:(SCCellActions *)actions {
    NSMutableArray<SCTableViewCell *> *callbackCells = [NSMutableArray array];
    NSMutableArray<NSIndexPath *> *callbackPaths = [NSMutableArray array];
    NSMutableArray<NSString *> *events = [NSMutableArray array];
    actions.willSelect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath) {
        [events addObject:@"willSelect"];
        if (cell) {
            [callbackCells addObject:cell];
        }
        [callbackPaths addObject:indexPath];
        return YES;
    };
    actions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath) {
        [events addObject:@"didSelect"];
        if (cell) {
            [callbackCells addObject:cell];
        }
        [callbackPaths addObject:indexPath];
    };
    actions.willDeselect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath) {
        [events addObject:@"willDeselect"];
        if (cell) {
            [callbackCells addObject:cell];
        }
        [callbackPaths addObject:indexPath];
        return YES;
    };
    actions.didDeselect = ^(SCTableViewCell *cell, NSIndexPath *indexPath) {
        [events addObject:@"didDeselect"];
        if (cell) {
            [callbackCells addObject:cell];
        }
        [callbackPaths addObject:indexPath];
    };

    XCTAssertEqualObjects([self.model tableView:self.table willSelectRowAtIndexPath:self.path], self.path);
    // UIKit marks the row selected before delivering didSelect. Avoid testing
    // the separate programmatic active-cell selection callback path here.
    [self.table selectRowAtIndexPath:self.path animated:NO scrollPosition:UITableViewScrollPositionNone];
    [self.model enterLoadingMode];
    XCTAssertFalse(self.model.live);
    [self.model tableView:self.table didSelectRowAtIndexPath:self.path];
    XCTAssertTrue(self.model.live, @"An attached underlying-table row is live UI even while search routes model.tableView elsewhere");
    XCTAssertEqual(self.model.activeCell, self.renderedCell);
    XCTAssertEqual(self.renderedCell.didSelectCount, 1u);
    XCTAssertEqualObjects([self.model tableView:self.table willDeselectRowAtIndexPath:self.path], self.path);
    [self.table deselectRowAtIndexPath:self.path animated:NO];
    [self.model tableView:self.table didDeselectRowAtIndexPath:self.path];
    XCTAssertEqual(self.renderedCell.willDeselectCount, 1u);
    XCTAssertEqual(self.renderedCell.didDeselectCount, 1u);
    XCTAssertEqualObjects(events, (@[@"willSelect", @"didSelect", @"willDeselect", @"didDeselect"]));
    XCTAssertEqualObjects(callbackCells, (@[self.renderedCell, self.renderedCell, self.renderedCell, self.renderedCell]));
    XCTAssertEqualObjects(callbackPaths, (@[self.path, self.path, self.path, self.path]));
    actions.willSelect = nil;
    actions.didSelect = nil;
    actions.willDeselect = nil;
    actions.didDeselect = nil;
}

- (void)testActiveEmptySearchHonorsRenderedCellSelectionAndDeselectionVetoes {
    __block NSUInteger willSelectCount = 0;
    __block NSUInteger willDeselectCount = 0;
    self.renderedCell.cellActions.willSelect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath) {
        willSelectCount++;
        return NO;
    };
    self.renderedCell.cellActions.willDeselect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath) {
        willDeselectCount++;
        return NO;
    };
    XCTAssertNil([self.model tableView:self.table willSelectRowAtIndexPath:self.path]);
    XCTAssertNil([self.model tableView:self.table willDeselectRowAtIndexPath:self.path]);
    XCTAssertEqual(willSelectCount, 1u);
    XCTAssertEqual(willDeselectCount, 1u);
    XCTAssertEqual(self.renderedCell.willDeselectCount, 0u);
}

@end
