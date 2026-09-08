@import XCTest;
@import STV;

@interface SCDateCell (ConstraintTests)
- (void)showEmbeddedDatePicker;
- (void)hideEmbeddedDatePicker;
@end

@interface SCLayoutPendingStore : SCDataStore
@end
@implementation SCLayoutPendingStore
- (void)asynchronousFetchObjectsWithOptions:(SCDataFetchOptions *)options success:(SCDataStoreFetchSuccess_Block)success failure:(SCDataStoreFailure_Block)failure noConnection:(SCNoConnection_Block)noConnection {
    // Keep the request pending so the loading state can be laid out.
}
@end

@interface SCCellConstraintLayoutTests : XCTestCase
@end

@implementation SCCellConstraintLayoutTests

- (CGFloat)fitCell:(SCTableViewCell *)cell width:(CGFloat)width {
    cell.bounds = CGRectMake(0, 0, width, 600);
    [cell setNeedsUpdateConstraints];
    [cell updateConstraintsIfNeeded];
    [cell setNeedsLayout];
    [cell layoutIfNeeded];
    CGSize size = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(CGRectGetWidth(cell.contentView.bounds), 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel];
    cell.bounds = CGRectMake(0, 0, width, size.height);
    [cell setNeedsLayout];
    [cell layoutIfNeeded];
    return size.height;
}

- (void)testBaseCellUsesConstraintsForLegacyAndExplicitHeight {
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"A title"];
    cell.sizingMode = SCTableViewCellSizingModeLegacy;
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    XCTAssertGreaterThanOrEqual([self fitCell:cell width:320], 44);
    XCTAssertFalse(cell.textLabel.translatesAutoresizingMaskIntoConstraints);
    cell.height = 61;
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], 61, 0.01);
    cell.height = UITableViewAutomaticDimension;
    XCTAssertLessThan([self fitCell:cell width:320], 61);
}

- (void)testGeneratedControlFamilyFitsAndReinstallsWithoutDuplicates {
    for (Class cellClass in @[[SCLabelCell class], [SCTextFieldCell class], [SCNumericTextFieldCell class], [SCSliderCell class], [SCSegmentedCell class], [SCSwitchCell class], [SCDateCell class]]) {
        SCControlCell *cell = [cellClass cellWithText:@"Title"];
        if ([cell isKindOfClass:[SCSegmentedCell class]])
            [[(SCSegmentedCell *)cell segmentedControl] insertSegmentWithTitle:@"One" atIndex:0 animated:NO];
        XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
        XCTAssertGreaterThanOrEqual([self fitCell:cell width:320], 44);
        XCTAssertFalse(cell.control.translatesAutoresizingMaskIntoConstraints);
        XCTAssertGreaterThan(cell.control.bounds.size.width, 0);
        XCTAssertGreaterThan(cell.control.bounds.size.height, 0);
        XCTAssertFalse(cell.control.hasAmbiguousLayout);
        NSUInteger count = cell.contentView.constraints.count;
        [self fitCell:cell width:760];
        [self fitCell:cell width:320];
        XCTAssertEqual(count, cell.contentView.constraints.count);
        XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetWidth(cell.contentView.bounds));
    }
}

- (void)testTextEditorGrowsWrapsAndCapsWithoutDiscardingInput {
    SCTextViewCell *cell = [SCTextViewCell cellWithText:@"Notes"];
    cell.minimumHeight = 40;
    cell.maximumHeight = 180;
    cell.textView.text = @"Short";
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    CGFloat shortHeight = [self fitCell:cell width:320];
    cell.textView.text = [@"Long wrapping note with several words. " stringByPaddingToLength:300 withString:@"More text to wrap. " startingAtIndex:0];
    CGFloat narrow = [self fitCell:cell width:320];
    CGFloat wide = [self fitCell:cell width:760];
    XCTAssertGreaterThan(narrow, shortHeight);
    XCTAssertGreaterThan(narrow, wide);
    XCTAssertLessThanOrEqual(cell.textView.bounds.size.height, 180.5);
    cell.textView.text = [@"A" stringByPaddingToLength:3000 withString:@" words " startingAtIndex:0];
    [self fitCell:cell width:320];
    XCTAssertLessThanOrEqual(cell.textView.bounds.size.height, 180.5);
    XCTAssertTrue([cell textView:cell.textView shouldChangeTextInRange:NSMakeRange(0, 0) replacementText:@"X"]);
    XCTAssertEqual(cell.textView.text.length, 3000u);
    cell.autoResize = NO;
    [self fitCell:cell width:320];
    XCTAssertEqualWithAccuracy(cell.textView.bounds.size.height, 40, 0.5);
    XCTAssertTrue(cell.textView.scrollEnabled);
    XCTAssertFalse(cell.textView.translatesAutoresizingMaskIntoConstraints);
}

