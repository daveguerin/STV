@import XCTest;
@import STV;

@interface STVSelectionSummaryLayoutTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@property (nonatomic, strong) UITableView *table;
@property (nonatomic, strong) SCTableViewModel *model;
@end

@implementation STVSelectionSummaryLayoutTests

- (void)setUp {
    [super setUp];
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    UIViewController *controller = [[UIViewController alloc] init];
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    self.table = [[UITableView alloc] initWithFrame:controller.view.bounds];
    [controller.view addSubview:self.table];
    self.model = [SCTableViewModel modelWithTableView:self.table];
}

- (void)tearDown {
    self.window.hidden = YES;
    self.window.rootViewController = nil;
    self.model = nil;
    self.table = nil;
    self.window = nil;
    [super tearDown];
}

- (void)settleLayout {
    [self.table layoutIfNeeded];
    [CATransaction flush];
    [[NSRunLoop mainRunLoop]
        runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    [self.table layoutIfNeeded];
}

- (SCSelectionCell *)cellForClass:(Class)cellClass {
    NSArray *items = @[@"Zero", @"One", @"Two", @"Three"];
    SCSelectionCell *cell;
    if (cellClass == [SCObjectSelectionCell class]) {
        SCStringDefinition *definition = [SCStringDefinition definition];
        definition.titlePropertyName = @"string";
        cell = [SCObjectSelectionCell cellWithText:@"Selection"
                                      boundObject:nil
                       selectedObjectPropertyName:nil
                                   selectionItems:items
                                   itemsDefintion:definition];
    }

    else {
        cell = [SCSelectionCell cellWithText:@"Selection"
                                boundObject:nil
                 selectedIndexesPropertyName:nil
                                      items:items
                     allowMultipleSelection:YES];
    }

    cell.allowMultipleSelection = YES;
    cell.commitChangesLive = NO;
    return cell;
}

- (void)displayCell:(SCSelectionCell *)cell {
    [self.model clear];
    SCTableViewSection *section = [SCTableViewSection section];
    section.commitCellChangesLive = NO;
    [section addCell:cell];
    [self.model addSection:section];
    [self.table reloadData];
    [self settleLayout];
}

- (void)testNewlineMultipleSelectionGrowsAndShrinksBothCellClasses {
    for (Class cellClass in @[[SCSelectionCell class],
                             [SCObjectSelectionCell class]]) {
        SCSelectionCell *cell = [self cellForClass:cellClass];
        cell.delimeter = @"\n";
        [self displayCell:cell];
        [cell.selectedItemsIndexes addObject:@0];
        [cell cellValueChanged];
        [self settleLayout];
        CGFloat singleHeight = CGRectGetHeight(cell.frame);

        [cell.selectedItemsIndexes addObjectsFromArray:@[@1, @2, @3]];
        [cell cellValueChanged];
        [self settleLayout];
        XCTAssertEqualObjects(cell.label.text, @"Zero\nOne\nTwo\nThree");
        XCTAssertEqual(cell.label.numberOfLines, 0);
        XCTAssertGreaterThan(CGRectGetHeight(cell.frame), singleHeight + 30);
        XCTAssertGreaterThanOrEqual(CGRectGetHeight(cell.label.frame),
                                   cell.label.font.lineHeight * 4 - 1);
        XCTAssertEqual([self.table cellForRowAtIndexPath:
            [NSIndexPath indexPathForRow:0 inSection:0]], cell);

        [cell.selectedItemsIndexes removeAllObjects];
        [cell.selectedItemsIndexes addObject:@0];
        [cell cellValueChanged];
        [self settleLayout];
        XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.frame), singleHeight, 1);
    }
}

- (void)testConfigurationChangesRefreshVisibleSummaryWithoutCommitting {
    for (Class cellClass in @[[SCSelectionCell class],
                             [SCObjectSelectionCell class]]) {
        SCSelectionCell *cell = [self cellForClass:cellClass];
        [self displayCell:cell];
        [cell.selectedItemsIndexes addObjectsFromArray:@[@0, @1, @2, @3]];
        [cell cellValueChanged];
        [self settleLayout];
        CGFloat singleHeight = CGRectGetHeight(cell.frame);
        __block NSUInteger valueChanges = 0;
        cell.cellActions.valueChanged = ^(SCTableViewCell *row,
                                          NSIndexPath *indexPath) {
            valueChanges++;
        };
        NSSet *indexes = [cell.selectedItemsIndexes copy];

        cell.delimeter = @"\n";
        [self settleLayout];
        XCTAssertEqual(cell.label.numberOfLines, 0);
        XCTAssertEqualObjects(cell.label.text, @"Zero\nOne\nTwo\nThree");
        XCTAssertGreaterThan(CGRectGetHeight(cell.frame), singleHeight + 30);

        cell.delimeter = @", ";
        [self settleLayout];
        XCTAssertEqual(cell.label.numberOfLines, 1);
        XCTAssertEqualObjects(cell.label.text, @"Zero, One, Two, Three");
        XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.frame), singleHeight, 1);

        cell.allowMultipleSelection = NO;
        cell.delimeter = @"\n";
        XCTAssertEqual(cell.label.numberOfLines, 1);
        cell.allowMultipleSelection = YES;
        [self settleLayout];
        XCTAssertEqual(cell.label.numberOfLines, 0);
        XCTAssertGreaterThan(CGRectGetHeight(cell.frame), singleHeight + 30);
        cell.allowMultipleSelection = NO;
        [self settleLayout];
        XCTAssertEqual(cell.label.numberOfLines, 1);
        XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.frame), singleHeight, 1);
        XCTAssertEqualObjects(cell.selectedItemsIndexes, indexes);
        XCTAssertEqual(valueChanges, 0u);
    }
}

- (void)testAutomaticLinePolicyRestoresCallerSettings {
    SCSelectionCell *cell = [self cellForClass:[SCSelectionCell class]];
    cell.label.numberOfLines = 3;
    cell.delimeter = @"\n";
    XCTAssertEqual(cell.label.numberOfLines, 0);
    cell.delimeter = @", ";
    XCTAssertEqual(cell.label.numberOfLines, 3);

    cell.label.numberOfLines = 0;
    cell.delimeter = @"\n";
    cell.allowMultipleSelection = NO;
    XCTAssertEqual(cell.label.numberOfLines, 0);

    cell.allowMultipleSelection = YES;
    cell.label.numberOfLines = 2;
    cell.delimeter = @"; ";
    XCTAssertEqual(cell.label.numberOfLines, 2);

    cell.delimeter = @"\n";
    UILabel *originalLabel = cell.label;
    UILabel *replacement = [[UILabel alloc] init];
    replacement.numberOfLines = 4;
    cell.control = replacement;
    [cell reloadBoundValue];
    XCTAssertEqual(originalLabel.numberOfLines, 2);
    XCTAssertEqual(replacement.numberOfLines, 0);
    cell.delimeter = @", ";
    XCTAssertEqual(replacement.numberOfLines, 4);
}

@end
