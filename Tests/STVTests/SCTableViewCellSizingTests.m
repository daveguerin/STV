@import XCTest;
@import STV;

@interface STVRecordingTableView : UITableView
@property (nonatomic, copy) NSArray<NSIndexPath *> *recordedReloadedIndexPaths;
@property (nonatomic) BOOL reportsUncommittedUpdates;
@end

@implementation STVRecordingTableView

- (void)reloadRowsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation
{
    self.recordedReloadedIndexPaths = indexPaths;
}

- (BOOL)hasUncommittedUpdates
{
    return self.reportsUncommittedUpdates;
}

@end

@interface STVRecordingSizingModel : SCTableViewModel
@property (nonatomic, copy) NSString *textObservedAtInvalidation;
@end

@implementation STVRecordingSizingModel

- (void)invalidateHeightForCell:(SCTableViewCell *)cell animated:(BOOL)animated
{
    self.textObservedAtInvalidation = cell.textLabel.text;
    [super invalidateHeightForCell:cell animated:animated];
}

@end

@interface STVBindingRecordingCustomCell : SCCustomCell
@property (nonatomic) NSUInteger bindingLoadCount;
@end

@implementation STVBindingRecordingCustomCell

- (void)loadBindingsIntoCustomControls
{
    self.bindingLoadCount += 1;
    [super loadBindingsIntoCustomControls];
}

@end

@interface SCTableViewCellSizingTests : XCTestCase
@end

@implementation SCTableViewCellSizingTests

- (void)testAutomaticSizingModePreservesUIKitAutomaticDimension
{
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];

    cell.sizingMode = SCTableViewCellSizingModeAutomatic;

    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
}

- (void)testAutomaticCustomCellHeightQueryDoesNotLoadBindings
{
    STVBindingRecordingCustomCell *cell = [[STVBindingRecordingCustomCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];
    cell.sizingMode = SCTableViewCellSizingModeAutomatic;

    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    XCTAssertEqual(cell.bindingLoadCount, 0u);
}

- (void)testArrayBackedAutomaticCustomCellLoadsBindingsDuringPreparationNotHeightQuery
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCArrayOfStringsSection *section = [SCArrayOfStringsSection sectionWithHeaderTitle:nil items:[NSMutableArray arrayWithObject:@"Bound value"]];
    STVBindingRecordingCustomCell *cell = [[STVBindingRecordingCustomCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];
    cell.sizingMode = SCTableViewCellSizingModeAutomatic;
    section.sectionActions.cellForRowAtIndexPath = ^SCCustomCell *(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath) {
        return cell;
    };
    [model addSection:section];
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:0];

    XCTAssertEqual([model tableView:tableView heightForRowAtIndexPath:indexPath], UITableViewAutomaticDimension);
    XCTAssertFalse(cell.configured);
    XCTAssertEqual(cell.bindingLoadCount, 0u);

    XCTAssertEqual([model tableView:tableView cellForRowAtIndexPath:indexPath], cell);
    XCTAssertTrue(cell.configured);
    XCTAssertEqual(cell.bindingLoadCount, 1u);
}

- (void)testLiteralFortyFourIsAnExplicitFixedHeight
{
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:nil];
    cell.sizingMode = SCTableViewCellSizingModeLegacy;
    cell.textLabel.text = @"Title";
    cell.detailTextLabel.text = @"Detail";
    cell.textLabel.font = [UIFont systemFontOfSize:50.0];
    cell.detailTextLabel.font = [UIFont systemFontOfSize:20.0];

    cell.height = UITableViewAutomaticDimension;
    XCTAssertGreaterThan(cell.height, 44.0);

    cell.height = 44.0;

    XCTAssertEqual(cell.height, 44.0);
}

- (void)testModelDefaultsInheritedCellsToFixedFortyFourPointHeight
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Fixed"];
    [section addCell:cell];
    [model addSection:section];

    CGFloat height = [model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];

    XCTAssertEqual(model.defaultCellSizingMode, SCTableViewCellSizingModeFixed);
    XCTAssertEqual(model.defaultCellHeight, 44.0);
    XCTAssertEqual(height, 44.0);
}

- (void)testModelFixedHeightAppliesToStandardCells
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellHeight = 52.0;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *standardCell = [[SCTableViewCell alloc] initWithText:@"Standard"];
    [section addCell:standardCell];
    [model addSection:section];

    XCTAssertEqualWithAccuracy([model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], 52.0, 0.001);
}

- (void)testGeneratedControlCellsInheritModelFixedHeight
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellHeight = 52.0;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCControlCell *controlCell = [[SCControlCell alloc] initWithText:@"Control"];
    [section addCell:controlCell];
    [model addSection:section];

    XCTAssertEqualWithAccuracy([model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], 52.0, 0.001);
    XCTAssertEqual(controlCell.sizingMode, SCTableViewCellSizingModeInherit);
}