- (void)testEmbeddedDatePickerParticipatesInConstraintFitting {
    SCDateCell *cell = [SCDateCell cellWithText:@"Date"];
    CGFloat collapsed = [self fitCell:cell width:320];
    [cell showEmbeddedDatePicker];
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);
    CGFloat expanded = [self fitCell:cell width:320];
    XCTAssertGreaterThan(expanded, collapsed);
    XCTAssertFalse(cell.datePicker.translatesAutoresizingMaskIntoConstraints);
    XCTAssertGreaterThanOrEqual(CGRectGetMinY(cell.datePicker.frame), CGRectGetMaxY(cell.label.frame));
    XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.datePicker.frame), CGRectGetWidth(cell.contentView.bounds));
    [cell hideEmbeddedDatePicker];
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], collapsed, 0.5);
    [cell showEmbeddedDatePicker];
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], expanded, 0.5);
}

- (void)testWindowBackedControlsHaveUnambiguousLayout {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 320, 900)];
    SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:UITableViewStyleInsetGrouped];
    window.rootViewController = controller;
    [window makeKeyAndVisible];
    SCTableViewSection *section = [SCTableViewSection section];
    for (Class cls in @[[SCTextViewCell class], [SCTextFieldCell class], [SCSliderCell class], [SCSegmentedCell class], [SCSwitchCell class]]) {
        SCControlCell *cell = [cls cellWithText:@"Title"];
        if ([cell isKindOfClass:[SCSegmentedCell class]])
            [[(SCSegmentedCell *)cell segmentedControl] insertSegmentWithTitle:@"One" atIndex:0 animated:NO];
        [section addCell:cell];
    }
    [controller.tableViewModel addSection:section];
    [controller.tableView reloadData];
    [controller.tableView layoutIfNeeded];
    for (NSUInteger i = 0; i < section.cellCount; i++) {
        SCControlCell *cell = (SCControlCell *)[section cellAtIndex:i];
        XCTAssertFalse(cell.control.hasAmbiguousLayout, @"%@ %@ %@", cell.class, [cell.control constraintsAffectingLayoutForAxis:UILayoutConstraintAxisHorizontal], [cell.control constraintsAffectingLayoutForAxis:UILayoutConstraintAxisVertical]);
    }
    window.hidden = YES;
    window.rootViewController = nil;
}

- (void)testSubtitleLabelsBadgeAndImageUseFittingLayout {
    SCTableViewCell *cell = [SCTableViewCell cellWithStyle:UITableViewCellStyleSubtitle];
    cell.textLabel.text = @"Title";
    cell.detailTextLabel.text = @"Subtitle";
    cell.textLabel.numberOfLines = cell.detailTextLabel.numberOfLines = 0;
    cell.badgeView.text = @"99";
    cell.imageView.image = [UIImage systemImageNamed:@"star.fill"];
    [self fitCell:cell width:320];
    XCTAssertGreaterThanOrEqual(CGRectGetMinY(cell.detailTextLabel.frame), CGRectGetMaxY(cell.textLabel.frame));
    XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.detailTextLabel.frame), CGRectGetMinX(cell.badgeView.frame));
    XCTAssertFalse(cell.badgeView.translatesAutoresizingMaskIntoConstraints);
    cell.textLabel.text = [@"Wrapping title " stringByPaddingToLength:180 withString:@"wrap " startingAtIndex:0];
    CGFloat narrow = [self fitCell:cell width:320];
    CGFloat wide = [self fitCell:cell width:760];
    XCTAssertGreaterThan(narrow, wide);
    cell.badgeView.hidden = YES;
    cell.imageView.hidden = YES;
    [self fitCell:cell width:320];
    XCTAssertEqualWithAccuracy(CGRectGetMinX(cell.textLabel.frame), cell.contentView.layoutMargins.left, 0.5);
    cell.badgeView.hidden = NO;
    cell.imageView.hidden = NO;
    [self fitCell:cell width:320];
    XCTAssertGreaterThan(CGRectGetMinX(cell.textLabel.frame), CGRectGetMaxX(cell.imageView.frame));
}

