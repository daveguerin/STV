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
/// A block that returns a Boolean decision for a section's detail model.
typedef BOOL (^SCConditionalDetailModelSectionAction_Block)(SCTableViewSection *section, SCTableViewModel *_Nullable detailModel, NSIndexPath *_Nullable indexPath);

/// A block invoked after a section fetches its items.
typedef void (^SCDidFetchSectionItemsAction_Block)(SCArrayOfItemsSection *itemsSection, NSMutableArray<id> *items);
/// A block called when fetch section items fails.
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
/// A block called when a section item delete cannot proceed without a connection.
typedef BOOL (^SCSectionItemDeleteNoConnectionAction_Block)(SCArrayOfItemsSection *itemsSection, NSObject *item);
/// A block called when a section item delete fails.
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
/// This class hosts a set of section action blocks. Once an action is set to a desired code block, it will execute the block as soon as the action occurs.
///
/// - SeeAlso: ``SCCellActions``, ``SCModelActions``.

@interface SCSectionActions : NSObject


#pragma mark - Section Actions

/// Action gets called right after the section has been added to a model.
///
/// This action is typically used to get hold of the section at data definition time and perform any additional configuration.
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

/// Action gets called when the section's header view is about to be displayed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willDisplayHeaderView = ^(SCTableViewSection *section, NSUInteger sectionIndex, UIView *view)
/// {
///     NSLog(@"Header view about to be displayed for section at index `%i`", sectionIndex);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionViewAction_Block willDisplayHeaderView;

/// Action gets called when the section's footer view is about to be displayed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.willDisplayFooterView = ^(SCTableViewSection *section, NSUInteger sectionIndex, UIView *view)
/// {
///     NSLog(@"Footer view about to be displayed for section at index `%i`", sectionIndex);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionViewAction_Block willDisplayFooterView;

/// Action gets called when the value of a section with an inherent value (such as an SCSelectionSection) has changed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.valueChanged = ^(SCTableViewSection *section, NSUInteger sectionIndex)
/// {
///     NSLog(@"Value for section at index `%i` has changed to %@", sectionIndex, section.boundValue);
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionAction_Block valueChanged;


#pragma mark - Detail Model Actions

/// Action gets called right after the section's detail model is created, before configuration is set or any sections are added.
///
/// This action is typically used to initially configure the detail model (like set a custom tag for example). Most of the model's settings can also be configure in the detailModelConfigured action.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
///
/// - SeeAlso: ``detailModelConfigured``
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelCreated;

/// Action gets called after the section's detail model is fully configured, including the addition of all automatically generated sections.
///
/// This action is typically used to add additional custom sections, or modify the already existing automatically generated ones.
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
/// - Note: In general, it is easier (and more recommended) to add your custom sections and cells using the data definitions, instead of using this action to do so. For more information, please refer to SCDataDefinition and SCCustomPropertyDefinition.
///
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelConfigured;

/// Implement action to provide your own custom presentation for the detail model instead of having the framework automatically present it.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block customPresentDetailModel;

/// Action gets called when the section's detail model is about to be presented in its own view controller.
///
/// This action is typically used to further customize the detail model's view controller.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelWillPresent;

/// Action gets called when the section's detail model has been presented in its own view controller.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelDidPresent;

/// Action gets called to give you a chance to decide if the detail model should be dismissed. Return YES to allow the detail model to be dismissed, otherwise return NO.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCConditionalDetailModelSectionAction_Block detailModelShouldDismiss;

/// Action gets called when the section's detail model's view controller is about to be dismissed.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelWillDismiss;

/// Action gets called when the section's detail model's view controller has been dismissed.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - Warning: In the case where the detail model is not associated with an existing row (such as the case when `SCArrayOfObjectsSection` creates a new item), indexPath.row will be set to NSNotFound. This is a good way to test if the detail model was generated for a new item or an already existing one.
@property (nonatomic, copy, nullable) SCDetailModelSectionAction_Block detailModelDidDismiss;

