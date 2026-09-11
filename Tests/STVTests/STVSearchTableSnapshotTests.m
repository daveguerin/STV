@import XCTest;
@import STV;

@interface STVSearchTableSnapshotTests : XCTestCase
@end

@implementation STVSearchTableSnapshotTests
- (void)testEditingLastSearchResultKeepsBothNativeTableSnapshotsConsistent {
    UIViewController *owner = [UIViewController new];
    UIWindow *window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, 1032, 1376)];
    window.rootViewController = owner;
    [window makeKeyAndVisible];
    UITableView *table = [[UITableView alloc] initWithFrame:owner.view.bounds style:UITableViewStyleInsetGrouped];
    [owner.view addSubview:table];
    NSMutableDictionary *item = [@{@"name": @"Amber"} mutableCopy];
    SCDictionaryDefinition *definition = [SCDictionaryDefinition definitionWithDictionaryKeyNamesString:@"name"];
    definition.titlePropertyName = @"name";
    SCArrayOfObjectsModel *model = [SCArrayOfObjectsModel modelWithTableView:table items:[NSMutableArray arrayWithObject:item] itemsDefinition:definition];
    model.enableSearchController = YES;
    [table reloadData];
    model.searchController.active = YES;
    model.searchBar.text = @"Amber";
    [model updateSearchResultsForSearchController:model.searchController];
    [table reloadData];
    SCArrayOfItemsSection *section = (id)[model sectionAtIndex:0];
    item[@"name"] = @"Apricot";
    [model itemModified:item inSection:section];
    XCTAssertEqual(model.sectionCount, 0u);
    XCTAssertEqual(model.tableView.numberOfSections, 0);
    XCTAssertEqual(table.numberOfSections, 0, @"Hidden native table shares the changed data source and must discard stale section/accessibility snapshots");
    model.enableSearchController = NO;
    window.hidden = YES;
}
@end
