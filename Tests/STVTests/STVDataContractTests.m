#import <XCTest/XCTest.h>
#import "SCArrayStore.h"
#import "SCClassDefinition.h"
#import "SCCoreDataFetchOptions.h"
#import "SCCoreDataStore.h"

@interface STVDataContractTests : XCTestCase
@end

@implementation STVDataContractTests

- (void)testAbstractDefinitionHasOptionalNamesAndNoCompatibleStore {
    SCDataDefinition *definition = [SCDataDefinition new];
    XCTAssertNil(definition.dataStructureName);
    XCTAssertNil(definition.keyPropertyName);
    XCTAssertNil(definition.titlePropertyName);
    XCTAssertNil(definition.descriptionPropertyName);
    XCTAssertNil(definition.ibUniqueID);
    XCTAssertNil(definition.ibName);
    XCTAssertNil([definition generateCompatibleDataStore]);
    XCTAssertNotNil([definition generateCompatibleDataFetchOptions]);
    XCTAssertNotNil(definition.cellActions);
    XCTAssertNotNil(definition.defaultPropertyGroup);
    XCTAssertNotNil(definition.propertyGroups);
}

- (void)testMissingLookupsReturnNilButIndexedLookupsThrow {
    SCDataDefinition *definition = [SCDataDefinition new];
    XCTAssertNil([definition propertyDefinitionWithName:@"missing"]);
    XCTAssertEqual([definition indexOfPropertyDefinitionWithName:@"missing"], NSNotFound);
    XCTAssertNil([definition objectWithTitle:@"missing" inObjectsArray:@[]]);
    XCTAssertThrowsSpecificNamed([definition propertyDefinitionAtIndex:0], NSException, NSRangeException);

    SCPropertyGroupArray *groups = [SCPropertyGroupArray groupArray];
    XCTAssertNil([groups groupByHeaderTitle:@"missing"]);
    XCTAssertThrowsSpecificNamed([groups groupAtIndex:0], NSException, NSRangeException);
    SCPropertyGroup *group = [SCPropertyGroup groupWithHeaderTitle:nil footerTitle:nil propertyNames:@[@"name"]];
    XCTAssertNil(group.headerTitle);
    XCTAssertNil(group.footerTitle);
    XCTAssertEqualObjects([group propertyNameAtIndex:0], @"name");
    XCTAssertThrowsSpecificNamed([group propertyNameAtIndex:1], NSException, NSRangeException);
}

- (void)testAbstractStoreNilResultsAndOptionalConfiguration {
    SCDataStore *store = [SCDataStore storeWithDefaultDataDefinition:nil];
    XCTAssertNil(store.defaultDataDefinition);
    XCTAssertNil(store.storedData);
    XCTAssertNil(store.defaultsDictionary);
    XCTAssertNil(store.postAsynchronousFetchObjectsAction);
    XCTAssertNil([store definitionForObject:@"unknown"]);
    XCTAssertNil([store createNewObject]);
    XCTAssertNil([store createNewObjectWithDefinition:nil]);
    XCTAssertNil([store fetchObjectsWithOptions:nil]);
    XCTAssertNil([store valueForPropertyName:@"missing" inObject:@{}]);
    XCTAssertNil([store stringValueForPropertyName:@"missing" inObject:@{} separateValuesUsingDelimiter:nil]);
    [store bindStoreToPropertyName:nil forObject:nil withDefinition:nil];
    [store addDataDefinition:nil];
}

- (void)testAbstractAsyncOperationsDeliverNilErrorAndAcceptMissingCallbacks {
    SCDataStore *store = [SCDataStore new];
    __block NSUInteger failures = 0;
    SCDataStoreFailure_Block failure = ^(NSError *_Nullable error) {
        XCTAssertNil(error);
        failures++;
    };
    [store asynchronousInsertObject:@"item" success:nil failure:failure noConnection:nil];
    [store asynchronousUpdateObject:@"item" success:nil failure:failure noConnection:nil];
    [store asynchronousDeleteObject:@"item" success:nil failure:failure noConnection:nil];
    [store asynchronousFetchObjectsWithOptions:nil success:nil failure:failure noConnection:nil];
    XCTAssertEqual(failures, 4u);
    [store asynchronousInsertObject:@"item" success:nil failure:nil noConnection:nil];
    [store asynchronousUpdateObject:@"item" success:nil failure:nil noConnection:nil];
    [store asynchronousDeleteObject:@"item" success:nil failure:nil noConnection:nil];
    [store asynchronousFetchObjectsWithOptions:nil success:nil failure:nil noConnection:nil];
}

