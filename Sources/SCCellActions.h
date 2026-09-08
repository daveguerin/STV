/*
 *  SCCellActions.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019 dgApps. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class SCTableViewModel;
@class SCTableViewCell;

/// A block invoked with a table-view cell and its index path.
/// The index path is nil when the cell is detached from its model.
typedef void (^SCCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that returns a Boolean decision for a table-view cell.
typedef BOOL (^SCBOOLReturnCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies a table-view cell's editing style.
typedef UITableViewCellEditingStyle (^SCCellCustomEditingStyleAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies legacy row-editing actions, or nil for default actions.
/// The array is untyped to avoid exposing a deprecated UIKit class in its type.
typedef NSArray *_Nullable (^SCCellEditActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies the leading swipe actions for a table-view cell.
typedef NSArray<UIContextualAction *> *_Nullable (^SCCellLeadingSwipeActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath); // dgApps
/// A block that supplies the trailing swipe actions for a table-view cell.
typedef NSArray<UIContextualAction *> *_Nullable (^SCCellTrailingSwipeActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath); // dgApps
/// A block that calculates a value for a table-view cell, or nil for no value.
typedef NSObject *_Nullable (^SCCellCalculatedValueAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that transforms a value loaded from a table-view cell's binding.
/// Both the supplied value and the transformed result may be nil.
typedef NSObject *_Nullable (^SCCellBoundValueAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, NSObject *_Nullable value);
/// A block invoked when a table-view cell's custom button is tapped.
typedef void (^SCCellCustomButtonTappedAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, UIButton *button);

/// A block that determines whether a table-view cell can perform an action.
typedef BOOL (^SCCellCanPerformAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, SEL action, id _Nullable sender);
/// A block that performs an action for a table-view cell.
typedef void (^SCCellPerformAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, SEL action, id _Nullable sender);


/// A block that supplies a detail controller, or nil to use the default.
typedef UIViewController *_Nullable (^SCCellDetailViewControllerAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies a detail model, or nil to use the controller's model.
typedef SCTableViewModel *_Nullable (^SCCellDetailTableViewModelAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);

/// A block invoked with a table-view cell and its detail model.
/// A custom controller without an STV model supplies a nil detail model.
typedef void (^SCDetailModelCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, SCTableViewModel *_Nullable detailModel);
/// A block that returns a Boolean decision for a table-view cell's detail model.
typedef BOOL (^SCConditionalDetailModelCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, SCTableViewModel *_Nullable detailModel);


#pragma mark - SCCellActions
/// This class hosts a set of cell action blocks. Once an action is set to a desired code block, it will execute the block as soon as the action occurs.
///
/// - SeeAlso: ``SCSectionActions``, ``SCModelActions``.
@interface SCCellActions : NSObject

#pragma mark - Actions

/// Action gets called before the cell is configured.
///
/// This action is typically used to set any attribute that will affect the cell's configuration and layout.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willConfigure = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     cell.selectionStyle = UITableViewCellSelectionStyleNone;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block willConfigure;

/// Action gets called after the cell has laid out all its subviews.
///
/// Use this callback to observe the completed layout or update drawing.
/// Install constraints once when constructing custom content, rather than
/// adding constraints or assigning subview frames in this repeatable callback.
@property (nonatomic, copy, nullable) SCCellAction_Block didLayoutSubviews;

/// Action gets called before the cell is displayed.
///
/// This action is typically used to set any attributes that will affect how the cell is displayed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willDisplay = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     cell.backgroundColor = [UIColor yellowColor];
/// };
/// ```
/// - Note: Changing cell properties that influence its layout (such as the cell's height) cannot be set here, and must be set in the willConfigure action instead.
@property (nonatomic, copy, nullable) SCCellAction_Block willDisplay;

/// Action gets called after the cell has been displayed and the table view has stopped scrolling.
///
/// This action is typically used to load any cell content that is too expensive to load in willDisplay, such as retrieving data from a web service. This guarantees smooth and uninterrupted scrolling of the table view.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.lazyLoad = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     cell.imageView.image = [self retrieveImageForRowAtIndexPath:indexPath];
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block lazyLoad;

/// Action gets called when the cell is about to be selected.
///
/// The block returns `NO` to prevent selection, or `YES` to allow it.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willSelect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ is about to be selected.", indexPath);
///
///     return TRUE;
/// };
/// ```
@property (nonatomic, copy, nullable) SCBOOLReturnCellAction_Block willSelect;

/// Action gets called when the cell has been selected.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ has been selected.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block didSelect;

/// Action gets called when the cell is about to be deselected.
///
/// The block returns `NO` to prevent deselection, or `YES` to allow it.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willDeselect = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ is about to be deselected.", indexPath);
///
///     return TRUE;
/// };
/// ```
@property (nonatomic, copy, nullable) SCBOOLReturnCellAction_Block willDeselect;

/// Action gets called when the cell has been deselected.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didDeselect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ has been selected.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block didDeselect;

/// Action gives you the opportunity to provide your own custom cell editing style.
///
/// The block returns the desired `UITableViewCellEditingStyle` value.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.customEditingStyle = ^UITableViewCellEditingStyle(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     // Prevent swipe-to-delete when not in editing mode.
///
///     if(cell.ownerTableViewModel.tableView.editing)
///         return UITableViewCellEditingStyleDelete;
///     //else
///     return UITableViewCellEditingStyleNone;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellCustomEditingStyleAction_Block customEditingStyle;

/// Action gets called to provide custom edit action buttons that appear when the user swipes the cell horizontally.
///
/// Use this action when you want to provide custom edit actions for your cell. When the user swipes horizontally, the table view moves the cell content aside to reveal your actions. Tapping one of the action buttons executes the handler block stored with the action object.
///
/// The block returns an array of `UITableViewRowAction` objects representing the actions for the cell. Each action creates a button that the user can tap.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.editActions = ^NSArray*(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     UITableViewRowAction *customButton = [UITableViewRowAction rowActionWithStyle:UITableViewRowActionStyleDefault title:@"Button" handler:^(UITableViewRowAction *action, NSIndexPath *indexPath)
///     {
///         NSLog(@"Custom edit action button tapped!");
///
///         [cell.ownerTableViewModel.tableView setEditing:NO]; // collapse the cell back after completing custom edit action
///     }];
///     customButton.backgroundColor = [UIColor greenColor]; //arbitrary color
///
///     UITableViewRowAction *deleteButton = [UITableViewRowAction rowActionWithStyle:UITableViewRowActionStyleDefault title:@"Delete" handler:^(UITableViewRowAction *action, NSIndexPath *indexPath)
///     {
///         if([cell.ownerSection isKindOfClass:[SCArrayOfItemsSection class]])
///             [(SCArrayOfItemsSection *)cell.ownerSection dispatchEventRemoveRowAtIndexPath:indexPath];  // delete the cell
///     }];
///     deleteButton.backgroundColor = [UIColor redColor];
///
///     return @[deleteButton, customButton];
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellEditActionsAction_Block editActions;


/// Action gets called to provide custom swipe action buttons that appear when the user swipes the cell horizontally from it's leading edge. For example, in a left-to-right language environment, they are displayed on the left side of the row when the user swipes from left to right.
///
/// Use this action when you want to provide custom edit actions for your cell. When the user swipes horizontally, the table view moves the cell content aside to reveal your actions. Tapping one of the action buttons executes the handler block stored with the action object.
///
/// - Note: It's an NSArray that is returned here, not a UISwipeActionsConfiguration.
///
/// - Returns: An array of UIContextualAction objects representing the actions for the cell. Each action you provide is used to create a button that the user can tap on the leading edge of the cell.
///
/// Example:
///
/// ```objective-c
///      cellActions.leadingSwipeActions = ^NSArray*(SCTableViewCell *cell, NSIndexPath *indexPath) {
///          UIContextualAction *inAction = [UIContextualAction contextualActionWithStyle:UIContextualActionStyleNormal title:@"In" handler:^(UIContextualAction * _Nonnull action, __kindof UIView * _Nonnull sourceView, void (^ _Nonnull completionHandler)(BOOL)) {
///              // code
///              completionHandler(YES);
///          }];
///
///
///          UIContextualAction *outAction = [UIContextualAction contextualActionWithStyle:UIContextualActionStyleNormal title:@"Out" handler:^(UIContextualAction * _Nonnull action, __kindof UIView * _Nonnull sourceView, void (^ _Nonnull completionHandler)(BOOL)) {
///              // code
///              completionHandler(YES);
///          }];
///
///          inAction.backgroundColor = [UIColor systemGreenColor];
///          outAction.backgroundColor = [UIColor systemYellowColor];
///
///          cell.leadingSwipePerformsFirstActionWithFullSwipe = NO;
///
///          return @[inAction, outAction];
///      };
/// ```

@property (nonatomic, copy, nullable) SCCellLeadingSwipeActionsAction_Block leadingSwipeActions;

/// Action gets called to provide custom swipe action buttons that appear when the user swipes the cell horizontally from it's trailing edge. For example, in a left-to-right language environment, they are displayed on the right side of the row when the user swipes from right to left.
///
/// Use this action when you want to provide custom edit actions for your cell. When the user swipes horizontally, the table view moves the cell content aside to reveal your actions. Tapping one of the action buttons executes the handler block stored with the action object.
///
/// Return nil if you want the cell to display the default set of actions, although trailingSwipePerformsFirstActionWithFullSwipe will then always be the default of YES. You have to return your own Delete UIContextualAction, and set trailingSwipePerformsFirstActionWithFullSwipe to NO, if you want the Delete with no full swipe.
///
/// - Note: It's an NSArray that is returned here, not a UISwipeActionsConfiguration.
///
/// - Returns: An array of UIContextualAction objects representing the actions for the cell. Each action you provide is used to create a button that the user can tap on the trailing edge of the cell.
///
/// Example:
///
/// ```objective-c
///  cellActions.trailingSwipeActions = ^NSArray*(SCTableViewCell *cell, NSIndexPath *indexPath) {
///      UIContextualAction *archiveAction = [UIContextualAction contextualActionWithStyle:UIContextualActionStyleNormal title:@"Archive" handler:^(UIContextualAction * _Nonnull action, __kindof UIView * _Nonnull sourceView, void (^ _Nonnull completionHandler)(BOOL)) {
///          // code
///          completionHandler(YES);
///      }];
///
///      UIContextualAction *deleteAction = [UIContextualAction contextualActionWithStyle:UIContextualActionStyleDestructive title:@"Delete" handler:^(UIContextualAction * _Nonnull action, __kindof UIView * _Nonnull sourceView, void (^ _Nonnull completionHandler)(BOOL)) {
///          // code
///          completionHandler(YES);
///      }];
///
///      archiveAction.backgroundColor = [UIColor systemOrangeColor];
///      deleteAction.backgroundColor = [UIColor systemRedColor];
///
///      cell.trailingSwipePerformsFirstActionWithFullSwipe = NO;
///
///      return @[archiveAction, deleteAction];
///  };
/// ```

@property (nonatomic, copy, nullable) SCCellTrailingSwipeActionsAction_Block trailingSwipeActions;


/// Action gets called when the cell (or more typically one of the cell's controls) becomes the first responder.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didBecomeFirstResponder = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ has become the first responder.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block didBecomeFirstResponder;

/// Action gets called when the cell (or more typically one of the cell's controls) resigns the first responder.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didResignFirstResponder = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ has resigned the first responder.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block didResignFirstResponder;

/// Action gets called when the cell's accessory button has been tapped.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.accessoryButtonTapped = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ accessory button has been tapped.", indexPath);
/// };
/// ```
/// - Note: For this action to get called, you must first have the cell's accessory button appear by setting its accessoryType property to UITableViewCellAccessoryDetailDisclosureButton.
@property (nonatomic, copy, nullable) SCCellAction_Block accessoryButtonTapped;

/// Action gets called when the cell keyboard's return button has been tapped.
///
/// This action is typically used to override STV's behavior when the return button is tapped, and define a custom one.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.returnButtonTapped = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     [self doMyCustomAction];
///     [cell.ownerTableViewModel moveToNextCellControl:YES];
/// };
/// ```
/// - Note: Action is only applicable to cells with controls that display a keyboard.
@property (nonatomic, copy, nullable) SCCellAction_Block returnButtonTapped;

/// Action gets called when the cell's bound property value has changed via a cell control or a detail model.
///
/// This action is typically used to provide a custom behavior when the cell's value changes.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.valueChanged = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ value has changed to: %@.", indexPath, cell.boundValue);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block valueChanged;

/// Action gets called when the cell's value needs to be validated.
///
/// This action is typically used to provide a custom cell value validation.
///
/// The block returns `YES` if the current cell value is valid, or `NO` otherwise.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.valueIsValid = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     BOOL valid = NO;
///
///     if([cell isKindOfClass:[SCTextFieldCell class]])
///     {
///         SCTextFieldCell *textFieldCell = (SCTextFieldCell *)cell;
///
///         // Make sure the password field is at least 8 characters long
///         if([textFieldCell.textField.text length] >= 8)
///             valid = YES;
///     }
///
///     return valid;
/// };
/// ```
@property (nonatomic, copy, nullable) SCBOOLReturnCellAction_Block valueIsValid;

/// Action is used to provide a value for calculated cells.
///
/// Cells implementing this action are typically calculated cells that do not have their own values in the data store and depend on this action to provide a value. The return value is typically a mathematical operation on serveral other boundObject properties. For a more elaborate example on calculated cells, please refer to the CalculatedCellsApp bundled sample application.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.calculatedValue = ^NSObject*(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSNumber *speed = [cell.boundObject valueForKey:@"speed"];
///     NSNumber *distance = [cell.boundObject valueForKey:@"distance"];
///     CGFloat time = 0;
///     if([speed floatValue])
///         time = [distance floatValue]/[speed floatValue];
///
///     return [NSNumber numberWithFloat:time];
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellCalculatedValueAction_Block calculatedValue;

/// Action gets called whenever a cell's bound value has been loaded.
///
/// This action is typically used to do any customization to the loaded bound value.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didLoadBoundValue = ^NSObject*(SCTableViewCell *cell, NSIndexPath *indexPath, NSObject *value)
/// {
///     // Make sure all string spaces are trimmed before displaying string.
///
///     NSString *stringValue = (NSStirng *)value;
///     NSString *trimmedString = [stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
///
///     return trimmedString;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellBoundValueAction_Block didLoadBoundValue;

/// Action gets called before a cell's bound value is committed to its bound object.
///
/// This action is typically used to do any customization to the bound value before being committed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willCommitBoundValue = ^NSObject*(SCTableViewCell *cell, NSIndexPath *indexPath, NSObject *value)
/// {
///     // Make sure all string spaces are trimmed before committing the string.
///
///     NSString *stringValue = (NSStirng *)value;
///     NSString *trimmedString = [stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
///
///     return trimmedString;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellBoundValueAction_Block willCommitBoundValue;

/// Action gets called whenever a cell's user defined custom button is tapped.
///
/// This action is typically used to easily provide a custom behavior for the cell's custom button(s).
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.customButtonTapped = ^(SCTableViewCell *cell, NSIndexPath *indexPath, UIButton *button)
/// {
///     NSLog(@"Custom button with tag:%i has been tapped for cell at indexPath:%@.", button.tag, indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellCustomButtonTappedAction_Block customButtonTapped;


#pragma mark - Copying and Pasting

/// Action gets called to ask if the editing menu should be shown for the cell.
///
/// The block returns `YES` to show the editing menu, or `NO` otherwise.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.shouldShowMenu = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     return TRUE;
/// };
/// ```
@property (nonatomic, copy, nullable) SCBOOLReturnCellAction_Block shouldShowMenu;


/// Action gets called to ask if the editing menu should omit the Copy or Paste command for the cell.
///
/// The block receives `action`, a selector identifying a method in the `UIResponderStandardEditActions` informal protocol, and `sender`, the object that initiated the action.
/// The block returns `YES` if the command should appear in the editing menu, or `NO` otherwise.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.canPerformAction = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath, SEL action, id sender)
/// {
///     return (action == @selector(copy:));  // only allow Copy
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellCanPerformAction_Block canPerformAction;


/// Action gets called to perform a copy or paste operation on the content of the cell.
///
/// The block receives `action`, a selector identifying a method in the `UIResponderStandardEditActions` informal protocol, and `sender`, the object that initiated the action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.performAction = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SEL action, id sender)
/// {
///     // perform operation here
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellPerformAction_Block performAction;


#pragma mark - Detail Model Actions


/// Action gets called to give you the chance to return a custom detail view controller for the cell.
///
/// This action is typically used to provide your own custom detail view controller, instead of the one automatically generated by the cell.
///
/// The block returns the custom view controller, which must be an ``SCViewController`` or ``SCTableViewController``. Returning `nil` ignores the action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailViewController = ^UIViewController*(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     MyCustomViewController *customVC = [[MyCustomViewController alloc] initWithNib:@"MyCustomViewController" bundle:nil];
///
///     return customVC;
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell and SCArrayOfObjectsCell.
@property (nonatomic, copy, nullable) SCCellDetailViewControllerAction_Block detailViewController;

/// Action gets called to give you the chance to return a custom detail model for the cell's detail view controller.
///
/// This action is typically used to provide your own custom detail model, instead of the one automatically generated by the cell. This might be needed in cases where the cell generates a detail SCArrayOfObjectsSection for example, and you need an SCArrayOfObjectsModel instead (to automatically generate sections for instance).
///
/// - Note: It is much more common to use the detailViewController action instead, assigning the custom model in the custom view controller's viewDidLoad method. This also gives you the chance to add a search bar (for example, to make use of SCArrayOfObjectsModel automatic searching functionality), or any other controls.
///
/// The block returns the custom detail model. It must not already be associated with a table view because the framework handles that association. Returning `nil` ignores the action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailTableViewModel = ^SCTableViewModel*(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     SCTableViewModel *detailModel = nil;
///     if([cell isKindOfClass:[SCArrayOfObjectsCell class]])
///     {
///         detailModel = [SCArrayOfObjectsModel modelWithTableView:nil];
///     }
///
///     return detailModel;
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell and SCArrayOfObjectsCell.
@property (nonatomic, copy, nullable) SCCellDetailTableViewModelAction_Block detailTableViewModel;


/// Action gets called right after the cell's detail model is created, before configuration is set or any sections are added.
///
/// This action is typically used to initially configure the detail model (like set a custom tag for example). Most of the model's settings can also be configure in the detailModelConfigured action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelCreated = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     detailModel.tag = 100;
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
///
/// - SeeAlso: ``detailModelConfigured``
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelCreated;

/// Action gets called after the cell's detail model is fully configured, including the addition of all automatically generated sections.
///
/// This action is typically used to add additional custom sections, or modify the already existing automatically generated ones.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelConfigured = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     SCTableViewSection *customSection = [SCTableViewSection section];
///     SCCustomCell *customCell = [SCCustomCell cellWithText:@"Custom Cell"];
///     [customSection addCell:customCell];
///
///     [detailModel addSection:customSection];
/// };
/// ```
/// - Note: In general, it is easier (and more recommended) to add your custom sections and cells using the data definitions, instead of using this action to do so. For more information, please refer to SCDataDefinition and SCCustomPropertyDefinition.
///
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelConfigured;

/// Action gets called when the cell's detail model is about to be presented in its own view controller.
///
/// This action is typically used to further customize the detail model's view controller.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelWillPresent = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     detailModel.viewController.title = @"My custom title";
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelWillPresent;

/// Action gets called when the cell's detail model has been presented in its own view controller.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelDidPresent = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     NSLog(@"Detail model has been presented.");
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelDidPresent;

/// Action gets called to give you a chance to decide if the detail model should be dismissed. Return YES to allow the detail model to be dismissed, otherwise return NO.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelShouldDismiss = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     return YES;  // allow detail model to be dismissed
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCConditionalDetailModelCellAction_Block detailModelShouldDismiss;

/// Action gets called when the cell's detail model's view controller is about to be dismissed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelWillDismiss = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     NSLog(@"Detail model will be dismissed.");
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelWillDismiss;

/// Action gets called when the cell's detail model's view controller has been dismissed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailModelDidDismiss = ^(SCTableViewCell *cell, NSIndexPath *indexPath, SCTableViewModel *detailModel)
/// {
///     NSLog(@"Detail model has been dismissed.");
/// };
/// ```
/// - Note: This action is only applicable to cells that generate detail views, such as SCSelectionCell.
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelDidDismiss;


#pragma mark - Cell Text Field Related Actions

/// A block that determines whether a text-field edit should be accepted.
typedef BOOL (^SCTextFieldShouldChangeCharactersInRangeAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, UITextField *textField, NSRange range, NSString *replacementString);

/// Implement action to control whether the specified text field's text should change.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.shouldChangeCharactersInRange = ^BOOL(SCTableViewCell *cell, NSIndexPath *indexPath, UITextField *textField, NSRange range, NSString *replacementString)
/// {
///     NSLog(@"shouldChangeCharactersInRange called with replacement string: %@", replacementString);
///
///     return YES;
/// };
/// ```
@property (nonatomic, copy, nullable) SCTextFieldShouldChangeCharactersInRangeAction_Block shouldChangeCharactersInRange;

#pragma mark - SCImagePickerCell Specific Actions
/*
These action types remain in source beside the excluded cell for a future
PhotosUI redesign, but are not part of STV 7.
*/
#if 0
/// A block invoked after an image picker finishes selecting media.
typedef void(^SCImagePickerDidFinishPickingMediaAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSDictionary *mediaInfo, ALAsset *mediaAsset);
/// A block that supplies a file name for an image-picker cell's selected image.
typedef NSString*(^SCImagePickerImageNameAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath);
/// A block that saves an image selected by an image-picker cell.
typedef void(^SCImagePickerSaveImageAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath);
/// A block that loads an image for an image-picker cell.
typedef UIImage*(^SCImagePickerLoadImageAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath);

