@import XCTest;
@import STV;

// Observe protected draft tracking without replacing any store behavior.
@interface STVPrimitiveTrackingStore : SCArrayStore
@property (nonatomic, readonly) NSArray *trackedDrafts;
@end

@implementation STVPrimitiveTrackingStore
- (NSArray *)trackedDrafts {
    return [_uninsertedObjects copy];
}
@end

@interface SCTableViewCell (STVPrimitiveTesting)
- (void)resetInitialBoundValue;
@end

@interface SCArrayOfItemsSection (STVPrimitiveTesting)
- (void)commitNewItem:(NSObject *)newItem detailModel:(SCTableViewModel *)detailModel;
@end


@interface STVPrimitiveArrayStoreTests : XCTestCase
@end

@implementation STVPrimitiveArrayStoreTests

- (void)testReadCallbackCannotReplacePrimitiveDraftIdentity {
    NSNumber *shared = @7;
    NSMutableArray *numbers = [@[shared] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    SCObjectSection *editor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draft boundObjectStore:store propertyGroup:nil];
    SCTableViewCell *cell = [editor cellAtIndex:0];
    cell.cellActions.didLoadBoundValue = ^NSObject *(SCTableViewCell *loadedCell, NSIndexPath *path, NSObject *value) {
        return @([(NSNumber *)value integerValue]);
    };
    cell.boundValue = shared;
    cell.boundValue = @9;
    XCTAssertEqualObjects(numbers, (@[@7]));
    XCTAssertTrue([store insertObject:draft]);
    XCTAssertEqualObjects(numbers, (@[@7, @9]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
}

- (void)testPrimitiveDraftIdentitySurvivesSnapshotResetButNotRebinding {
    NSMutableArray *numbers = [@[@7, @8] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    SCObjectSection *editor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draft boundObjectStore:store propertyGroup:nil];
    SCTableViewCell *cell = [editor cellAtIndex:0];
    cell.boundValue = @7;
    [cell resetInitialBoundValue];
    cell.boundValue = @9;
    XCTAssertEqualObjects(numbers, (@[@7, @8]));
    cell.boundObject = @8;
    cell.boundValue = @10;
    XCTAssertEqualObjects(numbers, (@[@7, @10]));
    XCTAssertTrue([store insertObject:draft]);
    XCTAssertEqualObjects(numbers, (@[@7, @10, @9]));
}

- (void)testGeneratedDraftsSharingReplacementKeepTheirOwnEditIdentity {
    NSMutableArray *numbers = [NSMutableArray array];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draftA = [store createNewObject];
    NSObject *draftB = [store createNewObject];
    SCObjectSection *editorA = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draftA boundObjectStore:store propertyGroup:nil];
    SCObjectSection *editorB = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draftB boundObjectStore:store propertyGroup:nil];
    SCTableViewCell *cellA = [editorA cellAtIndex:0];
    SCTableViewCell *cellB = [editorB cellAtIndex:0];
    NSNumber *shared = @7;
    cellA.boundValue = shared;
    cellB.boundValue = shared;
    cellB.boundValue = @9;
    XCTAssertTrue([store insertObject:draftA]);
    XCTAssertTrue([store insertObject:draftB]);
    XCTAssertEqualObjects(numbers, (@[@7, @9]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
}

- (void)testExistingGeneratedRowSharingDraftReplacementStillEditsStoredRow {
    NSNumber *shared = @7;
    NSMutableArray *numbers = [@[shared] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    SCObjectSection *draftEditor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draft boundObjectStore:store propertyGroup:nil];
    [draftEditor cellAtIndex:0].boundValue = shared;
    SCObjectSection *existingEditor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:shared boundObjectStore:store propertyGroup:nil];
    [existingEditor cellAtIndex:0].boundValue = @8;
    XCTAssertEqualObjects(numbers, (@[@8]));
    XCTAssertTrue([store insertObject:draft]);
    XCTAssertEqualObjects(numbers, (@[@8, @7]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
}

- (void)testGeneratedControlInsertionPreservesOtherDraftWithSharedValue {
    NSMutableArray *numbers = [NSMutableArray array];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draftA = [store createNewObject];
    NSObject *draftB = [store createNewObject];
    SCObjectSection *editorA = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draftA boundObjectStore:store propertyGroup:nil];
    SCObjectSection *editorB = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draftB boundObjectStore:store propertyGroup:nil];
    SCTableViewModel *modelA = [[SCTableViewModel alloc] init];
    SCTableViewModel *modelB = [[SCTableViewModel alloc] init];
    [modelA addSection:editorA];
    [modelB addSection:editorB];
    SCNumericTextFieldCell *cellA = (SCNumericTextFieldCell *)[editorA cellAtIndex:0];
    SCNumericTextFieldCell *cellB = (SCNumericTextFieldCell *)[editorB cellAtIndex:0];
    cellA.textField.text = @"7";
    [cellA cellValueChanged];
    cellB.textField.text = @"7";
    [cellB cellValueChanged];
    [modelA commitChanges];
    [modelB commitChanges];
    XCTAssertEqual(cellA.boundObject, cellB.boundObject);
    SCArrayOfObjectsSection *list = [SCArrayOfObjectsSection sectionWithHeaderTitle:nil dataStore:store];
    [list commitNewItem:draftB detailModel:modelB];
    XCTAssertEqual(store.trackedDrafts.count, 1u);
    XCTAssertEqual(store.trackedDrafts.firstObject, draftA);
    cellA.textField.text = @"9";
    [cellA cellValueChanged];
    [modelA commitChanges];
    XCTAssertEqualObjects(numbers, (@[@7]));
    [list commitNewItem:draftA detailModel:modelA];
    XCTAssertEqualObjects(numbers, (@[@7, @9]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
}

- (void)testGeneratedDateOnlyInsertionCommitsNormalizedPickerValue {
    NSMutableArray *dates = [NSMutableArray array];
    SCDateDefinition *definition = [SCDateDefinition definition];
    definition.datePropertyDefinition.attributes = [SCDateAttributes attributesWithDateFormatter:nil datePickerMode:UIDatePickerModeDate displayDatePickerAsInputAccessoryView:NO];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:dates defaultDefiniton:definition];
    NSObject *draft = [store createNewObject];
    SCObjectSection *editor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draft boundObjectStore:store propertyGroup:nil];
    SCTableViewModel *detailModel = [[SCTableViewModel alloc] init];
    [detailModel addSection:editor];
    SCDateCell *cell = (SCDateCell *)[editor cellAtIndex:0];
    XCTAssertTrue([cell isKindOfClass:SCDateCell.class]);
    XCTAssertEqual(cell.datePicker.datePickerMode, UIDatePickerModeDate);
    cell.commitChangesLive = NO;
    NSCalendar *calendar = [NSCalendar currentCalendar];
    NSDateComponents *components = [[NSDateComponents alloc] init];
    components.year = 2026;
    components.month = 9;
    components.day = 8;
    components.hour = 15;
    components.minute = 37;
    components.second = 24;
    cell.datePicker.date = [calendar dateFromComponents:components];
    // Hostless package test: invoke the same handler as pickerValueChanged.
    [cell cellValueChanged];
    NSDate *expected = [calendar startOfDayForDate:cell.datePicker.date];
    XCTAssertNotEqualObjects(cell.datePicker.date, expected);
    XCTAssertEqualObjects(cell.controlValue, expected);
    XCTAssertTrue(cell.needsCommit);
    [detailModel commitChanges];
    SCArrayOfObjectsSection *list = [SCArrayOfObjectsSection sectionWithHeaderTitle:nil dataStore:store];
    [list commitNewItem:draft detailModel:detailModel];
    XCTAssertEqualObjects(dates, (@[expected]));
    XCTAssertEqual(dates.firstObject, cell.boundObject);
    XCTAssertEqual(store.trackedDrafts.count, 0u);
    SCObjectSection *reopened = [SCObjectSection sectionWithHeaderTitle:nil boundObject:dates.firstObject boundObjectStore:store propertyGroup:nil];
    XCTAssertEqualObjects([reopened cellAtIndex:0].boundValue, expected);
}

- (void)testSectionInsertionUsesCommittedDecimalIdentity {
    NSMutableArray *numbers = [NSMutableArray array];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    SCObjectSection *editor = [SCObjectSection sectionWithHeaderTitle:nil boundObject:draft boundObjectStore:store propertyGroup:nil];
    SCTableViewModel *detailModel = [[SCTableViewModel alloc] init];
    [detailModel addSection:editor];
    SCNumericTextFieldCell *cell = (SCNumericTextFieldCell *)[editor cellAtIndex:0];
    cell.numberFormatter.generatesDecimalNumbers = YES;
    cell.textField.text = @"123.5";
    cell.boundValue = cell.controlValue;
    SCArrayOfObjectsSection *list = [SCArrayOfObjectsSection sectionWithHeaderTitle:nil dataStore:store];
    [list commitNewItem:draft detailModel:detailModel];
    XCTAssertEqualObjects(numbers.firstObject, [NSDecimalNumber decimalNumberWithString:@"123.5"]);
    XCTAssertEqual(store.trackedDrafts.count, 0u);
    XCTAssertEqual(numbers.firstObject, cell.boundObject);
}

- (void)testPrimitiveCellCanEditAfterClearing {
    NSMutableArray *numbers = [@[@7] mutableCopy];
    SCArrayStore *store = [SCArrayStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    SCObjectSection *section = [SCObjectSection sectionWithHeaderTitle:nil boundObject:numbers.firstObject boundObjectStore:store propertyGroup:nil];
    SCTableViewCell *cell = [section cellAtIndex:0];
    cell.boundValue = nil;
    cell.boundValue = @8;
    XCTAssertEqualObjects(numbers, (@[@8]));
}

- (void)testPrimitiveCellRollsBackRepeatedLiveEdits {
    NSMutableArray *numbers = [@[@7] mutableCopy];
    SCArrayStore *store = [SCArrayStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    SCObjectSection *section = [SCObjectSection sectionWithHeaderTitle:nil boundObject:numbers.firstObject boundObjectStore:store propertyGroup:nil];
    SCTableViewCell *cell = [section cellAtIndex:0];
    cell.boundValue = @8;
    cell.boundValue = @9;
    [cell rollbackToInitialBoundValue];
    XCTAssertEqualObjects(numbers, (@[@7]));
}

- (void)testNumberDraftIsDistinctZeroTrackedUntilInsertOrCancel {
    NSNumber *existingZero = @0;
    NSMutableArray *numbers = [@[existingZero] mutableCopy];
    SCNumberDefinition *definition = [SCNumberDefinition definition];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:definition];
    NSObject *draft = [store createNewObject];
    XCTAssertNotNil(draft);
    if (!draft) { return; }
    XCTAssertTrue([draft isKindOfClass:NSNumber.class]);
    XCTAssertEqualObjects(draft, @0);
    XCTAssertNotEqual(draft, existingZero);
    XCTAssertEqual([store definitionForObject:draft], definition);
    XCTAssertEqual(store.trackedDrafts.count, 1u);
    XCTAssertEqual(store.trackedDrafts.firstObject, draft);
    XCTAssertEqual(numbers.count, 1u);
    XCTAssertTrue([store insertObject:draft]);
    XCTAssertEqual(numbers.count, 2u);
    XCTAssertEqual(numbers.lastObject, draft);
    XCTAssertEqual(store.trackedDrafts.count, 0u);

    NSObject *cancelled = [store createNewObject];
    XCTAssertNotNil(cancelled);
    XCTAssertNotEqual(cancelled, draft);
    XCTAssertTrue([store discardUninsertedObject:cancelled]);
    XCTAssertEqual(store.trackedDrafts.count, 0u);
    XCTAssertEqual(numbers.count, 2u);
}

- (void)testEditedDraftInsertionClearsTrackingWithoutReplacingExistingNumber {
    NSMutableArray *numbers = [@[@0, @7] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    XCTAssertNotNil(draft);
    if (!draft) { return; }
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Number"];
    cell.boundObject = draft;
    cell.boundPropertyName = @"number";
    cell.boundObjectStore = store;
    cell.boundValue = @7;
    cell.boundValue = @9;
    XCTAssertEqualObjects(numbers, (@[@0, @7]));
    XCTAssertEqualObjects(cell.boundValue, @9);
    XCTAssertEqual(store.trackedDrafts.count, 1u);
    XCTAssertTrue([store insertObject:draft]);
    XCTAssertEqualObjects(numbers, (@[@0, @7, @9]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
    [store forceDiscardAllUnaddedObjects];
    XCTAssertEqualObjects(numbers, (@[@0, @7, @9]));
}

- (void)testOrderedInsertionOfEditedDraftUsesCurrentValueAndClearsTracking {
    NSMutableArray *numbers = [@[@12] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *draft = [store createNewObject];
    [store setValue:@7 forPropertyName:@"number" inObject:draft];
    XCTAssertTrue([store insertObject:draft atOrder:0]);
    XCTAssertEqualObjects(numbers, (@[@7, @12]));
    XCTAssertEqual(store.trackedDrafts.count, 0u);
}

- (void)testEditedDraftCanBeCancelledByOriginalIdentity {
    NSMutableArray *numbers = [@[@0, @7] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    for (NSUInteger attempt = 0; attempt < 2; attempt++) {
        NSObject *draft = [store createNewObject];
        [store setValue:@7 forPropertyName:@"number" inObject:draft];
        [store setValue:nil forPropertyName:@"number" inObject:draft];
        [store setValue:@9 forPropertyName:@"number" inObject:draft];
        XCTAssertEqualObjects(numbers, (@[@0, @7]));
        XCTAssertTrue([store discardUninsertedObject:draft]);
        XCTAssertEqual(store.trackedDrafts.count, 0u);
        // Cancellation also releases the replacement identity: ordinary edits
        // to the existing tagged number must now reach the stored row.
        [store setValue:@8 forPropertyName:@"number" inObject:@7];
        XCTAssertEqualObjects(numbers, (@[@0, @8]));
        [store setValue:@7 forPropertyName:@"number" inObject:@8];
    }
}

- (void)testForceDiscardRemovesAllEqualValuedDraftsWithoutChangingRows {
    NSMutableArray *numbers = [@[@0] mutableCopy];
    STVPrimitiveTrackingStore *store = [STVPrimitiveTrackingStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    NSObject *first = [store createNewObject];
    NSObject *second = [store createNewObject];
    XCTAssertNotEqual(first, second);
    [store setValue:@7 forPropertyName:@"number" inObject:first];
    [store setValue:@9 forPropertyName:@"number" inObject:second];
    XCTAssertEqual(store.trackedDrafts.count, 2u);
    [store forceDiscardAllUnaddedObjects];
    XCTAssertEqual(store.trackedDrafts.count, 0u);
    XCTAssertEqualObjects(numbers, (@[@0]));
}

- (void)testNumbersDatesAndStringsEditRepeatedlyAndReopenGeneratedControls {
    NSArray *definitions = @[[SCNumberDefinition definition], [SCDateDefinition definition], [SCStringDefinition definition]];
    NSArray *initials = @[@7, [NSDate dateWithTimeIntervalSince1970:1000], @"Initial string value"];
    NSArray *firstEdits = @[@8, [NSDate dateWithTimeIntervalSince1970:2000], @"First edited string value"];
    NSArray *secondEdits = @[@9, [NSDate dateWithTimeIntervalSince1970:3000], @"Second edited string value"];
    for (NSUInteger i = 0; i < definitions.count; i++) {
        NSObject *other = @"Unrelated row";
        NSMutableArray *objects = [@[initials[i], other] mutableCopy];
        SCArrayStore *store = [SCArrayStore storeWithObjectsArray:objects defaultDefiniton:definitions[i]];
        SCObjectSection *section = [SCObjectSection sectionWithHeaderTitle:nil boundObject:objects.firstObject boundObjectStore:store propertyGroup:nil];
        XCTAssertEqual(section.cellCount, 1u);
        SCTableViewCell *cell = [section cellAtIndex:0];
        XCTAssertTrue([cell isKindOfClass:SCControlCell.class]);
        cell.boundValue = firstEdits[i];
        XCTAssertEqual(objects.firstObject, firstEdits[i]);
        XCTAssertEqual(cell.boundObject, firstEdits[i]);
        cell.boundValue = secondEdits[i];
        XCTAssertEqual(objects.firstObject, secondEdits[i]);
        XCTAssertEqual(cell.boundObject, secondEdits[i]);
        XCTAssertEqual(objects.lastObject, other);
        XCTAssertEqual(objects.count, 2u);
        SCObjectSection *reopened = [SCObjectSection sectionWithHeaderTitle:nil boundObject:[store fetchObjectsWithOptions:nil].firstObject boundObjectStore:store propertyGroup:nil];
        SCControlCell *reopenedCell = (SCControlCell *)[reopened cellAtIndex:0];
        [reopenedCell reloadBoundValue];
        XCTAssertEqualObjects(reopenedCell.boundValue, secondEdits[i]);
        XCTAssertEqualObjects(reopenedCell.controlValue, secondEdits[i]);
        reopenedCell.boundValue = firstEdits[i];
        XCTAssertEqual(objects.firstObject, firstEdits[i]);
    }
}

- (void)testNilPrimitiveReplacementPreservesTargetAndOtherRows {
    NSNumber *target = @7;
    NSNumber *other = @12;
    NSMutableArray *numbers = [@[target, other] mutableCopy];
    SCArrayStore *store = [SCArrayStore storeWithObjectsArray:numbers defaultDefiniton:[SCNumberDefinition definition]];
    XCTAssertNoThrow([store setValue:nil forPropertyName:@"number" inObject:target]);
    XCTAssertEqual(numbers.count, 2u);
    XCTAssertEqual(numbers.firstObject, target);
    XCTAssertEqual(numbers.lastObject, other);
    [store setValue:@8 forPropertyName:@"number" inObject:target];
    XCTAssertEqualObjects(numbers, (@[@8, @12]));
}

@end
