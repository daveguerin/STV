@import XCTest;
@import STV;

@interface SCTableViewModel (STVTests)
- (void)setActiveCell:(SCTableViewCell *)cell;
@end

@interface SCTableViewModelSafetyTests : XCTestCase
@end

@implementation SCTableViewModelSafetyTests

- (void)testChangingActiveCellInvokesDeselectCallbackOnPreviousCell {
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0) style:UITableViewStylePlain];
    SCTableViewModel *model = [SCTableViewModel modelWithTableView:tableView];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:nil];
    SCTableViewCell *firstCell = [[SCTableViewCell alloc] initWithText:@"First"];
    SCTableViewCell *secondCell = [[SCTableViewCell alloc] initWithText:@"Second"];
    [section addCell:firstCell];
    [section addCell:secondCell];
    [model addSection:section];

    __block SCTableViewCell *callbackCell = nil;
    firstCell.cellActions.willDeselect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath) {
        callbackCell = cell;
        return YES;
    };

    [model setActiveCell:firstCell];
    XCTAssertNoThrow([model setActiveCell:secondCell]);
    XCTAssertEqual(callbackCell, firstCell);
}

- (void)testCellCloneUsesAnInstanceScopedClassMapping {
    UITableViewCell *sourceCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:nil];
    sourceCell.textLabel.text = @"Source";
    NSString *sourceClassName = NSStringFromClass(sourceCell.class);
    Class globalMappingBefore = [NSKeyedUnarchiver classForClassName:sourceClassName];

    SCCustomCell *clonedCell = [SCCustomCell cellWithCell:sourceCell];

    XCTAssertTrue([clonedCell isKindOfClass:[SCCustomCell class]]);
    XCTAssertEqualObjects(clonedCell.textLabel.text, @"Source");
    XCTAssertEqual([NSKeyedUnarchiver classForClassName:sourceClassName], globalMappingBefore);
}

@end
