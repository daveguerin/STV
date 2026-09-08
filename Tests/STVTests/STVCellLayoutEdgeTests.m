@import XCTest;
@import STV;

@interface SCDateCell (LayoutEdgeTests)
- (void)showEmbeddedDatePicker;
- (void)hideEmbeddedDatePicker;
@end

@interface STVCellLayoutEdgeTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@end

@implementation STVCellLayoutEdgeTests

- (void)setUp {
    [super setUp];
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 800, 1000)];
    self.window.rootViewController = [[UIViewController alloc] init];
    [self.window makeKeyAndVisible];
}

- (void)tearDown {
    self.window.hidden = YES;
    self.window.rootViewController = nil;
    self.window = nil;
    [super tearDown];
}

- (CGFloat)fitCell:(SCTableViewCell *)cell width:(CGFloat)width {
    if (!cell.superview)
        [self.window.rootViewController.view addSubview:cell];
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

- (void)testUntitledSwitchRetainsNativeWidthThroughTitleVisibilityChanges {
    SCSwitchCell *cell = [SCSwitchCell cellWithText:nil];
    CGFloat nativeWidth = cell.switchControl.intrinsicContentSize.width;
    for (NSNumber *width in @[@320, @760]) {
        for (NSNumber *state in @[@0, @1, @2, @3, @4]) {
            cell.textLabel.text = state.integerValue == 0 || state.integerValue == 2 ? nil : @"Title";
            cell.textLabel.hidden = state.integerValue == 3;
            [self fitCell:cell width:width.doubleValue];
            CGRect switchRect = [cell.switchControl alignmentRectForFrame:cell.switchControl.frame];
            XCTAssertEqualWithAccuracy(CGRectGetWidth(switchRect), nativeWidth, 0.5);
            XCTAssertEqualWithAccuracy(CGRectGetMaxX(switchRect), CGRectGetMaxX(cell.contentView.layoutMarginsGuide.layoutFrame), 0.5);
            XCTAssertGreaterThan(CGRectGetMinX(cell.switchControl.frame), CGRectGetMinX(cell.contentView.layoutMarginsGuide.layoutFrame));
            XCTAssertFalse(cell.switchControl.hasAmbiguousLayout);
        }
    }
}

- (void)testExpandedDatePickerTracksBaseLayoutChanges {
    SCDateCell *cell = [SCDateCell cellWithText:@"Date"];
    cell.label.text = @"Today";
    [cell showEmbeddedDatePicker];
    for (NSNumber *width in @[@320, @760]) {
        CGFloat expanded = [self fitCell:cell width:width.doubleValue];
        for (NSNumber *state in @[@0, @1, @2, @3]) {
            cell.badgeView.text = state.integerValue == 0 ? @"9" : nil;
            cell.textLabel.hidden = state.integerValue == 1;
            cell.textLabel.text = state.integerValue == 2 ? nil : @"Date";
            cell.controlMargin = 8 + state.integerValue;
            CGFloat height = [self fitCell:cell width:width.doubleValue];
            XCTAssertEqualWithAccuracy(height, expanded, 0.5);
            XCTAssertEqualWithAccuracy(CGRectGetMinY(cell.control.frame), 8, 0.5);
            XCTAssertEqualWithAccuracy(CGRectGetMinY(cell.datePicker.frame), CGRectGetMaxY(cell.control.frame) + 8, 0.5);
            XCTAssertLessThanOrEqual(CGRectGetMaxY(cell.datePicker.frame), height - 7.5);
        }
    }
}

- (void)testNibBadgeGetsFrameworkLayoutWithoutChangingAuthoredConstraints {
    SCCustomCell *cell = [SCCustomCell cellWithText:nil objectBindings:@{} nibName:@"STV_STVTests_AutomaticHeightCell"];
    XCTAssertNotNil(cell);
    XCTAssertTrue(cell.cellCreatedInIB);
    UILabel *authoredLabel = nil;
    for (UIView *view in cell.contentView.subviews)
        if ([view isKindOfClass:[UILabel class]] && view != cell.badgeView)
            authoredLabel = (UILabel *)view;
    XCTAssertNotNil(authoredLabel);
    NSArray<NSLayoutConstraint *> *authoredConstraints = [cell.contentView.constraints copy];
    NSArray *originalDescriptions = [authoredConstraints valueForKey:@"description"];
    for (NSNumber *width in @[@320, @760]) {
        cell.badgeView.text = nil;
        CGFloat baseline = [self fitCell:cell width:width.doubleValue];
        CGRect authoredFrame = authoredLabel.frame;
        cell.badgeView.text = @"9";
        XCTAssertEqualWithAccuracy([self fitCell:cell width:width.doubleValue], baseline, 0.5);
        XCTAssertGreaterThan(CGRectGetWidth(cell.badgeView.bounds), 0);
        XCTAssertGreaterThan(CGRectGetHeight(cell.badgeView.bounds), 0);
        XCTAssertEqualWithAccuracy(CGRectGetMaxX(cell.badgeView.frame), CGRectGetMaxX(cell.contentView.layoutMarginsGuide.layoutFrame), 0.5);
        XCTAssertEqualWithAccuracy(cell.badgeView.center.y, CGRectGetMidY(cell.contentView.bounds), 0.5);
        XCTAssertFalse(cell.badgeView.translatesAutoresizingMaskIntoConstraints);
        XCTAssertFalse(cell.badgeView.hasAmbiguousLayout);
        XCTAssertTrue(CGRectEqualToRect(authoredLabel.frame, authoredFrame));
        XCTAssertEqualObjects([authoredConstraints valueForKey:@"description"], originalDescriptions);
        for (NSLayoutConstraint *constraint in authoredConstraints)
            XCTAssertTrue(constraint.active);
        cell.badgeView.hidden = YES;
        [self fitCell:cell width:width.doubleValue];
        XCTAssertEqual(CGRectGetWidth(cell.badgeView.bounds), 0);
        cell.badgeView.hidden = NO;
        [self fitCell:cell width:width.doubleValue];
        XCTAssertGreaterThan(CGRectGetWidth(cell.badgeView.bounds), 0);
    }
}

- (void)testBaseBadgeIntrinsicHeightDrivesFittingRow {
    for (NSNumber *width in @[@320, @760]) {
        for (NSString *text in @[@"9", @"9\n8\n7"]) {
            SCTableViewCell *cell = [SCTableViewCell cellWithText:nil];
            cell.badgeView.font = [UIFont systemFontOfSize:64];
            cell.badgeView.numberOfLines = 0;
            cell.badgeView.text = text;
            CGFloat height = [self fitCell:cell width:width.doubleValue];
            XCTAssertGreaterThanOrEqual(height + 0.5, cell.badgeView.intrinsicContentSize.height + 16);
            XCTAssertGreaterThanOrEqual(CGRectGetMinY(cell.badgeView.frame), 7.5);
            XCTAssertLessThanOrEqual(CGRectGetMaxY(cell.badgeView.frame), height - 7.5);
            XCTAssertGreaterThanOrEqual(CGRectGetHeight(cell.badgeView.bounds) + 0.5, cell.badgeView.intrinsicContentSize.height);
            cell.badgeView.hidden = YES;
            XCTAssertLessThan([self fitCell:cell width:width.doubleValue], height);
            cell.badgeView.hidden = NO;
            XCTAssertEqualWithAccuracy([self fitCell:cell width:width.doubleValue], height, 0.5);
        }
    }
}

- (void)testExpandedDatePickerReanchorsAfterControlReplacement {
    SCDateCell *cell = [SCDateCell cellWithText:@"Date"];
    cell.label.text = @"Today";
    [cell showEmbeddedDatePicker];
    [self fitCell:cell width:320];
    UIView *original = cell.control;
    UILabel *replacement = [[UILabel alloc] init];
    cell.control = replacement;
    replacement.text = @"Replacement";
    for (NSNumber *width in @[@320, @760, @320]) {
        [self fitCell:cell width:width.doubleValue];
        XCTAssertEqualWithAccuracy(CGRectGetMinY(replacement.frame), 8, 0.5);
        XCTAssertEqualWithAccuracy(CGRectGetMinY(cell.datePicker.frame), CGRectGetMaxY(replacement.frame) + 8, 0.5);
        XCTAssertFalse(replacement.hasAmbiguousLayout);
        for (NSLayoutConstraint *constraint in cell.contentView.constraints)
            XCTAssertFalse(constraint.firstItem == original || constraint.secondItem == original);
    }
    NSUInteger count = cell.contentView.constraints.count;
    [self fitCell:cell width:320];
    XCTAssertEqual(count, cell.contentView.constraints.count);
    [cell hideEmbeddedDatePicker];
    CGFloat collapsed = [self fitCell:cell width:320];
    [cell showEmbeddedDatePicker];
    XCTAssertGreaterThan([self fitCell:cell width:320], collapsed);
}

@end
