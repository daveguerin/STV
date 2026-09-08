@import XCTest;
@import STV;

@interface STVStandardCellAppearanceTests : XCTestCase
@property (nonatomic, strong) UIWindow *window;
@property (nonatomic, strong) SCTableViewController *controller;
@end

@implementation STVStandardCellAppearanceTests

- (void)tearDown {
    self.window.hidden = YES;
    self.window.rootViewController = nil;
    self.window = nil;
    self.controller = nil;
    [super tearDown];
}

- (NSArray<SCTableViewCell *> *)showStandardCellsWithImage:(UIImage *)image width:(CGFloat)width {
    self.window.hidden = YES;
    self.window = [[UIWindow alloc] initWithFrame:CGRectMake(0, 0, width, 1400)];
    self.controller = [[SCTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    self.window.rootViewController = self.controller;
    [self.window makeKeyAndVisible];
    SCTableViewSection *section = [SCTableViewSection section];
    NSMutableArray *cells = [NSMutableArray array];
    for (Class cls in @[[SCTableViewCell class], [SCLabelCell class], [SCTextFieldCell class], [SCNumericTextFieldCell class], [SCSliderCell class], [SCSegmentedCell class], [SCSwitchCell class], [SCDateCell class]]) {
        SCTableViewCell *cell = [cls cellWithText:@"Title"];
        if ([cell isKindOfClass:[SCSegmentedCell class]]) {
            [[(SCSegmentedCell *)cell segmentedControl] insertSegmentWithTitle:@"One" atIndex:0 animated:NO];
        }

        cell.cellActions.willConfigure = ^(SCTableViewCell *configuredCell, NSIndexPath *indexPath) { configuredCell.imageView.image = image; };
        [section addCell:cell];
        [cells addObject:cell];
    }

    [self.controller.tableViewModel addSection:section];
    [self.controller.tableView reloadData];
    [self.controller.tableView layoutIfNeeded];
    return cells;
}

- (UIImage *)bitmapWithSize:(CGSize)size {
    UIGraphicsImageRenderer *renderer = [[UIGraphicsImageRenderer alloc] initWithSize:size];
    return [renderer imageWithActions:^(UIGraphicsImageRendererContext *context) {
        [UIColor systemBlueColor].setFill;
        UIRectFill(CGRectMake(0, 0, size.width, size.height));
    }];
}

- (void)testImagesKeepIntrinsicAlignmentSizeIncludingLargerCallerImages {
    UIImage *symbol = [UIImage systemImageNamed:@"gearshape.fill" withConfiguration:[UIImageSymbolConfiguration configurationWithPointSize:64 weight:UIImageSymbolWeightRegular]];
    for (UIImage *image in @[symbol, [self bitmapWithSize:CGSizeMake(80, 60)]]) {
        NSArray<SCTableViewCell *> *cells = [self showStandardCellsWithImage:image width:1032];
        for (SCTableViewCell *cell in cells) {
            CGRect alignment = [cell.imageView alignmentRectForFrame:cell.imageView.frame];
            CGSize intrinsic = cell.imageView.intrinsicContentSize;
            XCTAssertEqualWithAccuracy(alignment.size.width, intrinsic.width, 0.5, @"%@", cell.class);
            XCTAssertEqualWithAccuracy(alignment.size.height, intrinsic.height, 0.5, @"%@", cell.class);
            XCTAssertGreaterThan(CGRectGetHeight(cell.bounds), 50, @"%@", cell.class);
        }
    }
}

- (void)testDefaultRowsWithoutImagesKeepMatchingHeightsAndNativeControls {
    NSArray<SCTableViewCell *> *cells = [self showStandardCellsWithImage:nil width:1032];
    CGFloat height = CGRectGetHeight(cells.firstObject.bounds);
    for (SCTableViewCell *cell in cells) {
        XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.bounds), height, 0.5, @"%@", cell.class);
        if ([cell isKindOfClass:[SCControlCell class]]) {
            UIView *control = [(SCControlCell *)cell control];
            CGFloat intrinsicHeight = control.intrinsicContentSize.height;
            CGRect alignment = [control alignmentRectForFrame:control.frame];
            XCTAssertGreaterThanOrEqual(alignment.size.height + 0.5, intrinsicHeight, @"%@", cell.class);
            XCTAssertGreaterThanOrEqual(CGRectGetMinY(alignment), 0);
            XCTAssertLessThanOrEqual(CGRectGetMaxY(alignment), CGRectGetHeight(cell.contentView.bounds));
        }
    }
}

