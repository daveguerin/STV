/*
 *  SCViewController.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCGlobals.h"
#import "SCViewControllerTypedefs.h"
#import "SCViewControllerActions.h"

NS_ASSUME_NONNULL_BEGIN

@class SCTableViewModel;
@class SCTableViewController;


#pragma mark - SCViewController
/// A UIKit controller that hosts an STV table model and detail-editing flow.
/// Initialization creates a table model and optional action collection.
/// Navigation-bar presets provide Add, Edit, Done, and Cancel items; Done is
/// connected to model validation during appearance. Lifecycle actions and
/// optional delegate methods support customization without subclassing. The
/// controller does not implement automatic table teardown/recreation on memory
/// warnings.
/// Assign a table view or embed an `SCTableViewController`. The table setter
/// attaches an unhosted table to the root view but does not create its layout
/// constraints; the caller supplies layout.
@interface SCViewController : UIViewController {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    UITableView *_tableView;
}

#pragma mark - Configuration

/// The strongly held table used by this controller's own model; initially nil.
/// Assigning a table connects it to the model and, if it has no superview, adds
/// it to the root view. Supply constraints or other layout yourself. When a
/// contained controller is selected, the getter returns its table, but the
/// setter still updates this controller's own storage.
/// - Warning: Passing nil is not a supported removal operation in the current
/// setter: its automatic attachment path attempts to add the nil view.
@property (nonatomic, strong, nullable) IBOutlet UITableView *tableView;

/// The strongly held model created during initialization, replaceable by a
/// custom model or nil.
/// Replacement associates the model with this controller's own table and
/// editing/Add items, and preserves an existing detail controller when the
/// replacement has none. When a contained controller is selected, the getter
/// returns the child's model; the setter still updates the parent's own
/// storage.
@property (nonatomic, strong, nullable) SCTableViewModel *tableViewModel;

/// The navigation-item button arrangement; initially `SCNavigationBarTypeAuto`.
/// Setting this property creates the relevant STV buttons. A navigation
/// controller is required to display a navigation bar. Existing
/// application-owned bar items may prevent insertion of a generated button.
@property (nonatomic, readwrite) SCNavigationBarType navigationBarType;

/// The generated Add item for the selected navigation-bar type, or nil.
/// A nonnil item may be displaced by an existing application-owned navigation
/// item; it is not proof that Add is visible. An items model or section
/// supplies its target/action when connected.
@property (nonatomic, readonly, nullable) UIBarButtonItem *addButton;

/// The inherited `editButtonItem`, configured by STV's navigation setup.
/// Its action enters editing and uses its Done state to leave editing.
@property (nonatomic, readonly, nullable) UIBarButtonItem *editButton;

/// The generated Cancel item, or nil when the bar type does not create one.
/// `SCNavigationBarTypeEditRight` creates an editing-mode Cancel item even
/// while it is not visible; a nonnil value does not guarantee placement on the
/// bar.
@property (nonatomic, readonly, nullable) UIBarButtonItem *cancelButton;

/// Whether EditRight may show Cancel while entering table editing.
/// Defaults to `YES`. The controller also suppresses this button when its first
/// section is an `SCArrayOfItemsSection`. Other navigation-bar types do not use
/// this option.
@property (nonatomic, readwrite) BOOL allowEditingModeCancelButton;

/// The generated standalone Done item, or nil.
/// This is distinct from `editButton` while that item displays Done. STV
/// associates the standalone item with model validation during appearance.
/// Existing application-owned navigation items can prevent its display.
@property (nonatomic, readonly, nullable) UIBarButtonItem *doneButton;

/// The weak UIKit popover presentation association, or nil.
/// Assign after configuring the presented controller's modal style as
/// `UIModalPresentationPopover`. STV replaces the presentation delegate with
/// itself and clears that delegate on disassociation if it still owns it. UIKit
/// retains the presentation, not this property.
/// Generated popovers disable interactive dismissal with `modalInPresentation`;
/// compact-width presentation adapts through UIKit. If interaction is
/// explicitly enabled, STV treats confirmed interactive dismissal as Done and
/// delivers dismissal callbacks only after completion, not after a cancelled
/// gesture.
@property (nonatomic, weak, nullable) UIPopoverPresentationController *popoverController;

/// The controller-owned action collection, created during initialization.
/// Its hooks initially are nil. Assigned blocks are copied and retain captures;
/// capture the controller weakly or use the callback's controller argument to
/// avoid cycles.
@property (nonatomic, readonly) SCViewControllerActions *actions;

/// STV's appearance/dismissal bookkeeping state, initially New.
/// It is not a universal UIKit visibility test: STV updates it during
/// appearance, its own dismissal requests, detected navigation pops, and
/// completed associated-popover dismissal.
@property (nonatomic, readonly) SCViewControllerState state;

#pragma mark - Managing Button Events

/// The Cancel flag recorded for the current dismissal or focus-loss request.
/// Initially `NO`; reset on appearance and after a dismissal veto. It can
/// become `YES` before physical dismissal completes and remains available
/// during focus-loss notifications. Editing-mode Cancel does not set it.
@property (nonatomic, readonly) BOOL cancelButtonTapped;

/// The Done flag recorded for the current dismissal or focus-loss request.
/// Initially `NO`; reset on appearance and after a dismissal veto. Confirmed
/// interactive popover dismissal also sets it, without a physical button tap.
/// It is not proof that persistence or dismissal completed; the Edit item's
/// Done state does not set it.
@property (nonatomic, readonly) BOOL doneButtonTapped;

#pragma mark - Managing Contained View Controller

/// A weak reference to the child whose table and model this controller exposes.
/// The first `SCTableViewController` added to an `SCViewController` selects
/// itself when this property is nil, and clears the association on removal. Set
/// it manually to select another child. Only the table/model getters proxy to
/// the child; setting this property neither adds UIKit containment nor forwards
/// those setters.
@property (nonatomic, weak, nullable) SCTableViewController *containedViewController;

#pragma mark - Managing Delegate

/// The weak, optional controller delegate; initially nil.
/// Retain it elsewhere and adopt the corresponding controller delegate
/// protocol. STV checks protocol conformance and selector support before
/// sending optional messages. Generated detail setup assigns its cell or
/// section as delegate, and losing model focus clears the delegate.
@property (nonatomic, weak, nullable) id delegate;

#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Whether a master model has explicitly focused this controller.
/// Initially `NO`. This state is independent of ordinary UIKit visibility;
/// `gainFocus` and `loseFocus` connect the active or empty model to the table.
@property (nonatomic, readonly) BOOL hasFocus;

/// Initializes STV model, navigation, action, and state storage.
/// Called by the controller initializers. Override to add subclass setup, but
/// call `[super performInitialization]`; calling it again later replaces
/// existing STV state.
- (void)performInitialization;

/// Handles dismissal Cancel after consulting `actions.cancelButtonTapped`.
/// A `NO` hook result ignores the action; otherwise STV requests dismissal or
/// focus loss with Cancel set. The dismissal delegate can still veto the
/// request.
- (void)cancelButtonAction;

/// Handles standalone Done after consulting `actions.doneButtonTapped`.
/// A `NO` hook result ignores the action; otherwise STV requests dismissal or
/// focus loss with Done set. The dismissal delegate can still veto the request.
- (void)doneButtonAction;

/// Toggles editing through the model after the appropriate button hook.
/// Entry uses `actions.editButtonTapped`; exit uses `actions.doneButtonTapped`.
/// An active swipe-to-delete requests editing exit directly. Navigation items
/// may be updated before the model evaluates its own editing veto.
- (void)editButtonAction;

/// Handles editing-mode Cancel without dismissing the controller.
/// After the Cancel hook approves, restores navigation items, reloads section
/// bound values, requests editing exit, and sends the editing-exit delegate
/// event. It does not roll back changes already committed to the store.
- (void)editingModeCancelButtonAction;

/// Requests dismissal or ends focused detail editing after delegate approval.
/// A delegate veto clears the recorded flags and leaves the controller in
/// place. A focused controller loses model focus but remains hosted. Otherwise
/// STV records Dismissed and requests popover/modal dismissal or a navigation
/// pop; that state change is not confirmation of UIKit transition completion.
/// This method does not call the button action hooks.
/// - Parameter cancelValue: Cancel semantics to record for the request.
/// - Parameter doneValue: Done semantics to record for the request.
- (void)dismissWithCancelValue:(BOOL)cancelValue doneValue:(BOOL)doneValue;

/// Connects the controller's model as table data source/delegate and reloads
/// bound values and rows.
/// Sets `hasFocus` between the optional will/did-gain-focus notifications. Used
/// for an existing master/detail host rather than presenting it again.
- (void)gainFocus;

/// Ends model focus without removing the controller from its host.
/// Sends focus-loss delegate events, clears `hasFocus` and title, resets
/// non-None navigation to Auto, connects an empty model, reloads the table,
/// then clears the delegate. The recorded Done/Cancel flags are passed to the
/// focus-loss events.
- (void)loseFocus;

@end


#pragma mark - SCViewControllerDelegate
/// This protocol should be adopted by objects that want to mediate as a delegate for
/// SCViewController. All methods for this protocol are optional.
@protocol SCViewControllerDelegate

@optional

/// Called at the end of STV's `loadView`, before its `viewDidLoad` action.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerViewDidLoad:(SCViewController *)viewController;

/// Notifies the delegate that the view controller will appear.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerWillAppear:(SCViewController *)viewController;

/// Notifies the delegate that the view controller has appeared.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerDidAppear:(SCViewController *)viewController;

/// Notifies the delegate that the view controller will disappear.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerWillDisappear:(SCViewController *)viewController;

/// Notifies the delegate that the view controller has disappeared.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerDidDisappear:(SCViewController *)viewController;

/// Reports first appearance while STV state is New, after the will-appear event.
/// - Warning: The current implementation checks SCTableViewControllerDelegate
///   conformance for this selector, rather than SCViewControllerDelegate.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerWillPresent:(SCViewController *)viewController;

/// Reports first completed appearance while STV state is still New.
/// - Warning: The current implementation checks SCTableViewControllerDelegate
///   conformance for this selector, rather than SCViewControllerDelegate.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerDidPresent:(SCViewController *)viewController;

/// Approves an explicit dismissal/focus-loss request or interactive popover dismissal.
/// Native navigation Back is not routed through this veto.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: The request's Cancel flag; may be supplied programmatically.
/// - Parameter doneTapped: The request's Done flag; also YES for interactive popover dismissal.
/// - Returns: `YES` to allow dismissal or focus loss; `NO` to veto it. An unimplemented method permits the request.
- (BOOL)viewControllerShouldDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Reports STV-recognized dismissal; interactive popovers defer this until completion.
/// Focus loss uses the separate focus delegate events instead.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: The request's Cancel flag; may be supplied programmatically.
/// - Parameter doneTapped: The request's Done flag; also YES for interactive popover dismissal.
- (void)viewControllerWillDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Reports completed STV-recognized dismissal, not temporary disappearance.
/// Focus loss uses the separate focus delegate events instead.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: The request's Cancel flag; may be supplied programmatically.
/// - Parameter doneTapped: The request's Done flag; also YES for interactive popover dismissal.
- (void)viewControllerDidDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;


// Internal

/// Notifies the delegate that the view controller will gain focus from master model.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerWillGainFocus:(SCViewController *)viewController;

/// Notifies the delegate that the view controller did gain focus from master model.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerDidGainFocus:(SCViewController *)viewController;

/// Notifies the delegate that the view controller will lose focus to its master model.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: The request's Cancel flag; may be supplied programmatically.
/// - Parameter doneTapped: The request's Done flag; also YES for interactive popover dismissal.
- (void)viewControllerWillLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller did lose focus to its master model.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: The request's Cancel flag; may be supplied programmatically.
/// - Parameter doneTapped: The request's Done flag; also YES for interactive popover dismissal.
- (void)viewControllerDidLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Reserved editing-entry notification; SCViewController currently does not send it.
/// Use the model's `didBeginEditing` hook to observe accepted editing entry.
/// - Parameter viewController: The view controller associated with the operation.
- (void)viewControllerDidEnterEditingMode:(SCViewController *)viewController;

/// Reports a controller-handled editing exit.
/// EditRight Done sends this before requesting model exit; editing Cancel sends
/// it afterward. The event can still occur when the model vetoes that request.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to exit editing mode.
/// - Parameter doneTapped: TRUE if Done button has been tapped to exit editing mode.
- (void)viewControllerDidExitEditingMode:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

@end

NS_ASSUME_NONNULL_END
