@import XCTest;
@import STV;

@interface STVEditedSearchItem : NSObject
@property(nonatomic, copy) NSString *name;
@property(nonatomic, strong) NSNumber *enabled;
@end

@implementation STVEditedSearchItem
@end

@interface STVEditedSearchMembershipTests : XCTestCase
@property(nonatomic, strong) UIWindow *window;
@property(nonatomic, strong) UIViewController *owner;
@property(nonatomic, strong) UITableView *table;
@property(nonatomic, strong) SCArrayOfObjectsModel *model;
@property(nonatomic, strong) NSMutableArray<STVEditedSearchItem *> *objects;
@end

@implementation STVEditedSearchMembershipTests

- (void)setUp {
    [super setUp];
    XCTAssertTrue(NSThread.isMainThread);
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 390, 844)];
    self.owner = [UIViewController new];
    self.window.rootViewController = self.owner;
    [self.window makeKeyAndVisible];
    self.table = [[UITableView alloc] initWithFrame:self.owner.view.bounds style:UITableViewStylePlain];
    [self.owner.view addSubview:self.table];
    self.objects = [NSMutableArray array];
    for (NSString *name in @[@"Amber", @"Azure", @"Beta"]) {
        STVEditedSearchItem *item = [STVEditedSearchItem new];
        item.name = name;
        item.enabled = @YES;
        [self.objects addObject:item];
    }
    SCClassDefinition *definition = [SCClassDefinition definitionWithClass:STVEditedSearchItem.class propertyNamesString:@"name;enabled"];
    definition.titlePropertyName = @"name";
    [definition propertyDefinitionWithName:@"enabled"].type = SCPropertyTypeSwitch;
    self.model = [SCArrayOfObjectsModel modelWithTableView:self.table items:(NSMutableArray *)self.objects itemsDefinition:definition];
    self.model.dataFetchOptions = [SCDataFetchOptions optionsWithSortKey:@"name" sortAscending:YES filterPredicate:nil];
    self.model.modelActions.sectionHeaderTitleForItem = ^NSString *(SCArrayOfItemsModel *model, NSObject *item, NSUInteger index) {
        return [[(STVEditedSearchItem *)item name] substringToIndex:1];
    };
    [self.table reloadData];
    [self.table layoutIfNeeded];
}

- (void)tearDown {
    self.model.enableSearchController = NO;
    self.window.hidden = YES;
    self.model = nil;
    self.table = nil;
    self.owner = nil;
    self.window = nil;
    self.objects = nil;
    [super tearDown];
}

- (NSArray<NSString *> *)visibleNames {
    NSMutableArray *names = [NSMutableArray array];
    NSInteger count = [self.model numberOfSectionsInTableView:self.table];
    XCTAssertEqual(self.table.numberOfSections, count);
    for (NSInteger index = 0; index < count; index++) {
        SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:index];
        XCTAssertEqual([self.table numberOfRowsInSection:index], section.cellCount);
        for (STVEditedSearchItem *item in section.items) {
            [names addObject:item.name];
        }
    }
    return names;
}

- (SCTableViewController *)editorForSection:(SCArrayOfObjectsSection *)section row:(NSUInteger)row {
    NSIndexPath *path = [NSIndexPath indexPathForRow:row inSection:[self.model indexForSection:section]];
    // Follow dispatchEventSelectRowAtIndexPath's generation path without modal
    // presentation: SwiftPM's hostless runner does not drive UIKit dismissal.
    section.selectedCellIndexPath = path;
    SCTableViewController *editor = (id)[section generatedDetailViewControllerForCellAtIndexPath:path];
    XCTAssertTrue([editor isKindOfClass:SCTableViewController.class]);
    [editor loadViewIfNeeded];
    [editor.tableView reloadData];
    [editor.tableView layoutIfNeeded];
    XCTAssertEqual(editor.delegate, section);
    return editor;
}

- (SCTableViewCell *)cellForProperty:(NSString *)property editor:(SCTableViewController *)editor {
    for (NSUInteger index = 0; index < editor.tableViewModel.sectionCount; index++) {
        SCTableViewSection *section = [editor.tableViewModel sectionAtIndex:index];
        for (NSUInteger row = 0; row < section.cellCount; row++) {
            SCTableViewCell *cell = [section cellAtIndex:row];
            if ([cell.boundPropertyName isEqualToString:property]) {
                return cell;
            }
        }
    }
    XCTFail(@"Missing generated property %@", property);
    return nil;
}

