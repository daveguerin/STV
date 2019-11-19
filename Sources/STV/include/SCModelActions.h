/*
 *  SCModelActions.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import <Foundation/Foundation.h>

@class SCTableViewCell;
@class SCTableViewModel;
@class SCArrayOfItemsModel;
@class SCTableViewSection;

typedef void(^SCModelAction_Block)(SCTableViewModel *tableModel);
typedef BOOL(^SCConditionalModelAction_Block)(SCTableViewModel *tableModel);
typedef void(^SCDidAddSectionAction_Block)(SCTableViewModel *tableModel, SCTableViewSection *section, NSUInteger sectionIndex);
typedef void(^SCDidRemoveSectionAction_Block)(SCTableViewModel *tableModel, NSUInteger sectionIndex);
typedef void(^SCSortSectionsAction_Block)(SCTableViewModel *tableModel, NSMutableArray *sections);
typedef NSInteger(^SCSectionForSectionIndexTitleAction_Block)(SCTableViewModel *tableModel, NSString *title, NSInteger index);

typedef void(^SCDidFetchModelItemsAction_Block)(SCArrayOfItemsModel *itemsModel, NSMutableArray *items);
typedef NSString*(^SCSectionHeaderTitleForItemAction_Block)(SCArrayOfItemsModel *itemsModel, NSObject *item, NSUInteger itemIndex);
typedef NSArray*(^SCSectionHeaderTitlesAction_Block)(SCArrayOfItemsModel *itemsModel);
typedef NSArray*(^SCDidComputeSearchResultsAction_Block)(SCArrayOfItemsModel *itemsModel, NSString *searchText, NSArray *searchResults);
typedef void(^SCDidMoveCellAction_Block)(SCTableViewModel *tableModel, SCTableViewCell *cell, NSIndexPath *fromIndexPath, NSIndexPath *toIndexPath);


/****************************************************************************************/
/*	class SCModelActions	*/
/****************************************************************************************/ 
/**	
 This class hosts a set of model action blocks. Once an action is set to a desired code block, it will execute the block as soon as the action occurs.
 
 @see SCSectionActions, SCCellActions.
 */

@interface SCModelActions : NSObject



//////////////////////////////////////////////////////////////////////////////////////////
/// @name Actions
//////////////////////////////////////////////////////////////////////////////////////////

/** Action gets called as soon as a section has been added to the model.
 
 This action is typically used to customize automatically generated sections (e.g.: sections generated by SCArrayOfObjectsModel).
 
 Example:
 
    // Objective-C
    modelActions.didAddSection = ^(SCTableViewModel *tableModel, SCTableViewSection *section, NSUInteger sectionIndex)
    {
        // use custom data sorting for the first section in a Core Data SCArrayOfObjectsModel
        if(sectionIndex==0 && [section isKindOfClass:[SCArrayOfObjectsSection class]])
        {
            SCArrayOfObjectsSection *objectsSection = (SCArrayOfObjectsSection *)section;
            SCCoreDataFetchOptions *oldFetchOptions = (SCCoreDataFetchOptions *)objectsSection.dataFetchOptions;
            objectsSection.dataFetchOptions = [SCCoreDataFetchOptions optionsWithSortKey:oldFetchOptions.sortKey sortAscending:FALSE filterPredicate:oldFetchOptions.filterPredicate];
        }
    };
 
    // Swift
    modelActions.didAddSection =
    {
        (tableModel, section, sectionIndex) in
 
        // use custom data sorting for the first section in a Core Data SCArrayOfObjectsModel
        if let objectsSection = section as? SCArrayOfObjectsSection
        {
            if sectionIndex==0
            {
                let oldFetchOptions = objectsSection.dataFetchOptions
                objectsSection.dataFetchOptions = SCCoreDataFetchOptions(sortKey: oldFetchOptions.sortKey, sortAscending: false, filterPredicate: oldFetchOptions.filterPredicate)
            }
        }
    }
 
 */
