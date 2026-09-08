@import XCTest;
@import STV;

@interface SCCustomCell (STVEditingEvents)
- (void)textFieldEditingChanged:(id)sender;
- (void)sliderValueChanged:(id)sender;
@end

@interface STVEditingValues : NSObject
@property (nonatomic, copy) NSString *text;
@property (nonatomic, strong) NSNumber *value;
@end
@implementation STVEditingValues
@end

@interface STVEditingTableView : UITableView
@property (nonatomic) NSUInteger rowReloads;
@property (nonatomic) BOOL reportsUncommittedUpdates;
@end
@implementation STVEditingTableView
- (BOOL)hasUncommittedUpdates {
    return self.reportsUncommittedUpdates || [super hasUncommittedUpdates];
}
- (void)reloadRowsAtIndexPaths:(NSArray<NSIndexPath *> *)paths withRowAnimation:(UITableViewRowAnimation)animation {
    self.rowReloads += paths.count;
    [super reloadRowsAtIndexPaths:paths withRowAnimation:animation];
}
@end

@interface STVTrackingSlider : UISlider
@property (nonatomic) BOOL trackingForTest;
@end
@implementation STVTrackingSlider
- (BOOL)isTracking {
    // XCTest cannot construct UITouch; only the tracking state is simulated.
    return self.trackingForTest || [super isTracking];
}
@end

@interface STVEditingLayoutTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@property (nonatomic, strong) STVEditingTableView *table;
@property (nonatomic, strong) SCTableViewModel *model;
@property (nonatomic, strong) STVEditingValues *values;
@end

@implementation STVEditingLayoutTests

- (void)setUp {
    [super setUp];
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    UIViewController *controller = [[UIViewController alloc] init];
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    self.table = [[STVEditingTableView alloc] initWithFrame:controller.view.bounds];
    [controller.view addSubview:self.table];
    self.model = [SCTableViewModel modelWithTableView:self.table];
    self.values = [[STVEditingValues alloc] init];
    self.values.text = @"abcd";
    self.values.value = @0.5;
}

- (void)tearDown {
    [self.window endEditing:YES];
    self.window.hidden = YES;
    self.model = nil;
    self.table = nil;
    self.window = nil;
    [super tearDown];
}

- (void)settleLayout {
    [self.table layoutIfNeeded];
    [CATransaction flush];
    [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.08]];
    [self.table layoutIfNeeded];
}

- (void)displayCell:(SCTableViewCell *)cell {
    SCTableViewSection *section = [SCTableViewSection section];
    section.commitCellChangesLive = cell.commitChangesLive;
    [section addCell:cell];
    [self.model addSection:section];
    [self.table reloadData];
    [self settleLayout];
    XCTAssertEqual([self.table cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]], cell);
}

- (void)testSearchCanBeEnabledBeforeTableAndTransferredBetweenHosts {
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:nil];
    XCTAssertNoThrow(model.enableSearchController = YES);
    UISearchController *search = model.searchController;
    XCTAssertNotNil(search);
    model.tableView = self.table;
    [self settleLayout];
    XCTAssertEqual(self.table.tableHeaderView, search.searchBar);
    UITableView *second = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 300, 500)];
    [self.window.rootViewController.view addSubview:second];
    model.tableView = second;
    [self.window layoutIfNeeded];
    XCTAssertNil(self.table.tableHeaderView);
    XCTAssertEqual(second.tableHeaderView, search.searchBar);
    XCTAssertEqual(model.searchController, search);
    model.enableSearchController = NO;
    XCTAssertNil(second.tableHeaderView);
    XCTAssertNil(model.searchBar);
}

- (void)testGeneratedBindingLoadsBeforeConfigurationButNotHeightQueries {
    self.values.text = @"Existing long text\nSecond line\nThird line";
    SCTextViewCell *cell = [[SCTextViewCell alloc] initWithText:nil boundObject:self.values textViewTextPropertyName:@"text"];
    SCTableViewSection *section = [SCTableViewSection section];
    [section addCell:cell];
    [self.model addSection:section];
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    __block NSString *observed;
    cell.cellActions.willConfigure = ^(SCTableViewCell *row, NSIndexPath *indexPath) { observed = [((SCTextViewCell *)row).textView.text copy]; };
    [self.model tableView:self.table heightForRowAtIndexPath:path];
    XCTAssertEqual(cell.textView.text.length, 0u);
    XCTAssertNil(observed);
    [self.model tableView:self.table cellForRowAtIndexPath:path];
    XCTAssertEqualObjects(observed, self.values.text);
    XCTAssertEqualObjects(cell.textView.text, self.values.text);
}