- (void)testLargerFontsGrowRowsAndExplicitHeightStillWins {
    NSArray<SCTableViewCell *> *cells = [self showStandardCellsWithImage:nil width:1032];
    CGFloat normalHeight = CGRectGetHeight(cells.firstObject.bounds);
    for (SCTableViewCell *cell in cells) {
        cell.textLabel.font = [UIFont systemFontOfSize:40];
    }

    [self.controller.tableView reloadData];
    [self.controller.tableView layoutIfNeeded];
    for (SCTableViewCell *cell in cells) {
        XCTAssertGreaterThan(CGRectGetHeight(cell.bounds), normalHeight + 10, @"%@", cell.class);
        cell.height = 96;
    }

    [self.controller.tableView reloadData];
    [self.controller.tableView layoutIfNeeded];
    for (SCTableViewCell *cell in cells) {
        CGSize fitted = [cell.contentView systemLayoutSizeFittingSize:CGSizeMake(CGRectGetWidth(cell.contentView.bounds), 0) withHorizontalFittingPriority:UILayoutPriorityRequired verticalFittingPriority:UILayoutPriorityFittingSizeLevel];
        // UITableView adds its own row/separator rounding to the content fit.
        XCTAssertEqualWithAccuracy(fitted.height, 96, 0.5, @"%@", cell.class);
        XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.bounds), normalHeight - 44 + 96, 0.5, @"%@", cell.class);
    }
}

- (void)testPreferredSymbolConfigurationUpdatesIntrinsicSizing {
    UIImage *image = [UIImage systemImageNamed:@"gearshape.fill"];
    NSArray<SCTableViewCell *> *cells = [self showStandardCellsWithImage:image width:1032];
    CGFloat normalHeight = CGRectGetHeight(cells.firstObject.bounds);
    for (SCTableViewCell *cell in cells) {
        cell.imageView.preferredSymbolConfiguration = [UIImageSymbolConfiguration configurationWithPointSize:64 weight:UIImageSymbolWeightBold];
    }

    [self.controller.tableView reloadData];
    [self.controller.tableView layoutIfNeeded];
    for (SCTableViewCell *cell in cells) {
        CGRect alignment = [cell.imageView alignmentRectForFrame:cell.imageView.frame];
        XCTAssertEqualWithAccuracy(alignment.size.width, cell.imageView.intrinsicContentSize.width, 0.5, @"%@", cell.class);
        XCTAssertEqualWithAccuracy(alignment.size.height, cell.imageView.intrinsicContentSize.height, 0.5, @"%@", cell.class);
        XCTAssertGreaterThan(CGRectGetHeight(cell.bounds), normalHeight + 10, @"%@", cell.class);
    }
}

- (void)testStandardCellsKeepMatchingHeightsAndSymbolSizes {
    UIImage *image = [UIImage systemImageNamed:@"gearshape.fill" withConfiguration:[UIImageSymbolConfiguration configurationWithScale:UIImageSymbolScaleLarge]];
    for (NSNumber *width in @[@390, @1032]) {
        NSArray<SCTableViewCell *> *cells = [self showStandardCellsWithImage:image width:width.doubleValue];
        SCTableViewCell *reference = cells.firstObject;
        CGFloat height = CGRectGetHeight(reference.bounds);
        CGSize imageSize = reference.imageView.bounds.size;
        XCTAssertGreaterThanOrEqual(height, 44);
        for (SCTableViewCell *cell in cells) {
            XCTAssertEqualWithAccuracy(CGRectGetHeight(cell.bounds), height, 0.5, @"%@", cell.class);
            XCTAssertEqualWithAccuracy(cell.imageView.bounds.size.width, imageSize.width, 0.5, @"%@", cell.class);
            XCTAssertEqualWithAccuracy(cell.imageView.bounds.size.height, imageSize.height, 0.5, @"%@", cell.class);
            XCTAssertEqualObjects(cell.imageView.image, image);
            XCTAssertFalse(cell.imageView.hasAmbiguousLayout, @"%@", cell.class);
        }
    }
}

@end
