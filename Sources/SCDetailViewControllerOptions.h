/*
 *  SCDetailViewControllerOptions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>


#import "SCViewControllerTypedefs.h"

NS_ASSUME_NONNULL_BEGIN


#pragma mark - SCDetailViewControllerOptions
/// This class is used to specify options for automatically generated detail view controllers.
///
/// Use this class to specify options such as how the automatically generated detail views are presented, their navigation bar type, their title, and/or their table view style.
///
/// Sample use:
/// ```objective-c
/// SCArrayOfObjectsModel *myObjectsModel = ...;
/// myObjectsModel.detailViewControllerOptions.presentationMode = SCPresentationModeModal;
/// ```

@interface SCDetailViewControllerOptions : NSObject

/// The requested detail presentation mode; initially `SCPresentationModeAuto`.
/// Auto normally uses an active host or pushes when navigation is available,
/// otherwise presents modally. In the current array-section new-item path, the
/// presentation mode comes from the section's ordinary
/// `detailViewControllerOptions`, not `newItemDetailViewControllerOptions`, and
/// Auto resolves to Modal.
@property (nonatomic, readwrite) SCPresentationMode presentationMode;

/// The detail controller's requested modal style; initially
/// `UIModalPresentationFormSheet`.
/// The generated modal navigation wrapper copies this style, including when
/// Auto or an unavailable Push resolves to Modal. Popover mode overrides the
/// wrapper style with `UIModalPresentationPopover`.
@property (nonatomic, readwrite) UIModalPresentationStyle modalPresentationStyle;

/// The requested STV detail navigation buttons; initially
/// `SCNavigationBarTypeAuto`.
/// Auto lets the presenting cell/section choose. None adds no STV buttons:
/// UIKit Back is available only when pushed, not at a modal/popover root. A
/// custom STV controller with an explicit non-Auto bar type keeps that choice.
@property (nonatomic, readwrite) SCNavigationBarType navigationBarType;

/// Whether an EditRight detail controller may show editing-mode Cancel.
/// Initially `YES`. The controller also suppresses this item when its first
/// section is an array-of-items section; the option has no effect on other bar
/// types.
@property (nonatomic, readwrite) BOOL allowEditingModeCancelButton;

/// A copied title override for the detail controller; initially nil.
/// With nil, the presenting cell/section supplies its normal title behavior.
/// Cell-generated details normally use the row text; section-generated details
/// may preserve an existing custom controller title.
@property (nonatomic, copy, nullable) NSString *title;

/// The table style used when STV creates a detail table controller.
/// Initially `UITableViewStyleGrouped`. Individual presenting controls may
/// customize their options. This does not rebuild the table of a custom or
/// already-created controller.
@property (nonatomic, readwrite) UITableViewStyle tableViewStyle;

/// Whether the generated detail hides the bottom bar when pushed.
/// Initially `YES`; copied to the detail controller's UIKit property. It does
/// not control modal or popover navigation bars.
@property (nonatomic, readwrite) BOOL hidesBottomBarWhenPushed;

@end

NS_ASSUME_NONNULL_END
