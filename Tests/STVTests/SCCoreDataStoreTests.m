@import XCTest;
@import CoreData;
@import STV;

@interface STVFailingManagedObjectContext : NSManagedObjectContext
@end

@implementation STVFailingManagedObjectContext

- (BOOL)hasChanges {
    return YES;
}

- (BOOL)save:(NSError **)error {
    if (error)
        *error = [NSError errorWithDomain:@"STVTests" code:700 userInfo:@{NSLocalizedDescriptionKey: @"Expected save failure"}];
    return NO;
}

@end

@interface SCCoreDataStoreTests : XCTestCase
@end

@implementation SCCoreDataStoreTests

- (SCEntityDefinition *)testEntityDefinition {
    NSAttributeDescription *nameAttribute = [[NSAttributeDescription alloc] init];
    nameAttribute.name = @"name";
    nameAttribute.attributeType = NSStringAttributeType;

    NSEntityDescription *entity = [[NSEntityDescription alloc] init];
    entity.name = @"Item";
    entity.managedObjectClassName = @"NSManagedObject";
    entity.properties = @[nameAttribute];

    NSManagedObjectModel *model = [[NSManagedObjectModel alloc] init];
    model.entities = @[entity];

    NSPersistentStoreCoordinator *coordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:model];
    NSError *error = nil;
    XCTAssertNotNil([coordinator addPersistentStoreWithType:NSInMemoryStoreType configuration:nil URL:nil options:nil error:&error]);
    XCTAssertNil(error);

    NSManagedObjectContext *context = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSMainQueueConcurrencyType];
    context.persistentStoreCoordinator = coordinator;

    return [SCEntityDefinition definitionWithEntityName:entity.name managedObjectContext:context propertyNamesString:@"name"];
}

- (void)testCommitDataWithErrorReportsSaveFailureWithoutTerminatingHost {
    STVFailingManagedObjectContext *context = [[STVFailingManagedObjectContext alloc] initWithConcurrencyType:NSMainQueueConcurrencyType];
    SCCoreDataStore *store = [[SCCoreDataStore alloc] initWithManagedObjectContext:context defaultEntityDefinition:nil];
    NSError *error = nil;

    BOOL success = [store commitDataWithError:&error];

    XCTAssertFalse(success);
    XCTAssertEqualObjects(error.domain, @"STVTests");
    XCTAssertEqualObjects(store.lastCommitError, error);
}

- (void)testCoreDataConvenienceMethodsAreOwnedByTheirClasses {
    SCEntityDefinition *definition = [self testEntityDefinition];
    NSPredicate *predicate = [NSPredicate predicateWithValue:YES];

    SCArrayOfObjectsCell *cell = [SCArrayOfObjectsCell cellWithEntityDefinition:definition];
    XCTAssertTrue([cell.dataStore isKindOfClass:[SCCoreDataStore class]]);

    NSMutableSet *relationshipItems = [NSMutableSet set];
    SCArrayOfObjectsCell *relationshipCell = [SCArrayOfObjectsCell cellWithBoundItemsSet:relationshipItems boundSetEntityDefinition:definition boundSetOwnsObjects:YES];
    XCTAssertTrue([relationshipCell.dataStore isKindOfClass:[SCCoreDataStore class]]);
    SCCoreDataStore *relationshipStore = (SCCoreDataStore *)relationshipCell.dataStore;
    XCTAssertEqual(relationshipStore.boundSet, relationshipItems);
    XCTAssertTrue(relationshipStore.boundSetOwnsStoreObjects);

    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
    SCArrayOfObjectsModel *objectModel = [SCArrayOfObjectsModel modelWithTableView:tableView entityDefinition:definition filterPredicate:predicate];
    XCTAssertTrue([objectModel.dataStore isKindOfClass:[SCCoreDataStore class]]);
    XCTAssertTrue(objectModel.dataFetchOptions.filter);
    XCTAssertEqualObjects(objectModel.dataFetchOptions.filterPredicate, predicate);

    SCArrayOfObjectsSection *section = [SCArrayOfObjectsSection sectionWithHeaderTitle:@"Items" entityDefinition:definition filterPredicate:predicate];
    XCTAssertTrue([section.dataStore isKindOfClass:[SCCoreDataStore class]]);
    XCTAssertTrue(section.dataFetchOptions.filter);
    XCTAssertEqualObjects(section.dataFetchOptions.filterPredicate, predicate);

    SCObjectSelectionAttributes *attributes = [SCObjectSelectionAttributes attributesWithObjectsEntityDefinition:definition usingPredicate:predicate allowMultipleSelection:YES allowNoSelection:YES];
    XCTAssertTrue([attributes.selectionItemsStore isKindOfClass:[SCCoreDataStore class]]);
    XCTAssertTrue(attributes.selectionItemsFetchOptions.filter);
    XCTAssertEqualObjects(attributes.selectionItemsFetchOptions.filterPredicate, predicate);
    XCTAssertTrue(attributes.allowMultipleSelection);
    XCTAssertTrue(attributes.allowNoSelection);
}

@end
