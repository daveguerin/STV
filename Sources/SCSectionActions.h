/*
 *  SCSectionActions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


@class SCTableViewModel;
@class SCTableViewSection;
@class SCArrayOfItemsSection;
@class SCCustomCell;

/// A block invoked with a table-view section and its index.
typedef void (^SCSectionAction_Block)(SCTableViewSection *section, NSUInteger sectionIndex);
/// A block invoked with a table-view section's header or footer view.
typedef void (^SCSectionViewAction_Block)(SCTableViewSection *section, NSUInteger sectionIndex, UIView *view);

/// A block invoked with a table-view section and its detail model.
/// The model can be nil for a custom controller without an STV model. The
/// index path can be nil during new-item configuration; other new-item events
/// use an index path whose row is NSNotFound.
typedef void (^SCDetailModelSectionAction_Block)(SCTableViewSection *section, SCTableViewModel *_Nullable detailModel, NSIndexPath *_Nullable indexPath);
/// A block receiving a section, possibly nil detail model, and detail index path.
/// Return YES to allow dismissal or NO to veto it. New-item dismissal uses an
/// NSNotFound row; detail callback signatures also permit nil index paths.
typedef BOOL (^SCConditionalDetailModelSectionAction_Block)(SCTableViewSection *section, SCTableViewModel *_Nullable detailModel, NSIndexPath *_Nullable indexPath);

/// A block invoked after a section fetches its items.
typedef void (^SCDidFetchSectionItemsAction_Block)(SCArrayOfItemsSection *itemsSection, NSMutableArray<id> *items);
/// A block called when fetching section items asynchronously fails.
/// The error can be nil when the data store provides no failure details.
typedef void (^SCFetchSectionItemsFailedAction_Block)(SCArrayOfItemsSection *itemsSection, NSError *_Nullable error);
/// A block invoked after a section creates an item.
typedef void (^SCSectionItemDidCreateAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item);
/// A block that approves an item before a section inserts it.
/// The item model is nil when the new-item detail screen is skipped.
typedef BOOL (^SCSectionItemWillInsertAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, SCTableViewModel *_Nullable itemModel);
/// A block called when a section item insert cannot proceed without a connection.
typedef BOOL (^SCSectionItemInsertNoConnectionAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item);
/// A block called when a section item insert fails.
typedef void (^SCSectionItemInsertFailedAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *_Nullable error);
/// A block invoked after a section inserts an item.
typedef void (^SCSectionItemDidInsertAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath);
/// A block that approves an item before a section updates it.
typedef BOOL (^SCSectionItemWillUpdateAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath, SCTableViewModel *_Nullable itemModel);
/// A block called when a section item update cannot proceed without a connection.
typedef BOOL (^SCSectionItemUpdateNoConnectionAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item);
/// A block called when a section item update fails.
typedef void (^SCSectionItemUpdateFailedAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *_Nullable error);
/// A block invoked after a section updates an item.
typedef void (^SCSectionItemDidUpdateAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath);
/// A block that approves an item before a section deletes it.
typedef BOOL (^SCSectionItemWillDeleteAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath);
/// A reserved deletion no-connection callback; STV currently does not call it.
typedef BOOL (^SCSectionItemDeleteNoConnectionAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item);
/// A reserved deletion failure callback; STV currently does not call it.
typedef void (^SCSectionItemDeleteFailedAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *_Nullable error);
/// A block invoked after a section deletes an item.
typedef void (^SCSectionItemDidDeleteAction_Block)(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath);
/// A block invoked after a section adds its special cells.
/// The array contains fetched data objects as well as any special cells.
typedef void (^SCDidAddSpecialCellsAction_Block)(SCArrayOfItemsSection *itemsSection, NSMutableArray<id> *items);

/// A block that supplies the detail view controller for a row.
typedef UIViewController *_Nullable (^SCDetailViewControllerForRowAtIndexPathAction_Block)(SCTableViewSection *section, NSIndexPath *indexPath);
/// A block that supplies the detail table-view model for a row.
typedef SCTableViewModel *_Nullable (^SCDetailTableViewModelForRowAtIndexPathAction_Block)(SCTableViewSection *section, NSIndexPath *indexPath);

/// A block that supplies the custom cell for a row.
typedef SCCustomCell *_Nullable (^SCCellForRowAtIndexPathAction_Block)(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath);
/// A block that supplies the cell reuse identifier for a row.
typedef NSString *_Nullable (^SCReuseIdForRowAtIndexPathAction_Block)(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath);
/// A block that supplies the custom height for a row.
typedef CGFloat (^SCCustomHeightForRowAtIndexPathAction_Block)(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath);

#pragma mark - SCSectionActions
/// Optional, copied callbacks for section events and item-detail flows.
/// Hooks default to nil. STV normally selects the section hook before the owner
/// model's section hook; it does not broadcast or retry a lower-priority hook
/// after a nil result. Callback-specific limitations are documented below.
/// Blocks retain captured objects: capture owners weakly to avoid cycles.
///
/// - SeeAlso: ``SCCellActions``, ``SCModelActions``.

@interface SCSectionActions : NSObject


#pragma mark - Section Actions

/// Configures a section after it is added or inserted into a model.
/// The block receives the section and its index after any add-time sorting. The
/// model's `didAddSection` callback runs first. This also supports
/// configuration of sections generated from data definitions.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// myArrayOfObjectsAttributes.expandContentInCurrentView = YES;
/// myArrayOfObjectsAttributes.expandedContentSectionActions.didAddToModel = ^(SCTableViewSection *section, NSUInteger sectionIndex)
/// {
///     // set all expanded cells to a different background color
///     section.cellActions.willDisplay = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
///     {
///         cell.backgroundColor = [UIColor yellowColor];
///     };
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionAction_Block didAddToModel;

/// Customizes a section header immediately before UIKit displays it.
/// The block receives the section, its model index, and the header view. This
/// callback can repeat as headers are reused or displayed again.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willDisplayHeaderView = ^(SCTableViewSection *section, NSUInteger sectionIndex, UIView *view)
/// {
///     NSLog(@"Header view about to be displayed for section at index `%lu`", (unsigned long)sectionIndex);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionViewAction_Block willDisplayHeaderView;

/// Customizes a section footer immediately before UIKit displays it.
/// The block receives the section, its model index, and the footer view. This
/// callback can repeat as footers are reused or displayed again.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willDisplayFooterView = ^(SCTableViewSection *section, NSUInteger sectionIndex, UIView *view)
/// {
///     NSLog(@"Footer view about to be displayed for section at index `%lu`", (unsigned long)sectionIndex);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionViewAction_Block willDisplayFooterView;

/// Reports a section-level value-change event to the model.
/// The block receives the section and its model index after the model's
/// modification notification. It applies to selection values and item changes,
/// not only to sections with a single bound value, and does not confirm
/// persistence.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.valueChanged = ^(SCTableViewSection *section, NSUInteger sectionIndex)
/// {
///     NSLog(@"Value for section at index `%lu` has changed to %@", (unsigned long)sectionIndex, section.boundValue);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionAction_Block valueChanged;


#pragma mark - Detail Model Actions

/// Customizes a resolved item-detail model before its generated content is
/// built.
/// The block receives the section, the possibly nil detail model, and a row
/// index path. A newly added item uses `NSNotFound` for the row. A custom or
/// reused model may already contain content. Storyboard new-item construction
/// does not dispatch this hook.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelCreated = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     detailModel.tag = 100;
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelCreated;

/// Customizes an item-detail model during the STV controller's load
/// notification.
/// The block receives the section, a model with its detail content configured,
/// and the selected row index path. The index path is nil for new-item
/// configuration, not an `NSNotFound` row. The hook is skipped when the
/// controller has no STV model. It runs through the controller's load delegate,
/// not necessarily after a subclass's `viewDidLoad` has finished.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelConfigured = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     SCTableViewSection *customSection = [SCTableViewSection section];
///     SCCustomCell *customCell = [SCCustomCell cellWithText:@"Custom Cell"];
///     [customSection addCell:customCell];
///
///     [detailModel addSection:customSection];
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelConfigured;

/// Replaces STV's automatic presentation of an item-detail controller.
/// The block receives the section, the possibly nil detail model, and the row
/// index path (`NSNotFound` for a new item). STV associates the model with the
/// detail table before invoking it. Installing the hook is sufficient; it is
/// not restricted to `SCPresentationModeCustom`. Reusing an already-active
/// detail controller in Auto or Popover mode takes precedence and gains focus
/// instead.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.customPresentDetailModel = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     UIViewController *detailViewController = detailModel.viewController;
///     [myNavigationController pushViewController:detailViewController animated:YES];
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block customPresentDetailModel;

/// Reports that an STV item-detail controller is about to present or gain
/// focus.
/// The block receives the section, its possibly nil detail model, and the
/// selected row index path, or an `NSNotFound` row for a new item. Use this
/// event to customize controller UI. Plain UIKit controllers do not
/// automatically forward STV lifecycle events.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelWillPresent = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     detailModel.viewController.title = @"My custom title";
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelWillPresent;

/// Reports that an STV item-detail controller has presented or gained focus.
/// The block receives the section, its possibly nil detail model, and the
/// selected row index path, or an `NSNotFound` row for a new item. Delivery
/// depends on STV controller delegate/focus forwarding.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelDidPresent = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     NSLog(@"Detail model has been presented.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelDidPresent;

/// Approves dismissal or focus loss of an STV item-detail controller.
/// The block receives the section, its possibly nil detail model, and the
/// selected row index path, or an `NSNotFound` row for a new item. Return `YES`
/// to allow dismissal or `NO` to veto it. Both section- and model-level hooks
/// honor the result; no hook means `YES`.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelShouldDismiss = ^BOOL(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     return YES;  // allow detail model to be dismissed
/// };
/// ```
@property (nonatomic, copy, nullable) SCConditionalDetailModelSectionAction_Block detailModelShouldDismiss;

/// Reports dismissal or focus loss of an STV item-detail controller.
/// The block receives the section, its possibly nil detail model, and the
/// selected row index path, or an `NSNotFound` row for a new item. Interactive
/// popover notifications wait for UIKit's completed dismissal, so this hook may
/// run after the visual transition.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelWillDismiss = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     NSLog(@"Detail model will be dismissed.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelWillDismiss;

/// Reports completed dismissal or focus loss of an STV item-detail controller.
/// The block receives the section, its possibly nil detail model, and the
/// selected row index path, or an `NSNotFound` row for a new item. This is a
/// controller event, not confirmation that a data-store operation succeeded.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailModelDidDismiss = ^(SCTableViewSection *section, SCTableViewModel *detailModel, NSIndexPath *indexPath)
/// {
///     NSLog(@"Detail model has been dismissed.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelDidDismiss;

/// Supplies a controller for an item-detail screen.
/// The block receives the section and row index path, with an `NSNotFound` row
/// for a new item. Return a controller or nil to create the standard
/// controller; a nil result does not try lower-priority hooks or the model's
/// assigned detail controller. Use `SCViewController` or
/// `SCTableViewController` for automatic model/delegate integration; plain
/// UIKit controllers do not receive that integration.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailViewControllerForRowAtIndexPath = ^UIViewController*(SCTableViewSection *section, NSIndexPath *indexPath)
/// {
///     MyCustomViewController *customVC = [[MyCustomViewController alloc] initWithNibName:@"MyCustomViewController" bundle:nil];
///
///     return customVC;
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailViewControllerForRowAtIndexPathAction_Block detailViewControllerForRowAtIndexPath;

/// Supplies the model for an STV item-detail controller.
/// The block receives the section and row index path, with an `NSNotFound` row
/// for a new item. Return a model to assign to the STV controller, or nil to
/// keep its existing model. The controller retains and associates the model
/// with its table. Avoid sharing a model with another live table. A nil result
/// does not retry lower-priority hooks.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailTableViewModelForRowAtIndexPath = ^SCTableViewModel*(SCTableViewSection *section, NSIndexPath *indexPath)
/// {
///     return myCustomTableViewModel;
/// };
/// ```
@property (nonatomic, copy, nullable) SCDetailTableViewModelForRowAtIndexPathAction_Block detailTableViewModelForRowAtIndexPath;


#pragma mark - SCArrayOfItemsSection Actions

/// Creates a custom cell for an array-section item when no reusable cell is
/// available.
/// The block receives the items section and row index path. Return an
/// `SCCustomCell` or subclass, or nil to let STV generate its standard cell.
/// The hook is not called for every display, and items that are already cell
/// objects bypass it. Use a distinct `reuseIdentifierForRowAtIndexPath` result
/// for each incompatible cell layout/type.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.cellForRowAtIndexPath = ^SCCustomCell*(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath)
/// {
///     // `1` and `2` are the tags of the labels corresponding to
///     // the firstName and lastName object properties  ( NO NEED to provide bindingsString if your cell uses curly brace binding )
///     NSString *bindingsString = @"1:firstName;2:lastName";
///
///     SCCustomCell *customCell = [SCCustomCell cellWithText:nil objectBindingsString:bindingsString nibName:@"MyCustomCell"];
///
///     return customCell;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCellForRowAtIndexPathAction_Block cellForRowAtIndexPath;

/// Chooses the reuse pool for an array-section item.
/// The block receives the items section and row index path and returns an
/// identifier, or nil to use the section's `cellIdentifier`. Supply distinct
/// identifiers for incompatible custom cells; this is queried before attempting
/// to dequeue a cell.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.cellForRowAtIndexPath = ^SCCustomCell*(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath)
/// {
///     SCCustomCell *customCell;
///     if(indexPath.row % 2)
///         customCell = [[MyCustomOddCell alloc] init];
///     else
///         customCell = [[MyCustomEvenCell alloc] init];
///
///     return customCell;
/// };
/// sectionActions.reuseIdentifierForRowAtIndexPath = ^NSString*(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath)
/// {
///     NSString *reuseId;
///     if(indexPath.row % 2)
///         reuseId = @"OddCell";
///     else
///         reuseId = @"EvenCell";
///
///     return reuseId;
/// };
/// ```
@property (nonatomic, copy, nullable) SCReuseIdForRowAtIndexPathAction_Block reuseIdentifierForRowAtIndexPath;

/// Overrides sizing for an array-section row during cell preparation.
/// The block receives the items section and index path. Return a positive
/// height for a resolved fixed-height constraint, or
/// `UITableViewAutomaticDimension` to leave the cell's normal sizing policy in
/// effect. It does not force a fixed-height or legacy-mode cell into automatic
/// sizing. This is not a separate offscreen cell-configuration callback.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.customHeightForRowAtIndexPath = ^CGFloat(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath)
/// {
///     CGFloat cellHeight;
///     if(indexPath.row % 2)
///         cellHeight = 44;
///     else
///         cellHeight = 60;
///
///     return cellHeight;
/// };
/// ```
@property (nonatomic, copy, nullable) SCCustomHeightForRowAtIndexPathAction_Block customHeightForRowAtIndexPath;

/// Customizes fetched or model-distributed section items.
/// The block receives the items section and a mutable array of data objects
/// and/or cell objects. It can add, remove, or reorder entries before the
/// section installs a fetched batch. An items model also calls it after
/// distributing and sorting section items, including regenerated search
/// results; it does not always signify a fresh store request.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didFetchItemsFromStore = ^(SCArrayOfItemsSection *itemsSection, NSMutableArray *items)
/// {
///     // Add a button cell at the end of the items list
///     SCTableViewCell *buttonCell = [SCTableViewCell cellWithText:@"Tap me!" textAlignment:NSTextAlignmentCenter];
///     buttonCell.cellActions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
///     {
///         NSLog(@"buttonCell tapped!");
///     };
///
///     [items addObject:buttonCell];
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidFetchSectionItemsAction_Block didFetchItemsFromStore;

/// Reports failure from the section's asynchronous fetch path.
/// The block receives the items section and the store's error, which may be
/// nil. Synchronous fetches have no equivalent error callback here. STV
/// forwards the store completion without adding a queue hop.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
///  sectionActions.fetchItemsFromStoreFailed = ^(SCArrayOfItemsSection *itemsSection, NSError *error)
///  {
///       NSLog(@"Failed retrieving section items with error: %@", error);
///  };
/// ```
@property (nonatomic, copy, nullable) SCFetchSectionItemsFailedAction_Block fetchItemsFromStoreFailed;

/// Initializes a newly created temporary item before the add-item flow
/// continues.
/// The block receives the items section and new item, before either
/// presentation of its detail controller or the `skipNewItemDetailView`
/// insertion path. It is not called if item creation returns nil, and does not
/// mean the item has been inserted.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didCreateItem = ^(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     // Set an initial value for startDate
///     [item setValue:[NSDate date] forKey:@"startDate"];
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDidCreateAction_Block didCreateItem;

/// Approves insertion of a newly created item.
/// The block receives the items section, item, and detail model (nil when the
/// new-item detail screen is skipped). Return `YES` to continue or `NO` to
/// discard the insertion; no hook means `YES`. In the detail flow, pending
/// model values are committed before this decision, unlike `willUpdateItem`.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willInsertItem = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item, SCTableViewModel *itemModel)
/// {
///     // Set a default description if no description is set
///     if(![item valueForKey:@"description"])
///         [item setValue:@"My default description" forKey:@"description"];
///
///     // Accept insert
///     return TRUE;
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemWillInsertAction_Block willInsertItem;

/// Handles an asynchronous new-item insertion's no-connection callback.
/// The block receives the items section and item. At section level its BOOL
/// result is passed to the store: `YES` requests store-specific deferred
/// handling and `NO` declines it. STV supplies no retry queue and defaults to
/// `NO`. The skip-detail insertion path does not install this callback.
/// - Warning: The current model-level fallback invokes the block but discards
/// its result, leaving `NO`.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.insertItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection; requesting store-specific deferred handling.");
///
///     return YES;  // request deferred handling if the store supports it
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemInsertNoConnectionAction_Block insertItemNoConnection;

/// Reports an asynchronous insertion failure in the new-item detail flow.
/// The block receives the items section, item, and possibly nil error from the
/// store. It is not dispatched for synchronous insertion or when the new-item
/// detail screen is skipped. STV forwards the store completion without adding a
/// queue hop.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.insertItemFailed = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *error)
/// {
///     NSLog(@"Operation failed with error: %@", error);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemInsertFailedAction_Block insertItemFailed;

/// Reports addition of a new item's row to the section.
/// The block receives the items section, item, and inserted row index path.
/// Normal asynchronous detail insertion adds the row on success; the
/// skip-detail path adds it without waiting for success. This notification
/// alone therefore does not guarantee persistence.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didInsertItem = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath)
/// {
///     NSLog(@"New item inserted at indexPath: %@", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDidInsertAction_Block didInsertItem;

/// Approves applying an existing item's detail edits.
/// The block receives the items section, item, its index path, and possibly nil
/// detail model. Return `YES` to continue or `NO` to skip the detail commit and
/// store update; no hook means `YES`. This runs before the detail model's
/// pending values are committed when automatic detail commits are enabled.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willUpdateItem = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath, SCTableViewModel *itemModel)
/// {
///     if(myCondition)
///         return TRUE; // Accept update
///     //else
///     return FALSE; // Reject update
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemWillUpdateAction_Block willUpdateItem;

/// Handles an asynchronous detail update's no-connection callback.
/// The block receives the items section and item. At section level its BOOL
/// result is passed to the store: `YES` requests store-specific deferred
/// handling and `NO` declines it. STV supplies no retry queue and defaults to
/// `NO`.
/// - Warning: The current model-level fallback invokes the block but discards
/// its result, leaving `NO`.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.updateItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection; requesting store-specific deferred handling.");
///
///     return YES;  // request deferred handling if the store supports it
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemUpdateNoConnectionAction_Block updateItemNoConnection;

/// Reports failure from an asynchronous detail-item update.
/// The block receives the items section, item, and possibly nil error from the
/// store. Synchronous updates do not dispatch this hook. STV forwards the store
/// completion without adding a queue hop.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.updateItemFailed = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *error)
/// {
///     NSLog(@"Operation failed with error: %@", error);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemUpdateFailedAction_Block updateItemFailed;

/// Reports completion of STV's item-update path.
/// The block receives the items section, item, and row index path used for the
/// update. The synchronous path calls it after requesting a store update
/// without checking that operation's BOOL result; the asynchronous path calls
/// it from success. It is not an unconditional guarantee of persistence.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didUpdateItem = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath)
/// {
///     NSLog(@"Update path completed for item at %@", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDidUpdateAction_Block didUpdateItem;

/// Approves removing an item and its row.
/// The block receives the items section, item, and original row index path.
/// Return `YES` to proceed or `NO` to skip the store deletion and table
/// mutation. With no hook STV permits deletion.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willDeleteItem = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath)
/// {
///     BOOL deleteItem;
///
///     if(myCondition)
///         deleteItem = TRUE;
///     else
///         deleteItem = FALSE;
///
///     return deleteItem;
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemWillDeleteAction_Block willDeleteItem;

/// Reserved no-connection hook for item deletion.
/// The block signature receives the items section and item and returns a
/// store-specific decision. The current deletion path passes nil completion
/// blocks to asynchronous deletion, so STV never dispatches this hook or uses
/// its BOOL result.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.deleteItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection; requesting store-specific deferred handling.");
///
///     return YES;  // request deferred handling if the store supports it
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDeleteNoConnectionAction_Block deleteItemNoConnection;

/// Reserved failure hook for item deletion.
/// The block signature receives the items section, item, and possibly nil
/// error. The current deletion path does not dispatch this hook: asynchronous
/// deletion passes a nil failure callback, and synchronous deletion has no
/// error forwarding here.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.deleteItemFailed = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSError *error)
/// {
///     NSLog(@"Operation failed with error: %@", error);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDeleteFailedAction_Block deleteItemFailed;

/// Reports removal of an item's row from the section.
/// The block receives the items section and the deleted row's former index
/// path. STV removes the row without awaiting asynchronous store success or
/// checking synchronous success, so this callback does not confirm successful
/// persistent deletion.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didDeleteItem = ^(SCArrayOfItemsSection *itemsSection, NSIndexPath *indexPath)
/// {
///     NSLog(@"Item at indexPath: %@ has been deleted.", indexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionItemDidDeleteAction_Block didDeleteItem;


/// Customizes the section's display array after special-cell insertion.
/// The block receives the items section and its mutable display array,
/// containing fetched data objects as well as any placeholder, add-item, or
/// fetch-more cells. It can add, remove, or reorder entries. This may run
/// whenever special cells are rebuilt, not only after a fresh fetch.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didAddSpecialCells = ^(SCArrayOfItemsSection *itemsSection, NSMutableArray *items)
/// {
///     // Add a button cell at the end of the items list
///     SCTableViewCell *buttonCell = [SCTableViewCell cellWithText:@"Tap me!" textAlignment:NSTextAlignmentCenter];
///     buttonCell.cellActions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
///     {
///         NSLog(@"buttonCell tapped!");
///     };
///
///     [items addObject:buttonCell];
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidAddSpecialCellsAction_Block didAddSpecialCells;


#pragma mark - Miscellaneous

/// Copies the source's nonnil hooks into this actions object.
/// Nil source hooks never clear an existing destination hook, even when
/// overriding. Each assigned block is copied; the source object itself is not
/// retained.
/// - Parameter actions: Source actions, or nil to leave all hooks unchanged.
/// - Parameter override: `YES` replaces existing hooks where the source is
/// nonnil; `NO` fills only currently nil hooks.
- (void)setActionsTo:(SCSectionActions *_Nullable)actions overrideExisting:(BOOL)override;

@end

NS_ASSUME_NONNULL_END