- (void)finishEditor:(SCTableViewController *)editor section:(SCArrayOfObjectsSection *)section cancel:(BOOL)cancel {
    [section tableViewControllerWillDismiss:editor cancelButtonTapped:cancel doneButtonTapped:!cancel];
    [section tableViewControllerDidDismiss:editor cancelButtonTapped:cancel doneButtonTapped:!cancel];
    // No application-side refetch/reload: dismissal must reconcile membership.
    [self.table layoutIfNeeded];
}

- (void)testOrdinaryEditWithoutSearchPreservesSectionAndSortsRows {
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    __block NSUInteger fetched = 0;
    self.model.modelActions.didFetchItemsFromStore = ^(SCArrayOfItemsModel *model, NSMutableArray *items) { fetched++; };
    SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
    cell.textField.text = @"Azzurro";
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Azzurro", @"Beta"]));
    XCTAssertEqual([self.model sectionAtIndex:0], section);
    XCTAssertEqualObjects(section.selectedCellIndexPath, [NSIndexPath indexPathForRow:1 inSection:0]);
    XCTAssertEqual(fetched, 0u);
    XCTAssertNil(self.model.searchBar);
}

- (void)testConsecutiveSameGroupSearchEditsPreserveSectionHooksWithoutReplay {
    self.model.enableSearchController = YES;
    self.model.searchPropertyName = @"name";
    self.model.searchBar.text = @"A";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    SCArrayOfObjectsSection *originalSection = (id)[self.model sectionAtIndex:0];
    originalSection.footerTitle = @"Section-local configuration";
    __block NSUInteger updated = 0;
    __block NSUInteger fetched = 0;
    __block NSUInteger added = 0;
    __block NSUInteger computed = 0;
    originalSection.sectionActions.didUpdateItem = ^(SCArrayOfItemsSection *section, NSObject *item, NSIndexPath *path) { updated++; };
    self.model.sectionActions.didFetchItemsFromStore = ^(SCArrayOfItemsSection *section, NSMutableArray *items) { fetched++; };
    self.model.sectionActions.didAddToModel = ^(SCTableViewSection *section, NSUInteger index) { added++; };
    self.model.modelActions.didComputeSearchResults = ^NSArray *(SCArrayOfItemsModel *model, NSString *text, NSArray *results) {
        computed++;
        return results;
    };
    for (NSString *name in @[@"Apricot", @"Azzurro"]) {
        SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
        SCTableViewController *editor = [self editorForSection:section row:0];
        SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
        cell.textField.text = name;
        [cell cellValueChanged];
        [self finishEditor:editor section:section cancel:NO];
        XCTAssertEqual([self.model sectionAtIndex:0], originalSection);
        XCTAssertEqualObjects([self.model sectionAtIndex:0].footerTitle, @"Section-local configuration");
    }
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Azzurro", @"Beta"]));
    XCTAssertEqualObjects(originalSection.selectedCellIndexPath, [NSIndexPath indexPathForRow:1 inSection:0]);
    XCTAssertEqual(updated, 2u);
    XCTAssertEqual(computed, 2u);
    XCTAssertEqual(fetched, 0u);
    XCTAssertEqual(added, 0u);
}

- (void)testDisabledPredicateDoesNotChangeOrdinaryEditing {
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    self.model.dataFetchOptions.filter = NO;
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    cell.switchControl.on = NO;
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects(self.objects[0].enabled, @NO);
    XCTAssertEqualObjects([self visibleNames], (@[@"Amber", @"Azure", @"Beta"]));
    XCTAssertEqual([self.model sectionAtIndex:0], section);
}

- (void)testPredicateAndSearchComposeAfterEditWithoutRefetching {
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    self.model.enableSearchController = YES;
    self.model.searchPropertyName = @"name";
    self.model.searchBar.text = @"A";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    __block NSUInteger fetched = 0;
    self.model.modelActions.didFetchItemsFromStore = ^(SCArrayOfItemsModel *model, NSMutableArray *items) { fetched++; };
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    cell.switchControl.on = NO;
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Beta"]));
    XCTAssertEqual(fetched, 0u);
    self.model.searchBar.text = @"";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Beta"]));
    self.model.dataFetchOptions.filterPredicate = nil;
    [self.model reloadBoundValues];
    [self.table reloadData];
    XCTAssertEqualObjects([self visibleNames], (@[@"Amber", @"Azure", @"Beta"]));
    XCTAssertEqual(fetched, 1u);
}

