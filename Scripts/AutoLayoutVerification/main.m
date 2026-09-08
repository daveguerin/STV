#import <UIKit/UIKit.h>
#import "SensibleTableView.h"

@interface LayoutFixture : NSObject
@property (nonatomic, copy) NSString *preNote;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, strong) NSNumber *level;
@property (nonatomic, strong) NSNumber *choice;
@property (nonatomic, strong) NSNumber *enabled;
@property (nonatomic, strong) NSDate *date;
@end
@implementation LayoutFixture
@end

@interface LayoutDemoDelegate : UIResponder <UIApplicationDelegate>
@property (nonatomic, strong) UIWindow *window;
@end

@implementation LayoutDemoDelegate
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)options {
    self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    UISplitViewController *split = [[UISplitViewController alloc] initWithStyle:UISplitViewControllerStyleDoubleColumn];
    split.preferredSplitBehavior = UISplitViewControllerSplitBehaviorTile;
    split.preferredDisplayMode = UISplitViewControllerDisplayModeOneBesideSecondary;
    UIViewController *sidebar = [[UIViewController alloc] init];
    sidebar.title = @"App-owned sidebar";
    sidebar.view.backgroundColor = UIColor.secondarySystemBackgroundColor;
    UILabel *label = [[UILabel alloc] init];
    label.text = @"STV does not manage this split view.\n\nThe app owns navigation.";
    label.numberOfLines = 0;
    label.translatesAutoresizingMaskIntoConstraints = NO;
    [sidebar.view addSubview:label];
    [NSLayoutConstraint activateConstraints:@[[label.leadingAnchor constraintEqualToAnchor:sidebar.view.layoutMarginsGuide.leadingAnchor], [label.trailingAnchor constraintEqualToAnchor:sidebar.view.layoutMarginsGuide.trailingAnchor], [label.centerYAnchor constraintEqualToAnchor:sidebar.view.centerYAnchor]]];
    SCTableViewController *editor = [[SCTableViewController alloc] initWithStyle:UITableViewStyleInsetGrouped];
    editor.title = @"STV Auto Layout verification";
    [editor loadViewIfNeeded];
    editor.tableViewModel.inputAccessoryView = [[SCInputAccessoryView alloc] init];
    SCTableViewSection *section = [SCTableViewSection sectionWithHeaderTitle:@"Generated editor fixtures"];
    LayoutFixture *fixture = [LayoutFixture new];
    fixture.name = @"Verification song";
    fixture.level = @0.5;
    fixture.choice = @1;
    fixture.enabled = @YES;
    fixture.date = [NSDate dateWithTimeIntervalSince1970:0];
    SCTextViewCell *note = [[SCTextViewCell alloc] initWithText:@"Pre Note" boundObject:fixture textViewTextPropertyName:@"preNote"];
    note.autoResize = YES;
    fixture.preNote = @"This is test content, not a saved SetList note. The editor should wrap within its cell as the app-owned sidebar changes the available width. Auto Layout determines the row height without STV calculating frames.";
    SCTextFieldCell *field = [[SCTextFieldCell alloc] initWithText:@"Song" placeholder:@"Song name" boundObject:fixture textFieldTextPropertyName:@"name"];
    SCSliderCell *slider = [[SCSliderCell alloc] initWithText:@"Level" boundObject:fixture sliderValuePropertyName:@"level"];
    SCSegmentedCell *segments = [[SCSegmentedCell alloc] initWithText:@"Mode" boundObject:fixture selectedSegmentIndexPropertyName:@"choice" segmentTitlesArray:@[@"One", @"Two"]];
    SCSwitchCell *toggle = [[SCSwitchCell alloc] initWithText:@"Enabled" boundObject:fixture switchOnPropertyName:@"enabled"];
    SCDateCell *date = [[SCDateCell alloc] initWithText:@"Date" boundObject:fixture datePropertyName:@"date"];
    SCExpandCollapseCell *expand = [SCExpandCollapseCell cellWithExpandText:@"Expand" collapseText:@"Collapse" ownerSectionExpanded:NO];
    for (SCTableViewCell *cell in @[note, field, slider, segments, toggle, date])
        [section addCell:cell];
    [editor.tableViewModel addSection:section];
    SCTableViewSection *more = [SCTableViewSection sectionWithHeaderTitle:@"Collapsible fixture"];
    more.expandCollapseCell = expand;
    [more addCell:[SCTableViewCell cellWithText:@"Additional test content"]];
    [editor.tableViewModel addSection:more];
    [editor.tableView reloadData];
    UINavigationController *navigation = [[UINavigationController alloc] initWithRootViewController:editor];
    [split setViewController:[[UINavigationController alloc] initWithRootViewController:sidebar] forColumn:UISplitViewControllerColumnPrimary];
    [split setViewController:navigation forColumn:UISplitViewControllerColumnSecondary];
    self.window.rootViewController = split;
    [self.window makeKeyAndVisible];
    if ([NSProcessInfo.processInfo.arguments containsObject:@"--keyboard"])
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [note.textView becomeFirstResponder]; });
    if ([NSProcessInfo.processInfo.arguments containsObject:@"--date"] || [NSProcessInfo.processInfo.arguments containsObject:@"--embedded-date"])
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            date.displayDatePickerAsInputAccessoryView = [NSProcessInfo.processInfo.arguments containsObject:@"--date"];
            date.datePicker.preferredDatePickerStyle = UIDatePickerStyleWheels;
            [(id<UITableViewDelegate>)editor.tableViewModel tableView:editor.tableView didSelectRowAtIndexPath:[editor.tableViewModel indexPathForCell:date]];
        });
    if ([NSProcessInfo.processInfo.arguments containsObject:@"--no-sidebar"])
        [split hideColumn:UISplitViewControllerColumnPrimary];
    return YES;
}
@end

int main(int argc, char *argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass(LayoutDemoDelegate.class));
    }
}
