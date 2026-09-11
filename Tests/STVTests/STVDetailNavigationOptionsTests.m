#import <XCTest/XCTest.h>
#import "SensibleTableView.h"

@interface SCTableViewCell (STVDetailNavigationTesting)
- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType;
- (void)configureDetailViewController:(UIViewController *)controller;
@end

@interface STVDetailNavigationRecord : NSObject
@property (nonatomic, copy) NSString *name;
@end
@implementation STVDetailNavigationRecord
@end

@interface STVDetailNavigationOptionsTests : XCTestCase
@end

@implementation STVDetailNavigationOptionsTests

- (SCObjectCell *)cellRequiringEditing:(BOOL)requiresEditing {
    SCClassDefinition *definition = [SCClassDefinition definitionWithClass:STVDetailNavigationRecord.class propertyNamesString:@"name"];
    definition.requireEditingModeToEditPropertyValues = requiresEditing;
    STVDetailNavigationRecord *record = [STVDetailNavigationRecord new];
    record.name = @"Original";
    return [SCObjectCell cellWithBoundObject:record boundObjectDefinition:definition];
}

- (void)testExplicitNoneDoesNotAddDetailButtons {
    SCObjectCell *cell = [self cellRequiringEditing:NO];
    cell.detailViewControllerOptions.navigationBarType = SCNavigationBarTypeNone;
    XCTAssertEqual([cell defaultDetailViewControllerNavigationBarType], SCNavigationBarTypeNone);
    SCTableViewController *detail = [SCTableViewController new];
    [cell configureDetailViewController:detail];
    XCTAssertEqual(detail.navigationBarType, SCNavigationBarTypeNone);
    XCTAssertNil(detail.navigationItem.leftBarButtonItem);
    XCTAssertNil(detail.navigationItem.rightBarButtonItem);
}

- (void)testAutoRetainsLegacyDoneCancelDefault {
    SCObjectCell *cell = [self cellRequiringEditing:NO];
    XCTAssertEqual(cell.detailViewControllerOptions.navigationBarType, SCNavigationBarTypeAuto);
    XCTAssertEqual([cell defaultDetailViewControllerNavigationBarType], SCNavigationBarTypeDoneRightCancelLeft);
    SCTableViewController *detail = [SCTableViewController new];
    [cell configureDetailViewController:detail];
    XCTAssertEqual(detail.navigationBarType, SCNavigationBarTypeDoneRightCancelLeft);
    XCTAssertNotNil(detail.navigationItem.leftBarButtonItem);
    XCTAssertNotNil(detail.navigationItem.rightBarButtonItem);
}

- (void)testAutoRetainsLegacyEditDefault {
    SCObjectCell *cell = [self cellRequiringEditing:YES];
    XCTAssertEqual(cell.detailViewControllerOptions.navigationBarType, SCNavigationBarTypeAuto);
    XCTAssertEqual([cell defaultDetailViewControllerNavigationBarType], SCNavigationBarTypeEditRight);
    SCTableViewController *detail = [SCTableViewController new];
    [cell configureDetailViewController:detail];
    XCTAssertEqual(detail.navigationBarType, SCNavigationBarTypeEditRight);
    XCTAssertNotNil(detail.navigationItem.rightBarButtonItem);
}

- (void)testExplicitOptionsOverrideBothDefinitionDefaults {
    for (NSNumber *requiresEditing in @[@NO, @YES]) {
        SCObjectCell *cell = [self cellRequiringEditing:requiresEditing.boolValue];
        for (NSNumber *option in @[@(SCNavigationBarTypeNone), @(SCNavigationBarTypeDoneRightCancelLeft), @(SCNavigationBarTypeEditRight)]) {
            SCNavigationBarType requested = option.integerValue;
            cell.detailViewControllerOptions.navigationBarType = requested;
            XCTAssertEqual([cell defaultDetailViewControllerNavigationBarType], requested);
            SCTableViewController *tableDetail = [SCTableViewController new];
            [cell configureDetailViewController:tableDetail];
            XCTAssertEqual(tableDetail.navigationBarType, requested);
            SCViewController *viewDetail = [SCViewController new];
            [cell configureDetailViewController:viewDetail];
            XCTAssertEqual(viewDetail.navigationBarType, requested);
        }
    }
}

- (void)testCustomControllerExplicitChoiceIsPreserved {
    SCObjectCell *cell = [self cellRequiringEditing:NO];
    cell.detailViewControllerOptions.navigationBarType = SCNavigationBarTypeNone;
    SCTableViewController *tableDetail = [SCTableViewController new];
    tableDetail.navigationBarType = SCNavigationBarTypeEditRight;
    [cell configureDetailViewController:tableDetail];
    XCTAssertEqual(tableDetail.navigationBarType, SCNavigationBarTypeEditRight);
    SCViewController *viewDetail = [SCViewController new];
    viewDetail.navigationBarType = SCNavigationBarTypeDoneRightCancelLeft;
    [cell configureDetailViewController:viewDetail];
    XCTAssertEqual(viewDetail.navigationBarType, SCNavigationBarTypeDoneRightCancelLeft);
}

@end