- (void)testPredicateExcludingLastStoredSearchRowKeepsBothNativeTablesCoherent {
    [self.objects removeObjectsInRange:NSMakeRange(1, 2)];
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    [self.model reloadBoundValues];
    [self.table reloadData];
    self.model.enableSearchController = YES;
    self.model.searchController.active = YES;
    self.model.searchBar.text = @"A";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    UITableView *resultsTable = self.model.tableView;
    XCTAssertNotEqual(resultsTable, self.table);
    __block NSUInteger computed = 0;
    self.model.modelActions.didComputeSearchResults = ^NSArray *(SCArrayOfItemsModel *model, NSString *text, NSArray *results) {
        computed++;
        XCTAssertEqualObjects(results, @[]);
        return results;
    };
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    cell.switchControl.on = NO;
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqual(self.model.items.count, 0u);
    XCTAssertEqual(self.model.sectionCount, 0u);
    XCTAssertEqual(resultsTable.numberOfSections, 0);
    XCTAssertEqual(self.table.numberOfSections, 0);
    XCTAssertEqual(computed, 1u);
    XCTAssertEqual(self.objects.count, 1u);
    XCTAssertEqualObjects(self.objects[0].enabled, @NO);
}

- (void)testPredicateExcludingLastRowAfterClearingActiveSearchKeepsBothTablesCoherent {
    [self.objects removeObjectsInRange:NSMakeRange(1, 2)];
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    [self.model reloadBoundValues];
    [self.table reloadData];
    self.model.enableSearchController = YES;
    self.model.searchController.active = YES;
    self.model.searchBar.text = @"";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    UITableView *resultsTable = self.model.tableView;
    XCTAssertNotEqual(resultsTable, self.table);
    __block NSUInteger computed = 0;
    self.model.modelActions.didComputeSearchResults = ^NSArray *(SCArrayOfItemsModel *model, NSString *text, NSArray *results) {
        computed++;
        XCTAssertEqualObjects(results, @[]);
        return results;
    };
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    cell.switchControl.on = NO;
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqual(self.model.items.count, 0u);
    XCTAssertEqual(self.model.sectionCount, 0u);
    XCTAssertEqual(resultsTable.numberOfSections, 0);
    XCTAssertEqual(self.table.numberOfSections, 0);
    XCTAssertEqual(computed, 0u);
    XCTAssertEqual(self.objects.count, 1u);
    XCTAssertEqualObjects(self.objects[0].enabled, @NO);
}

- (void)testPredicateReconciliationDoesNotMutateCallerOwnedItems {
    self.model.autoFetchItems = NO;
    [self.model setMutableItems:(NSMutableArray *)self.objects];
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    [self.model reloadBoundValues];
    [self.table reloadData];
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    cell.switchControl.on = NO;
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Beta"]));
    XCTAssertEqual(self.objects.count, 3u);
    XCTAssertEqualObjects(self.objects[0].enabled, @NO);
}

- (void)testCancelledGeneratedEditLeavesSearchMembershipUnchanged {
    self.model.enableSearchController = YES;
    self.model.searchBar.text = @"Amber";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
    cell.textField.text = @"Azzurro";
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:YES];
    XCTAssertEqualObjects(self.objects[0].name, @"Amber");
    XCTAssertEqualObjects([self visibleNames], (@[@"Amber"]));
    XCTAssertEqual([self.model sectionAtIndex:0], section);
}

- (void)testSearchEditRegroupsAndPreservesDeclaredEmptyGroupsAndIndexTitles {
    self.model.autoSortSections = YES;
    self.model.autoGenerateSectionIndexTitles = YES;
    self.model.modelActions.sectionHeaderTitles = ^NSArray *(SCArrayOfItemsModel *model) { return @[@"A", @"B", @"G"]; };
    self.model.enableSearchController = YES;
    self.model.searchBar.text = @"a";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
    cell.textField.text = @"Gamma";
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Beta", @"Gamma"]));
    XCTAssertEqualObjects([self.model sectionIndexTitlesForTableView:self.table], (@[@"A", @"B", @"G"]));
    XCTAssertEqualObjects([(SCArrayOfObjectsSection *)[self.model sectionWithHeaderTitle:@"G"] items], (@[self.objects[0]]));
    self.model.searchBar.text = @"missing";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertEqualObjects([self visibleNames], @[]);
    XCTAssertEqual(self.model.sectionCount, 3u);
}

