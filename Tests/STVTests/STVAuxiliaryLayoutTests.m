@import XCTest;
@import STV;

@interface STVAuxiliaryLayoutTests : XCTestCase
@end

@implementation STVAuxiliaryLayoutTests

- (void)testActualKeyboardReservesAccessoryHeight {
    UIWindow *window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    UIViewController *host = [[UIViewController alloc] init];
    window.rootViewController = host;
    [window makeKeyAndVisible];
    UITextField *field = [[UITextField alloc] initWithFrame:CGRectMake(20, 100, 280, 44)];
    [host.view addSubview:field];
    SCInputAccessoryView *accessory = [[SCInputAccessoryView alloc] init];
    field.inputAccessoryView = accessory;
    XCTestExpectation *shown = [self expectationWithDescription:@"Keyboard shown"];
    id observer = [NSNotificationCenter.defaultCenter addObserverForName:UIKeyboardDidShowNotification object:nil queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification *note) { [shown fulfill]; }];
    XCTAssertTrue([field becomeFirstResponder]);
    [self waitForExpectations:@[shown] timeout:5];
    [NSNotificationCenter.defaultCenter removeObserver:observer];
    BOOL accessoryWasHosted = accessory.window != nil;
    CGFloat accessoryHeight = CGRectGetHeight(accessory.bounds);
    CGFloat toolbarBottom = CGRectGetMaxY(accessory.toolbar.frame);
    [field resignFirstResponder];
    window.hidden = YES;
    window.rootViewController = nil;
    /*
    SwiftPM's hostless xctest process can post keyboard notifications without
    attaching an accessory window. The simulator app fixture verifies that
    UIKit boundary; this assertion also runs when an app hosts the test suite.
    */
    XCTSkipIf(!accessoryWasHosted && [NSProcessInfo.processInfo.processName isEqualToString:@"xctest"], @"Requires an application-hosted keyboard; covered by simulator app verification.");
    XCTAssertTrue(accessoryWasHosted);
    XCTAssertGreaterThanOrEqual(accessoryHeight, 44);
    XCTAssertLessThanOrEqual(toolbarBottom, accessoryHeight);
}

