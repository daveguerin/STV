/*
 *  SCModelActions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SCTableViewCell;
@class SCTableViewModel;
@class SCArrayOfItemsModel;
@class SCTableViewSection;

/// A block invoked with a table-view model.
typedef void (^SCModelAction_Block)(SCTableViewModel *tableModel);
/// A block receiving a model and approving an editing-state change.
/// Return YES to allow the requested change or NO to leave editing unchanged.
typedef BOOL (^SCConditionalModelAction_Block)(SCTableViewModel *tableModel);
/// A block invoked after a section is added to a table-view model.
typedef void (^SCDidAddSectionAction_Block)(SCTableViewModel *tableModel, SCTableViewSection *section, NSUInteger sectionIndex);
/// A block invoked after a section is removed from a table-view model.
typedef void (^SCDidRemoveSectionAction_Block)(SCTableViewModel *tableModel, NSUInteger sectionIndex);
/// A block that sorts a table-view model's mutable section array.
typedef void (^SCSortSectionsAction_Block)(SCTableViewModel *tableModel, NSMutableArray<SCTableViewSection *> *sections);
/// A block that maps a section-index title to a section index.
typedef NSInteger (^SCSectionForSectionIndexTitleAction_Block)(SCTableViewModel *tableModel, NSString *title, NSInteger index);

/// A block invoked after an items model fetches its items synchronously.
typedef void (^SCDidFetchModelItemsAction_Block)(SCArrayOfItemsModel *itemsModel, NSMutableArray<id> *items);
/// A block that supplies the section-header title for an item.
typedef NSString *_Nullable (^SCSectionHeaderTitleForItemAction_Block)(SCArrayOfItemsModel *itemsModel, NSObject *item, NSUInteger itemIndex);
/// A block that supplies section titles to precreate before item grouping.
typedef NSArray<NSString *> *_Nullable (^SCSectionHeaderTitlesAction_Block)(SCArrayOfItemsModel *itemsModel);
/// A block that post-processes search results computed by an items model.
typedef NSArray<id> *_Nullable (^SCDidComputeSearchResultsAction_Block)(SCArrayOfItemsModel *itemsModel, NSString *searchText, NSArray<id> *_Nullable searchResults);
/// A block invoked after a table-view cell moves between index paths.
typedef void (^SCDidMoveCellAction_Block)(SCTableViewModel *tableModel, SCTableViewCell *cell, NSIndexPath *fromIndexPath, NSIndexPath *toIndexPath);


#pragma mark - SCModelActions
/// Optional, copied callbacks for model lifecycle and customization.
/// Hooks default to nil. Each hook is invoked only by its corresponding model
/// path; installing one is not general observation of all UIKit or store
/// changes. Blocks retain captured objects, so capture owners weakly to avoid
/// cycles.
///
/// - SeeAlso: ``SCSectionActions``, ``SCCellActions``.

@interface SCModelActions : NSObject


#pragma mark - Actions

/// Customizes a section after the model adds or inserts it.
/// The block receives the model, section, and its index after any add-time
/// sorting. It runs before the section's `didAddToModel` hook.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didAddSection = ^(SCTableViewModel *tableModel, SCTableViewSection *section, NSUInteger sectionIndex)
/// {
///     // use custom data sorting for the first section in a Core Data SCArrayOfObjectsModel
///     if(sectionIndex==0 && [section isKindOfClass:[SCArrayOfObjectsSection class]])
///     {
///         SCArrayOfObjectsSection *objectsSection = (SCArrayOfObjectsSection *)section;
///         SCCoreDataFetchOptions *oldFetchOptions = (SCCoreDataFetchOptions *)objectsSection.dataFetchOptions;
///         objectsSection.dataFetchOptions = [SCCoreDataFetchOptions optionsWithSortKey:oldFetchOptions.sortKey sortAscending:FALSE filterPredicate:oldFetchOptions.filterPredicate];
///     }
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidAddSectionAction_Block didAddSection;

/// Reports removal of one section by index.
/// The block receives the model and the section's former index.
/// `removeAllSections` clears the array directly and does not dispatch this
/// hook for each removed section.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didRemoveSection = ^(SCTableViewModel *tableModel, NSUInteger sectionIndex)
/// {
///     NSLog(@"Section at index `%lu` has been removed.", (unsigned long)sectionIndex);
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidRemoveSectionAction_Block didRemoveSection;

/// Orders the model's mutable section array when a section is added.
/// The block receives the model and its actual section array; rearrange that
/// array in place. Installing this hook takes precedence over
/// `autoSortSections`. It is not a notification after every possible array
/// mutation.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.sortSections = ^(SCTableViewModel *tableModel, NSMutableArray *sections)
/// {
///     // Rearrange the `sections` mutable array here as you see fit
/// };
/// ```
@property (nonatomic, copy, nullable) SCSortSectionsAction_Block sortSections;

/// Maps a tapped section-index entry to a table section.
/// The block receives the model, title, and the title's index in
/// `sectionIndexTitles`; return the destination section index. STV returns the
/// result directly without clamping. Without a hook it uses the title index
/// when in range, otherwise the last section index.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.sectionForSectionIndexTitle = ^NSInteger(SCTableViewModel *tableModel, NSString *title, NSInteger index)
/// {
///     if(index < tableModel.sectionCount)
///        return index;
///     // else return the last section index
///     return tableModel.sectionCount-1;
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionForSectionIndexTitleAction_Block sectionForSectionIndexTitle;

/// Reports the end of a visible-cell loading pass.
/// The block receives the model when `willDisplayCell` reaches the tracked last
/// visible row while the table is not dragging, after STV exits loading mode.
/// It does not mean every offscreen row was instantiated, and may run more than
/// once as visible rows change.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didFinishLoadingCells = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The visible-cell loading pass finished.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didFinishLoadingCells;

/// Reports that the model's refresh handler requested a reload.
/// The block receives the model after `reloadBoundValues`, table reloading, and
/// `endRefreshing` on the native `UIRefreshControl`. It does not wait for
/// asynchronous store requests started by that reload to finish.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didRefresh = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model has been refreshed.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didRefresh;


/// Reports a scroll-view scrolling update.
/// The block receives the model each time UIKit calls `scrollViewDidScroll:`,
/// not only when scrolling starts.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didScroll = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model's table view scrolled.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didScroll;

/// Reports the end of table dragging, whether or not deceleration follows.
/// The block receives the model. When no deceleration follows, visible-cell
/// `lazyLoad` hooks run first. The callback itself does not receive UIKit's
/// deceleration flag.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didEndDragging = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model's table view has ended dragging with decelerating=%i.", tableModel.tableView.decelerating);
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didEndDragging;

/// Reports completion of table deceleration.
/// The block receives the model after STV invokes visible-cell `lazyLoad`
/// hooks.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didEndDecelerating = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model's table view has ended decelerating.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didEndDecelerating;


/// Approves entry into editing through the model's editing setter.
/// The block receives the model and returns `YES` to continue or `NO` to leave
/// editing unchanged. No hook means `YES`. Direct UIKit changes that bypass
/// `setTableViewEditing:animated:` do not use this gate.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.shouldBeginEditing = ^BOOL(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model is about to enter editing mode.");
///
///     return YES;     // allow editing to begin
/// };
/// ```
@property (nonatomic, copy, nullable) SCConditionalModelAction_Block shouldBeginEditing;

/// Reports completion of the model's accepted editing-entry path.
/// The block receives the model after table/section editing state is updated.
/// This is not a universal observer of direct UIKit editing changes.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didBeginEditing = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model has entered editing mode.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didBeginEditing;

/// Approves leaving editing through the model's editing setter.
/// The block receives the model and returns `YES` to continue or `NO` to leave
/// editing unchanged. No hook means `YES`. Direct UIKit changes that bypass
/// `setTableViewEditing:animated:` do not use this gate.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.shouldEndEditing = ^BOOL(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model is about to exit editing mode.");
///
///     return YES;     // allow editing to end
/// };
/// ```
@property (nonatomic, copy, nullable) SCConditionalModelAction_Block shouldEndEditing;

/// Reports completion of the model's accepted editing-exit path.
/// The block receives the model after table/section editing state is updated.
/// This is not a universal observer of direct UIKit editing changes.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didEndEditing = ^(SCTableViewModel *tableModel)
/// {
///     NSLog(@"The model has exited editing mode.");
/// };
/// ```
@property (nonatomic, copy, nullable) SCModelAction_Block didEndEditing;


/// Reports a row move handled by the model.
/// The block receives the model, the cell resolved at the destination after
/// section movement, and the original and destination index paths. It runs
/// after the section's move handler.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didMoveCell = ^(SCTableViewModel *tableModel, SCTableViewCell *cell, NSIndexPath *fromIndexPath, NSIndexPath *toIndexPath)
/// {
///     NSLog(@"A cell has been moved from:%@ to:%@.", fromIndexPath, toIndexPath);
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidMoveCellAction_Block didMoveCell;


#pragma mark - SCArrayOfItemsModel Actions


/// Customizes an items model's synchronous fetch result.
/// The block receives the items model and its mutable items array; entries may
/// be data objects or cell objects and can be added, removed, or reordered
/// before section generation. The current asynchronous model-fetch path does
/// not dispatch this hook. Section-level fetch hooks also run when the model
/// distributes items into sections.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didFetchItemsFromStore = ^(SCArrayOfItemsModel *itemsModel, NSMutableArray *items)
/// {
///     // Add a button cell at the end of the fetched items list
///     SCTableViewCell *buttonCell = [SCTableViewCell cellWithText:@"Tap me!" textAlignment:NSTextAlignmentCenter];
///     buttonCell.cellActions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
///     {
///         NSLog(@"buttonCell tapped!");
///     };
///
///     [items addObject:buttonCell];
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidFetchModelItemsAction_Block didFetchItemsFromStore;


/// Groups an items model's items by section header title.
/// The block receives the items model, item, and index in the current full or
/// filtered items array. Return a title to group matching items, or nil for an
/// untitled section. Without a hook items use an untitled section;
/// fetch-indicator cells bypass the hook. STV creates missing sections as
/// needed.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.sectionHeaderTitleForItem = ^NSString*(SCArrayOfItemsModel *itemsModel, NSObject *item, NSUInteger itemIndex)
/// {
///     NSString *objectName = (NSString *)[item valueForKey:@"firstName"];
///
///     // Empty names belong to the untitled section.
///     return objectName.length ? [[objectName substringToIndex:1] uppercaseString] : nil;
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionHeaderTitleForItemAction_Block sectionHeaderTitleForItem;

/// Precreates titled sections before distributing an items model's items.
/// The block receives the items model and returns an array of header strings. A
/// nil or empty result precreates no sections; item grouping can still create
/// sections through `sectionHeaderTitleForItem`. This is not a replacement for
/// the section-index title array.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.sectionHeaderTitles = ^NSArray*(SCArrayOfItemsModel *itemsModel)
/// {
///     return @[@"Section A", @"Section B", @"Section C"];
/// };
/// ```
@property (nonatomic, copy, nullable) SCSectionHeaderTitlesAction_Block sectionHeaderTitles;


/// Post-processes an object-items model's computed search results.
/// The block receives the items model, search text, and possibly nil computed
/// results. Return a replacement array, an empty array for no matches, or nil
/// to keep the computed result. This is called by the `SCArrayOfObjectsModel`
/// search implementation, not by the base items model's abstract search
/// behavior.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// modelActions.didComputeSearchResults = ^NSArray*(SCArrayOfItemsModel *itemsModel, NSString *searchText, NSArray *searchResults)
/// {
///     return myCustomSearchResults;
/// };
/// ```
@property (nonatomic, copy, nullable) SCDidComputeSearchResultsAction_Block didComputeSearchResults;


#pragma mark - Miscellaneous

/// The weak model association for this actions object, or nil.
/// The model sets this association when it creates its actions object. It does
/// not retain the model and is not copied by `setActionsTo:overrideExisting:`.
@property (nonatomic, weak, nullable) SCTableViewModel *ownerTableViewModel;

/// Copies the source's nonnil hooks into this actions object.
/// Nil source hooks never clear an existing destination hook, even when
/// overriding. Each assigned block is copied; the source object itself is not
/// retained.
/// - Parameter actions: Source actions, or nil to leave all hooks unchanged.
/// - Parameter override: `YES` replaces existing hooks where the source is
/// nonnil; `NO` fills only currently nil hooks.
- (void)setActionsTo:(SCModelActions *_Nullable)actions overrideExisting:(BOOL)override;

@end

NS_ASSUME_NONNULL_END