- (void)testSameGroupRenameRecomputesNativeSearchAndClearingRestoresSortedItems {
    self.model.enableSearchController = YES;
    self.model.searchPropertyName = @"name";
    self.model.searchBar.text = @"Amber";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertEqualObjects([self visibleNames], (@[@"Amber"]));
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    XCTAssertEqualObjects(section.headerTitle, @"A");
    SCTableViewController *editor = [self editorForSection:section row:0];
    __block NSUInteger computed = 0;
    __block NSUInteger updated = 0;
    __block NSUInteger dismissed = 0;
    self.model.modelActions.didComputeSearchResults = ^NSArray *(SCArrayOfItemsModel *model, NSString *text, NSArray *results) {
        computed++;
        XCTAssertEqualObjects(text, @"Amber");
        XCTAssertEqual(results.count, 0u);
        return results;
    };
    section.sectionActions.didUpdateItem = ^(SCArrayOfItemsSection *source, NSObject *item, NSIndexPath *path) {
        updated++;
        XCTAssertEqualObjects([(STVEditedSearchItem *)item name], @"Azzurro");
        XCTAssertEqualObjects(path, [NSIndexPath indexPathForRow:0 inSection:0]);
    };
    section.sectionActions.detailModelDidDismiss = ^(SCTableViewSection *source, SCTableViewModel *detail, NSIndexPath *path) {
        dismissed++;
        XCTAssertEqual(detail, editor.tableViewModel);
        XCTAssertEqualObjects(path, [NSIndexPath indexPathForRow:0 inSection:0]);
    };
    SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
    XCTAssertTrue([cell isKindOfClass:SCTextFieldCell.class]);
    cell.textField.text = @"Azzurro";
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects(self.objects[0].name, @"Azzurro");
    XCTAssertEqualObjects([self visibleNames], @[]);
    XCTAssertEqual(computed, 1u);
    XCTAssertEqual(updated, 1u);
    XCTAssertEqual(dismissed, 1u);
    XCTAssertEqualObjects(self.model.searchBar.text, @"Amber");
    self.model.searchBar.text = @"";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Azzurro", @"Beta"]));
    XCTAssertEqualObjects([self.model sectionAtIndex:0].headerTitle, @"A");
    XCTAssertEqualObjects([self.model sectionAtIndex:1].headerTitle, @"B");
}

- (void)testClearingActiveNativeSearchReloadsUnderlyingRenderedRows {
    self.model.enableSearchController = YES;
    self.model.searchController.active = YES;
    [self.table layoutIfNeeded];
    self.model.searchBar.text = @"Amber";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    XCTAssertNotEqual(self.model.tableView, self.table);
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCTextFieldCell *cell = (id)[self cellForProperty:@"name" editor:editor];
    cell.textField.text = @"Apricot";
    [cell cellValueChanged];
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects(self.objects[0].name, @"Apricot");
    self.model.searchBar.text = @"";
    [self.model updateSearchResultsForSearchController:self.model.searchController];
    [self.table layoutIfNeeded];
    // UIKit hides results for an empty query while search remains active.
    XCTAssertTrue(self.model.searchController.active);
    XCTAssertEqualObjects([self.table cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]].textLabel.text, @"Apricot");
}

- (void)testGeneratedSwitchEditRemovesItemExcludedByFetchPredicate {
    self.model.dataFetchOptions.filterPredicate = [NSPredicate predicateWithFormat:@"enabled == YES"];
    [self.model reloadBoundValues];
    [self.table reloadData];
    XCTAssertEqualObjects([self visibleNames], (@[@"Amber", @"Azure", @"Beta"]));
    SCArrayOfObjectsSection *section = (id)[self.model sectionAtIndex:0];
    SCTableViewController *editor = [self editorForSection:section row:0];
    SCSwitchCell *cell = (id)[self cellForProperty:@"enabled" editor:editor];
    XCTAssertTrue([cell isKindOfClass:SCSwitchCell.class]);
    cell.switchControl.on = NO;
    // Hostless XCTest has no UIApplication target/action dispatch. Deliver the
    // control's value-change notification to STV, retaining its real commit path.
    [cell cellValueChanged];
    XCTAssertEqual(cell.boundObject, self.objects[0]);
    XCTAssertTrue(cell.needsCommit || [self.objects[0].enabled isEqual:@NO]);
    XCTAssertFalse(cell.switchControl.on);
    XCTAssertFalse(section.dataStore.defaultDataDefinition.requireEditingModeToEditPropertyValues);
    [self finishEditor:editor section:section cancel:NO];
    XCTAssertEqualObjects(self.objects[0].enabled, @NO);
    XCTAssertEqualObjects([self visibleNames], (@[@"Azure", @"Beta"]));
    XCTAssertFalse([self.model.items containsObject:self.objects[0]]);
    XCTAssertEqual(self.objects.count, 3u, @"Filtering must not delete from the store");
}

@end