@property (nonatomic, copy) SCDidAddSectionAction_Block didAddSection;

/** Action gets called as soon as a section has been removed from the model.
 
 Example:
 
    // Objective-C
    modelActions.didRemoveSection = ^(SCTableViewModel *tableModel, NSUInteger sectionIndex)
    {
        NSLog(@"Section at index '%i' has been removed.", sectionIndex);
    };
 
    // Swift
    modelActions.didRemoveSection =
    {
        (tableModel, sectionIndex) in
 
        NSLog("Section at index '%i' has been removed.", sectionIndex)
    }
 
 */
@property (nonatomic, copy) SCDidRemoveSectionAction_Block didRemoveSection;

/** Action gets called to give you a chance to provide custom section sorting.
 
 Example:
 
    // Objective-C
    modelActions.sortSections = ^(SCTableViewModel *tableModel, NSMutableArray *sections)
    {
        // Rearrange the 'sections' mutable array here as you see fit
    };
 
    // Swift
    modelActions.sortSections =
    {
        (tableModel, sections) in
 
        // Rearrange the 'sections' mutable array here as you see fit
    }
 
 @note To have the model automatically sort its sections alphabetically, simply set its 'autoSortSections' property to TRUE.
 
 */
@property (nonatomic, copy) SCSortSectionsAction_Block sortSections;

/** Action gets called to give you a chance  to return the index of the section having the given title and section title index.
 
 Example:
 
    // Objective-C
    modelActions.sectionForSectionIndexTitle = ^NSInteger(SCTableViewModel *tableModel, NSString *title, NSInteger index)
    {
        if(index < tableModel.sectionCount)
           return index;
        // else return the last section index
        return tableModel.sectionCount-1;
    };
 
    // Swift
    modelActions.sectionForSectionIndexTitle =
    {
        (tableModel, title, index)->NSInteger in
 
        if index < tableModel.sectionCount
        {
           return index
        }
        // else return the last section index
        return tableModel.sectionCount-1
    }
 
 @note 'index' is an index number identifying a section title in the SCTableViewModel's 'sectionIndexTitles' array.
 
 */
@property (nonatomic, copy) SCSectionForSectionIndexTitleAction_Block sectionForSectionIndexTitle;

/** Action gets called as soon as the model's table view has finished loading all its cells.
 
 Example:
 
    // Objective-C
    modelActions.didFinishLoadingCells = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The table view has finished loading cells.");
    };
 
    // Swift
    modelActions.didFinishLoadingCells =
    {
        (tableModel) in
 
        NSLog("The table view has finished loading cells.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didFinishLoadingCells;

/** Action gets called as soon as the model has refreshed its contents after returning from a pull-to-refresh operation.
 
 Example:
 
    // Objective-C
    modelActions.didRefresh = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model has been refreshed.");
    };
 
    // Swift
    modelActions.didRefresh =
    {
        (tableModel) in
 
        NSLog("The model has been refreshed.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didRefresh;


/** Action gets called after the model's table view has started scrolling.
 
 Example:
 
    // Objective-C
    modelActions.didScroll = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model's table view has started scrolling.");
    };
 
    // Swift
    modelActions.didScroll =
    {
        (tableModel) in
 
        NSLog("The model's table view has started scrolling.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didScroll;

/** Action gets called after the model's table view has ended dragging.
 
 Example:
 
    // Objective-C
    modelActions.didEndDragging = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model's table view has ended dragging with decelerating=%i.", tableModel.tableView.decelerating);
    };
 
    // Swift
    modelActions.didEndDragging =
    {
        (tableModel) in
 
        NSLog("The model's table view has ended dragging with decelerating=%i.", tableModel.tableView.decelerating)
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didEndDragging;

/** Action gets called after the model's table view has ended decelerating.
 
 Example:
 
    // Objective-C
    modelActions.didEndDecelerating = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model's table view has ended decelerating.");
    };
 
    // Swift
    modelActions.didEndDecelerating =
    {
        (tableModel) in
 
        NSLog("The model's table view has ended decelerating.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didEndDecelerating;


/** Action gets called as the model's table view is about to enter editing mode. Return NO to prevent the table view from entering editing mode, otherwise return YES.
 
 Example:
 
    // Objective-C
    modelActions.shouldBeginEditing = ^BOOL(SCTableViewModel *tableModel)
    {
        NSLog(@"The model is about to enter editing mode.");
 
        return YES;     // allow editing to begin
    };
 
    // Swift
    modelActions.shouldBeginEditing =
    {
        (tableModel)->Bool in
 
        NSLog("The model is about to enter editing mode.")
 
        return true     // allow editing to begin
    }
 
 */
