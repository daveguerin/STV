/*
 *  SCViewController.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
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
/// This class functions as a means to simplify development with SCTableViewModel.
///
/// SCViewController conveniently provides several ready made navigation
/// bar types based on SCNavigationBarType, provided that it is a subview of a navigation controller.
/// `SCViewController` also defines placeholders for a tableView and a tableViewModel that
/// the user can allocate and assign. If a tableViewModel is defined, SCViewController also
/// connects its doneButton (if present) to tableViewModel's commitButton automatically.
///
/// In addition, SCViewController fully manages memory warnings and makes sure the assigned table view is released once a memory warning occurs and reloaded once the view controller is loaded once more.
///
/// Finally, SCViewController provides several useful actions (SCViewControllerActions) and delegate methods (SCViewControllerDelegate) that notify the delegate object of events like the view appearing or disappearing.
///
/// - Note: You do NOT have to use `SCViewController` in order to be able to use SCTableViewModel, but it's highly recommended that you do so whenever you need a UIViewController.
@interface SCViewController : UIViewController {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    UITableView *_tableView;
}

#pragma mark - Configuration

/// Set this outlet to the table view that will be associated with tableViewModel. Once a valid table view is set, it will automatically be associated with tableViewModel.
/// - Note: If the table view is added programmatically, then the user must also add it to the view controller's view.
@property (nonatomic, strong, nullable) IBOutlet UITableView *tableView;

/// Contains a valid SCTableViewModel that is associated with tableView and ready to use. If this model is replaced by a custom one, the class will automatically take care of associating it with tableView.
@property (nonatomic, strong, nullable) SCTableViewModel *tableViewModel;

/// The type of the navigation bar.
@property (nonatomic, readwrite) SCNavigationBarType navigationBarType;

/// The navigation bar's Add button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly, nullable) UIBarButtonItem *addButton;

/// The editButtonItem of SCViewController's superclass.
@property (nonatomic, readonly, nullable) UIBarButtonItem *editButton;

/// The navigation bar's Cancel button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly, nullable) UIBarButtonItem *cancelButton;

/// Set to TRUE to allow the cancel button to appear when entering editing mode. Default: TRUE.
/// - Note: Only applicable if navigationBarType == SCNavigationBarTypeEditRight.
@property (nonatomic, readwrite) BOOL allowEditingModeCancelButton;

/// The navigation bar's Done button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly, nullable) UIBarButtonItem *doneButton;

/// The UIKit popover presentation associated with this controller, or nil.
/// UIKit owns the presentation; this weak association avoids a retain cycle.
/// Assign after setting the presented navigation controller's modal style to
/// UIModalPresentationPopover. STV becomes its delegate until disassociated.
/// Generated popovers require explicit Done/Cancel; compact widths adapt to a
/// sheet. If interactive dismissal is enabled, its callbacks commit as Done
/// only after UIKit confirms completion (not when a swipe is cancelled).
@property (nonatomic, weak, nullable) UIPopoverPresentationController *popoverController;

/// The set of view controller action blocks.
@property (nonatomic, readonly) SCViewControllerActions *actions;

/// The current state of the view controller.
@property (nonatomic, readonly) SCViewControllerState state;

#pragma mark - Managing Button Events

/// Property is TRUE if the view controller have been dismissed due to the user tapping the
/// Cancel button. This property is useful if you do not with to subclass this view controller.
/// See also SCViewControllerDelegate to get notified when the view controller is dismissed.
@property (nonatomic, readonly) BOOL cancelButtonTapped;

/// Property is TRUE if the view controller have been dismissed due to the user tapping the
/// Done button. This property is useful if you do not with to subclass this view controller.
/// See also SCViewControllerDelegate to get notified when the view controller is dismissed.
@property (nonatomic, readonly) BOOL doneButtonTapped;

#pragma mark - Managing Contained View Controller

///  This property is automatically set to the contained child view controller, allowing the view controller to act as a proxy for this child. When acting as a proxy, both the view controller's tableView and tableViewModel will return the child's instead of its own.
///
/// - Note: If your view controller has more than once contained view controller child, you may manually assign this property to the child of your choice.
@property (nonatomic, weak, nullable) SCTableViewController *containedViewController;

#pragma mark - Managing Delegate

/// The object that acts as the delegate of `SCViewController`. The object must adopt the SCViewControllerDelegate protocol.
@property (nonatomic, weak, nullable) id delegate;

#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Returns TRUE if the view controller currently has been given focus by its master model.
@property (nonatomic, readonly) BOOL hasFocus;

/// Method should be overridden by subclasses to perform any required initialization.
/// - Warning: Subclasses must call [super performInitialization] from within the method's implementation.
- (void)performInitialization;

/// Method gets called when the Cancel button is tapped. If what you want is to get notified
/// when the Cancel button gets tapped without subclassing `SCViewController`, consider
/// using SCViewControllerDelegate.
- (void)cancelButtonAction;

/// Method gets called when the Done button is tapped. If what you want is to get notified
/// when the Cancel button gets tapped without subclassing `SCViewController`, consider
/// using SCViewControllerDelegate.
- (void)doneButtonAction;

/// Method gets called when the Edit button is tapped.
- (void)editButtonAction;

/// Method gets called when the Cancel button is tapped while the table view is in editing mode.
- (void)editingModeCancelButtonAction;

/// Ends focused detail editing, or dismisses an ordinary pushed/modal editor.
/// Focused detail controllers remain hosted after losing model focus.
/// - Parameter cancelValue: Whether the cancel button was selected.
/// - Parameter doneValue: Whether the done button was selected.
- (void)dismissWithCancelValue:(BOOL)cancelValue doneValue:(BOOL)doneValue;

/// Called by master model to have the view controller gain focus.
- (void)gainFocus;

/// Called by master model to have the view controller lose focus.
- (void)loseFocus;

@end


#pragma mark - SCViewControllerDelegate
/// This protocol should be adopted by objects that want to mediate as a delegate for
/// SCViewController. All methods for this protocol are optional.
@protocol SCViewControllerDelegate

@optional

/// Notifies the delegate that the view controller's view has been loaded.
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

/// Notifies the delegate that the view controller will be presented.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerWillPresent:(SCViewController *)viewController;

/// Notifies the delegate that the view controller has been presented.
/// - Parameter viewController: The view controller informing the delegate of the event.
- (void)viewControllerDidPresent:(SCViewController *)viewController;

/// Asks the delegate if the view controller should be dismissed.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
/// - Returns: Retrun TRUE to have the view controller dismissed, otherwise return FALSE.
- (BOOL)viewControllerShouldDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller will be dismissed.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)viewControllerWillDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller has been dismissed.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
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
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)viewControllerWillLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller did lose focus to its master model.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)viewControllerDidLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller did enter editing mode.
/// - Parameter viewController: The view controller associated with the operation.
- (void)viewControllerDidEnterEditingMode:(SCViewController *)viewController;

/// Notifies the delegate that the view controller did exit editing mode.
/// - Parameter viewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to exit editing mode.
/// - Parameter doneTapped: TRUE if Done button has been tapped to exit editing mode.
- (void)viewControllerDidExitEditingMode:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

@end

NS_ASSUME_NONNULL_END