- (void)testPostFetchCompletionAcceptsNilErrorAndResults {
    SCDataStore *store = [SCDataStore new];
    store.postAsynchronousFetchObjectsAction = ^(NSArray *_Nullable results, SCPostFetchAsyncronousCompletionHandler_Block completion) {
        XCTAssertEqualObjects(results, @[@"item"]);
        completion(nil, nil);
    };
    __block BOOL called = NO;
    [store fetchObjectsSuccessful:@[@"item"] successBlock:^(NSArray *_Nullable results) {
        called = YES;
        XCTAssertNil(results);
    } failure:nil];
    XCTAssertTrue(called);
}

- (void)testAttributesRepresentUnsetValuesWithoutInventedDefaults {
    SCNumericTextFieldAttributes *numeric = [SCNumericTextFieldAttributes attributesWithMinimumValue:nil maximumValue:nil allowFloatValue:YES placeholder:nil];
    XCTAssertNil(numeric.minimumValue);
    XCTAssertNil(numeric.maximumValue);
    XCTAssertNil(numeric.placeholder);
    XCTAssertNotNil(numeric.numberFormatter);
    XCTAssertNil([SCDateAttributes attributesWithDateFormatter:nil].dateFormatter);
    XCTAssertNil([SCSegmentedAttributes new].segmentTitlesArray);
    XCTAssertNil([SCPropertyAttributes new].imageView);
    XCTAssertNil([SCPropertyAttributes new].imageViewArray);
    SCSelectionAttributes *selection = [SCSelectionAttributes new];
    XCTAssertNil(selection.selectionItemsStore);
    XCTAssertNil(selection.selectionItemsFetchOptions);
    XCTAssertNil(selection.items);
    SCArrayOfObjectsAttributes *objects = [SCArrayOfObjectsAttributes new];
    XCTAssertNil(objects.defaultObjectsDefinition);
    XCTAssertNil(objects.objectsFetchOptions);
    XCTAssertNotNil(objects.sectionActions);
}

- (void)testPropertyBindingsUseStringKeysAndStringValues {
    SCPropertyDefinition *property = [SCPropertyDefinition definitionWithName:@"name"];
    XCTAssertNil(property.title);
    XCTAssertNil(property.attributes);
    XCTAssertNil(property.editingModeAttributes);
    XCTAssertNil(property.ownerDataStuctureDefinition);
    XCTAssertNil(property.uiElementClass);
    XCTAssertNil(property.uiElementNibName);
    XCTAssertNil(property.objectBindings);
    XCTAssertNil(property.objectBindingsString);
    XCTAssertNotNil(property.cellActions);
    property.objectBindingsString = @"1:name;2:detail";
    NSDictionary<NSString *, NSString *> *bindings = property.objectBindings;
    XCTAssertEqualObjects(bindings[@"1"], @"name");
    XCTAssertEqualObjects(bindings[@"2"], @"detail");
    property.objectBindingsString = nil;
    XCTAssertEqual(property.objectBindings.count, 0u);
    XCTAssertNil(property.objectBindingsString);
    // NSObject's inherited init does not run initWithName:.
    XCTAssertNil([SCPropertyDefinition new].name);
    XCTAssertNil([SCPropertyDefinition new].cellActions);
}

- (void)testUnsetFetchOptionsProduceAnEmptyNonNilSortDescriptorArray {
    SCDataFetchOptions *options = [SCDataFetchOptions optionsWithSortKey:nil sortAscending:YES filterPredicate:nil];
    XCTAssertNil(options.sortKey);
    XCTAssertNil(options.filterPredicate);
    XCTAssertNotNil(options.sortDescriptors);
    XCTAssertEqual(options.sortDescriptors.count, 0u);
    options.sortKey = @"name;detail";
    NSArray<NSSortDescriptor *> *descriptors = options.sortDescriptors;
    XCTAssertEqualObjects(descriptors[0].key, @"name");
    XCTAssertEqualObjects(descriptors[1].key, @"detail");
    XCTAssertNil([SCCoreDataFetchOptions new].orderAttributeName);
}

- (void)testUnconfiguredCoreDataStoreClearsOptionalErrorOutput {
    SCCoreDataStore *store = [SCCoreDataStore new];
    XCTAssertNil(store.managedObjectContext);
    XCTAssertNil(store.boundSet);
    XCTAssertNil(store.boundOrderedSet);
    XCTAssertNil(store.lastCommitError);
    XCTAssertNil(store.commitFailure);
    NSError *error = [NSError errorWithDomain:@"STVDataContractTests" code:1 userInfo:nil];
    XCTAssertTrue([store commitDataWithError:&error]);
    XCTAssertNil(error);
    XCTAssertTrue([store commitDataWithError:NULL]);
    XCTAssertNil(store.lastCommitError);
    XCTAssertNil([SCEntityDefinition new].entity);
    XCTAssertNil([SCEntityDefinition new].managedObjectContext);
    XCTAssertNil([SCClassDefinition new].cls);
    XCTAssertNil([SCArrayStore storeWithObjectsArray:nil defaultDefiniton:nil].objectsArray);
}

@end
