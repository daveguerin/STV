/*
 *  SCCellActions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019-2026 dgApps. All rights reserved.
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
/// A block receiving a cell and possibly nil index path.
/// The owning hook defines the BOOL decision: selection/menu permission or
/// validation. Programmatic active-cell selection notifications ignore it.
typedef BOOL (^SCBOOLReturnCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies a table-view cell's editing style.
typedef UITableViewCellEditingStyle (^SCCellCustomEditingStyleAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies legacy row-editing actions, or nil for default actions.
/// The array is untyped to avoid exposing a deprecated UIKit class in its type.
typedef NSArray *_Nullable (^SCCellEditActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies the leading swipe actions for a table-view cell.
typedef NSArray<UIContextualAction *> *_Nullable (^SCCellLeadingSwipeActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that supplies the trailing swipe actions for a table-view cell.
typedef NSArray<UIContextualAction *> *_Nullable (^SCCellTrailingSwipeActionsAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that calculates a value, or returns nil to use ordinary binding.
typedef NSObject *_Nullable (^SCCellCalculatedValueAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath);
/// A block that transforms a value read from or written to a cell binding.
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
/// A block receiving a cell, possibly nil index path, and possibly nil detail model.
/// Return YES to allow dismissal or NO to veto; see detailModelShouldDismiss
/// for the current section/model fallback limitation.
typedef BOOL (^SCConditionalDetailModelCellAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, SCTableViewModel *_Nullable detailModel);


#pragma mark - SCCellActions
/// Optional, copied callbacks for cell events and customization.
/// Hooks default to nil. Normally STV chooses the first installed hook on the
/// cell, then its section, then its model; it does not broadcast to every level
/// or retry a hook because a result is nil. Individual properties document
/// current fallback exceptions. Blocks retain captured objects: capture owners
/// weakly to avoid cycles.
///
/// - SeeAlso: ``SCSectionActions``, ``SCModelActions``.
@interface SCCellActions : NSObject

#pragma mark - Actions

/// Configures a cell before STV marks it configured.
/// The block receives the cell and its index path, which can be nil when
/// detached. Binding values may already have been loaded. Use this repeatable
/// callback to set configuration and sizing properties, not to install
/// duplicate subviews or constraints.
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

/// Runs after the cell lays out its subviews.
/// The block receives the cell and its possibly nil index path. Observe layout
/// or update drawing here. Install constraints once when constructing custom
/// content rather than adding constraints or assigning subview frames on every
/// layout pass.
@property (nonatomic, copy, nullable) SCCellAction_Block didLayoutSubviews;

/// Customizes a cell when UIKit is about to display it.
/// The block receives the cell and row index path, after section-specific
/// display configuration. Prefer `willConfigure` for sizing and structural
/// changes; use this repeatable callback for display attributes.
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
@property (nonatomic, copy, nullable) SCCellAction_Block willDisplay;

/// Loads visible-cell content when dragging or deceleration finishes.
/// The block receives the visible cell and its index path. It runs
/// synchronously in the scroll delegate callback, not on a background queue,
/// and is not called merely because an initial row appears. Start expensive
/// work asynchronously and verify cell identity before applying results after
/// reuse.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.lazyLoad = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     // Start an asynchronous load; check cell identity before applying it.
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block lazyLoad;

/// Decides whether UIKit may select a row.
/// The block receives the cell and its index path. Return `YES` to allow
/// selection or `NO` to veto UIKit's selection request; without a hook
/// selection is allowed subject to the model's selection locks. Programmatic
/// active-cell changes also invoke this hook but ignore its return value.
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

/// Handles selection of a cell.
/// The block receives the selected cell and its index path. For table
/// selection, installing this hook replaces the default section/cell selection
/// handler, including automatic detail presentation. It can also run when STV
/// programmatically selects an active cell.
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

/// Decides whether UIKit may deselect a row.
/// The block receives the cell being deselected and its saved index path.
/// Return `YES` to allow or `NO` to veto UIKit's request; the default is `YES`.
/// Active-cell replacement also invokes this hook for the old cell but ignores
/// its return value.
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

/// Reports deselection of a cell.
/// The block receives the deselected cell and its index path, not the incoming
/// replacement cell. It can also run when the model replaces or clears its
/// active cell.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didDeselect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     NSLog(@"Cell at indexPath:%@ has been deselected.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block didDeselect;

/// Supplies the editing style for a cell.
/// The block receives the cell and row index path and returns a
/// `UITableViewCellEditingStyle`. Without a hook STV returns
/// `cell.cellEditingStyle`.
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

/// Supplies legacy row-editing buttons.
/// The block receives the cell and row index path and returns an array of
/// `UITableViewRowAction` objects, or nil to leave the result to UIKit. Prefer
/// `leadingSwipeActions` and `trailingSwipeActions` for modern contextual
/// actions; this hook serves the legacy table delegate API.
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


/// Supplies contextual actions revealed from the row's leading edge.
/// The block receives the cell and row index path. Return an array of
/// `UIContextualAction` objects, not a `UISwipeActionsConfiguration`. STV wraps
/// a nonnil array and copies
/// `cell.leadingSwipePerformsFirstActionWithFullSwipe` into the configuration.
/// An empty array supplies no actions; nil returns no custom configuration to
/// UIKit. Each action handler must call its completion handler.
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

/// Supplies contextual actions revealed from the row's trailing edge.
/// The block receives the cell and row index path. Return an array of
/// `UIContextualAction` objects, not a `UISwipeActionsConfiguration`. STV wraps
/// a nonnil array and uses
/// `cell.trailingSwipePerformsFirstActionWithFullSwipe`. An empty array
/// suppresses custom buttons; nil leaves the configuration to UIKit, including
/// any default Delete action and its full-swipe behavior. Call each action's
/// completion handler.
/// - Warning: The current model-level fallback tests `leadingSwipeActions`
/// before invoking `trailingSwipeActions`. Prefer a cell- or section-level
/// hook; a model with only a leading hook can invoke a nil trailing block.
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


/// Reports that a cell or its control became first responder.
/// The block receives the cell and its possibly nil index path. This is a
/// notification, not a request to approve focus.
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

/// Reports that a cell or its control resigned first responder.
/// The block receives the cell and its possibly nil index path. This is a
/// notification, not a request to approve focus loss.
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

/// Handles a row's accessory-button tap.
/// The block receives the cell and index path. Use a tappable UIKit accessory,
/// such as a detail button or detail-disclosure button. Without a hook STV
/// routes the tap through its row-selection handler.
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
@property (nonatomic, copy, nullable) SCCellAction_Block accessoryButtonTapped;

/// Overrides the text field's default Return-key behavior.
/// The block receives the cell and its possibly nil index path. After calling a
/// hook, STV accepts Return without performing its default responder
/// navigation; the block must move or resign focus when desired. This applies
/// to the cell's text-field delegate path, not every keyboard-bearing control.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.returnButtonTapped = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     [cell.ownerTableViewModel moveToNextCellControl:YES];
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellAction_Block returnButtonTapped;

/// Reports a cell-control or detail-model value-change event.
/// The block receives the cell and row index path after the model updates its
/// modification and validation state. This notification does not guarantee that
/// pending control values have been committed to the bound object.
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

/// Validates an enabled cell in place of its built-in validation.
/// The block receives the cell and its possibly nil index path and returns
/// `YES` for valid or `NO` for invalid. Disabled cells return `YES` without
/// calling the block. Without a hook STV uses automatic validation when
/// enabled, otherwise it accepts the value.
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

/// Supplies a calculated result when `boundValue` is read.
/// The block receives the cell and its possibly nil index path. A nonnil result
/// is returned immediately, bypassing ordinary binding and `didLoadBoundValue`.
/// A nil result falls through to ordinary binding; it does not unconditionally
/// force a nil bound value. Do not read `cell.boundValue` inside the block,
/// which would recurse.
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

/// Transforms an ordinary bound-value read.
/// The block receives the cell, its possibly nil index path, and the loaded
/// value, which may be nil. Its result replaces the read value, including when
/// the result is nil. It is skipped for a nonnil calculated result and for `~`
/// placeholder property names. Avoid reading `cell.boundValue` here because
/// that recurses.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.didLoadBoundValue = ^NSObject*(SCTableViewCell *cell, NSIndexPath *indexPath, NSObject *value)
/// {
///     // Make sure all string spaces are trimmed before displaying string.
///
///     NSString *stringValue = (NSString *)value;
///     NSString *trimmedString = [stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
///
///     return trimmedString;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellBoundValueAction_Block didLoadBoundValue;

/// Transforms a value before `setBoundValue:` writes it.
/// The block receives the cell, its possibly nil index path, and the proposed
/// value, which may be nil. Its result is written through the binding;
/// returning nil supplies nil rather than cancelling the write. It runs only
/// with a bound object and a nonempty, non-placeholder property name. This is
/// not a data-store save-completion notification.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.willCommitBoundValue = ^NSObject*(SCTableViewCell *cell, NSIndexPath *indexPath, NSObject *value)
/// {
///     // Make sure all string spaces are trimmed before committing the string.
///
///     NSString *stringValue = (NSString *)value;
///     NSString *trimmedString = [stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
///
///     return trimmedString;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellBoundValueAction_Block willCommitBoundValue;

/// Handles a custom button connected to the cell's button action.
/// The block receives the cell, its possibly nil index path, and the tapped
/// button. The hook does not automatically connect arbitrary buttons added by
/// application code.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.customButtonTapped = ^(SCTableViewCell *cell, NSIndexPath *indexPath, UIButton *button)
/// {
///     NSLog(@"Custom button with tag:%ld has been tapped for cell at indexPath:%@.", (long)button.tag, indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellCustomButtonTappedAction_Block customButtonTapped;


#pragma mark - Copying and Pasting

/// Decides whether the legacy row editing menu is available.
/// The block receives the cell and row index path and returns `YES` to allow
/// the menu or `NO` to suppress it. With no hook STV returns `NO`. This is the
/// legacy table editing-menu path, not a context-menu provider.
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


/// Decides whether a command in the legacy editing menu is available.
/// The block receives the cell, index path, command selector, and possibly nil
/// sender. Return `YES` to enable the command or `NO` to reject it. Without a
/// matching hook the default is `NO`.
/// - Warning: Section- and model-level fallbacks currently test
/// `shouldShowMenu` rather than this property. Configure all three menu hooks
/// together at those levels, or use cell-level hooks, to avoid invoking a nil
/// block.
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


/// Performs a command from the legacy row editing menu.
/// The block receives the cell, index path, command selector, and possibly nil
/// sender. Implement the requested operation; STV provides no default
/// copy/paste implementation.
/// - Warning: Section- and model-level fallbacks currently test
/// `shouldShowMenu` rather than this property. Configure all three menu hooks
/// together at those levels, or use cell-level hooks, to avoid invoking a nil
/// block.
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


/// Supplies a controller for a cell-generated detail screen.
/// The block receives the cell and its possibly nil index path. Return a
/// controller, or nil to create the standard controller; returning nil does not
/// try lower-priority hooks or the model's preassigned detail controller. Use
/// `SCViewController` or `SCTableViewController` for automatic model/delegate
/// integration. Plain UIKit controllers can be presented but do not gain that
/// integration.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.detailViewController = ^UIViewController*(SCTableViewCell *cell, NSIndexPath *indexPath)
/// {
///     MyCustomViewController *customVC = [[MyCustomViewController alloc] initWithNibName:@"MyCustomViewController" bundle:nil];
///
///     return customVC;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellDetailViewControllerAction_Block detailViewController;

/// Supplies the model for a cell-generated detail controller.
/// The block receives the cell and its possibly nil index path. Return a model
/// to assign to an STV detail controller, or nil to keep that controller's
/// existing model. Lower-priority hooks are not retried on a nil result. The
/// controller retains the supplied model and associates it with its table;
/// avoid sharing a model with another live table.
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
@property (nonatomic, copy, nullable) SCCellDetailTableViewModelAction_Block detailTableViewModel;


/// Configures the resolved detail model before the cell builds its detail
/// sections.
/// The block receives the cell, its possibly nil index path, and the detail
/// model, which can be nil for a plain UIKit controller. A reused or custom
/// model may already contain sections; this event does not guarantee a newly
/// allocated, empty model.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelCreated;

/// Customizes the detail model after the cell's build/configuration steps.
/// The block receives the cell, its possibly nil index path, and the possibly
/// nil detail model. A generated controller may not yet have loaded its view,
/// so `detailModel.viewController` may still be nil. Use
/// `detailModelWillPresent` for controller UI customization.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelConfigured;

/// Reports that an STV detail controller is about to present or gain focus.
/// The block receives the cell, its possibly nil index path, and the detail
/// model. Plain UIKit controllers do not automatically forward STV lifecycle
/// events; model arguments can be nil where no STV model exists.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelWillPresent;

/// Reports that an STV detail controller has presented or gained focus.
/// The block receives the cell, its possibly nil index path, and the possibly
/// nil detail model. This depends on STV controller delegate/focus forwarding,
/// not simply on a call to UIKit presentation.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelDidPresent;

/// Approves dismissal or focus loss of a cell's STV detail controller.
/// The block receives the cell, its possibly nil index path, and the possibly
/// nil detail model. A cell-level hook returns `YES` to allow or `NO` to veto;
/// without a hook dismissal is allowed.
/// - Warning: The current section- and model-level fallback calls discard this
/// block's BOOL result. Install the hook on the cell itself when a veto is
/// required.
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
@property (nonatomic, copy, nullable) SCConditionalDetailModelCellAction_Block detailModelShouldDismiss;

/// Reports dismissal or focus loss of an STV detail controller.
/// The block receives the cell, its possibly nil index path, and the possibly
/// nil detail model. For interactive popover dismissal, notification waits
/// until UIKit confirms completion and can therefore arrive after the visual
/// transition.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelWillDismiss;

/// Reports completed dismissal or focus loss of an STV detail controller.
/// The block receives the cell, its possibly nil index path, and the possibly
/// nil detail model. The cell processes non-Cancel detail changes before this
/// callback. It does not itself confirm a successful persistent-store save.
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
@property (nonatomic, copy, nullable) SCDetailModelCellAction_Block detailModelDidDismiss;


#pragma mark - Cell Text Field Related Actions

/// A block that determines whether a text-field edit should be accepted.
typedef BOOL (^SCTextFieldShouldChangeCharactersInRangeAction_Block)(SCTableViewCell *cell, NSIndexPath *_Nullable indexPath, UITextField *textField, NSRange range, NSString *replacementString);

/// Approves a proposed text-field edit.
/// The block receives the cell, its possibly nil index path, the text field,
/// the range in its current text, and the replacement string (empty for
/// deletion). Return `YES` to accept the edit or `NO` to reject it. Without a
/// hook STV accepts the edit.
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
/// Unavailable in STV 7; retained only with the excluded image-picker source.
typedef void(^SCImagePickerDidFinishPickingMediaAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSDictionary *mediaInfo, ALAsset *mediaAsset);
/// A block that supplies a file name for an image-picker cell's selected image.
/// Unavailable in STV 7; retained only with the excluded image-picker source.
typedef NSString*(^SCImagePickerImageNameAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath);
/// A block that saves an image selected by an image-picker cell.
/// Unavailable in STV 7; retained only with the excluded image-picker source.
typedef void(^SCImagePickerSaveImageAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath);
/// A block that loads an image for an image-picker cell.
/// Unavailable in STV 7; retained only with the excluded image-picker source.
typedef UIImage*(^SCImagePickerLoadImageAction_Block)(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath);

/// Implement action to get notified when media is selected by SCImagePickerCell.
/// Unavailable in STV 7; retained only with the excluded image-picker source.
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
/// Unavailable in STV 7; retained only with the excluded image-picker source.
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
/// Unavailable in STV 7; retained only with the excluded image-picker source.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// cellActions.saveImage = ^(SCImagePickerCell *imagePickerCell, NSIndexPath *indexPath, NSString *imagePath)
/// {
///     [UIImageJPEGRepresentation(imagePickerCell.selectedImage, 0.8) writeToFile:imagePath atomically:YES];
/// };
/// ```
@property (nonatomic, copy) SCImagePickerSaveImageAction_Block saveImage;

/// Implement action to provide your own custom code for loading the SCImagePickerCell image from imagePath.
/// Unavailable in STV 7; retained only with the excluded image-picker source.
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

/// Copies the source's nonnil hooks into this actions object.
/// Nil source hooks never clear an existing destination hook, even when
/// overriding. Each assigned block is copied; the source object itself is not
/// retained.
/// - Parameter actions: Source actions, or nil to leave all hooks unchanged.
/// - Parameter override: `YES` replaces existing hooks where the source is
/// nonnil; `NO` fills only currently nil hooks.
- (void)setActionsTo:(SCCellActions *_Nullable)actions overrideExisting:(BOOL)override;

@end

NS_ASSUME_NONNULL_END
