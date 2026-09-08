@import XCTest;
@import STV;

@interface SCTableViewCell (STVPopoverTesting)
- (void)presentDetailViewController:(UIViewController *)controller forCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath withPresentationMode:(SCPresentationMode)mode;
@end

@interface SCArrayOfItemsSection (STVPopoverTesting)
- (void)presentDetailViewController:(UIViewController *)controller forCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath withPresentationMode:(SCPresentationMode)mode;
@end

@interface SCArrayOfItemsModel (STVSearchPresentationTesting)
- (void)attachSearchController;
@end

@interface STVPopoverViewDelegate : NSObject <SCViewControllerDelegate>
@property (nonatomic) BOOL allowDismissal;
@property (nonatomic, strong) NSMutableArray *events;
@end
@implementation STVPopoverViewDelegate
- (instancetype)init {
    if ((self = [super init])) {
        _allowDismissal = YES;
        _events = [NSMutableArray array];
    }
    return self;
}
- (BOOL)viewControllerShouldDismiss:(SCViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"should", @(cancel), @(done)]];
    return self.allowDismissal;
}
- (void)viewControllerWillDismiss:(SCViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"will", @(cancel), @(done)]];
}
- (void)viewControllerDidDismiss:(SCViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"did", @(cancel), @(done)]];
}
@end

@interface STVPopoverTableDelegate : NSObject <SCTableViewControllerDelegate>
@property (nonatomic) BOOL allowDismissal;
@property (nonatomic, strong) NSMutableArray *events;
@end
@implementation STVPopoverTableDelegate
- (instancetype)init {
    if ((self = [super init])) {
        _allowDismissal = YES;
        _events = [NSMutableArray array];
    }
    return self;
}
- (BOOL)tableViewControllerShouldDismiss:(SCTableViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"should", @(cancel), @(done)]];
    return self.allowDismissal;
}
- (void)tableViewControllerWillDismiss:(SCTableViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"will", @(cancel), @(done)]];
}
- (void)tableViewControllerDidDismiss:(SCTableViewController *)controller cancelButtonTapped:(BOOL)cancel doneButtonTapped:(BOOL)done {
    [self.events addObject:@[@"did", @(cancel), @(done)]];
}
@end

@interface STVNotUIPopoverView : UIView
@end
@implementation STVNotUIPopoverView
@end

@interface STVPopoverTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@property (nonatomic, strong) SCTableViewController *owner;
@end