- (void)testSelectionLoadingIndicatorStaysCenteredUsingConstraints {
    SCSelectionCell *cell = [SCSelectionCell cellWithText:@"Loading"];
    SCLayoutPendingStore *store = [[SCLayoutPendingStore alloc] init];
    store.storeMode = SCStoreModeAsynchronous;
    cell.selectionItemsStore = store;
    (void)cell.items;
    UIActivityIndicatorView *indicator = nil;
    for (UIView *view in cell.contentView.subviews)
        if ([view isKindOfClass:[UIActivityIndicatorView class]])
            indicator = (UIActivityIndicatorView *)view;
    XCTAssertNotNil(indicator);
    XCTAssertFalse(indicator.translatesAutoresizingMaskIntoConstraints);
    for (NSNumber *width in @[@320, @760]) {
        [self fitCell:cell width:width.doubleValue];
        XCTAssertEqualWithAccuracy(indicator.center.x, CGRectGetMidX(cell.contentView.bounds), 0.5);
        XCTAssertEqualWithAccuracy(indicator.center.y, CGRectGetMidY(cell.contentView.bounds), 0.5);
    }
}

- (void)testConstrainedNibEditorRetainsItsLayoutAndScrollingPolicy {
    SCTextViewCell *cell = [SCTextViewCell cellWithText:nil];
    [self fitCell:cell width:320];
    UITextView *editor = [[UITextView alloc] init];
    editor.translatesAutoresizingMaskIntoConstraints = NO;
    editor.scrollEnabled = NO;
    [cell.contentView addSubview:editor];
    NSArray *constraints = @[[editor.leadingAnchor constraintEqualToAnchor:cell.contentView.leadingAnchor constant:14], [editor.trailingAnchor constraintEqualToAnchor:cell.contentView.trailingAnchor constant:-22], [editor.topAnchor constraintEqualToAnchor:cell.contentView.topAnchor constant:12], [editor.bottomAnchor constraintEqualToAnchor:cell.contentView.bottomAnchor constant:-18], [editor.heightAnchor constraintEqualToConstant:100]];
    [NSLayoutConstraint activateConstraints:constraints];
    cell.control = editor;
    [cell awakeFromNib];
    cell.minimumHeight = 400;
    cell.maximumHeight = 500;
    cell.autoResize = YES;
    XCTAssertFalse(editor.scrollEnabled);
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], 130, 0.5);
    XCTAssertEqualWithAccuracy(CGRectGetMinX(editor.frame), 14, 0.5);
    [self fitCell:cell width:760];
    XCTAssertEqualWithAccuracy(CGRectGetWidth(editor.bounds), CGRectGetWidth(cell.contentView.bounds) - 36, 0.5);
    for (NSLayoutConstraint *constraint in constraints)
        XCTAssertTrue(constraint.active);
}

- (void)testTextEditorBlankTrailingNewlineShrinkAndSelection {
    SCTextViewCell *cell = [SCTextViewCell cellWithText:nil];
    cell.minimumHeight = 20;
    cell.maximumHeight = 400;
    cell.textView.text = @"";
    CGFloat blank = [self fitCell:cell width:320];
    cell.textView.text = @"First line";
    CGFloat oneLine = [self fitCell:cell width:320];
    cell.textView.text = @"First line\n";
    CGFloat newline = [self fitCell:cell width:320];
    XCTAssertGreaterThan(newline, oneLine);
    cell.textView.selectedRange = NSMakeRange(3, 0);
    [self fitCell:cell width:760];
    XCTAssertTrue(NSEqualRanges(cell.textView.selectedRange, NSMakeRange(3, 0)));
    XCTAssertEqualObjects(cell.textView.text, @"First line\n");
    cell.textView.text = @"";
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], blank, 0.5);
}