/// Action gets called to give you the chance to return a custom detail view controller for the section.
///
/// This action is typically used to provide your own custom detail view controller, instead of the one automatically generated by the section.
///
/// The block returns the custom view controller, which must be an ``SCViewController`` or ``SCTableViewController``. Returning `nil` ignores the action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.detailViewController = ^UIViewController*(SCTableViewSection *section, NSIndexPath *indexPath)
/// {
///     MyCustomViewController *customVC = [[MyCustomViewController alloc] initWithNib:@"MyCustomViewController" bundle:nil];
///
///     return customVC;
/// };
/// ```
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCDetailViewControllerForRowAtIndexPathAction_Block detailViewControllerForRowAtIndexPath;

/// Action gets called to give you the chance to return a custom detail model for the section's detail view controller.
///
/// This action is typically used to provide your own custom detail model, instead of the one automatically generated by the section.
///
/// - Note: It is much more common to use the detailViewController action instead, assigning the custom model in the custom view controller's viewDidLoad method.
///
/// The block returns the custom detail model. It must not already be associated with a table view because the framework handles that association. Returning `nil` ignores the action.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCDetailTableViewModelForRowAtIndexPathAction_Block detailTableViewModelForRowAtIndexPath;


#pragma mark - SCArrayOfItemsSection Actions

/// Action gets called to give you the chance to return a custom cell for the section's item, instead of the automatically generated standard SCTableViewCell.
///
/// The block returns an ``SCCustomCell`` instance or subclass. Returning `nil` ignores the action.
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
/// - Warning: If more than one type of custom cell is returned (e.g.: depending on the indexPath), then you *must* also use the reuseIdentifierForRowAtIndexPath action to return a unique reuse id for each different cell you return. Otherwise, there is no need to set the reuseIdentifierForRowAtIndexPath action.
///
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
///
/// - SeeAlso: ``reuseIdentifierForRowAtIndexPath``
@property (nonatomic, copy, nullable) SCCellForRowAtIndexPathAction_Block cellForRowAtIndexPath;

/// In case more than one type of custom cell is returned in cellForRowAtIndexPath, this action gets called to give you the chance to return a custom cell reuse identifier for each different type.
///
/// - Note: You only need to set this action if more than one type of custom cell is returned in cellForRowAtIndexPath.
///
/// The block returns the custom cell reuse identifier.
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
/// - Note: This action is only applicable to sections that generate detail views, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCReuseIdForRowAtIndexPathAction_Block reuseIdentifierForRowAtIndexPath;

/// Action gets called to give you the chance to return a custom cell height for each of the section's cells, instead of STV's automatically calculated cell height.
///
/// The block returns the custom cell height. Returning `UITableViewAutomaticDimension` asks the framework to calculate the height automatically.
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
/// - Note: Implement this action only if you would like to the cell height directly instead of having the framework automatically calculate it for you.
@property (nonatomic, copy, nullable) SCCustomHeightForRowAtIndexPathAction_Block customHeightForRowAtIndexPath;

/// Action gets called as soon as the section has retrieved its items from their data store.
///
/// This action is typically used to customize the `items` array after it has been fetched from the data store. Items can be added, removed, or rearranged. The added items can either be objects that are suppored by the data store, or normal SCTableViewCell (or subclass) objects.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCDidFetchSectionItemsAction_Block didFetchItemsFromStore;

/// Action gets called if the section fails to retrieve its items from their data store.
///
/// This action is typically used when the section is bound to a web service and you want to give the user feedback that their web service was not reachable.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCFetchSectionItemsFailedAction_Block fetchItemsFromStoreFailed;

/// Action gets called as soon as the section has created a new item to be used in its add new item detail view (which usually apprears right after the user taps the section's Add button).
///
/// This action is typically used to customize the newly created item before the new item detail view is displayed.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDidCreateAction_Block didCreateItem;

/// Action gets called right before a newly created item is inserted into the section.
///
/// This action is typically used to customize the newly created item before it's inserted. Furthermore, returning FALSE gives you the chance to discard adding the new item altogether.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemWillInsertAction_Block willInsertItem;