@property (nonatomic, copy) SCConditionalModelAction_Block shouldBeginEditing;

/** Action gets called after the model's table view has entered editing mode.
 
 Example:
 
    // Objective-C
    modelActions.didBeginEditing = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model has entered editing mode.");
    };
 
    // Swift
    modelActions.didBeginEditing =
    {
        (tableModel) in
 
        NSLog("The model has entered editing mode.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didBeginEditing;

/** Action gets called as the model's table view is about to exit editing mode. Return NO to prevent the table view from exiting editing mode, otherwise return YES.
 
 Example:
 
    // Objective-C
    modelActions.shouldEndEditing = ^BOOL(SCTableViewModel *tableModel)
    {
        NSLog(@"The model is about to exit editing mode.");
 
        return YES;     // allow editing to end
    };
 
    // Swift
    modelActions.shouldEndEditing =
    {
        (tableModel)->Bool in
 
        NSLog("The model is about to exit editing mode.")
 
        return true     // allow editing to end
    }
 
 */
@property (nonatomic, copy) SCConditionalModelAction_Block shouldEndEditing;

/** Action gets called after the model's table view has exited editing mode.
 
 Example:
 
    // Objective-C
    modelActions.didEndEditing = ^(SCTableViewModel *tableModel)
    {
        NSLog(@"The model has exited editing mode.");
    };
 
    // Swift
    modelActions.didEndEditing =
    {
        (tableModel) in
 
        NSLog("The model has exited editing mode.")
    }
 
 */
@property (nonatomic, copy) SCModelAction_Block didEndEditing;


/** Action gets called after any of the model's sections' cells has moved.
 
 Example:
 
    // Objective-C
    modelActions.didMoveCell = ^(SCTableViewModel *tableModel, SCTableViewCell *cell, NSIndexPath *fromIndexPath, NSIndexPath *toIndexPath)
    {
        NSLog(@"A cell has been moved from:%@ to:%@.", fromIndexPath, toIndexPath);
    };
 
    // Swift
    modelActions.didMoveCell =
    {
        (tableModel, cell, fromIndexPath, toIndexPath) in
 
        NSLog("A cell has been moved from:%@ to:%@.", fromIndexPath, toIndexPath)
    }
 
 */
@property (nonatomic, copy) SCDidMoveCellAction_Block didMoveCell;




//////////////////////////////////////////////////////////////////////////////////////////
/// @name SCArrayOfItemsModel Actions
//////////////////////////////////////////////////////////////////////////////////////////


