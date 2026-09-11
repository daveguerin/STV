@import XCTest;
@import STV;

@interface STVExpandCollapseFixtureController : SCTableViewController
@property (nonatomic, strong) SCTableViewSection *collapsibleSection;
@end

@implementation STVExpandCollapseFixtureController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.collapsibleSection = [SCTableViewSection sectionWithHeaderTitle:@"TRY IT"];
    [self.collapsibleSection addCell:[SCTableViewCell cellWithText:@"Section content"]];
    self.collapsibleSection.expandCollapseCell = [SCExpandCollapseCell
        cellWithExpandText:@"Show content" collapseText:@"Hide content"
        ownerSectionExpanded:NO];
    self.collapsibleSection.footerTitle = @"Tap repeatedly to expand and collapse.";
    [self.tableViewModel addSection:self.collapsibleSection];
    SCTableViewSection *feedback = [SCTableViewSection
        sectionWithHeaderTitle:@"LIVE FEEDBACK" footerTitle:@"In-memory example"];
    [feedback addCell:[SCTableViewCell cellWithText:@"Ready"]];
    [self.tableViewModel addSection:feedback];
}

@end

@interface STVExpandCollapseInteractionTests : XCTestCase
@end

@implementation STVExpandCollapseInteractionTests

- (void)testDisplayedSectionSelectionKeepsVisibleRowsInSync {
    [self verifyDisplayedSectionSelectionAfterCountQuery:NO];
}

- (void)testDisplayedSectionSelectionAfterDataSourceCountQueryKeepsVisibleRowsInSync {
    [self verifyDisplayedSectionSelectionAfterCountQuery:YES];
}

- (void)testLoadingModeDefersProgrammaticExpansionUntilTableReload {
    UITableView *table = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 768, 1024)];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:table];
    SCTableViewSection *section = [SCTableViewSection section];
    [section addCell:[SCTableViewCell cellWithText:@"Content"]];
    section.expandCollapseCell = [SCExpandCollapseCell cellWithExpandText:@"Show"
        collapseText:@"Hide" ownerSectionExpanded:NO];
    [model addSection:section];
    [table reloadData];
    XCTAssertEqual([table numberOfRowsInSection:0], 1);
    [model enterLoadingMode];
    section.expandCollapseCell.ownerSectionExpanded = YES;
    XCTAssertFalse(model.live);
    XCTAssertEqual(section.cellCount, 2u);
    XCTAssertEqual([table numberOfRowsInSection:0], 1);
    section.expandCollapseCell.ownerSectionExpanded = NO;
    section.expandCollapseCell.ownerSectionExpanded = YES;
    XCTAssertFalse(model.live);
    XCTAssertEqual([table numberOfRowsInSection:0], 1);
    [table reloadData];
    XCTAssertEqual([table numberOfRowsInSection:0], 2);
}

- (void)verifyDisplayedSectionSelectionAfterCountQuery:(BOOL)queryCounts {
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 768, 1024)];
    STVExpandCollapseFixtureController *controller = [[STVExpandCollapseFixtureController alloc]
        initWithStyle:UITableViewStyleInsetGrouped];
    window.rootViewController = [[UINavigationController alloc] initWithRootViewController:controller];
    [window makeKeyAndVisible];
    [window layoutIfNeeded];
    [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];
    UITableView *table = controller.tableView;
    SCTableViewModel *model = controller.tableViewModel;
    SCTableViewSection *section = controller.collapsibleSection;
    SCExpandCollapseCell *toggle = section.expandCollapseCell;
    NSIndexPath *path = [NSIndexPath indexPathForRow:0 inSection:0];
    XCTAssertEqual(toggle.window, window);
    XCTAssertEqual([table cellForRowAtIndexPath:path], toggle);
    XCTAssertEqual([table numberOfRowsInSection:0], 1);
    @try {
        for (NSNumber *expanded in @[@YES, @NO, @YES, @NO]) {
            if (queryCounts) {
                /*
                 hermes:
                 A placeholder/content observer can query the data source after
                 rendering, without triggering another willDisplay callback.
                 This is the count-query sequence used by UITableView+dgApps.
                 */
                id<UITableViewDataSource> dataSource = table.dataSource;
                NSInteger count = [dataSource numberOfSectionsInTableView:table];
                for (NSInteger index = 0; index < count; index++) {
                    [dataSource tableView:table numberOfRowsInSection:index];
                }

            }

            NSLog(@"Expand fixture before selection: live=%d tableRows=%ld modelRows=%lu",
                  model.live, (long)[table numberOfRowsInSection:0], (unsigned long)section.cellCount);
            [model tableView:table didSelectRowAtIndexPath:path];
            [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];
            [table layoutIfNeeded];
            XCTAssertEqual(toggle.ownerSectionExpanded, expanded.boolValue);
            XCTAssertEqual(section.cellCount, expanded.boolValue ? 2u : 1u);
            XCTAssertEqual([table numberOfRowsInSection:0], (NSInteger)section.cellCount);
            XCTAssertEqual([table cellForRowAtIndexPath:path], toggle);
            XCTAssertEqualObjects(toggle.textLabel.text, expanded.boolValue ? @"Hide content" : @"Show content");
            if ([table numberOfRowsInSection:0] != (NSInteger)section.cellCount) {
                break;
            }

            if (expanded.boolValue) {
                UITableViewCell *content = [table cellForRowAtIndexPath:[NSIndexPath indexPathForRow:1 inSection:0]];
                XCTAssertNotNil(content);
                XCTAssertEqual(content.window, window);
                XCTAssertGreaterThan(content.alpha, 0.99);
            }

        }

    }
    @finally {
        window.hidden = YES;
    }
}

@end