/// Action gets called when a newly created item is being inserted into the section, but no data store connection could be established.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore. Return YES to try operation again later when connection is available (currently only available with SCParseDefinition), otherwise return NO to call the insertItemFailed action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.insertItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection available for current operation. Will try again later.");
///
///     return YES;  // try again later
/// };
/// ```
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemInsertNoConnectionAction_Block insertItemNoConnection;

/// Action gets called when a newly created item could not be inserted into the section.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemInsertFailedAction_Block insertItemFailed;

/// Action gets called as soon as a new item has been added to the section.
///
/// This action is typically used to provide custom functionality after a new item has been added.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDidInsertAction_Block didInsertItem;

/// Action gets called right before an existing item is updated.
///
/// This action is typically used to give the user the chance to discard updating the item altogether by returning FALSE.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemWillUpdateAction_Block willUpdateItem;

/// Action gets called when an existing item is being updated, but no data store connection could be established.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore. Return YES to try operation again later when connection is available (currently only available with SCParseDefinition), otherwise return NO to call the updateItemFailed action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.updateItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection available for current operation. Will try again later.");
///
///     return YES;  // try again later
/// };
/// ```
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemUpdateNoConnectionAction_Block updateItemNoConnection;

/// Action gets called when an existing item could not be updated.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemUpdateFailedAction_Block updateItemFailed;

/// Action gets called as soon as an existing item has been updated.
///
/// This action is typically used to modify the item's values or provide custom functionality after an item has been updated.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.didUpdateItem = ^(SCArrayOfItemsSection *itemsSection, NSObject *item, NSIndexPath *indexPath)
/// {
///     // Set a default description if no description is set
///     if(![item valueForKey:@"description"])
///         [item setValue:@"My default description" forKey:@"description"];
/// };
/// ```
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDidUpdateAction_Block didUpdateItem;

/// Action gets called right before an existing item is deleted.
///
/// This action is typically used to provide you with a chance to override the item delete operation by returning FALSE.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemWillDeleteAction_Block willDeleteItem;

/// Action gets called when an existing item is being deleted, but no data store connection could be established.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore. Return YES to try operation again later when connection is available (currently only available with SCParseDefinition), otherwise return NO to call the deleteItemFailed action.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// sectionActions.deleteItemNoConnection = ^BOOL(SCArrayOfItemsSection *itemsSection, NSObject *item)
/// {
///     NSLog(@"No connection available for current operation. Will try again later.");
///
///     return YES;  // try again later
/// };
/// ```
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDeleteNoConnectionAction_Block deleteItemNoConnection;

/// Action gets called when an existing item could not be deleted.
///
/// This action is typically called for Internet connection dependent data stores, such as SCWebServiceStore and SCParseStore.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDeleteFailedAction_Block deleteItemFailed;

/// Action gets called as soon as an existing item has been deleted.
///
/// This action is typically used to provide custom functionality after an item has been deleted.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCSectionItemDidDeleteAction_Block didDeleteItem;


/// Action gets called after the section has retrieved its items from their data store, and the framework has automatically added any needed special cells (e.g.: placeholder cell, load more cell, etc.) to the items array.
///
/// This action is typically used to customize the `items` array after it has been fetched from the data store and the special cells added to it.
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
/// - Note: This action is only applicable to SCArrayOfItemsSection subclasses, such as SCArrayOfObjectsSection.
@property (nonatomic, copy, nullable) SCDidAddSpecialCellsAction_Block didAddSpecialCells;


#pragma mark - Miscellaneous

/// Method assigns all the actions of another `SCSectionActions` class to the current one.
///
/// - Parameter actions: The source actions, or nil to leave existing hooks alone.
/// - Parameter override: Set to TRUE to override any existing actions, otherwise set to FALSE.
- (void)setActionsTo:(SCSectionActions *_Nullable)actions overrideExisting:(BOOL)override;

@end

NS_ASSUME_NONNULL_END