@implementation STVPopoverTests
- (void)settle {
    XCTestExpectation *settled = [self expectationWithDescription:@"UIKit transition completes"];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [settled fulfill]; });
    [self waitForExpectations:@[settled] timeout:4];
}
- (void)setUp {
    [super setUp];
    self.owner = [[SCTableViewController alloc] init];
    UIWindowScene *scene = (id)UIApplication.sharedApplication.connectedScenes.anyObject;
    self.window = scene ? [[UIWindow alloc] initWithWindowScene:scene] : [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 1024, 768)];
    self.window.frame = CGRectMake(0, 0, 1024, 768);
    self.window.rootViewController = [[UINavigationController alloc] initWithRootViewController:self.owner];
    [self.window makeKeyAndVisible];
    [self settle];
}
- (void)tearDown {
    [self.window.rootViewController dismissViewControllerAnimated:NO completion:nil];
    [self settle];
    self.window.hidden = YES;
    self.window.rootViewController = nil;
    self.window = nil;
    self.owner = nil;
    [super tearDown];
}
- (SCTableViewCell *)anchorCell {
    SCTableViewSection *section = [SCTableViewSection section];
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Detail"];
    [section addCell:cell];
    [self.owner.tableViewModel addSection:section];
    [self.owner.tableView reloadData];
    [self.owner.tableView layoutIfNeeded];
    return cell;
}
- (void)testHeaderSearchRemainsVisibleWhileEditingAfterNavigationReattachment {
    SCArrayOfStringsModel *model = [SCArrayOfStringsModel modelWithTableView:self.owner.tableView items:[NSMutableArray arrayWithObjects:@"Alpha", @"Beta", nil]];
    self.owner.tableViewModel = model;
    model.enableSearchController = YES;
    for (NSNumber *hidden in @[@YES, @NO, @YES, @NO, @YES]) {
        [self.owner.navigationController setNavigationBarHidden:hidden.boolValue animated:NO];
        [model attachSearchController];
        [self.window layoutIfNeeded];
    }
    [self settle];
    for (NSUInteger cycle = 0; cycle < 2; cycle++) {
        [model.searchBar.searchTextField becomeFirstResponder];
        [self settle];
        UISearchTextField *field = model.searchBar.searchTextField;
        XCTAssertTrue(field.isFirstResponder);
        XCTAssertEqual(field.window, self.window);
        XCTAssertGreaterThan(field.bounds.size.width, 100);
        XCTAssertGreaterThan(field.bounds.size.height, 0);
        CGRect visibleField = [field convertRect:field.bounds toView:self.window];
        XCTAssertTrue(CGRectContainsRect(self.window.bounds, visibleField));
        field.text = @"";
        [field insertText:@"Alpha"];
        [self settle];
        XCTAssertEqual([model sectionAtIndex:0].cellCount, 1u);
        UITableView *results = [(UITableViewController *)model.searchController.searchResultsController tableView];
        UITableViewCell *result = [results cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
        XCTAssertEqual(result.window, self.window);
        CGRect resultRect = [result convertRect:result.bounds toView:self.window];
        XCTAssertFalse(CGRectIntersectsRect(resultRect, visibleField));
        model.searchController.active = NO;
        [self settle];
        XCTAssertEqual(self.owner.tableView.tableHeaderView, model.searchBar);
        XCTAssertGreaterThan(model.searchBar.searchTextField.bounds.size.width, 100);
        XCTAssertTrue(self.owner.navigationController.navigationBarHidden);
    }
    model.enableSearchController = NO;
}
- (void)testViewControllerDelegateVetoesExplicitDismissal {
    SCTableViewCell *cell = [self anchorCell];
    SCViewController *detail = [[SCViewController alloc] init];
    STVPopoverViewDelegate *delegate = [[STVPopoverViewDelegate alloc] init];
    detail.delegate = delegate;
    delegate.allowDismissal = NO;
    [cell presentDetailViewController:detail forCell:cell forRowAtIndexPath:nil withPresentationMode:SCPresentationModePopover];
    [self settle];
    [detail dismissWithCancelValue:NO doneValue:YES];
    [self settle];
    XCTAssertNotNil(detail.view.window);
    XCTAssertEqualObjects(delegate.events, (@[@[@"should", @NO, @YES]]));
    XCTAssertFalse(detail.doneButtonTapped);
    delegate.allowDismissal = YES;
    [detail dismissWithCancelValue:YES doneValue:NO];
    [self settle];
    XCTAssertNil(self.owner.presentedViewController);
    XCTAssertEqualObjects(delegate.events, (@[@[@"should", @NO, @YES], @[@"should", @YES, @NO], @[@"will", @YES, @NO], @[@"did", @YES, @NO]]));
}
- (void)testAdaptiveDismissalDoesNotCommitUntilCompleted {
    if (@available(iOS 17.0, *)) {
        self.window.traitOverrides.horizontalSizeClass = UIUserInterfaceSizeClassCompact;
    }
    SCTableViewCell *cell = [self anchorCell];
    for (BOOL table = NO;; table = YES) {
        id detail = table ? (id)[[SCTableViewController alloc] init] : [[SCViewController alloc] init];
        id delegate = table ? (id)[[STVPopoverTableDelegate alloc] init] : [[STVPopoverViewDelegate alloc] init];
        [detail setDelegate:delegate];
        [cell presentDetailViewController:detail forCell:cell forRowAtIndexPath:nil withPresentationMode:SCPresentationModePopover];
        [self settle];
        UIPopoverPresentationController *popover = table ? [(SCTableViewController *)detail popoverController] : [(SCViewController *)detail popoverController];
        id<UIPopoverPresentationControllerDelegate> handler = popover.delegate;
        XCTAssertTrue([handler respondsToSelector:@selector(presentationControllerShouldDismiss:)]);
        XCTAssertTrue([handler respondsToSelector:@selector(presentationControllerDidDismiss:)]);
        if ([handler respondsToSelector:@selector(presentationControllerShouldDismiss:)] && [handler respondsToSelector:@selector(presentationControllerDidDismiss:)]) {
            [delegate setAllowDismissal:NO];
            XCTAssertFalse([handler presentationControllerShouldDismiss:popover]);
            [delegate setAllowDismissal:YES];
            XCTAssertTrue([handler presentationControllerShouldDismiss:popover]);
            [[delegate events] removeAllObjects];
            // UIKit can announce an interactive dismissal and then cancel it.
            if ([handler respondsToSelector:@selector(presentationControllerWillDismiss:)]) {
                [handler presentationControllerWillDismiss:popover];
            }
            XCTAssertEqual([[delegate events] count], 0u);
            XCTAssertFalse([(SCViewController *)detail doneButtonTapped]);
            XCTAssertNotEqual([detail state], SCViewControllerStateDismissed);
            // Real disappearance, then UIKit's completion callback (not a swipe simulation).
            [self.owner dismissViewControllerAnimated:NO completion:nil];
            [self settle];
            [handler presentationControllerDidDismiss:popover.adaptiveSheetPresentationController];
            XCTAssertEqual([[delegate events] count], 2u);
            [handler presentationControllerDidDismiss:popover];
            XCTAssertEqualObjects([delegate events], (@[@[@"will", @NO, @YES], @[@"did", @NO, @YES]]));
            XCTAssertNil([detail popoverController]);
        }
        [self.owner dismissViewControllerAnimated:NO completion:nil];
        [self settle];
        if (table)
            break;
    }
}
- (void)testPopoverDetectionDoesNotGuessFromClassNames {
    UIView *decoy = [STVNotUIPopoverView new];
    UIView *child = [UIView new];
    [decoy addSubview:child];
    XCTAssertFalse([SCUtilities isViewInsidePopover:child]);
}
- (void)testRegularAndCompactPresentationUseUIKitAdaptation {
    SCTableViewCell *cell = [self anchorCell];
    for (NSNumber *width in @[@(UIUserInterfaceSizeClassRegular), @(UIUserInterfaceSizeClassCompact)]) {
        if (@available(iOS 17.0, *)) {
            self.window.traitOverrides.horizontalSizeClass = width.integerValue;
            self.window.rootViewController.traitOverrides.horizontalSizeClass = width.integerValue;
            self.window.rootViewController.traitOverrides.verticalSizeClass = UIUserInterfaceSizeClassRegular;
        }
        [self settle];
        SCTableViewController *detail = [[SCTableViewController alloc] init];
        detail.preferredContentSize = CGSizeMake(320, 400);
        [cell presentDetailViewController:detail forCell:cell forRowAtIndexPath:nil withPresentationMode:SCPresentationModePopover];
        [self settle];
        UIPopoverPresentationController *popover = detail.popoverController;
        XCTAssertNotNil(detail.view.window);
        if (width.integerValue == UIUserInterfaceSizeClassRegular) {
            XCTAssertEqual([popover adaptivePresentationStyleForTraitCollection:self.window.traitCollection], UIModalPresentationNone);
            XCTAssertTrue([SCUtilities isViewInsidePopover:detail.view]);
        }
        else {
            XCTAssertNotEqual([popover adaptivePresentationStyleForTraitCollection:self.window.traitCollection], UIModalPresentationNone);
            XCTAssertFalse([SCUtilities isViewInsidePopover:detail.view]);
        }
        [detail dismissWithCancelValue:NO doneValue:YES];
        [self settle];
        XCTAssertNil(self.owner.presentedViewController);
    }
}
- (void)testDoneAndCancelActionsPreserveValidationAndCallbacks {
    SCTableViewCell *cell = [self anchorCell];
    for (BOOL table = NO;; table = YES) {
        id detail = table ? (id)[[SCTableViewController alloc] init] : [[SCViewController alloc] init];
        id delegate = table ? (id)[[STVPopoverTableDelegate alloc] init] : [[STVPopoverViewDelegate alloc] init];
        [detail setDelegate:delegate];
        [detail setNavigationBarType:SCNavigationBarTypeDoneRightCancelLeft];
        __block BOOL allowAction = NO;
        __block NSInteger actionCount = 0;
        if (table) {
            [(SCTableViewController *)detail actions].doneButtonTapped = ^BOOL(SCTableViewController *c) {
                actionCount++;
                return allowAction;
            };
            [(SCTableViewController *)detail actions].cancelButtonTapped = ^BOOL(SCTableViewController *c) {
                actionCount++;
                return allowAction;
            };
        }
        else {
            [(SCViewController *)detail actions].doneButtonTapped = ^BOOL(SCViewController *c) {
                actionCount++;
                return allowAction;
            };
            [(SCViewController *)detail actions].cancelButtonTapped = ^BOOL(SCViewController *c) {
                actionCount++;
                return allowAction;
            };
        }
        [cell presentDetailViewController:detail forCell:cell forRowAtIndexPath:nil withPresentationMode:SCPresentationModePopover];
        [self settle];
        UIBarButtonItem *done = [detail doneButton];
        UIBarButtonItem *cancel = [detail cancelButton];
        XCTAssertNotNil(done);
        XCTAssertNotNil(cancel);
        [UIApplication.sharedApplication sendAction:done.action to:done.target from:done forEvent:nil];
        [UIApplication.sharedApplication sendAction:cancel.action to:cancel.target from:cancel forEvent:nil];
        XCTAssertEqual(actionCount, 2);
        XCTAssertEqual([[delegate events] count], 0u);
        XCTAssertNotNil([(UIViewController *)detail view].window);
        allowAction = YES;
        [delegate setAllowDismissal:NO];
        [UIApplication.sharedApplication sendAction:done.action to:done.target from:done forEvent:nil];
        XCTAssertNotNil([(UIViewController *)detail view].window);
        [delegate setAllowDismissal:YES];
        [UIApplication.sharedApplication sendAction:done.action to:done.target from:done forEvent:nil];
        [self settle];
        XCTAssertNil(self.owner.presentedViewController);
        XCTAssertEqualObjects([delegate events], (@[@[@"should", @NO, @YES], @[@"should", @NO, @YES], @[@"will", @NO, @YES], @[@"did", @NO, @YES]]));
        if (table)
            break;
    }
}
- (void)testAssociationDoesNotRetainPresentationOrStealOtherDelegateOnClear {
    for (id detail in @[[SCViewController new], [SCTableViewController new]]) {
        __weak UIPopoverPresentationController *weakPopover;
        @autoreleasepool {
            UIViewController *presented = [UIViewController new];
            presented.modalPresentationStyle = UIModalPresentationPopover;
            UIPopoverPresentationController *popover = presented.popoverPresentationController;
            [detail setPopoverController:popover];
            weakPopover = popover;
            XCTAssertEqual(popover.delegate, detail);
            UIViewController *other = [UIViewController new];
            other.modalPresentationStyle = UIModalPresentationPopover;
            [detail setPopoverController:other.popoverPresentationController];
            XCTAssertNil(popover.delegate);
            [detail setPopoverController:popover];
            NSObject *replacement = [NSObject new];
            popover.delegate = (id)replacement;
            [detail setPopoverController:nil];
            XCTAssertEqual(popover.delegate, replacement);
            [detail setPopoverController:popover];
        }
        XCTAssertNil(weakPopover);
        XCTAssertNil([detail popoverController]);
    }
}
- (void)testSectionAnchorsToAddButtonAndFallbackBounds {
    for (NSInteger anchor = 0; anchor < 3; anchor++) {
        SCArrayOfItemsSection *section = [[SCArrayOfItemsSection alloc] init];
        [self.owner.tableViewModel addSection:section];
        SCTableViewCell *addCell = nil;
        if (anchor == 0) {
            section.addButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:nil action:nil];
            self.owner.navigationItem.rightBarButtonItem = section.addButtonItem;
        }
        else if (anchor == 1) {
            addCell = [SCTableViewCell cellWithText:@"Add"];
            section.addNewItemCell = addCell;
        }
        [self.owner.tableView reloadData];
        [self.owner.tableView layoutIfNeeded];
        SCTableViewController *detail = [[SCTableViewController alloc] init];
        XCTAssertNoThrow([section presentDetailViewController:detail forCell:nil forRowAtIndexPath:nil withPresentationMode:SCPresentationModePopover]);
        [self settle];
        UINavigationController *presented = (id)self.owner.presentedViewController;
        XCTAssertEqual(presented.topViewController, detail);
        UIPopoverPresentationController *popover = presented.popoverPresentationController;
        XCTAssertNotNil(popover);
        if (anchor == 0) {
            XCTAssertEqual(popover.sourceItem, section.addButtonItem);
        }
        else {
            UIView *source = addCell ?: self.owner.tableView;
            XCTAssertEqual(popover.sourceView, source);
            XCTAssertTrue(CGRectEqualToRect(popover.sourceRect, source.bounds));
        }
        XCTAssertNotNil(detail.view.window);
        [self.owner dismissViewControllerAnimated:NO completion:nil];
        [self settle];
        [self.owner.tableViewModel removeAllSections];
    }
}
- (void)testCellPresentsActualDetailNavigationController {
    SCTableViewCell *cell = [self anchorCell];
    SCTableViewController *detail = [[SCTableViewController alloc] init];
    XCTAssertNoThrow([cell presentDetailViewController:detail forCell:cell forRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0] withPresentationMode:SCPresentationModePopover]);
    [self settle];
    UINavigationController *presented = (id)self.owner.presentedViewController;
    XCTAssertNotNil(presented);
    XCTAssertEqual(presented.topViewController, detail);
    XCTAssertEqual(presented.modalPresentationStyle, UIModalPresentationPopover);
    XCTAssertEqual(presented.popoverPresentationController.sourceView, cell);
    XCTAssertTrue(CGRectEqualToRect(presented.popoverPresentationController.sourceRect, cell.bounds));
    XCTAssertEqual((id)detail.popoverController, presented.popoverPresentationController);
    XCTAssertEqual(presented.popoverPresentationController.delegate, (id)detail);
    XCTAssertTrue(presented.modalInPresentation);
    XCTAssertNotNil(detail.view.window);
}
@end
