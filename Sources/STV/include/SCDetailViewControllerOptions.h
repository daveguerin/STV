/*
 *  SCDetailViewControllerOptions.h
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>


#import "SCViewControllerTypedefs.h"



/****************************************************************************************/
/*	class SCDetailViewControllerOptions	*/
/****************************************************************************************/ 
/**	
 This class is used to specify options for automatically generated detail view controllers.
 
 Use this class to specify options such has how the automatically generated detail views are presented, their navigation bar type, their title, and/or their table view style.
 
 Sample use:
    SCArrayOfObjectsModel *myObjectsModel = ...;
    myObjectsModel.detailViewControllerOptions.presentationMode = SCPresentationModeModal;
 */

@interface SCDetailViewControllerOptions : NSObject

/** This value controls how the detail view controller is presented. Default: SCPresentationModeAuto. */
@property (nonatomic, readwrite) SCPresentationMode presentationMode;

/** The modal presentation style of the detail view. Default: UIModalPresentationFormSheet.
 @note: Only applicable if presentationMode equals SCPresentationModeModal. */
@property (nonatomic, readwrite) UIModalPresentationStyle modalPresentationStyle;

/** The navigation bar type of the detail view controller. Default: SCNavigationBarTypeAuto. Set to SCNavigationBarTypeNone to have simple back button navigation. */
@property (nonatomic, readwrite) SCNavigationBarType navigationBarType;

/** Set to TRUE to allow the cancel button to appear when entering editing mode in the detail view controller. Default: TRUE.
 @note: Only applicable if navigationBarType == SCNavigationBarTypeEditRight. */
@property (nonatomic, readwrite) BOOL allowEditingModeCancelButton;

/** The title of the detail view controller. Set to nil to have the presenting control set a title automatically. Default: nil. */
@property (nonatomic, copy) NSString *title;

/** The style of the detail view controller's table view. Default: Depends on the control presenting the view controller. */
@property (nonatomic, readwrite) UITableViewStyle tableViewStyle;

/** Indicates whether the bar at the bottom of the screen is hidden when the detail view controller is pushed. Default: TRUE. */
@property (nonatomic, readwrite) BOOL hidesBottomBarWhenPushed;

@end


