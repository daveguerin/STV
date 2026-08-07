/*
 *  SCTableViewController.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCGlobals.h"
#import "SCViewControllerTypedefs.h"
#import "SCTableViewControllerActions.h"

@class SCTableViewModel;
@class SCDataDefinition;
@class SCObjectSection;
@class SCArrayOfObjectsSection;


#pragma mark - SCTableViewController
/// This class functions as a means to simplify development with SCTableViewModel.
///
/// SCTableViewController conveniently provides several ready made navigation
/// bar types based on SCNavigationBarType, provided that it is a subview of a navigation controller.
/// SCTableViewController also defines placeholders for a tableView and a tableViewModel that
/// the user can allocate and assign. If a tableViewModel is defined, SCTableViewController also
/// connects its doneButton (if present) to tableViewModel's commitButton automatically.
///
/// In addition, SCTableViewController fully manages memory warnings and makes sure the assigned table view is released once a memory warning occurs and reloaded once the view controller is loaded once more.
///
/// Finally, SCTableViewController provides several useful actions (SCTableViewControllerActions) that notify the delegate object of events like the view appearing or disappearing.


@interface SCTableViewController : UITableViewController <UISplitViewControllerDelegate, UIPopoverControllerDelegate>
{
}

#pragma mark - Configuration

/// Contains a valid SCTableViewModel that is associated with tableView and ready to use. If this model is replaced by a custom one, the class will automatically take care of associating it with tableView.
@property (nonatomic, strong) SCTableViewModel *tableViewModel;

/// The type of the navigation bar.
@property (nonatomic, readwrite) SCNavigationBarType navigationBarType;

/// The navigation bar's Add button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly) UIBarButtonItem *addButton;

/// The editButtonItem of `SCTableViewController`'s superclass.
@property (nonatomic, readonly) UIBarButtonItem *editButton;

/// The navigation bar's Cancel button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly) UIBarButtonItem *cancelButton;

/// Set to TRUE to allow the cancel button to appear when entering editing mode. Default: TRUE.
/// - Note: Only applicable if navigationBarType == SCNavigationBarTypeEditRight.
@property (nonatomic, readwrite) BOOL allowEditingModeCancelButton;

/// The navigation bar's Done button. Only contains a value if the button exists on the bar.
@property (nonatomic, readonly) UIBarButtonItem	*doneButton;

/// When set to YES, the view controller will automatically disable all navigation bar buttons until its view fully appears. This setting is inhereted by all automatically generated detail view controllers. Default: NO.
@property (nonatomic, readwrite) BOOL autoDisableNavigationButtonsUntilViewAppears;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
/// The popover controller associated with the view controller. Set this property when presenting the view controller from a popover. The view controller takes over the popover controller's delegate while associated with it.
@property (nonatomic, strong) UIPopoverController *popoverController;
#pragma clang diagnostic pop

/// The set of view controller action blocks.
@property (nonatomic, readonly) SCTableViewControllerActions *actions;

/// The current state of the view controller.
@property (nonatomic, readonly) SCViewControllerState state;

#pragma mark - Managing Button Events

/// Property is TRUE if the view controller have been dismissed due to the user tapping the
/// Cancel button. This property is useful if you do not with to subclass this view controller.
/// See also SCTableViewControllerDelegate to get notified when the view controller is dismissed.
@property (nonatomic, readonly) BOOL cancelButtonTapped;

/// Property is TRUE if the view controller have been dismissed due to the user tapping the
/// Done button. This property is useful if you do not with to subclass this view controller.
/// See also SCTableViewControllerDelegate to get notified when the view controller is dismissed.
@property (nonatomic, readonly) BOOL doneButtonTapped;

#pragma mark - Interface Builder Related

///  Method returns the Interface Builder data definition with the given name.
///
/// - Parameter ibName: The name of the data definition. This name is typically found in IB's document outline pane.
///
/// - Returns: The data definition.
- (SCDataDefinition *)dataDefinitionWithIBName:(NSString *)ibName;

///  This method should be implemented by SCTableViewController subclasses that wish to provide data source object to an SCObjectSection that has been created in Interface Builder.
///
/// - Note: There is no need to implement this method for sections with data definitions of type SCUserDefaultsDefinition or SCiCloudKeyValueDefinition.
///
/// - Parameter objectSection: The object section that needs the data source object.
/// - Parameter index: The object section index.
///
/// - Returns: An object compatible with objectSection's data definition.
- (NSObject *)objectForSection:(SCObjectSection *)objectSection atIndex:(NSUInteger)index;

///  This method should be implemented by SCTableViewController subclasses that wish to provide data source objects to an SCArrayOfObjectsSection that has been created in Interface Builder. The method is typically called by the STV framework for sections with data definitions of type SCClassDefinition or SCDictionaryDefinition.
///
/// - Note: There is no need to implement this method for sections with data definitions of type SCEntityDefinition or SCWebServiceDefinition.
///
/// - Parameter objectsSection: The objects section that needs the data source objects.
/// - Parameter index: The objects section index.
///
/// - Returns: A mutable array of objects compatible with objectsSection's data definition.
- (NSMutableArray *)objectsForSection:(SCArrayOfObjectsSection *)objectsSection atIndex:(NSUInteger)index;


#pragma mark - Managing Delegate

/// The object that acts as the delegate of `SCTableViewController`. The object must adopt the SCTableViewControllerDelegate protocol.
@property (nonatomic, weak) id delegate;

#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// The data definitions connected to the controller through Interface Builder.
@property (nonatomic, retain, readonly) IBOutletCollection(SCDataDefinition) NSArray *_STV_ibDataDefinitions;

// For internal use only
/// Whether the controller was embedded by Interface Builder.
@property (nonatomic, readonly) BOOL ibEmbedded;

/// Returns TRUE if the view controller currently has been given focus by its master model.
@property (nonatomic, readonly) BOOL hasFocus;

// For internal use only
/// Whether the controller's static content has been added to the model.
@property (nonatomic, readonly) BOOL staticContentAddedToModel;

// For internal use only
/// Invalidates static table content so the model can rebuild it.
- (void)invalidateStaticContent;

/// Method used internally to add all static content added in IB to the table view model.
- (void)addStaticContentToModel;

/// Method should be overridden by subclasses to perform any required initialization.
/// - Warning: Subclasses must call [super performInitialization] from within the method's implementation.
- (void)performInitialization;

/// Method gets called when the Cancel button is tapped. If what you want is to get notified
/// when the Cancel button gets tapped without subclassing `SCTableViewController`, consider
/// using SCTableViewControllerDelegate.
- (void)cancelButtonAction;

/// Method gets called when the Done button is tapped. If what you want is to get notified
/// when the Cancel button gets tapped without subclassing `SCTableViewController`, consider
/// using SCTableViewControllerDelegate.
- (void)doneButtonAction;

/// Method gets called when the Edit button is tapped.
- (void)editButtonAction;

/// Method gets called when the Cancel button is tapped while the table view is in editing mode.
- (void)editingModeCancelButtonAction;

/// Dismisses the view controller with the specified values for cancel and done.
/// - Parameter cancelValue: Whether the cancel button was selected.
/// - Parameter doneValue: Whether the done button was selected.
- (void)dismissWithCancelValue:(BOOL)cancelValue doneValue:(BOOL)doneValue;

/// Called by master model to have the view controller gain focus.
- (void)gainFocus;

/// Called by master model to have the view controller lose focus.
- (void)loseFocus;

@end



#pragma mark - SCTableViewControllerDelegate
/// This protocol should be adopted by objects that want to mediate as a delegate for
/// SCTableViewController. All methods for this protocol are optional.
@protocol SCTableViewControllerDelegate

@optional

/// Notifies the delegate that the view controller's view has been loaded.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerViewDidLoad:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller will appear.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerWillAppear:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller has appeared.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerDidAppear:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller will disappear.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerWillDisappear:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller has disappeared.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerDidDisappear:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller will be presented.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerWillPresent:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller has been presented.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerDidPresent:(SCTableViewController *)tableViewController;

/// Asks the delegate if the view controller should be dismissed.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
/// - Returns: Retrun TRUE to have the view controller dismissed, otherwise return FALSE.
- (BOOL)tableViewControllerShouldDismiss:(SCTableViewController *)tableViewController
                 cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller will be dismissed.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)tableViewControllerWillDismiss:(SCTableViewController *)tableViewController
               cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller has been dismissed.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)tableViewControllerDidDismiss:(SCTableViewController *)tableViewController
                   cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;



// Internal

/// Notifies the delegate that the view controller will gain focus from master model.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerWillGainFocus:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller did gain focus from master model.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
- (void)tableViewControllerDidGainFocus:(SCTableViewController *)tableViewController;

/// Notifies the delegate that the view controller will lose focus to its master model.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)tableViewControllerWillLoseFocus:(SCTableViewController *)tableViewController
                      cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller did lose focus to its master model.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to dismiss the view controller.
/// - Parameter doneTapped: TRUE if Done button has been tapped to dismiss the view controller.
- (void)tableViewControllerDidLoseFocus:(SCTableViewController *)tableViewController
                     cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Notifies the delegate that the view controller did enter editing mode.
/// - Parameter viewController: The view controller associated with the operation.
- (void)tableViewControllerDidEnterEditingMode:(SCTableViewController *)viewController;

/// Notifies the delegate that the view controller did exit editing mode.
/// - Parameter tableViewController: The view controller informing the delegate of the event.
/// - Parameter cancelTapped: TRUE if Cancel button has been tapped to exit editing mode.
/// - Parameter doneTapped: TRUE if Done button has been tapped to exit editing mode.
- (void)tableViewControllerDidExitEditingMode:(SCTableViewController *)tableViewController
                           cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

@end