- (void)testTextViewResizesWithoutLosingUncommittedTextOrSelection {
    SCTextViewCell *cell = [[SCTextViewCell alloc] initWithText:nil boundObject:self.values textViewTextPropertyName:@"text"];
    cell.commitChangesLive = NO;
    cell.autoResize = YES;
    cell.maximumHeight = 400;
    [self displayCell:cell];
    UITextView *input = cell.textView;
    XCTAssertTrue([input becomeFirstResponder]);
    [self settleLayout];
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    CGFloat originalHeight = [self.table rectForRowAtIndexPath:path].size.height;
    NSString *longText = @"First line\nSecond line\nThird line\nFourth line\nFifth line\nSixth line\nSeventh line\nEighth line";
    for (NSString *text in @[longText, @"Short", longText]) {
        input.text = text;
        input.selectedRange = NSMakeRange(2, 1);
        [input.delegate textViewDidChange:input];
        [self settleLayout];
        XCTAssertTrue(input.isFirstResponder);
        XCTAssertEqualObjects(input.text, text);
        XCTAssertTrue(NSEqualRanges(input.selectedRange, NSMakeRange(2, 1)));
        XCTAssertEqualObjects(self.values.text, @"abcd");
        XCTAssertTrue(cell.needsCommit);
        CGFloat height = [self.table rectForRowAtIndexPath:path].size.height;
        if ([text isEqualToString:longText])
            XCTAssertGreaterThan(height, originalHeight + 40);
        else
            XCTAssertEqualWithAccuracy(height, originalHeight, 1);
    }
    [cell commitChanges];
    XCTAssertEqualObjects(self.values.text, longText);
    XCTAssertEqual(self.table.rowReloads, 0u);
}

- (void)testMarkedTextSurvivesInvalidationDuringTableUpdates {
    SCTextFieldCell *cell = [[SCTextFieldCell alloc] initWithText:@"Name" placeholder:nil boundObject:self.values textFieldTextPropertyName:@"text"];
    cell.commitChangesLive = NO;
    [self displayCell:cell];
    UITextField *input = cell.textField;
    XCTAssertTrue([input becomeFirstResponder]);
    for (NSString *composition in @[@"ni", @"nih", @"nihao"]) {
        [input setMarkedText:composition selectedRange:NSMakeRange(1, 0)];
        XCTAssertNotNil(input.markedTextRange);
        NSString *expected = [input.text copy];
        self.table.reportsUncommittedUpdates = YES;
        [cell textFieldEditingChanged:input];
        self.table.reportsUncommittedUpdates = NO;
        [self settleLayout];
        XCTAssertTrue(input.isFirstResponder);
        XCTAssertNotNil(input.markedTextRange);
        XCTAssertEqualObjects([input textInRange:input.markedTextRange], composition);
        XCTAssertEqualObjects(input.text, expected);
        XCTAssertEqual([input offsetFromPosition:input.markedTextRange.start toPosition:input.selectedTextRange.start], 1);
        XCTAssertEqualObjects(self.values.text, @"abcd");
    }
    [input unmarkText];
    [cell commitChanges];
    XCTAssertEqualObjects(self.values.text, input.text);
    XCTAssertEqual(self.table.rowReloads, 0u);
}

- (void)testDeferredInvalidationRechecksNewFirstResponder {
    SCTextFieldCell *cell = [[SCTextFieldCell alloc] initWithText:@"Name" placeholder:nil boundObject:self.values textFieldTextPropertyName:@"text"];
    [self displayCell:cell];
    self.table.reportsUncommittedUpdates = YES;
    [self.model invalidateHeightForCell:cell animated:NO];
    XCTAssertTrue([cell.textField becomeFirstResponder]);
    self.table.reportsUncommittedUpdates = NO;
    [self settleLayout];
    XCTAssertTrue(cell.textField.isFirstResponder);
    XCTAssertTrue(cell.configured);
    XCTAssertEqual(self.table.rowReloads, 0u);
}

