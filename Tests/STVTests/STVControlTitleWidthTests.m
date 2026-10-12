@import XCTest;
@import STV;

@interface STVCustomizedTitleFieldCell : SCTextFieldCell
@end
@implementation STVCustomizedTitleFieldCell
- (void)performInitialization {
    [super performInitialization];
    [self.textLabel setContentCompressionResistancePriority:800 forAxis:UILayoutConstraintAxisHorizontal];
}
@end

@interface STVControlTitleWidthTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@end

@implementation STVControlTitleWidthTests

- (void)setUp {
    [super setUp];
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 760, 600)];
    self.window.rootViewController = [[UIViewController alloc] init];
    [self.window makeKeyAndVisible];
}

- (void)tearDown {
    self.window.hidden = YES;
    self.window.rootViewController = nil;
    self.window = nil;
    [super tearDown];
}

- (void)layoutCell:(SCControlCell *)cell width:(CGFloat)width {
    if (!cell.superview) {
        [self.window.rootViewController.view addSubview:cell];
    }

    cell.bounds = CGRectMake(0, 0, width, 100);
    [cell setNeedsUpdateConstraints];
    [cell updateConstraintsIfNeeded];
    [cell setNeedsLayout];
    [cell layoutIfNeeded];
}

- (void)testPointCapAllowsTitleBeyondFormerPercentageLimit {
    for (Class cellClass in @[[SCLabelCell class], [SCTextFieldCell class]]) {
        SCControlCell *cell = [cellClass cellWithText:@"Songs from Playlist"];
        cell.textLabel.font = [UIFont systemFontOfSize:17];
        cell.maxTextLabelWidth = 1000;
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        [self layoutCell:cell width:320];
        CGFloat naturalWidth = cell.textLabel.intrinsicContentSize.width;
        CGFloat availableWidth = cell.contentView.layoutMarginsGuide.layoutFrame.size.width;
        XCTAssertGreaterThan(naturalWidth, availableWidth * 0.45);
        XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, naturalWidth, 0.5, @"%@ should show its entire title", cellClass);
        XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), CGRectGetMaxX(cell.textLabel.frame) + cell.controlMargin - 0.5);
        XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetMaxX(cell.contentView.layoutMarginsGuide.layoutFrame) + 0.5);
        XCTAssertFalse(cell.control.hasAmbiguousLayout);
    }
}

- (void)testChangingPointCapSchedulesLayoutAfterInitialLayout {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:@"Songs from Playlist"];
    cell.textLabel.font = [UIFont systemFontOfSize:17];
    cell.maxTextLabelWidth = 80;
    [self layoutCell:cell width:320];
    XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, 80, 0.5);
    XCTAssertFalse(cell.needsUpdateConstraints);
    NSUInteger constraintCount = cell.contentView.constraints.count;

    cell.maxTextLabelWidth = 180;
    XCTAssertTrue(cell.needsUpdateConstraints);
    [cell layoutIfNeeded];
    XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, cell.textLabel.intrinsicContentSize.width, 0.5);
    XCTAssertEqual(cell.contentView.constraints.count, constraintCount);

    cell.maxTextLabelWidth = 60;
    XCTAssertTrue(cell.needsUpdateConstraints);
    [cell layoutIfNeeded];
    XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, 60, 0.5);
    XCTAssertEqual(cell.contentView.constraints.count, constraintCount);
    XCTAssertFalse(cell.needsUpdateConstraints);

    cell.maxTextLabelWidth = 60;
    XCTAssertFalse(cell.needsUpdateConstraints);
}

- (void)testPointCapIsAMaximumRatherThanAFixedWidth {
    for (Class cellClass in @[[SCLabelCell class], [SCTextFieldCell class]]) {
        SCControlCell *cell = [cellClass cellWithText:@"Songs from Playlist"];
        cell.textLabel.font = [UIFont systemFontOfSize:17];
        XCTAssertEqual(cell.maxTextLabelWidth, 200);
        cell.maxTextLabelWidth = 80;
        for (NSNumber *width in @[@320, @760, @320]) {
            [self layoutCell:cell width:width.doubleValue];
            XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, 80, 0.5);
        }

        cell.maxTextLabelWidth = 1000;
        cell.controlIndentation = 0;
        cell.textLabel.text = @"Song";
        [self layoutCell:cell width:320];
        XCTAssertEqualWithAccuracy(cell.textLabel.bounds.size.width, cell.textLabel.intrinsicContentSize.width, 0.5);
    }
}