/// Implement action to get notified when media is selected by SCImagePickerCell.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didFinishPickingMedia = ^(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSDictionary *mediaInfo, ALAsset *mediaAsset)
/// {
///     // Determine selected media date
///     NSDate *date = [mediaAsset valueForProperty:ALAssetPropertyDate];
///     NSLog(@"Selected media date: %@", date);
/// };
/// ```
@property (nonatomic, copy) SCImagePickerDidFinishPickingMediaAction_Block didFinishPickingMedia;

/// Implement action to provide your own custom image name of the SCImagePickerCell image.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.imageName = ^NSString*(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath)
/// {
///     return @"My Custom Image Name";
/// };
/// ```
@property (nonatomic, copy) SCImagePickerImageNameAction_Block imageName;

/// Implement action to provide your own custom code for saving the SCImagePickerCell image to imagePath.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.saveImage = ^(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath)
/// {
///     [UIImageJPEGRepresentation(imagePickerCell.selectedImage, 80) writeToFile:imagePath atomically:YES];
/// };
/// ```
@property (nonatomic, copy) SCImagePickerSaveImageAction_Block saveImage;

/// Implement action to provide your own custom code for loading the SCImagePickerCell image from imagePath.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.loadImage = ^UIImage*(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath)
/// {
///     return [UIImage imageWithContentsOfFile:imagePath];
/// };
/// ```
@property (nonatomic, copy) SCImagePickerLoadImageAction_Block loadImage;
#endif


#pragma mark - Miscellaneous

/// Method assigns all the actions of another `SCCellActions` class to the current one.
///
/// - Parameter actions: The source actions, or nil to leave existing hooks alone.
/// - Parameter override: Set to TRUE to override any existing actions, otherwise set to FALSE.
- (void)setActionsTo:(SCCellActions *_Nullable)actions overrideExisting:(BOOL)override;

@end

NS_ASSUME_NONNULL_END