/** Action gets called as soon as the model has retrieved its items from their data store.
 
 This action is typically used to customize the 'items' array after it has been fetched from the data store. Items can be added, removed, or rearranged. The added items can either be objects that are suppored by the data store, or normal SCTableViewCell (or subclass) objects.
 
 Example:
 
    // Objective-C
    modelActions.didFetchItemsFromStore = ^(SCArrayOfItemsModel *itemsModel, NSMutableArray *items)
    {
        // Add a button cell at the end of the fetched items list
        SCTableViewCell *buttonCell = [SCTableViewCell cellWithText:@"Tap me!" textAlignment:NSTextAlignmentCenter];
        buttonCell.cellActions.didSelect = ^(SCTableViewCell *cell, NSIndexPath *indexPath)
        {
            NSLog(@"buttonCell tapped!");
        };
 
        [items addObject:buttonCell];
    };
 
    // Swift
    modelActions.didFetchItemsFromStore =
    {
        (itemsModel, items) in
 
        // Add a button cell at the end of the fetched items list
 
        let buttonCell = SCTableViewCell(text: "Tap me!", textAlignment: .Center)
        buttonCell.cellActions.didSelect =
        {
            (cell, indexPath) in
            
            NSLog("buttonCell tapped!")
        }
 
        items.addObject(buttonCell)
    }
 
 @note This action is only applicable to SCArrayOfItemsModel subclasses, such as SCArrayOfObjectsModel. 
 
 */
@property (nonatomic, copy) SCDidFetchModelItemsAction_Block didFetchItemsFromStore;


/** Action gets called when SCArrayOfItemsModel requests a section header title for each of its items.
 
 Use this action to control how the automatically generated sections are created. In the following example, a new section will be created for each group of items sharing the same first letter (i.e.: items will be grouped alphabetically).
 
 Example:
 
    // Objective-C
    modelActions.sectionHeaderTitleForItem = ^NSString*(SCArrayOfItemsModel *itemsModel, NSObject *item, NSUInteger itemIndex)
    {
        NSString *objectName = (NSString *)[item valueForKey:@"firstName"];
 
        // Return first charcter of objectName
        return [[objectName substringToIndex:1] uppercaseString];
    };
 
    // Swift
    modelActions.sectionHeaderTitleForItem =
    {
        (itemsModel, item, itemIndex)->String! in
 
        let objectName = item.valueForKey("firstName") as! String
 
        // Return first charcter of objectName
        return objectName.substringToIndex(advance(objectName.startIndex, 1)).uppercaseString
    }
 */
 @property (nonatomic, copy) SCSectionHeaderTitleForItemAction_Block sectionHeaderTitleForItem;

/** Implement this action to provide custom sections for SCArrayOfItemsModel.
 
 Example:
 
    // Objective-C
    modelActions.sectionHeaderTitles = ^NSArray*(SCArrayOfItemsModel *itemsModel)
    {
        return @[@"Section A", @"Section B", @"Section C"];
    };
 
    // Swift
    modelActions.sectionHeaderTitles =
    {
        (tableModel)->[AnyObject] in
            
        return ["Section A", "Section B", "Section C"]
    }
 */
@property (nonatomic, copy) SCSectionHeaderTitlesAction_Block sectionHeaderTitles;


/** Action gets called when SCArrayOfItemsModel has computed the search results for the given searchText.
 
 Use this action is typically used to customize the automatically computed searchResults array.
 
 Example:
 
    // Objective-C
    modelActions.didComputeSearchResults = ^NSArray*(SCArrayOfItemsModel *itemsModel, NSString *searchText, NSArray *searchResults)
    {
        return myCustomSearchResults;
    };
 
    // Swift
    modelActions.didComputeSearchResults =
    {
        (itemsModel, searchText, searchResults)->[AnyObject] in
 
        return myCustomSearchResults
    }
 */
@property (nonatomic, copy) SCDidComputeSearchResultsAction_Block didComputeSearchResults;



//////////////////////////////////////////////////////////////////////////////////////////
/// @name Miscellaneous
//////////////////////////////////////////////////////////////////////////////////////////

/** The owner table view model of the actions object.
 *
 * @warning This property gets set automatically by the framework, you should never set this property manually */
@property (nonatomic, weak) SCTableViewModel *ownerTableViewModel;

/** Method assigns all the actions of another 'SCModelActions' class to the current one.
 
 @param actions The source 'SCModelActions' class.
 @param override Set to TRUE to override any existing actions, otherwise set to FALSE.
 */
- (void)setActionsTo:(SCModelActions *)actions overrideExisting:(BOOL)override;

@end
