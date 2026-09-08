/*
 *  SCTableViewControllerActions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SCTableViewController;

/// A block invoked with an STV table-view controller.
typedef void (^SCTableViewControllerAction_Block)(SCTableViewController *viewController);
/// A block that approves default button handling.
/// The block receives the controller and returns `YES` to continue or `NO` to
/// ignore the action.
typedef BOOL (^SCTableViewControllerButtonTappedAction_Block)(SCTableViewController *viewController);


#pragma mark - SCTableViewControllerActions
/// Optional, copied callbacks for an STV controller.
/// Hooks default to nil and receive the controller that owns the event. Blocks
/// retain captured objects; use the supplied controller or capture an owner
/// weakly to avoid a retain cycle. Lifecycle callbacks supplement delegate
/// notifications rather than replacing them.
@interface SCTableViewControllerActions : NSObject

/// Runs from the controller's `viewDidLoad`, after calling super.
/// The block receives the controller. Install it before the view loads;
/// assigning it afterward does not replay the event. The similarly named
/// delegate method runs earlier, from `loadView`.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block viewDidLoad;

/// Runs from `viewWillAppear:` for each appearance.
/// The block receives the controller after STV resets its Done/Cancel flags and
/// before the corresponding delegate notification. Appearance is not
/// necessarily a new presentation.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block willAppear;

/// Runs from `viewDidAppear:` for each completed appearance.
/// The block receives the controller before the corresponding delegate
/// notification. On first appearance, `didPresent` follows this hook before the
/// state changes from New to Active.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block didAppear;

/// Runs from `viewWillDisappear:` for each disappearance.
/// The block receives the controller before the corresponding delegate
/// notification. Being covered by another controller is not necessarily
/// dismissal; use the dismissal hooks for STV's dismissal state.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block willDisappear;

/// Runs from `viewDidDisappear:` for each completed disappearance.
/// The block receives the controller before the corresponding delegate
/// notification. It also runs for temporary disappearance, not just dismissal.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block didDisappear;

/// Runs while the controller is New during `viewWillAppear:`.
/// The block receives the controller after `willAppear`. It is a
/// first-appearance hook, not a callback for every push or modal presentation
/// of the same instance.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block willPresent;

/// Runs while the controller is New during `viewDidAppear:`.
/// The block receives the controller after `didAppear`, before STV changes its
/// state to Active. Presenting the same instance again does not reset it to
/// New.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block didPresent;

/// Reports STV-recognized dismissal before `didDismiss`.
/// The block receives the controller. It normally runs during disappearance
/// after an accepted dismissal request or detected navigation pop. For
/// interactive dismissal of an associated popover, it waits until UIKit
/// confirms completion. Losing master-model focus uses delegate focus callbacks
/// instead.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block willDismiss;

/// Reports completed STV-recognized dismissal.
/// The block receives the controller after disappearance or confirmed
/// interactive dismissal of its associated popover. Notifications are
/// deduplicated for that presentation. Losing master-model focus does not call
/// this hook.
@property (nonatomic, copy, nullable) SCTableViewControllerAction_Block didDismiss;

/// Approves a Cancel action before STV handles it.
/// The block receives the controller and returns `YES` to continue or `NO` to
/// ignore the action. Nil permits the action. It applies both to dismissal
/// Cancel and editing-mode Cancel; the latter reloads bound values and requests
/// editing exit without dismissing the controller.
///
/// Example:
/// ```objective-c
/// action = ^BOOL(SCTableViewController *viewController)
/// {
///     // your code here
///
///     return TRUE;    // accept the tap
/// };
/// ```
@property (nonatomic, copy, nullable) SCTableViewControllerButtonTappedAction_Block cancelButtonTapped;

/// Approves a Done action before STV handles it.
/// The block receives the controller and returns `YES` to continue or `NO` to
/// ignore the action. Nil permits the action. It also handles the Edit item's
/// Done state when leaving table editing; that path need not dismiss the
/// controller. A separate dismissal delegate or model-editing hook can still
/// veto subsequent handling.
///
/// Example:
/// ```objective-c
/// action = ^BOOL(SCTableViewController *viewController)
/// {
///     // your code here
///
///     return TRUE;    // accept the tap
/// };
/// ```
@property (nonatomic, copy, nullable) SCTableViewControllerButtonTappedAction_Block doneButtonTapped;

/// Approves entering table editing from the Edit item.
/// The block receives the controller and returns `YES` to continue or `NO` to
/// ignore the action. Nil permits the action. Leaving editing uses
/// `doneButtonTapped` instead. When swipe-to-delete is active, STV requests
/// editing exit directly without invoking either button hook.
///
/// Example:
/// ```objective-c
/// action = ^BOOL(SCTableViewController *viewController)
/// {
///     // your code here
///
///     return TRUE;    // accept the tap
/// };
/// ```
@property (nonatomic, copy, nullable) SCTableViewControllerButtonTappedAction_Block editButtonTapped;

@end

NS_ASSUME_NONNULL_END
