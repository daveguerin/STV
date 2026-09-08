/*
 *  SCViewControllerTypedefs.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import <Foundation/Foundation.h>

/// The types of presentation modes used with SCViewController and SCTableViewController.
typedef NS_ENUM(NSInteger, SCPresentationMode) {
    /// Uses an active detail host when supported; otherwise pushes when a navigation
    /// controller is available and presents modally when it is not. New-item
    /// section flows resolve Auto to Modal before presentation.
    SCPresentationModeAuto,
    /// Presents the detail controller modally inside a navigation controller.
    SCPresentationModeModal,
    /// Pushes onto the owner's navigation stack, falling back to modal presentation
    /// when no navigation controller is available.
    SCPresentationModePush,
    /// Uses UIModalPresentationPopover and UIPopoverPresentationController with a
    /// cell, table, or Add-item anchor. UIKit adapts compact-width presentations.
    /// An already-active section detail host may gain focus instead.
    SCPresentationModePopover,
    /// Skips STV's automatic push/modal/popover operation. Application code must
    /// present the detail; array sections offer `customPresentDetailModel`.
    /// Merely selecting this mode does not itself provide a presentation hook.
    SCPresentationModeCustom,
};


/// The types of navigation bars used in SCViewController and SCTableViewController.
typedef NS_ENUM(NSInteger, SCNavigationBarType) {
    /// Have the presenter control determine the best navigation bar type. Only applicable when the view controller is a detail view controller.
    SCNavigationBarTypeAuto,
    /// Adds no STV buttons. UIKit can display Back for a pushed controller; this
    /// does not create a dismiss button for a modal or popover root.
    SCNavigationBarTypeNone,
    /// Navigation bar with an Add button on the left.
    SCNavigationBarTypeAddLeft,
    /// Navigation bar with an Add button on the right.
    SCNavigationBarTypeAddRight,
    /// Navigation bar with an Edit button on the left.
    SCNavigationBarTypeEditLeft,
    /// Navigation bar with an Edit button on the right.
    SCNavigationBarTypeEditRight,
    /// Navigation bar with an Add button on the right and an Edit button on the left.
    SCNavigationBarTypeAddRightEditLeft,
    /// Navigation bar with an Add button on the left and an Edit button on the right.
    SCNavigationBarTypeAddLeftEditRight,
    /// Navigation bar with a Done button on the left.
    SCNavigationBarTypeDoneLeft,
    /// Navigation bar with a Done button on the right.
    SCNavigationBarTypeDoneRight,
    /// Navigation bar with a Cancel button on the left.
    SCNavigationBarTypeCancelLeft,
    /// Navigation bar with a Done button on the left and a Cancel button on the right.
    SCNavigationBarTypeDoneLeftCancelRight,
    /// Navigation bar with a Done button on the right and a Cancel button on the left.
    SCNavigationBarTypeDoneRightCancelLeft,
    /// Navigation bar with both an Add and Edit buttons on the right.
    SCNavigationBarTypeAddEditRight

};


/// The types of SCViewController and SCTableViewController states.
typedef NS_ENUM(NSInteger, SCViewControllerState) {
    /// Initial state, retained through first-appearance callbacks until STV finishes
    /// `viewDidAppear:`. Presenting the same instance again does not reset it.
    SCViewControllerStateNew,
    /// STV has activated the controller during appearance. On later appearances
    /// this is set in `viewWillAppear:`, so it does not guarantee visibility yet.
    SCViewControllerStateActive,
    /// STV has begun a nondismissal disappearance; set in `viewWillDisappear:`.
    SCViewControllerStateInactive,
    /// STV accepted a dismissal request, detected a navigation pop, or received
    /// completed interactive popover dismissal. Explicit requests set this before
    /// UIKit completes the transition; focus loss alone does not set it.
    SCViewControllerStateDismissed

};