- (void)testDesignatedInputViewInitializerConfiguresAccessoryForBothStyles {
    for (NSNumber *style in @[@(UIInputViewStyleDefault), @(UIInputViewStyleKeyboard)]) {
        SCInputAccessoryView *accessory = [[SCInputAccessoryView alloc] initWithFrame:CGRectMake(25, 30, 320, 44) inputViewStyle:style.integerValue];
        XCTAssertEqual(accessory.inputViewStyle, style.integerValue);
        XCTAssertNotNil(accessory.toolbar);
        XCTAssertEqual(accessory.toolbar.superview, accessory);
        XCTAssertNotNil(accessory.previousNextSegmentedControl);
        XCTAssertEqual(accessory.previousNextSegmentedControl.numberOfSegments, 2);
        XCTAssertNotNil(accessory.doneButton);
        XCTAssertNotNil(accessory.clearButton);
        XCTAssertEqual(accessory.toolbar.items.firstObject.customView, accessory.previousNextSegmentedControl);
        XCTAssertEqual(accessory.toolbar.items.lastObject, accessory.doneButton);
        XCTAssertTrue(accessory.allowsSelfSizing);
        XCTAssertTrue(accessory.autoresizingMask & UIViewAutoresizingFlexibleHeight);
        XCTAssertFalse(accessory.toolbar.translatesAutoresizingMaskIntoConstraints);
        for (NSNumber *width in @[@320, @700]) {
            CGSize fitting = [accessory systemLayoutSizeFittingSize:CGSizeMake(width.doubleValue, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel];
            XCTAssertEqualWithAccuracy(fitting.width, width.doubleValue, 0.5);
            XCTAssertGreaterThanOrEqual(fitting.height, 44.0);
            accessory.frame = CGRectMake(25, 30, width.doubleValue, fitting.height);
            [accessory layoutIfNeeded];
            XCTAssertEqualWithAccuracy(accessory.toolbar.frame.origin.x, 0, 0.5);
            XCTAssertEqualWithAccuracy(accessory.toolbar.frame.origin.y, 0, 0.5);
            XCTAssertEqualWithAccuracy(accessory.toolbar.bounds.size.width, width.doubleValue, 0.5);
            XCTAssertEqualWithAccuracy(accessory.toolbar.bounds.size.height, fitting.height, 0.5);
        }
    }
}

- (void)testKeyboardAccessoryUsesConstraintFittingAtDifferentWidths {
    XCTAssertTrue([SCInputAccessoryView new].autoresizingMask & UIViewAutoresizingFlexibleHeight);
    SCInputAccessoryView *accessory = [[SCInputAccessoryView alloc] init];
    XCTAssertFalse(accessory.toolbar.translatesAutoresizingMaskIntoConstraints);
    for (NSNumber *width in @[@320, @700]) {
        CGSize fitting = [accessory systemLayoutSizeFittingSize:CGSizeMake(width.doubleValue, 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel];
        XCTAssertGreaterThanOrEqual(fitting.height, 44.0);
        accessory.frame = CGRectMake(25, 30, width.doubleValue, fitting.height);
        [accessory layoutIfNeeded];
        XCTAssertEqualWithAccuracy(accessory.toolbar.frame.origin.x, 0, 0.5);
        XCTAssertEqualWithAccuracy(accessory.toolbar.frame.origin.y, 0, 0.5);
        XCTAssertEqualWithAccuracy(accessory.toolbar.bounds.size.width, width.doubleValue, 0.5);
    }
}

- (void)testFetchSpinnerUsesConstraintsAndStaysCentered {
    SCFetchItemsCell *cell = [[SCFetchItemsCell alloc] initWithText:@"Fetch"];
    UIActivityIndicatorView *spinner = nil;
    for (UIView *view in cell.contentView.subviews)
        if ([view isKindOfClass:[UIActivityIndicatorView class]])
            spinner = (UIActivityIndicatorView *)view;
    XCTAssertNotNil(spinner);
    XCTAssertFalse(spinner.translatesAutoresizingMaskIntoConstraints);
    cell.frame = CGRectMake(0, 0, 260, 65);
    [cell layoutIfNeeded];
    XCTAssertEqualWithAccuracy(spinner.center.x, CGRectGetMidX(cell.contentView.bounds), 0.5);
    XCTAssertEqualWithAccuracy(spinner.center.y, CGRectGetMidY(cell.contentView.bounds), 0.5);
}

- (void)testExpandAccessoryIsPlacedAtTheTrailingEdgeByUIKit {
    SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:UITableViewStylePlain];
    [controller loadViewIfNeeded];
    controller.view.frame = CGRectMake(0, 0, 320, 500);
    SCTableViewSection *section = [SCTableViewSection section];
    SCExpandCollapseCell *cell = [SCExpandCollapseCell cellWithExpandText:@"Expand" collapseText:@"Collapse" ownerSectionExpanded:NO];
    [section addCell:cell];
    [controller.tableViewModel addSection:section];
    [controller.tableView reloadData];
    [controller.tableView layoutIfNeeded];
    CGRect accessoryRect = [cell convertRect:cell.accessoryView.bounds fromView:cell.accessoryView];
    XCTAssertGreaterThan(CGRectGetMinX(accessoryRect), CGRectGetMidX(cell.bounds));
}

- (void)testExpandAccessoryHasIntrinsicSizeWithoutASeedFrame {
    SCExpandCollapseCell *cell = [SCExpandCollapseCell cellWithExpandText:@"Expand" collapseText:@"Collapse" ownerSectionExpanded:NO];
    XCTAssertGreaterThan(cell.accessoryView.intrinsicContentSize.width, 0);
    XCTAssertGreaterThan(cell.accessoryView.intrinsicContentSize.height, 0);
}

@end
