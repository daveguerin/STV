@import XCTest;
@import STV;

@interface STVLayoutIntegrationTests : XCTestCase
@end

@implementation STVLayoutIntegrationTests

- (void)testSearchRemainsVisibleWithoutANavigationController {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 320, 700)];
    UIViewController *host = [[UIViewController alloc] init];
    window.rootViewController = host;
    [window makeKeyAndVisible];
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 320, 600)];
    [host.view addSubview:table];
    SCArrayOfStringsModel *model = [[SCArrayOfStringsModel alloc] initWithTableView:table];
    model.enableSearchController = YES;
    [table reloadData];
    [host.view layoutIfNeeded];
    UISearchBar *bar = host.navigationItem.searchController.searchBar ?: model.searchBar;
    XCTAssertNotNil(bar);
    XCTAssertTrue([bar isDescendantOfView:host.view], @"Search must not vanish in a non-navigation host");
    XCTAssertGreaterThan(CGRectGetHeight(bar.bounds), 0);
    window.hidden = YES;
    window.rootViewController = nil;
}

- (void)testGeneratedEditorsFitNarrowAndWideContainers {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 1024, 1000)];
    UIViewController *host = [[UIViewController alloc] init];
    window.rootViewController = host;
    [window makeKeyAndVisible];
    SCTableViewController *controller = [[SCTableViewController alloc] initWithStyle:UITableViewStyleInsetGrouped];
    [host addChildViewController:controller];
    controller.view.translatesAutoresizingMaskIntoConstraints = NO;
    [host.view addSubview:controller.view];
    NSLayoutConstraint *width = [controller.view.widthAnchor constraintEqualToConstant:320];
    [NSLayoutConstraint activateConstraints:@[[controller.view.leadingAnchor constraintEqualToAnchor:host.view.safeAreaLayoutGuide.leadingAnchor], [controller.view.topAnchor constraintEqualToAnchor:host.view.safeAreaLayoutGuide.topAnchor], [controller.view.bottomAnchor constraintEqualToAnchor:host.view.safeAreaLayoutGuide.bottomAnchor], width]];
    [controller didMoveToParentViewController:host];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:@"Generated controls"];
    SCTextViewCell *note = [[SCTextViewCell alloc] initWithText:@"Pre Note" boundObject:nil textViewTextPropertyName:nil];
    note.autoResize = YES;
    note.textView.text = @"A longer Pre Note should wrap within the visible "
                         @"editor, without any text escaping beyond its right edge. "
                         @"The same editor must adapt when the sidebar or window width changes.";
    SCTextFieldCell *field = [[SCTextFieldCell alloc] initWithText:@"Name" placeholder:@"Song name" boundObject:nil textFieldTextPropertyName:nil];
    SCSliderCell *slider = [[SCSliderCell alloc] initWithText:@"Level" boundObject:nil sliderValuePropertyName:nil];
    SCSegmentedCell *segments = [[SCSegmentedCell alloc] initWithText:@"Choice" boundObject:nil selectedSegmentIndexPropertyName:nil segmentTitlesArray:@[@"One", @"Two"]];
    SCSwitchCell *toggle = [[SCSwitchCell alloc] initWithText:@"Enabled" boundObject:nil switchOnPropertyName:nil];
    SCDateCell *date = [[SCDateCell alloc] initWithText:@"Date" boundObject:nil datePropertyName:nil];
    SCExpandCollapseCell *expand = [SCExpandCollapseCell cellWithExpandText:@"Expand" collapseText:@"Collapse" ownerSectionExpanded:NO];
    for (SCTableViewCell *cell in @[note, field, slider, segments, toggle, date, expand])
        [section addCell:cell];
    [controller.tableViewModel addSection:section];
    [controller.tableView reloadData];
    CGFloat narrowHeight = 0;
    for (NSNumber *containerWidth in @[@320, @760]) {
        width.constant = containerWidth.doubleValue;
        [host.view setNeedsLayout];
        [host.view layoutIfNeeded];
        [controller.tableView reloadData];
        [controller.tableView layoutIfNeeded];
        [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
        XCTAssertEqualWithAccuracy(controller.view.bounds.size.width, width.constant, 0.5);
        for (SCControlCell *cell in @[note, field, slider, segments, toggle]) {
            XCTAssertFalse(cell.control.translatesAutoresizingMaskIntoConstraints);
            XCTAssertGreaterThan(cell.control.bounds.size.width, 0);
            XCTAssertGreaterThan(cell.control.bounds.size.height, 0);
            XCTAssertGreaterThanOrEqual(CGRectGetMinX(cell.control.frame), -0.5);
            XCTAssertLessThanOrEqual(CGRectGetMaxX(cell.control.frame), CGRectGetWidth(cell.contentView.bounds) + 0.5);
            XCTAssertFalse(cell.control.hasAmbiguousLayout);
        }
        XCTAssertGreaterThan(expand.accessoryView.bounds.size.width, 0);
        if (width.constant == 320)
            narrowHeight = note.bounds.size.height;
        else
            XCTAssertGreaterThanOrEqual(narrowHeight, note.bounds.size.height);
        UIGraphicsImageRenderer *renderer = [[UIGraphicsImageRenderer alloc] initWithSize:controller.view.bounds.size];
        UIImage *image = [renderer imageWithActions:^(UIGraphicsImageRendererContext *context) { [controller.view.layer renderInContext:context.CGContext]; }];
        XCTAttachment *attachment = [XCTAttachment attachmentWithImage:image];
        attachment.name = [NSString stringWithFormat:@"STV-controls-%@", containerWidth];
        attachment.lifetime = XCTAttachmentLifetimeKeepAlways;
        [self addAttachment:attachment];
    }
    window.hidden = YES;
    window.rootViewController = nil;
}

@end