- (void)testCellFixedSizingUsesModelDefaultHeight
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellSizingMode = SCTableViewCellSizingModeAutomatic;
    model.defaultCellHeight = 52.0;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Fixed"];
    cell.sizingMode = SCTableViewCellSizingModeFixed;
    [section addCell:cell];
    [model addSection:section];

    XCTAssertEqualWithAccuracy([model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], 52.0, 0.001);
}

- (void)testExplicitCellHeightOverridesModelFixedHeight
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellHeight = 52.0;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Explicit"];
    cell.height = 61.0;
    [section addCell:cell];
    [model addSection:section];

    XCTAssertEqualWithAccuracy([model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], 61.0, 0.001);
}

- (void)testExplicitAutomaticCellOverridesModelFixedSizing
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Automatic"];
    cell.sizingMode = SCTableViewCellSizingModeAutomatic;
    [section addCell:cell];
    [model addSection:section];

    XCTAssertEqual([model tableView:tableView heightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], UITableViewAutomaticDimension);
}

- (void)testCellConfigurationOccursDuringCellPreparationNotHeightResolution
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Configure"];
    __block NSUInteger configureCount = 0;
    cell.cellActions.willConfigure = ^(SCTableViewCell *configuredCell, NSIndexPath *indexPath) {
        configureCount++;
    };
    [section addCell:cell];
    [model addSection:section];
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:0];

    [model tableView:tableView cellForRowAtIndexPath:indexPath];
    XCTAssertEqual(configureCount, 1u);

    [model tableView:tableView heightForRowAtIndexPath:indexPath];
    XCTAssertEqual(configureCount, 1u);
}

- (void)testControlCellsKeepLegacyManualLayoutUntilLoadedFromInterfaceBuilder
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellSizingMode = SCTableViewCellSizingModeAutomatic;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCControlCell *cell = [[SCControlCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];
    [section addCell:cell];
    [model addSection:section];

    XCTAssertEqual(cell.sizingMode, SCTableViewCellSizingModeInherit);
    XCTAssertEqual(cell.height, 44.0);

    [cell awakeFromNib];

    XCTAssertEqual(cell.sizingMode, SCTableViewCellSizingModeAutomatic);
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
}

- (void)testAutomaticCellsExposeTheirEstimatedHeightToUITableView
{
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    model.defaultCellSizingMode = SCTableViewCellSizingModeAutomatic;
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Estimate"];
    cell.estimatedHeight = 123.0;
    [section addCell:cell];
    [model addSection:section];

    CGFloat estimate = [model tableView:tableView estimatedHeightForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];

    XCTAssertEqual(estimate, 123.0);
}

- (void)testInvalidatingAutomaticCellReloadsItsOwningRow
{
    STVRecordingTableView *tableView = [[STVRecordingTableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Invalidate"];
    [section addCell:cell];
    [model addSection:section];
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:0];
    cell.configured = YES;

    [model invalidateHeightForCell:cell animated:NO];

    XCTAssertFalse(cell.configured);
    XCTAssertEqualObjects(tableView.recordedReloadedIndexPaths, @[indexPath]);
}

- (void)testInvalidationDefersUntilTableUpdatesAreCommitted
{
    STVRecordingTableView *tableView = [[STVRecordingTableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Deferred"];
    [section addCell:cell];
    [model addSection:section];
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:0];
    tableView.reportsUncommittedUpdates = YES;

    [model invalidateHeightForCell:cell animated:NO];
    XCTAssertNil(tableView.recordedReloadedIndexPaths);

    XCTestExpectation *expectation = [self expectationWithDescription:@"Deferred row invalidation"];
    tableView.reportsUncommittedUpdates = NO;
    dispatch_async(dispatch_get_main_queue(), ^{
        XCTAssertEqualObjects(tableView.recordedReloadedIndexPaths, @[indexPath]);
        [expectation fulfill];
    });
    [self waitForExpectations:@[expectation] timeout:1.0];
}

- (void)testXIBCanvasHeightBecomesEstimateRatherThanFixedHeight
{
    SCCustomCell *cell = [SCCustomCell cellWithText:nil
                                    objectBindings:@{}
                                           nibName:@"STV_STVTests_AutomaticHeightCell"];

    XCTAssertNotNil(cell);
    XCTAssertGreaterThan(cell.frame.size.height, 100.0);
    XCTAssertEqual(cell.sizingMode, SCTableViewCellSizingModeAutomatic);
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    XCTAssertEqualWithAccuracy(cell.estimatedHeight, 173.0, 1.0);
}

- (void)testValueChangedCallbackUpdatesContentBeforeAutomaticHeightInvalidation
{
    STVRecordingTableView *tableView = [[STVRecordingTableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    STVRecordingSizingModel *model = [STVRecordingSizingModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *cell = [[SCTableViewCell alloc] initWithText:@"Before"];
    cell.sizingMode = SCTableViewCellSizingModeAutomatic;
    cell.cellActions.valueChanged = ^(SCTableViewCell *changedCell, NSIndexPath *indexPath) {
        changedCell.textLabel.text = @"After";
    };
    [section addCell:cell];
    [model addSection:section];

    [cell cellValueChanged];

    XCTAssertEqualObjects(model.textObservedAtInvalidation, @"After");
}

@end