- (void)testStandaloneSearchHeaderOccupiesSpaceAboveRows {
    self.model = [[SCArrayOfStringsModel alloc] initWithTableView:self.table items:[NSMutableArray arrayWithObject:@"Row"]];
    [self.table reloadData];
    [self settleLayout];
    XCTAssertNotNil([self.table cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]]);
    ((SCArrayOfStringsModel *)self.model).enableSearchController = YES;
    [self settleLayout];
    UISearchBar *bar = ((SCArrayOfStringsModel *)self.model).searchBar;
    XCTAssertNotNil(bar);
    XCTAssertEqual(self.table.tableHeaderView, bar);
    XCTAssertEqual(bar.window, self.window);
    XCTAssertGreaterThan(bar.bounds.size.height, 0);
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    XCTAssertGreaterThanOrEqual(CGRectGetMinY([self.table rectForRowAtIndexPath:path]), CGRectGetMaxY(bar.frame));
    XCTAssertEqualWithAccuracy(bar.bounds.size.width, self.table.bounds.size.width, 0.5);
    ((SCArrayOfStringsModel *)self.model).enableSearchController = NO;
    XCTAssertNil(self.table.tableHeaderView);
}

- (void)testTrackingSliderDoesNotReloadOrReconfigureAcrossContinuousEvents {
    SCSliderCell *cell = [[SCSliderCell alloc] initWithText:@"Level" boundObject:self.values sliderValuePropertyName:@"value"];
    STVTrackingSlider *slider = [[STVTrackingSlider alloc] init];
    cell.control = slider;
    cell.commitChangesLive = YES;
    __block NSUInteger configurations = 0;
    cell.cellActions.willConfigure = ^(SCTableViewCell *row, NSIndexPath *path) { configurations++; };
    [self displayCell:cell];
    NSUInteger initialConfigurations = configurations;
    slider.trackingForTest = YES;
    for (NSNumber *value in @[@0.2, @0.4, @0.8]) {
        slider.value = value.floatValue;
        [cell sliderValueChanged:slider];
        [self settleLayout];
        XCTAssertEqualWithAccuracy(self.values.value.floatValue, value.floatValue, 0.001);
        XCTAssertEqual(self.model.activeCellControl, slider);
        XCTAssertTrue(slider.isTracking);
        XCTAssertEqual(slider.window, self.window);
    }
    XCTAssertEqual(configurations, initialConfigurations);
    XCTAssertEqual(self.table.rowReloads, 0u);
    slider.trackingForTest = NO;
}

- (void)testTextFieldKeepsFirstResponderCaretAndLiveBindingAcrossEdits {
    SCTextFieldCell *cell = [[SCTextFieldCell alloc] initWithText:@"Name" placeholder:nil boundObject:self.values textFieldTextPropertyName:@"text"];
    cell.commitChangesLive = YES;
    __block NSUInteger configurations = 0;
    __block NSUInteger callbacks = 0;
    cell.cellActions.willConfigure = ^(SCTableViewCell *row, NSIndexPath *path) { configurations++; };
    cell.cellActions.valueChanged = ^(SCTableViewCell *row, NSIndexPath *path) { callbacks++; };
    [self displayCell:cell];
    UITextField *input = cell.textField;
    XCTAssertTrue([input becomeFirstResponder]);
    [self settleLayout];
    NSUInteger initialConfigurations = configurations;
    UITextPosition *position = [input positionFromPosition:input.beginningOfDocument offset:2];
    input.selectedTextRange = [input textRangeFromPosition:position toPosition:position];
    for (NSString *insertion in @[@"X", @"Y", @"Z"]) {
        [input insertText:insertion];
        NSString *expectedText = [input.text copy];
        NSInteger expectedCaret = [input offsetFromPosition:input.beginningOfDocument toPosition:input.selectedTextRange.start];
        // Hostless SwiftPM XCTest has no UIApp to dispatch target actions.
        // Exercise the registered STV handler after real UITextInput editing.
        [cell textFieldEditingChanged:input];
        [self settleLayout];
        XCTAssertTrue(input.isFirstResponder);
        XCTAssertEqualObjects(input.text, expectedText);
        XCTAssertEqualObjects(self.values.text, expectedText);
        XCTAssertEqual([input offsetFromPosition:input.beginningOfDocument toPosition:input.selectedTextRange.start], expectedCaret);
        XCTAssertEqual(self.model.activeCell, cell);
        XCTAssertTrue(cell.configured);
    }
    XCTAssertEqualObjects(input.text, @"abXYZcd");
    XCTAssertEqual(callbacks, 3u);
    XCTAssertEqual(configurations, initialConfigurations);
    XCTAssertEqual(self.table.rowReloads, 0u);
}
@end
