@import XCTest;
@import STV;
#import <objc/runtime.h>

@interface STVPropertyStorageTests : XCTestCase
@end

@implementation STVPropertyStorageTests

- (void)testLegacyPropertyStorageUsesDefaultUnderscoredNames {
    NSDictionary<NSString *, NSArray<NSString *> *> *properties = @{@"SCPropertyDefinition": @[@"name", @"title", @"required"], @"SCClassDefinition": @[@"cls"], @"SCTableViewCell": @[@"editable", @"enabled", @"ownerSection"], @"SCTableViewSection": @[@"cells", @"headerTitle"], @"SCTableViewModel": @[@"tag", @"activeCell", @"viewController"], @"SCViewController": @[@"tableView", @"tableViewModel"], @"SCTableViewController": @[@"ibDataDefinitions"], @"SCSearchViewController": @[@"searchBar"], @"SCSelectionCell": @[@"selectedItemsIndexes"], @"SCModelActions": @[@"didFetchItemsFromStore"]};
    [properties enumerateKeysAndObjectsUsingBlock:^(NSString *className, NSArray<NSString *> *names, BOOL *stop) {
        Class cls = NSClassFromString(className);
        XCTAssertNotNil(cls);
        for (NSString *name in names) {
            objc_property_t property = class_getProperty(cls, name.UTF8String);
            XCTAssertNotEqual(property, NULL);
            if (!property) {
                continue;
            }
            char *storage = property_copyAttributeValue(property, "V");
            XCTAssertNotEqual(storage, NULL, @"%@.%@ must retain reflection metadata", className, name);
            if (storage) {
                XCTAssertEqualObjects(@(storage), [@"_" stringByAppendingString:name]);
                free(storage);
            }
            XCTAssertEqual(class_getInstanceVariable(cls, name.UTF8String), NULL);
        }
    }];
}

- (void)testCustomAccessorsAndInheritedStorageRemainCoherent {
    SCClassDefinition *definition = [[SCClassDefinition alloc] initWithClass:[NSObject class] autoGeneratePropertyDefinitions:NO];
    XCTAssertEqual(definition.cls, [NSObject class]);

    SCPropertyDefinition *property = [[SCPropertyDefinition alloc] initWithName:@"value"];
    XCTAssertEqualObjects(property.name, @"value");
    property.title = @"Title";
    property.required = YES;
    XCTAssertEqualObjects([property valueForKey:@"title"], @"Title");
    XCTAssertTrue(property.required);

    SCSelectionCell *cell = [[SCSelectionCell alloc] init];
    cell.enabled = NO;
    cell.editable = YES;
    XCTAssertFalse(cell.enabled);
    XCTAssertTrue(cell.editable);
    XCTAssertNotNil(cell.selectedItemsIndexes);
    cell.height = 73;
    XCTAssertEqual(cell.height, UITableViewAutomaticDimension);

    SCTableViewSection *section = [[SCTableViewSection alloc] init];
    section.headerTitle = @"Header";
    XCTAssertEqualObjects(section.headerTitle, @"Header");
    XCTAssertNotNil([section valueForKey:@"cells"]);
}

@end