- (void)testPopulatedEditorResizesWithinDecoratedContent {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:@"Songs from Playlist"];
    cell.maxTextLabelWidth = 1000;
    cell.textField.text = @"A";
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.imageView.image = [UIImage systemImageNamed:@"star.fill"];
    cell.badgeView.text = @"9";
    NSUInteger constraintCount = 0;
    for (NSNumber *width in @[@390, @760, @240, @390]) {
        [self layoutCell:cell width:width.doubleValue];
        if (constraintCount == 0) {
            constraintCount = cell.contentView.constraints.count;
        }

        XCTAssertEqual(cell.contentView.constraints.count, constraintCount);
        XCTAssertEqualObjects(cell.textField.text, @"A");
        XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.textLabel.frame), CGRectGetMaxX(cell.imageView.frame));
        XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), CGRectGetMaxX(cell.textLabel.frame) + cell.controlMargin - 0.5);
        XCTAssertGreaterThanOrEqual(cell.control.bounds.size.width, 0);
        XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetMinX(cell.badgeView.frame) + 0.5);
        XCTAssertFalse(cell.control.hasAmbiguousLayout, @"width %@ title %@ editor %@ priorities %g/%g constraints %@", width, NSStringFromCGRect(cell.textLabel.frame), NSStringFromCGRect(cell.control.frame), [cell.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], [cell.control contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], [cell.control constraintsAffectingLayoutForAxis:UILayoutConstraintAxisHorizontal]);
    }
}

- (void)testLongSwitchTitlePreservesNativeControlWidth {
    SCSwitchCell *cell = [SCSwitchCell cellWithText:@"Songs from Playlist and more songs"];
    cell.maxTextLabelWidth = 1000;
    CGFloat nativeWidth = cell.switchControl.intrinsicContentSize.width;
    for (NSNumber *width in @[@240, @320, @760, @240]) {
        [self layoutCell:cell width:width.doubleValue];
        CGRect controlRect = [cell.switchControl alignmentRectForFrame:cell.switchControl.frame];
        XCTAssertEqualWithAccuracy(controlRect.size.width, nativeWidth, 0.5);
        XCTAssertGreaterThanOrEqual(CGRectGetMinX(controlRect), CGRectGetMaxX(cell.textLabel.frame) + cell.controlMargin - 0.5);
        XCTAssertEqualWithAccuracy(CGRectGetMaxX(controlRect), CGRectGetMaxX(cell.contentView.layoutMarginsGuide.layoutFrame), 0.5);
        XCTAssertFalse(cell.control.hasAmbiguousLayout);
    }
}

- (void)testCallerCompressionResistanceSurvivesWidthChanges {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:@"Songs from Playlist"];
    [cell.textLabel setContentCompressionResistancePriority:800 forAxis:UILayoutConstraintAxisHorizontal];
    for (NSNumber *cap in @[@80, @1000, @60]) {
        cell.maxTextLabelWidth = cap.doubleValue;
        [self layoutCell:cell width:320];
        XCTAssertEqual([cell.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], 800);
    }
}