- (void)testNibMultilineLabelFitsWithoutCloningOrStalePreferredWidth {
    SCCustomCell *cell = [SCCustomCell cellWithText:nil objectBindings:@{} nibName:@"STV_STVTests_AutomaticHeightCell"];
    UILabel *label = nil;
    for (UIView *view in cell.contentView.subviews)
        if ([view isKindOfClass:[UILabel class]] && [(UILabel *)view numberOfLines] == 0)
            label = (UILabel *)view;
    XCTAssertNotNil(label);
    label.text = [@"Text " stringByPaddingToLength:500 withString:@"words " startingAtIndex:0];
    CGFloat narrow = [self fitCell:cell width:320];
    CGFloat wide = [self fitCell:cell width:760];
    XCTAssertGreaterThan(narrow, wide);
    XCTAssertEqualWithAccuracy([self fitCell:cell width:320], narrow, 0.5);
    XCTAssertEqual(label.preferredMaxLayoutWidth, 0);
}

- (void)testGeneratedControlsReserveImageAndBadgeSpace {
    for (Class cls in @[[SCTextFieldCell class], [SCTextViewCell class]]) {
        SCControlCell *cell = [cls cellWithText:@"Title"];
        cell.imageView.image = [UIImage systemImageNamed:@"star.fill"];
        cell.badgeView.text = @"9";
        [self fitCell:cell width:320];
        XCTAssertFalse(cell.badgeView.translatesAutoresizingMaskIntoConstraints);
        XCTAssertFalse(cell.imageView.translatesAutoresizingMaskIntoConstraints);
        XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), CGRectGetMaxX(cell.imageView.frame));
        XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetMinX(cell.badgeView.frame));
        cell.textLabel.text = nil;
        cell.imageView.image = nil;
        cell.badgeView.text = nil;
        [self fitCell:cell width:320];
        XCTAssertEqual(CGRectGetWidth(cell.textLabel.bounds), 0);
        XCTAssertTrue(!cell.imageView.superview || CGRectGetWidth(cell.imageView.bounds) == 0);
        XCTAssertEqual(CGRectGetWidth(cell.badgeView.bounds), 0);
    }
}

- (void)testLiveDynamicTypeChangesFittingHeight {
    if (@available(iOS 17.0, *)) {
        UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 320, 900)];
        UIViewController *host = [[UIViewController alloc] init];
        window.rootViewController = host;
        [window makeKeyAndVisible];
        SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Title that grows with Dynamic Type"];
        [host.view addSubview:cell];
        cell.textLabel.numberOfLines = 0;
        CGFloat standard = [self fitCell:cell width:320];
        CGFloat pointSize = cell.textLabel.font.pointSize;
        cell.traitOverrides.preferredContentSizeCategory = UIContentSizeCategoryAccessibilityExtraExtraExtraLarge;
        [self fitCell:cell width:320];
        XCTAssertGreaterThan(cell.textLabel.font.pointSize, pointSize);
        XCTAssertGreaterThan([self fitCell:cell width:320], standard);
        window.hidden = YES;
        window.rootViewController = nil;
    }
}

- (void)testControlIndentationConfiguresConstraintsAndEditingStaysInsideContent {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:@"Name"];
    cell.controlIndentation = 180;
    cell.controlMargin = 12;
    cell.maxTextLabelWidth = 50;
    [self fitCell:cell width:320];
    XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), cell.contentView.layoutMargins.left + 180);
    XCTAssertLessThanOrEqual(CGRectGetWidth(cell.textLabel.bounds), 50);
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    [cell setEditing:YES animated:NO];
    [self fitCell:cell width:320];
    XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), 0);
    XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetWidth(cell.contentView.bounds));
}

@end