- (void)testLongPopulatedEditorKeepsItsIntrinsicWidthWhenTitleMustCompress {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:@"Songs from Playlist"];
    cell.maxTextLabelWidth = 1000;
    cell.controlIndentation = 0;
    cell.textField.text = @"Playlist name";
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.imageView.image = [UIImage systemImageNamed:@"star.fill"];
    cell.badgeView.text = @"9";
    CGFloat naturalEditorWidth = cell.textField.intrinsicContentSize.width;
    [self layoutCell:cell width:240];
    XCTAssertEqualObjects(cell.textField.text, @"Playlist name");
    XCTAssertGreaterThanOrEqual(cell.textField.bounds.size.width + 0.5, naturalEditorWidth);
    XCTAssertLessThan(cell.textLabel.bounds.size.width, cell.textLabel.intrinsicContentSize.width);
    XCTAssertGreaterThanOrEqual(cell.textLabel.bounds.size.width, 0);
    XCTAssertFalse(cell.textLabel.hasAmbiguousLayout);
    XCTAssertFalse(cell.textField.hasAmbiguousLayout);
    XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.textField.frame), CGRectGetMaxX(cell.textLabel.frame) + cell.controlMargin - 0.5);
    XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.textField.frame), CGRectGetMinX(cell.badgeView.frame) + 0.5);
}

- (void)testSubclassInitializationPriorityIsPreserved {
    STVCustomizedTitleFieldCell *cell = [STVCustomizedTitleFieldCell cellWithText:@"Songs from Playlist"];
    XCTAssertEqual([cell.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], 800);
    cell.maxTextLabelWidth = 1000;
    [self layoutCell:cell width:320];
    XCTAssertEqual([cell.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], 800);
}

- (void)testDecodedTitleRetainsAuthoredCompressionResistance {
    UITableViewCell *source = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:nil];
    source.textLabel.text = @"Authored title";
    source.textLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [source.textLabel setContentCompressionResistancePriority:812 forAxis:UILayoutConstraintAxisHorizontal];
    [NSLayoutConstraint activateConstraints:@[[source.textLabel.leadingAnchor constraintEqualToAnchor:source.contentView.leadingAnchor constant:14], [source.textLabel.widthAnchor constraintEqualToConstant:200], [source.textLabel.centerYAnchor constraintEqualToAnchor:source.contentView.centerYAnchor]]];
    SCControlCell *decoded = [SCControlCell cellWithCell:source];
    XCTAssertNotNil(decoded);
    XCTAssertTrue(decoded.cellCreatedInIB);
    XCTAssertEqual([decoded.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], 812);
    [self layoutCell:decoded width:320];
    CGRect authoredFrame = decoded.textLabel.frame;
    NSArray<NSLayoutConstraint *> *authoredConstraints = [decoded.contentView.constraints copy];
    decoded.maxTextLabelWidth = 60;
    [decoded layoutIfNeeded];
    XCTAssertEqual([decoded.textLabel contentCompressionResistancePriorityForAxis:UILayoutConstraintAxisHorizontal], 812);
    XCTAssertTrue(CGRectEqualToRect(decoded.textLabel.frame, authoredFrame));
    for (NSLayoutConstraint *constraint in authoredConstraints) {
        XCTAssertTrue(constraint.active);
    }
}

- (void)testWidthChangesPreserveAuthoredControlConstraints {
    SCTextFieldCell *cell = [SCTextFieldCell cellWithText:nil];
    UITextField *editor = [[UITextField alloc] init];
    editor.translatesAutoresizingMaskIntoConstraints = NO;
    cell.control = editor;
    NSArray<NSLayoutConstraint *> *authored = @[[editor.leadingAnchor constraintEqualToAnchor:cell.contentView.leadingAnchor constant:14], [editor.trailingAnchor constraintEqualToAnchor:cell.contentView.trailingAnchor constant:-22], [editor.topAnchor constraintEqualToAnchor:cell.contentView.topAnchor constant:12], [editor.bottomAnchor constraintEqualToAnchor:cell.contentView.bottomAnchor constant:-18]];
    [NSLayoutConstraint activateConstraints:authored];
    [cell awakeFromNib];
    for (NSNumber *cap in @[@80, @1000, @60]) {
        cell.maxTextLabelWidth = cap.doubleValue;
        [self layoutCell:cell width:320];
        XCTAssertEqualWithAccuracy(CGRectGetMinX(editor.frame), 14, 0.5);
        XCTAssertEqualWithAccuracy(editor.bounds.size.width, cell.contentView.bounds.size.width - 36, 0.5);
        for (NSLayoutConstraint *constraint in authored) {
            XCTAssertTrue(constraint.active);
        }
    }
}

@end
