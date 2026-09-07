/*
 *  SCTableViewModel.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019 dgApps. All rights reserved.
 *
 */

#import <objc/message.h>

#import "SCTableViewModel.h"

#import "SCStringDefinition.h"
#import "SCArrayStore.h"
#import "SCEntityDefinition.h"
#import "SCCoreDataStore.h"
#import "SCUserDefaultsStore.h"


@interface SCTableViewSection (SCSupplementaryLayout)
- (UITableViewHeaderFooterView *)supplementaryViewForHeader:(BOOL)isHeader hideTitle:(BOOL)hideTitle;
- (void)invalidateCellSizingPolicy;
@end

@interface SCModelGeometryObserver : UIView
@property (nonatomic, copy) void (^geometryDidChange)(void);
@end

@implementation SCModelGeometryObserver
- (void)layoutSubviews
{
    [super layoutSubviews];
    if (self.geometryDidChange) {
        self.geometryDidChange();
    }

}
- (void)safeAreaInsetsDidChange
{
    [super safeAreaInsetsDidChange];
    if (self.geometryDidChange) {
        self.geometryDidChange();
    }

}
@end

@interface SCTableViewModel ()
{
    BOOL _loading;
    __weak UITableView *_keyboardInsetTable;
    CGRect _keyboardEndFrame;
    SCModelGeometryObserver *_keyboardGeometryObserver;
    BOOL _reconcilingKeyboardInset;
    NSMutableSet<SCTableViewCell *> *_pendingHeightCells;
    BOOL _heightUpdateScheduled;
    BOOL _pendingHeightAnimation;
}

- (void)removeKeyboardInset;
- (void)reconcileKeyboardInset;
- (void)scheduleHeightUpdates;
- (void)applyPendingHeightUpdates;
- (void)prepareCell:(SCTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath;

- (void)prepareSectionForOwnership:(SCTableViewSection *)section;
- (void)callDidAddSectionActionsForSection:(SCTableViewSection *)section;
- (void)addSectionForObject:(NSObject *)object withDataStore:(SCDataStore *)store usingGroup:(SCPropertyGroup *)group newObject:(BOOL)newObject;
- (SCTableViewSection *)getSectionForPropertyDefinition:(SCPropertyDefinition *)propertyDef withBoundObject:(NSObject *)object withDataStore:(SCDataStore *)store;

- (void)tableAnimationEnded:(NSString*)animationID finished:(NSNumber *)finished contextInfo:(void *)context;

@end




@implementation SCTableViewModel

@synthesize masterModel;
@synthesize activeDetailModel;
@synthesize viewController;
@synthesize editButtonItem;
@synthesize autoResizeForKeyboard;
@synthesize sectionIndexTitles;
@synthesize autoGenerateSectionIndexTitles;
@synthesize autoSortSections;
@synthesize hideSectionHeaderTitles;
@synthesize lockCellSelection;
@synthesize tag;
@synthesize activeCell;
@synthesize activeCellIndexPath;
@synthesize activeCellControl;
@synthesize commitButton;
@synthesize swipeToDeleteActive;
@synthesize enablePullToRefresh;


+ (instancetype)modelWithTableView:(UITableView *)tableView
{
	return [[[self class] alloc] initWithTableView:tableView];
}


- (instancetype)init
{
    if ( (self=[super init])  ) {
        _loading = FALSE;
        
        _tableView = nil;
        
        lastReturnedCellIndexPath = nil;
        lastReturnedCell = nil;
        lastVisibleCellIndexPath = nil;
        target = nil;
        action = nil;
        masterModel = nil;
        activeDetailModel = nil;
		
        editButtonItem = nil;
        sectionIndexTitles = nil;
        autoGenerateSectionIndexTitles = FALSE;
        autoSortSections = FALSE;
        hideSectionHeaderTitles = FALSE;
        lockCellSelection = FALSE;
        tag = 0;
        
        enablePullToRefresh = FALSE;
        [self setRefreshControl:[[UIRefreshControl alloc] init]]; // call setter
        
        sections = [[NSMutableArray alloc] init];
        activeCell = nil;
        activeCellIndexPath = nil;
        activeCellControl = nil;
		
        commitButton = nil;
		
        keyboardShown = FALSE;
        keyboardOverlap = 0;
        /*
        hermes:
        UIKit owns row measurement; numeric height options configure content
        constraints rather than replacing automatic table sizing.
        */
        _defaultCellSizingMode = SCTableViewCellSizingModeAutomatic;
        _defaultCellHeight = 44.0;
        swipeToDeleteActive = FALSE;
        
        _detailViewController = nil;
        
        _modelActions = [[SCModelActions alloc] init];
        _modelActions.ownerTableViewModel = self;
        _sectionActions = [[SCSectionActions alloc] init];
        _cellActions = [[SCCellActions alloc] init];
        
        // Register with the shared model center
        [[SCModelCenter sharedModelCenter] registerModel:self];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
{
    if ( (self=[self init]) ) {
        [self setTableView:tableView];
    }

    return self;
}

- (void)dealloc
{
    [self removeKeyboardInset];
	// Unregister from the shared model center
    [[SCModelCenter sharedModelCenter] unregisterModel:self];
}


- (void)setTableView:(UITableView *)tableView
{
    if (_tableView != tableView) {
        [self removeKeyboardInset];
        _tableView.refreshControl = nil;
    }

    _tableView = tableView;

    if (_tableView) {
        // Remove tableView from any previos model
        if (_tableView.dataSource && _tableView.dataSource!=self && [_tableView.dataSource isKindOfClass:[SCTableViewModel class]]) {
            [(SCTableViewModel *)_tableView.dataSource setTableView:nil];
        }

        _tableView.dataSource = self;
        _tableView.delegate = self;
        _tableView.allowsSelectionDuringEditing = TRUE;
    }

    if ([self.viewController isKindOfClass:[UITableViewController class]]) {
        self.autoResizeForKeyboard = FALSE;
    }

    else {
        self.autoResizeForKeyboard = TRUE;
    }

    if (self.enablePullToRefresh) {
        _tableView.refreshControl = self.refreshControl;
    }

    _tableView.rowHeight = UITableViewAutomaticDimension;
    _tableView.selfSizingInvalidation = UITableViewSelfSizingInvalidationEnabledIncludingConstraints;
    _tableView.estimatedRowHeight = 44.0f;

    _tableView.sectionHeaderHeight = UITableViewAutomaticDimension;
    _tableView.sectionFooterHeight = UITableViewAutomaticDimension;
    _tableView.estimatedSectionHeaderHeight = 28.0;
    _tableView.estimatedSectionFooterHeight = 28.0;
    [self reconcileKeyboardInset];
}

- (void)setDefaultCellHeight:(CGFloat)height
{
    _defaultCellHeight = height;
    self.defaultCellSizingMode = height > 0 ? SCTableViewCellSizingModeFixed : SCTableViewCellSizingModeAutomatic;
}

- (void)setDefaultCellSizingMode:(SCTableViewCellSizingMode)mode
{
    _defaultCellSizingMode = mode;
    for(SCTableViewSection *section in sections) {
        [section invalidateCellSizingPolicy];
    }

    for(SCTableViewCell *cell in self.tableView.visibleCells)
    {
        [cell setNeedsUpdateConstraints];
        [cell updateConstraintsIfNeeded];
    }
    [self reloadAfterSizingPolicyChange];
}

- (void)reloadAfterSizingPolicyChange
{
    UITableView *tableView = self.tableView;
    if (!tableView) {
        return;
    }

    if (tableView.hasUncommittedUpdates) {
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf reloadAfterSizingPolicyChange];
        });
        return;
    }

    [tableView reloadData];
}

- (UITableView *)modeledTableView
{
    return [self tableView];
}

- (void)setModeledTableView:(UITableView *)modeledTableView
{
    [self setTableView:modeledTableView];
}

- (UITableView *)trueTableView
{
    return _tableView;
}

- (UIViewController *)viewController
{
    if (!self.trueTableView) {
        return nil;
    }


    id vc = [self.trueTableView nextResponder];
    while(![vc isKindOfClass:[UIViewController class]] && vc!=nil)
    {
        vc = [vc nextResponder];
    }
    
    return vc;
}

- (void)setInputAccessoryView:(SCInputAccessoryView *)accessoryView
{
    _inputAccessoryView = accessoryView;
    _inputAccessoryView.delegate = self;
}

- (void)setEnablePullToRefresh:(BOOL)enable
{
    enablePullToRefresh = enable;
    self.tableView.refreshControl = enable ? self.refreshControl : nil;
}

- (void)setRefreshControl:(UIRefreshControl *)refreshControl
{
    _refreshControl = refreshControl;
    
    [self.refreshControl addTarget:self action:@selector(pullToRefreshDidStartLoading) forControlEvents:UIControlEventValueChanged];

    if (self.enablePullToRefresh) {
        [self setEnablePullToRefresh:TRUE];  // call setter to configure refreshControl
    }

}

- (void)setDetailViewController:(UIViewController *)detailViewController
{
    _detailViewController = detailViewController;
    
    SCTableViewModel *detailModel = nil;
    if ([detailViewController isKindOfClass:[SCTableViewController class]]) {
        detailModel = [(SCTableViewController *)detailViewController tableViewModel];
    }

    else if ([detailViewController isKindOfClass:[SCViewController class]]) {
        detailModel = [(SCViewController *)detailViewController tableViewModel];
    }

    if (detailModel) {
        [self configureDetailModel:detailModel];
    }

}

- (BOOL)live
{
    BOOL live;
    /*
     The if was previously

     ([self.tableView.visibleCells count] && !_loading && !(self.tableView.dragging || self.tableView.decelerating))

     but [self.tableView.visibleCells count]  was causing an assert on iOS 13:

     "Attempted to access the table view's visibleCells while they were in the
     process of being updated, which is not allowed."

     dgApps
     */


    if (!_loading && !(self.tableView.dragging || self.tableView.decelerating)) {
        live = TRUE;
    }

    else {
        live = FALSE;
    }

    return live;
}

- (void)enterLoadingMode
{
    _loading = TRUE;
}

- (void)exitLoadingMode
{
    _loading = FALSE;
    [self clearLastReturnedCellData];
}

- (void)clearLastReturnedCellData
{
    if (lastReturnedCellIndexPath) {
        lastReturnedCellIndexPath = nil;
        
        lastReturnedCell.configured = FALSE;
        lastReturnedCell = nil;
    }

}

- (void)configureDetailModel:(SCTableViewModel *)detailModel
{
    detailModel.masterModel = self;
    
    detailModel.tag = self.tag + 1;
}

- (NSArray *)sectionIndexTitles
{
    if (!self.autoGenerateSectionIndexTitles) {
        return sectionIndexTitles;
    }

    // Generate sectionIndexTitles
	NSMutableArray *titles = [NSMutableArray arrayWithCapacity:self.sectionCount];
	for(SCTableViewSection *section in sections) {
        if ([section.headerTitle length]) {
            // Add first letter of the header title to section titles
            [titles addObject:[section.headerTitle substringToIndex:1]];
        }

	}


    return titles;
}

- (void)setAutoSortSections:(BOOL)autoSort
{
	autoSortSections = autoSort;
    if (autoSort) {
        [sections sortUsingSelector:@selector(compare:)];
    }

}


- (void)rollbackToInitialCellValues
{
    for(SCTableViewSection *section in sections)
    {
        [section rollbackToInitialCellValues];
    }
}

- (void)reloadCellsIfNeeded
{
    for(SCTableViewSection *section in sections)
    {
        if (![section isKindOfClass:[SCObjectSection class]]) {
            continue;
        }

        [(SCObjectSection *)section reloadCellsIfNeeded];
    }
}


- (void)setActiveCell:(SCTableViewCell *)cell
{
    if (activeCell == cell) {
        return;
    }

    if (activeCell) {
        if (activeCell.cellActions.willDeselect) {
            /*
            Invoke the exact block that was checked, using the cell being
            deselected; STV 6 dereferenced the incoming cell's possibly nil
            block.
            */
            activeCell.cellActions.willDeselect(activeCell, self.activeCellIndexPath);
        }

        else if (activeCell.ownerSection.cellActions.willDeselect) {
            /*
                Section-level deselection callbacks describe the previous active
                cell, so pass that cell rather than its replacement.
                */
            activeCell.ownerSection.cellActions.willDeselect(activeCell, self.activeCellIndexPath);
        }

        else if (self.cellActions.willDeselect) {
            /*
                    Model-level deselection callbacks receive the cell actually
                    being deselected.
                    */
            self.cellActions.willDeselect(activeCell, self.activeCellIndexPath);
        }

        [activeCell willDeselectCell];
        
        [self.tableView deselectRowAtIndexPath:self.activeCellIndexPath animated:NO];

        if (activeCell.cellActions.didDeselect) {
            /*
            Deselect callbacks belong to the previous active cell, not the
            replacement cell.
            */
            activeCell.cellActions.didDeselect(activeCell, self.activeCellIndexPath);
        }

        else if (activeCell.ownerSection.cellActions.didDeselect) {
            /*
                Keep section callback ownership and callback arguments aligned
                during active-cell replacement.
                */
            activeCell.ownerSection.cellActions.didDeselect(activeCell, self.activeCellIndexPath);
        }

        else if (self.cellActions.didDeselect) {
            /*
                    Keep model callback ownership and callback arguments aligned
                    during active-cell replacement.
                    */
            self.cellActions.didDeselect(activeCell, self.activeCellIndexPath);
        }

        [activeCell didDeselectCell];
    }

    activeCell = cell;
    activeCellIndexPath = [self indexPathForCell:activeCell];

    if (![activeCell isKindOfClass:[SCCustomCell class]]) {
        self.activeCellControl = nil;
    }

    if (activeCell) {
        if (!activeCell.selected) {
            if (activeCell.cellActions.willSelect) {
                /*
                Call the checked active-cell block directly so the callback
                cannot become a null block-function invocation.
                */
                activeCell.cellActions.willSelect(activeCell, self.activeCellIndexPath);
            }

            else if (activeCell.ownerSection.cellActions.willSelect) {
                activeCell.ownerSection.cellActions.willSelect(cell, self.activeCellIndexPath);
            }

            else if (activeCell.ownerTableViewModel.cellActions.willSelect) {
                self.cellActions.willSelect(cell, self.activeCellIndexPath);
            }

            activeCell.selected = YES;

            if (activeCell.cellActions.didSelect) {
                /*
                Keep callback ownership and callback arguments aligned for
                analyzer safety and predictable selection behavior.
                */
                activeCell.cellActions.didSelect(activeCell, self.activeCellIndexPath);
            }

            else if (activeCell.ownerSection.cellActions.didSelect) {
                activeCell.ownerSection.cellActions.didSelect(cell, self.activeCellIndexPath);
            }

            else if (activeCell.ownerTableViewModel.cellActions.didSelect) {
                self.cellActions.didSelect(cell, self.activeCellIndexPath);
            }

        }

        [self.tableView scrollToRowAtIndexPath:activeCellIndexPath
                                     atScrollPosition:UITableViewScrollPositionNone
                                             animated:YES];
        [self.tableView selectRowAtIndexPath:activeCellIndexPath animated:NO scrollPosition:UITableViewScrollPositionNone];
    }

}

- (void)setActiveCellControl:(UIResponder *)control
{
    activeCellControl = control;
}

- (void)setCommitButton:(UIBarButtonItem *)button
{
	commitButton = button;
	
	commitButton.enabled = self.valuesAreValid;
}

- (void)setEditButtonItem:(UIBarButtonItem *)barButtonItem
{
	editButtonItem = barButtonItem;
	
	editButtonItem.target = self;
	editButtonItem.action = @selector(didTapEditButtonItem);
}

- (void)valueChangedForSectionAtIndex:(NSUInteger)index
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if (target) {
        [target performSelector:action];
    }

#pragma clang diagnostic pop
    
    SCTableViewSection *section = [self sectionAtIndex:index];
    if (section.sectionActions.valueChanged) {
        section.sectionActions.valueChanged(section, index);
    }

    else if (self.sectionActions.valueChanged) {
        self.sectionActions.valueChanged(section, index);
    }

}

- (void)valueChangedForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (!indexPath) {
        return;
    }

    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    /*
    Resolve the authoritative model cell rather than asking UITableView for a
    visible instance, which may be nil during detail dismissal and suppress
    value callbacks.
    */
	SCTableViewCell *cell = [self cellAtIndexPath:indexPath];
    if (cell != self.activeCell) {
        if (self.activeCell.autoResignFirstResponder) {
            [self.activeCell resignFirstResponder];
        }

        self.activeCell = cell;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
    if (target) {
        [target performSelector:action];
    }

#pragma clang diagnostic pop

    if (self.commitButton) {
        self.commitButton.enabled = self.valuesAreValid;
    }

    if (cell.cellActions.valueChanged) {
        cell.cellActions.valueChanged(cell, indexPath);
    }

    else if (section.cellActions.valueChanged) {
        section.cellActions.valueChanged(cell, indexPath);
    }

    else if (self.cellActions.valueChanged) {
        self.cellActions.valueChanged(cell, indexPath);
    }

}

- (void)setTargetForModelModifiedEvent:(id)_target action:(SEL)_action
{
	target = _target;
	action = _action;
}

- (void)didTapEditButtonItem
{	
	BOOL editing = !self.tableView.editing;		// toggle editing state
	
	[self setTableViewEditing:editing animated:TRUE];
}

- (void)pullToRefreshDidStartLoading
{
    if (self.detailViewController) {
        // Clear detail view
        if ([self.detailViewController isKindOfClass:[SCTableViewController class]]) {
            SCTableViewController *detailVC = (SCTableViewController *)self.detailViewController;
            [detailVC loseFocus];
        }

        else if ([self.detailViewController isKindOfClass:[SCViewController class]]) {
            SCViewController *detailVC = (SCViewController *)self.detailViewController;
            [detailVC loseFocus];
        }

    }


    [self reloadBoundValues];
    [self.tableView reloadData];
    
    [self.refreshControl endRefreshing];

    if (self.modelActions.didRefresh) {
        self.modelActions.didRefresh(self);
    }

}

- (NSUInteger)sectionCount
{
	return sections.count;
}

- (void)prepareSectionForOwnership:(SCTableViewSection *)section
{
    section.ownerTableViewModel = self;

    if (self.tableView.editing && [section isKindOfClass:[SCObjectSection class]]) {
        [(SCObjectSection *)section generateCellsForEditingState:TRUE];
    }

}

- (void)addSection:(SCTableViewSection *)section
{
	[self prepareSectionForOwnership:section];
    
	[sections addObject:section];


    if (self.modelActions.sortSections) {
        self.modelActions.sortSections(self, sections);
    }

    else if (self.autoSortSections) {
        [sections sortUsingSelector:@selector(compare:)];
    }

    [self callDidAddSectionActionsForSection:section];
}

- (void)insertSection:(SCTableViewSection *)section atIndex:(NSUInteger)index
{
	[self prepareSectionForOwnership:section];
    
	[sections insertObject:section atIndex:index];
    
    [self callDidAddSectionActionsForSection:section];
}

- (void)callDidAddSectionActionsForSection:(SCTableViewSection *)section
{
    NSUInteger sectionIndex = [sections indexOfObjectIdenticalTo:section];

    if (self.modelActions.didAddSection) {
        self.modelActions.didAddSection(self, section, sectionIndex);
    }

    if (section.sectionActions.didAddToModel) {
        section.sectionActions.didAddToModel(section, sectionIndex);
    }

    else if (self.sectionActions.didAddToModel) {
        self.sectionActions.didAddToModel(section, sectionIndex);
    }

}

- (SCTableViewSection *)sectionAtIndex:(NSUInteger)index
{
    if (index < self.sectionCount) {
        return [sections objectAtIndex:index];
    }

    //else
	return nil;
}

- (SCTableViewSection *)sectionWithHeaderTitle:(NSString *)title
{
	for(SCTableViewSection *section in sections) {
        if (title) {
            if ([section.headerTitle isEqualToString:title]) {
                return section;
            }

        }

        else {
            if (!section.headerTitle) {
                return section;
            }

        }

	}



    return nil;
}

- (NSUInteger)indexForSection:(SCTableViewSection *)section
{
	return [sections indexOfObjectIdenticalTo:section];
}

- (void)removeSectionAtIndex:(NSUInteger)index
{
	[sections removeObjectAtIndex:index];

    if (self.modelActions.didRemoveSection) {
        self.modelActions.didRemoveSection(self, index);
    }

}

- (void)removeAllSections
{
    activeCell = nil;
    activeCellControl = nil;
    
	[sections removeAllObjects];
}

- (void)generateSectionsForObject:(NSObject *)object withDefinition:(SCDataDefinition *)definition
{
    [self generateSectionsForObject:object withDefinition:definition newObject:NO];
}

- (void)generateSectionsForObject:(NSObject *)object withDefinition:(SCDataDefinition *)definition newObject:(BOOL)newObject
{
    SCDataStore *objStore = [definition generateCompatibleDataStore];
    [self generateSectionsForObject:object withDataStore:objStore newObject:newObject];
}

- (void)generateSectionsForObject:(NSObject *)object withDataStore:(SCDataStore *)store
{
    [self generateSectionsForObject:object withDataStore:store newObject:NO];
}

- (void)generateSectionsForObject:(NSObject *)object withDataStore:(SCDataStore *)store newObject:(BOOL)newObject
{
    SCDataDefinition *definition = [store definitionForObject:object];
    [definition generateDefaultPropertyGroupProperties];
    
    [self addSectionForObject:object withDataStore:store usingGroup:definition.defaultPropertyGroup newObject:newObject];
    
    for(NSInteger i=0; i<definition.propertyGroups.groupCount; i++)
    {
        SCPropertyGroup *propertyGroup = [definition.propertyGroups groupAtIndex:i];
        [self addSectionForObject:object withDataStore:store usingGroup:propertyGroup newObject:newObject];
    }
}

- (void)generateSectionsForUserDefaultsDefinition:(SCUserDefaultsDefinition *)userDefaultsDefinition
{
    SCUserDefaultsStore *store = (SCUserDefaultsStore *)[userDefaultsDefinition generateCompatibleDataStore];
    NSObject *object = store.standardUserDefaultsObject;
    
    [self generateSectionsForObject:object withDataStore:store newObject:FALSE];
}

- (void)addSectionForObject:(NSObject *)object withDataStore:(SCDataStore *)store usingGroup:(SCPropertyGroup *)group newObject:(BOOL)newObject
{
    NSInteger propertyNameCount = group.propertyNameCount;
    if (!propertyNameCount) {
        return;
    }

    SCDataDefinition *definition = [store definitionForObject:object];
    
    SCPropertyGroup *subGroup = [SCPropertyGroup groupWithHeaderTitle:group.headerTitle footerTitle:group.footerTitle propertyNames:nil];
    for(NSInteger i=0; i<propertyNameCount; i++)
    {
        NSString *propertyName = [group propertyNameAtIndex:i];
        SCPropertyDefinition *propertyDef = [definition propertyDefinitionWithName:propertyName];

        if ( (newObject && !propertyDef.existsInCreationMode) || (!newObject && !propertyDef.existsInDetailMode) ) {
            continue;
        }

        if (!propertyDef.attributes.expandContentInCurrentView) {
            [subGroup addPropertyName:propertyName];
            continue;
        }

        else {
            if (subGroup.propertyNameCount) {
                SCObjectSection *objectSection = [[SCObjectSection alloc] initWithHeaderTitle:nil boundObject:object boundObjectStore:store propertyGroup:subGroup];
                [self addSection:objectSection];
                
                // reset subGroup
                subGroup = [SCPropertyGroup groupWithHeaderTitle:group.headerTitle footerTitle:group.footerTitle propertyNames:nil];
            }

            SCTableViewSection *section = [self getSectionForPropertyDefinition:propertyDef withBoundObject:object withDataStore:store];
            if (section) {
                section.headerTitle = group.headerTitle;
                section.footerTitle = group.footerTitle;
                section.detailViewControllerOptions.hidesBottomBarWhenPushed = self.viewController.hidesBottomBarWhenPushed;
                
                [self addSection:section];
            }

        }

    }

    if (subGroup.propertyNameCount) {
        SCObjectSection *objectSection = [[SCObjectSection alloc] initWithHeaderTitle:nil boundObject:object boundObjectStore:store propertyGroup:subGroup];
        [self addSection:objectSection];
    }

}

- (SCTableViewSection *)getSectionForPropertyDefinition:(SCPropertyDefinition *)propertyDef withBoundObject:(NSObject *)boundObj withDataStore:(SCDataStore *)store
{
    SCTableViewSection *section = nil;
    
    switch (propertyDef.type)
    {
        case SCPropertyTypeObject:
        {
            NSObject *object = [store valueForPropertyName:propertyDef.name inObject:boundObj];
            
            SCDataDefinition *objDef = nil;
            if ([propertyDef.attributes isKindOfClass:[SCObjectAttributes class]]) {
                objDef = [(SCObjectAttributes *)propertyDef.attributes objectDefinition];
            }

            if (!objDef) {
                break;
            }

            SCDataStore *objStore = [objDef generateCompatibleDataStore];
            if (!object) {
                // create a new object
                object = [objStore createNewObjectWithDefinition:objDef];
                [objStore insertObject:object];
                
                [store setValue:object forPropertyName:propertyDef.name inObject:boundObj];
            }

            if (!object) {
                break;
            }

            section = [SCObjectSection sectionWithHeaderTitle:nil boundObject:object boundObjectStore:objStore propertyGroup:nil];
        }
            break;
            
        case SCPropertyTypeArrayOfObjects:
        {
            SCDataDefinition *objectsDefinition = nil;
            if ([propertyDef.attributes isKindOfClass:[SCArrayOfObjectsAttributes class]]) {
                objectsDefinition = [(SCArrayOfObjectsAttributes *)propertyDef.attributes defaultObjectsDefinition];
            }

            SCDataStore *objectsStore = [objectsDefinition generateCompatibleDataStore];
            if (objectsStore) {
                if (![objectsStore valueForPropertyName:propertyDef.name inObject:boundObj]) {
                    if ( (propertyDef.dataType==SCDataTypeNSMutableArray && !propertyDef.dataReadOnly)
                       || propertyDef.dataType==SCDataTypeDictionaryItem) {
                        if ([objectsStore isKindOfClass:[SCArrayStore class]]) { // if statement required to prevent problems with other stores not supporting this kind of initialization
                            [objectsStore setValue:[NSMutableArray array] forPropertyName:propertyDef.name inObject:boundObj];
                        }

                    }

                }

                SCDataDefinition *boundObjDef = [store definitionForObject:boundObj];
                [objectsStore bindStoreToPropertyName:propertyDef.name forObject:boundObj withDefinition:boundObjDef];
            }

            section = [SCArrayOfObjectsSection sectionWithHeaderTitle:nil dataStore:objectsStore];
        }
            break;
            
        case SCPropertyTypeSelection:
            if (propertyDef.dataType==SCDataTypeNSNumber || propertyDef.dataType==SCDataTypeInt) {
                section = [SCSelectionSection sectionWithHeaderTitle:nil boundObject:boundObj selectedIndexPropertyName:propertyDef.name items:nil];
            }

            else if (propertyDef.dataType == SCDataTypeNSString) {
                section = [SCSelectionSection sectionWithHeaderTitle:nil boundObject:boundObj selectionStringPropertyName:propertyDef.name items:nil];
            }

            else if (propertyDef.dataType == SCDataTypeNSMutableSet) {
                section = [SCSelectionSection sectionWithHeaderTitle:nil boundObject:boundObj selectedIndexesPropertyName:propertyDef.name items:nil allowMultipleSelection:FALSE];
            }

            break;
        case SCPropertyTypeObjectSelection:
            section = [SCObjectSelectionSection sectionWithHeaderTitle:nil boundObject:boundObj selectedObjectPropertyName:propertyDef.name selectionItemsStore:nil];
            break;
            
        default:
            section = nil;
    }

    if (section) {
        [section setAttributesTo:propertyDef.attributes];
        if (propertyDef.attributes) {
            [section.sectionActions setActionsTo:propertyDef.attributes.expandedContentSectionActions overrideExisting:YES];
        }

    }

    return  section;
}

- (void)clear
{
	[self removeAllSections];
	activeCell = nil;
    activeCellIndexPath = nil;
}

- (void)setTableViewEditing:(BOOL)editing animated:(BOOL)animate
{
    if (editing == self.tableView.editing) {
        return;
    }

    if (self.swipeToDeleteActive) {
        [self.tableView setEditing:NO animated:animate];
        swipeToDeleteActive = FALSE;
        return;
    }

    [self clearLastReturnedCellData];
    
    BOOL shouldContinue = TRUE;
    if (editing) {
        if (self.modelActions.shouldBeginEditing) {
            shouldContinue = self.modelActions.shouldBeginEditing(self);
        }

    }

    else {
        if (self.modelActions.shouldEndEditing) {
            shouldContinue = self.modelActions.shouldEndEditing(self);
        }

    }

    if (!shouldContinue) {
        return;
    }


    [self.tableView beginUpdates];
    
    // Update sections to reflect new state
    for(SCTableViewSection *section in sections) {
        [section editingModeWillChange];
    }

    
    // Set editing mode
    [self.viewController setEditing:editing animated:animate];
	[self.tableView setEditing:editing animated:animate];
    
    [self.tableView endUpdates];
    
    // Notify section that editing mode has changed
    for(SCTableViewSection *section in sections) {
        [section editingModeDidChange];
    }



    if (editing) {
        if (self.modelActions.didBeginEditing) {
            self.modelActions.didBeginEditing(self);
        }

    }

    else {
        if (self.modelActions.didEndEditing) {
            self.modelActions.didEndEditing(self);
        }

    }

}

- (void)setModeledTableViewEditing:(BOOL)editing animated:(BOOL)animate 
{
    [self setTableViewEditing:editing animated:animate];
}


- (UIViewController *)detailViewControllerForCellAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    
    return [section generatedDetailViewControllerForCellAtIndexPath:indexPath];
}


- (void)setBoundObjectForAllCells:(NSObject *)boundObject dataDefinition:(SCDataDefinition *)dataDefinition
{
    SCDataStore *dataStore = [dataDefinition generateCompatibleDataStore];
    
    [self setBoundObjectForAllCells:boundObject boundObjectStore:dataStore];
}

- (void)setBoundObjectForAllCells:(NSObject *)boundObject boundObjectStore:(SCDataStore *)boundObjectStore
{
    for(NSUInteger i=0; i<self.sectionCount; i++)
    {
        SCTableViewSection *section = [self sectionAtIndex:i];
        for(NSUInteger j=0; j<section.cellCount; j++)
        {
            SCTableViewCell *cell = [section cellAtIndex:j];
            cell.boundObjectStore = boundObjectStore;
            cell.boundObject = boundObject;
        }
    }
}

- (SCTableViewCell *)cellAtIndexPath:(NSIndexPath *)indexPath
{
    if (!indexPath || indexPath.row == NSNotFound) {
        return nil;
    }

    SCTableViewCell *cell = nil;

    if (self.live) {
        /*
         In iOS 13 calling cellForRowAtIndexPath: here was causing an Assert:

         "Attempted to call -cellForRowAtIndexPath: on the table view while it
         was in the process of updating its visible cells, which is not
         allowed."

         The live method previously was checking if there were any cells by
         using visbleCells which is also not allowed if the cells were being
         updated, so the error has now moved here.

         Not calling cellForRowAtIndexPath: will obviously be less efficient,
         but it all works without any asserts!

         dgApps
         */
        //cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];
    }

    if (!cell) {
        BOOL needsOptimization = FALSE;
        SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
        if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
            needsOptimization = TRUE;
        }

        if (needsOptimization && lastReturnedCellIndexPath && indexPath.section==lastReturnedCellIndexPath.section && indexPath.row==lastReturnedCellIndexPath.row) {
            cell = lastReturnedCell;
        }

        else {
            [self clearLastReturnedCellData];
            
            cell = [section cellAtIndex:indexPath.row];

            if (needsOptimization) {
                lastReturnedCellIndexPath = indexPath;
                lastReturnedCell = cell;
            }

        }

    }

    return cell;
}

- (NSIndexPath *)indexPathForCell:(SCTableViewCell *)cell
{
    if (!cell) {
        return nil;
    }

    if (cell == lastReturnedCell) {
        return lastReturnedCellIndexPath;
    }

    for(NSUInteger i=0; i<self.sectionCount; i++)
	{
		NSUInteger index = [[self sectionAtIndex:i] indexForCell:cell];
        if (index == NSNotFound) {
            continue;
        }

        return [NSIndexPath indexPathForRow:index inSection:i];
	}
	return nil;
}

- (SCTableViewCell *)cellWithBoundPropertyName:(NSString *)boundPropertyName
{
    SCTableViewCell *cell = nil;
    
    for(SCTableViewSection *section in sections)
    {
        if (![section isKindOfClass:[SCObjectSection class]]) {
            continue;
        }

        SCObjectSection *objectSection = (SCObjectSection *)section;
        cell = [objectSection cellForPropertyName:boundPropertyName];
        if (cell) {
            break;
        }

    }
    
    return cell;
}

- (NSIndexPath *)indexPathForCellAfterCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind
{
    if (self.sectionCount==1 && [self sectionAtIndex:0].cellCount==1) {
        return nil;		// only one cell in model
    }

    SCTableViewSection *cellSection = [self sectionAtIndex:indexPath.section];
    if (indexPath.row+1 < cellSection.cellCount) {
        return [NSIndexPath indexPathForRow:indexPath.row+1 inSection:indexPath.section];
    }

    if (indexPath.section+1 < self.sectionCount) {
        return [NSIndexPath indexPathForRow:0 inSection:indexPath.section+1];
    }

    if (!rewind) {
        return nil;
    }

    return [NSIndexPath indexPathForRow:0 inSection:0];
}

- (NSIndexPath *)indexPathForCellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind
{
    NSIndexPath *indexPath = [self indexPathForCell:cell];
    
    return [self indexPathForCellAfterCellAtIndexPath:indexPath rewind:rewind];
}

- (SCTableViewCell *)cellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind
{
	NSIndexPath *nextCellIndexPath = [self indexPathForCellAfterCell:cell rewind:rewind];

    if (!nextCellIndexPath) {
        return nil;
    }

    //else
    return (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:nextCellIndexPath];
}

- (NSIndexPath *)indexPathForCellBeforeCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind
{
    if (self.sectionCount==1 && [self sectionAtIndex:0].cellCount==1) {
        return nil;		// only one cell in model
    }

    if (indexPath.row-1 >= 0) {
        return [NSIndexPath indexPathForRow:indexPath.row-1 inSection:indexPath.section];
    }

    if (indexPath.section-1 >= 0) {
        SCTableViewSection *prevSection = [self sectionAtIndex:indexPath.section-1];
        return [NSIndexPath indexPathForRow:prevSection.cellCount-1 inSection:indexPath.section-1];
    }

    if (!rewind) {
        return nil;
    }

    NSUInteger lastSectionIndex = self.sectionCount-1;
    SCTableViewSection *lastSection = [self sectionAtIndex:lastSectionIndex];
    if (lastSection.cellCount) {
        return [NSIndexPath indexPathForRow:lastSection.cellCount-1 inSection:lastSectionIndex]; // last cell
    }

    //else 
    return nil;
}

- (NSIndexPath *)indexPathForCellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind
{
    NSIndexPath *indexPath = [self indexPathForCell:cell];
    
    return [self indexPathForCellAfterCellAtIndexPath:indexPath rewind:rewind];
}

- (SCTableViewCell *)cellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind
{
    NSIndexPath *prevCellIndexPath = [self indexPathForCellBeforeCell:cell rewind:rewind];

    if (!prevCellIndexPath) {
        return nil;
    }

    //else
    return (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:prevCellIndexPath];
}

- (void)moveToNextCellControl:(BOOL)rewind
{
    // check that there are no other TextFields/TextViews in the current cell before
    // moving on to the next cell
    if ([self.activeCell isKindOfClass:[SCCustomCell class]]) {
        NSArray *inputControls = [(SCCustomCell *)self.activeCell inputControlsSortedByTag];
        NSUInteger activeControlIndex = [inputControls indexOfObjectIdenticalTo:self.activeCellControl];
        if (activeControlIndex<inputControls.count-1 && activeControlIndex!=NSNotFound) {
            [(UIResponder *)[inputControls objectAtIndex:activeControlIndex+1] becomeFirstResponder];
            return;
        }

    }

    // get next cell
    NSIndexPath *currentCellIndexPath = self.activeCellIndexPath;
    SCTableViewCell *nextCell = nil;
    NSIndexPath *nextCellIndexPath = nil;
    while( (nextCellIndexPath = [self indexPathForCellAfterCellAtIndexPath:currentCellIndexPath rewind:rewind])) 
    {
        if (!nextCellIndexPath) {
            nextCell = nil;
            break;
        }

        nextCell = [self cellAtIndexPath:nextCellIndexPath];
        if ([nextCell canBecomeFirstResponder]) {
            break;
        }

        //else
        
        // prevent infinite loop
        if (nextCellIndexPath.row==self.activeCellIndexPath.row && nextCellIndexPath.section==self.activeCellIndexPath.section) {
            nextCell = nil;
            break;
        }

        currentCellIndexPath = nextCellIndexPath;
        nextCell = nil;
        nextCellIndexPath = nil;
    }

    if (nextCell) {
        self.activeCell = nextCell;
        [self.activeCell becomeFirstResponder];
    }

    else {
        [self.activeCell resignFirstResponder];
        self.activeCell = nil;
    }

}

- (void)moveToPreviousCellControl:(BOOL)rewind
{
    // check that there are no other TextFields/TextViews in the current cell before
    // moving on to the previous cell
    if ([self.activeCell isKindOfClass:[SCCustomCell class]]) {
        NSArray *inputControls = [(SCCustomCell *)self.activeCell inputControlsSortedByTag];
        NSUInteger activeControlIndex = [inputControls indexOfObjectIdenticalTo:self.activeCellControl];
        if (activeControlIndex>0 && activeControlIndex!=NSNotFound) {
            [(UIResponder *)[inputControls objectAtIndex:activeControlIndex-1] becomeFirstResponder];
            return;
        }

    }

    // get previous cell
    NSIndexPath *currentCellIndexPath = self.activeCellIndexPath;
    SCTableViewCell *prevCell = nil;
    NSIndexPath *prevCellIndexPath = nil;
    while( (prevCellIndexPath = [self indexPathForCellBeforeCellAtIndexPath:currentCellIndexPath rewind:rewind])) 
    {
        if (!prevCellIndexPath) {
            prevCell = nil;
            break;
        }

        prevCell = [self cellAtIndexPath:prevCellIndexPath];
        if ([prevCell canBecomeFirstResponder]) {
            break;
        }

        //else
        
        // prevent infinite loop
        if (prevCellIndexPath.row==self.activeCellIndexPath.row && prevCellIndexPath.section==self.activeCellIndexPath.section) {
            prevCell = nil;
            break;
        }

        currentCellIndexPath = prevCellIndexPath;
        prevCell = nil;
        prevCellIndexPath = nil;
    }

    if (prevCell) {
        self.activeCell = prevCell;
        [self.activeCell becomeFirstResponder];
    }

    else {
        [self.activeCell resignFirstResponder];
        self.activeCell = nil;
    }

}

- (void)dismissAllDetailViewsWithCommit:(BOOL)commit
{
    if (!self.activeDetailModel) {
        return;
    }

    SCTableViewModel *detailModel = self.activeDetailModel;
    [detailModel dismissAllDetailViewsWithCommit:commit];
    
    BOOL doneValue = commit;
    BOOL cancelValue = !commit;
    if ([detailModel.viewController isKindOfClass:[SCViewController class]]) {
        SCViewController *detailView = (SCViewController *)detailModel.viewController;
        detailView.delegate = nil;  // disable delegates
        [detailView dismissWithCancelValue:cancelValue doneValue:doneValue];
    }

    else if ([detailModel.viewController isKindOfClass:[SCTableViewController class]]) {
        SCTableViewController *detailView = (SCTableViewController *)detailModel.viewController;
        detailView.delegate = nil;  // disable delegates
        [detailView dismissWithCancelValue:cancelValue doneValue:doneValue];
    }

    self.activeDetailModel = nil;
}

- (BOOL)valuesAreValid
{
	for(SCTableViewSection *section in sections) {
        if (!section.valuesAreValid) {
            return FALSE;
        }

	}


    return TRUE;
}

- (BOOL)needsCommit
{
	for(SCTableViewSection *section in sections) {
        if (section.needsCommit) {
            return TRUE;
        }

	}


    return FALSE;
}

- (void)commitChanges
{
    for(NSUInteger i=0; i<self.sectionCount; i++)
	{
		SCTableViewSection *section = [self sectionAtIndex:i];
        [section commitCellChanges];
	}
}

- (void)reloadBoundValues
{
    [self clearLastReturnedCellData];

    if (self.detailViewController) {
        if ([self.detailViewController isKindOfClass:[SCViewController class]]) {
            [(SCViewController *)self.detailViewController loseFocus];
        }

        else if ([self.detailViewController isKindOfClass:[SCTableViewController class]]) {
            [(SCTableViewController *)self.detailViewController loseFocus];
        }

    }

    for(SCTableViewSection *section in sections) {
		[section reloadBoundValues];
    }

}

- (BOOL)sc_viewContainsActiveInput:(UIView *)view
{
    if (view.isFirstResponder ||
       ([view isKindOfClass:[UIControl class]] && [(UIControl *)view isTracking])) {
        return YES;
    }

    for(UIView *subview in view.subviews) {
        if ([self sc_viewContainsActiveInput:subview]) {
            return YES;
        }

    }


    return NO;
}

- (void)invalidateHeightForCell:(SCTableViewCell *)cell animated:(BOOL)animated
{
    if (!cell || ![self indexPathForCell:cell] || !self.tableView) {
        return;
    }

    if (!_pendingHeightCells) {
        _pendingHeightCells = [NSMutableSet set];
    }

    [_pendingHeightCells addObject:cell];
    _pendingHeightAnimation |= animated;
    [self scheduleHeightUpdates];
}

- (void)scheduleHeightUpdates
{
    if (_heightUpdateScheduled || !_pendingHeightCells.count) {
        return;
    }

    _heightUpdateScheduled = YES;
    __weak typeof(self) weakSelf = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.01 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [weakSelf applyPendingHeightUpdates];
    });
}

- (void)applyPendingHeightUpdates
{
    _heightUpdateScheduled = NO;
    UITableView *table = self.tableView;
    if (table.hasUncommittedUpdates) {
        [self scheduleHeightUpdates];
        return;
    }

    NSArray *cells = _pendingHeightCells.allObjects;
    [_pendingHeightCells removeAllObjects];
    BOOL animated = _pendingHeightAnimation;
    _pendingHeightAnimation = NO;
    if (!table) {
        return;
    }

    /*
    hermes:
    Static rows must not be both the old and new reload-animation participant.
    Refit the existing cells in one update cycle. Active editors keep their
    bindings, marked text, selection and control identity throughout the fit.
    */
    void (^update)(void) = ^{
        for(SCTableViewCell *cell in cells)
        {
            NSIndexPath *path = [self indexPathForCell:cell];
            if (!path) {
                continue;
            }

            if (![self sc_viewContainsActiveInput:cell]) {
                cell.configured = NO;
                [self prepareCell:cell atIndexPath:path];
            }

            [cell setNeedsUpdateConstraints];
            [cell updateConstraintsIfNeeded];
            [cell.contentView invalidateIntrinsicContentSize];
            [cell invalidateIntrinsicContentSize];
        }
        [table beginUpdates];
        [table endUpdates];
    };
    if (animated) {
        update();
    }

    else {
        [UIView performWithoutAnimation:update];
    }

}

- (void)configureCell:(SCTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];

    if (cell.cellActions.willConfigure) {
        cell.cellActions.willConfigure(cell, indexPath);
    }

    else if (section.cellActions.willConfigure) {
        section.cellActions.willConfigure(cell, indexPath);
    }

    else if (self.cellActions.willConfigure) {
        self.cellActions.willConfigure(cell, indexPath);
    }

    cell.configured = TRUE;
}

#pragma mark -
#pragma mark UITableViewDataSource methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    [self enterLoadingMode];
    
	return self.sectionCount;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    [self clearLastReturnedCellData];
    
    return [self sectionAtIndex:section].cellCount;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    /*
    hermes:
    Our constrained supplementary view owns its label. Returning a title here
    also makes UIKit populate the native label, drawing the title twice.
    */
    return nil;
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    return nil;
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView
{
	return self.sectionIndexTitles;
}

- (NSInteger)tableView:(UITableView *)tableView sectionForSectionIndexTitle:(NSString *)title atIndex:(NSInteger)index
{
    if (self.modelActions.sectionForSectionIndexTitle) {
        return self.modelActions.sectionForSectionIndexTitle(self, title, index);
    }

    if (index < self.sectionCount) {
        return index;
    }

    //else return the last section index
	return self.sectionCount-1;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
	return [self cellAtIndexPath:indexPath].editable;  
}

- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    BOOL movable = [self cellAtIndexPath:indexPath].movable;
	return movable;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    SCTableViewCell *cell = [self cellAtIndexPath:indexPath];
    [self prepareCell:cell atIndexPath:indexPath];
    return cell;
}

- (void)prepareCell:(SCTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];

    /*
    Configure all size-affecting content before UIKit performs its Auto Layout
    fitting pass; height delegate callbacks must remain policy-only.
    */
    if (!cell.configured) {
        if (!cell.needsCommit && ![self sc_viewContainsActiveInput:cell]) {
            if ([cell isKindOfClass:[SCControlCell class]]) {
                [(SCControlCell *)cell loadBoundValueIntoControl];
            }

            if ([cell isKindOfClass:[SCCustomCell class]]) {
                [(SCCustomCell *)cell loadBindingsIntoCustomControls];
            }

        }

        if ([section.cellsImageViews count] > indexPath.row) {
            UIImageView *imageView = [section.cellsImageViews objectAtIndex:indexPath.row];
            if ([imageView isKindOfClass:[UIImageView class]]) {
                cell.imageView.image = imageView.image;
            }

        }

        if ([section isKindOfClass:[SCSelectionSection class]]) {
            [(SCSelectionSection *)section configureCellForDisplay:cell atIndex:indexPath.row];
        }

        else if ([section isKindOfClass:[SCObjectSelectionSection class]]) {
            [(SCObjectSelectionSection *)section configureCellForDisplay:cell atIndex:indexPath.row];
        }

        [self configureCell:cell atIndexPath:indexPath];
    }

    CGFloat resolvedHeight = UITableViewAutomaticDimension;
    if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
        SCArrayOfItemsSection *itemsSection = (SCArrayOfItemsSection *)section;
        if (section.sectionActions.customHeightForRowAtIndexPath) {
            resolvedHeight = section.sectionActions.customHeightForRowAtIndexPath(itemsSection, indexPath);
        }

        else if (self.sectionActions.customHeightForRowAtIndexPath) {
            resolvedHeight = self.sectionActions.customHeightForRowAtIndexPath(itemsSection, indexPath);
        }

    }

    [cell sc_setResolvedHeightConstraint:resolvedHeight];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
	SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
        [(SCArrayOfItemsSection *)section commitEditingStyle:editingStyle forCellAtIndexPath:indexPath];
    }

}

- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath 
													toIndexPath:(NSIndexPath *)toIndexPath
{
	SCTableViewSection *section = [self sectionAtIndex:fromIndexPath.section];
    if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
        [(SCArrayOfItemsSection *)section moveCellAtIndexPath:fromIndexPath toIndexPath:toIndexPath];
    }

    if (self.modelActions.didMoveCell) {
        SCTableViewCell *cell = [self cellAtIndexPath:toIndexPath];
        self.modelActions.didMoveCell(self, cell, fromIndexPath, toIndexPath);
    }

}

#pragma mark - UITableViewDelegate methods

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewAutomaticDimension;
}

- (CGFloat)tableView:(UITableView *)tableView estimatedHeightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewCell *cell = [[self sectionAtIndex:indexPath.section] cellAtIndex:indexPath.row];
    /*
    Nib/storyboard canvas heights are estimates only, giving UIKit stable
    scrolling while preserving constraint-driven runtime sizing.
    */
    if (cell.estimatedHeight > 0.0) {
        return cell.estimatedHeight;
    }

    if (tableView.estimatedRowHeight > 0.0) {
        return tableView.estimatedRowHeight;
    }

    return 44.0;
}

- (BOOL)sectionHidesSupplementaryViews:(SCTableViewSection *)section
{
    return [section isKindOfClass:[SCArrayOfItemsSection class]] &&
        [(SCArrayOfItemsSection *)section hideWhenEmpty] && !section.cellCount;
}

- (BOOL)section:(SCTableViewSection *)section hasSupplementaryHeader:(BOOL)isHeader
{
    if ([self sectionHidesSupplementaryViews:section]) {
        return NO;
    }

    if (isHeader) {
        return section.headerView || (!self.hideSectionHeaderTitles && section.headerTitle.length) || section.headerHeight > 0;
    }

    return section.footerView || section.footerTitle.length || section.footerHeight > 0;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    /*
    hermes:
    UIKit treats zero as its default spacing, not a hidden supplementary view.
    A least-positive height with a nil view suppresses absent section content.
    Visible content is always measured by UIKit from constraints.
    */
    return [self section:[self sectionAtIndex:section] hasSupplementaryHeader:YES] ? UITableViewAutomaticDimension : CGFLOAT_MIN;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return [self section:[self sectionAtIndex:section] hasSupplementaryHeader:NO] ? UITableViewAutomaticDimension : CGFLOAT_MIN;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    [self exitLoadingMode];
    SCTableViewSection *scSection = [self sectionAtIndex:section];
    if (![self section:scSection hasSupplementaryHeader:YES]) {
        return nil;
    }

    return [scSection supplementaryViewForHeader:YES hideTitle:self.hideSectionHeaderTitles];
}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
    [self exitLoadingMode];
    SCTableViewSection *scSection = [self sectionAtIndex:section];
    if (![self section:scSection hasSupplementaryHeader:NO]) {
        return nil;
    }

    return [scSection supplementaryViewForHeader:NO hideTitle:NO];
}

- (void)tableView:(UITableView *)tableView willDisplayHeaderView:(UIView *)view forSection:(NSInteger)section
{
    SCTableViewSection *scSection = [self sectionAtIndex:section];

    if (scSection.sectionActions.willDisplayHeaderView) {
        scSection.sectionActions.willDisplayHeaderView(scSection, section, view);
    }

    else if (self.sectionActions.willDisplayHeaderView) {
        self.sectionActions.willDisplayHeaderView(scSection, section, view);
    }

}

- (void)tableView:(UITableView *)tableView willDisplayFooterView:(UIView *)view forSection:(NSInteger)section
{
    SCTableViewSection *scSection = [self sectionAtIndex:section];

    if (scSection.sectionActions.willDisplayFooterView) {
        scSection.sectionActions.willDisplayFooterView(scSection, section, view);
    }

    else if (self.sectionActions.willDisplayFooterView) {
        self.sectionActions.willDisplayFooterView(scSection, section, view);
    }

}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCellEditingStyle editingStyle = UITableViewCellEditingStyleNone;
    BOOL customEditingStyle = NO;
    
    SCTableViewCell *cell = [self cellAtIndexPath:indexPath];

    if (cell.cellActions.customEditingStyle) {
        editingStyle = cell.cellActions.customEditingStyle(cell, indexPath);
        customEditingStyle = YES;
    }

    else if (cell.ownerSection.cellActions.customEditingStyle) {
        editingStyle = cell.ownerSection.cellActions.customEditingStyle(cell, indexPath);
        customEditingStyle = YES;
    }

    else if (cell.ownerTableViewModel.cellActions.customEditingStyle) {
        editingStyle = cell.ownerTableViewModel.cellActions.customEditingStyle(cell, indexPath);
        customEditingStyle = YES;
    }

    if (!customEditingStyle) {
        editingStyle = cell.cellEditingStyle;
    }

    return editingStyle;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
	SCTableViewCell *scCell = (SCTableViewCell *)cell;
	[scCell willDisplay];
	
	// Check if the cell has an image in its section
	SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    if ([section.cellsImageViews count] > indexPath.row) {
        UIImageView *imageView = [section.cellsImageViews objectAtIndex:indexPath.row];
        if ([imageView isKindOfClass:[UIImageView class]]) {
            scCell.imageView.image = imageView.image;
        }

    }

    if ([section isKindOfClass:[SCSelectionSection class]]) {
        [(SCSelectionSection *)section configureCellForDisplay:scCell atIndex:indexPath.row];
    }

    else if ([section isKindOfClass:[SCObjectSelectionSection class]]) {
        [(SCObjectSelectionSection *)section configureCellForDisplay:scCell atIndex:indexPath.row];
    }


    if (scCell.cellActions.willDisplay) {
        scCell.cellActions.willDisplay(scCell, indexPath);
    }

    else if (section.cellActions.willDisplay) {
        section.cellActions.willDisplay(scCell, indexPath);
    }

    else if (self.cellActions.willDisplay) {
        self.cellActions.willDisplay(scCell, indexPath);
    }


    if (!self.tableView.dragging) {
        if (!lastVisibleCellIndexPath) {
            lastVisibleCellIndexPath = [[self.tableView indexPathsForVisibleRows] lastObject];
        }

        if (indexPath.section==lastVisibleCellIndexPath.section && indexPath.row==lastVisibleCellIndexPath.row) {
            [self exitLoadingMode];
            lastVisibleCellIndexPath = nil;

            if (self.modelActions.didFinishLoadingCells) {
                self.modelActions.didFinishLoadingCells(self);
            }

        }

    }

}

- (NSArray *)tableView:(UITableView *)tableView editActionsForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray *editActions = nil;
    
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (cell.cellActions.editActions) {
        editActions = cell.cellActions.editActions(cell, indexPath);
    }

    else if (section.cellActions.editActions) {
        editActions = section.cellActions.editActions(cell, indexPath);
    }

    else if (self.cellActions.editActions) {
        editActions = self.cellActions.editActions(cell, indexPath);
    }

    return editActions;
}


// dgApps...
- (UISwipeActionsConfiguration *)tableView:(UITableView *)tableView leadingSwipeActionsConfigurationForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSArray *leadingSwipeActions = nil;

    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (cell.cellActions.leadingSwipeActions) {
        leadingSwipeActions = cell.cellActions.leadingSwipeActions(cell, indexPath);
    }

    else {
        if (section.cellActions.leadingSwipeActions) {
            leadingSwipeActions = section.cellActions.leadingSwipeActions(cell, indexPath);
        }

        else {
            if (self.cellActions.leadingSwipeActions) {
                leadingSwipeActions = self.cellActions.leadingSwipeActions(cell, indexPath);
            }

        }

    }

    if (leadingSwipeActions) {
        UISwipeActionsConfiguration *swipeActionsConfiguration = [UISwipeActionsConfiguration configurationWithActions:leadingSwipeActions];
        swipeActionsConfiguration.performsFirstActionWithFullSwipe = cell.leadingSwipePerformsFirstActionWithFullSwipe;
        return swipeActionsConfiguration;
    }

    // else
    return nil;
}


- (UISwipeActionsConfiguration *)tableView:(UITableView *)tableView trailingSwipeActionsConfigurationForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSArray *trailingSwipeActions = nil;

    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (cell.cellActions.trailingSwipeActions) {
        trailingSwipeActions = cell.cellActions.trailingSwipeActions(cell, indexPath);
    }

    else {
        if (section.cellActions.trailingSwipeActions) {
            trailingSwipeActions = section.cellActions.trailingSwipeActions(cell, indexPath);
        }

        else {
            if (self.cellActions.leadingSwipeActions) {
                trailingSwipeActions = self.cellActions.trailingSwipeActions(cell, indexPath);
            }

        }

    }

    if (trailingSwipeActions) {
        UISwipeActionsConfiguration *swipeActionsConfiguration = [UISwipeActionsConfiguration configurationWithActions:trailingSwipeActions];
        swipeActionsConfiguration.performsFirstActionWithFullSwipe = cell.trailingSwipePerformsFirstActionWithFullSwipe;
        return swipeActionsConfiguration;
    }

    // else
    return nil;

}

// ...dgApps






- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (self.lockCellSelection) {
        return nil;
    }

    if (self.activeDetailModel) {
        [self.activeDetailModel dismissAllDetailViewsWithCommit:YES];
    }

    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
	SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (!cell.selectable || !cell.enabled) {
        return nil;
    }

    BOOL allowSelection = YES;
    if (cell.cellActions.willSelect) {
        allowSelection = cell.cellActions.willSelect(cell, indexPath);
    }

    else if (section.cellActions.willSelect) {
        allowSelection = section.cellActions.willSelect(cell, indexPath);
    }

    else if (self.cellActions.willSelect) {
        allowSelection = self.cellActions.willSelect(cell, indexPath);
    }

    if (allowSelection) {
        return indexPath;
    }

    //else
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (!cell.enabled) {
        return;
    }

    if (cell != self.activeCell) {
        SCTableViewCell *prevCell = self.activeCell;
        self.activeCell = cell;
        if (![self.activeCell becomeFirstResponder]) {
            [prevCell resignFirstResponder];
        }

    }

    [self.activeCell didSelectCell];
	
	SCTableViewSection *section = [self sectionAtIndex:indexPath.section];

    if (cell.cellActions.didSelect) {
        cell.cellActions.didSelect(cell, indexPath);
    }

    else if (section.cellActions.didSelect) {
        section.cellActions.didSelect(cell, indexPath);
    }

    else if (self.cellActions.didSelect) {
        self.cellActions.didSelect(cell, indexPath);
    }

    else if (![self.activeCell isKindOfClass:[SCFetchItemsCell class]] && [section isKindOfClass:[SCArrayOfItemsSection class]]) {
        [(SCArrayOfItemsSection *)section didSelectCellAtIndexPath:indexPath];
    }

    else if ([cell.ibDetailViewControllerIdentifier length] && !cell.generatesDetailView) {
        UIViewController *detailViewController = [SCUtilities instantiateViewControllerWithIdentifier:cell.ibDetailViewControllerIdentifier usingStoryboard:self.viewController.storyboard];

        if (detailViewController) {
            [cell presentDetailViewController:detailViewController forCell:cell forRowAtIndexPath:indexPath withPresentationMode:cell.detailViewControllerOptions.presentationMode];
        }

        else {
            SCDebugLog(@"Warning: Could not instantiate view controller with id '%@' from Storyboard.", cell.ibDetailViewControllerIdentifier);
        }

    }

}

- (NSIndexPath *)tableView:(UITableView *)tableView willDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];
    
    BOOL allowDeselect = YES;
    if (cell.cellActions.willDeselect) {
        allowDeselect = cell.cellActions.willDeselect(cell, indexPath);
    }

    else if (section.cellActions.willDeselect) {
        allowDeselect = section.cellActions.willDeselect(cell, indexPath);
    }

    else if (self.cellActions.willDeselect) {
        allowDeselect = self.cellActions.willDeselect(cell, indexPath);
    }

    if (allowDeselect) {
        [cell willDeselectCell];
        if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
            [(SCArrayOfItemsSection *)section willDeselectCellAtIndexPath:indexPath];
        }

        return indexPath;
    }

    //else
    return nil;
}

- (void)tableView:(UITableView *)tableView didDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
	SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];
	[cell didDeselectCell];

    if (cell.cellActions.didDeselect) {
        cell.cellActions.didDeselect(cell, indexPath);
    }

    else if (section.cellActions.didDeselect) {
        section.cellActions.didDeselect(cell, indexPath);
    }

    else if (self.cellActions.didDeselect) {
        self.cellActions.didDeselect(cell, indexPath);
    }

}

- (BOOL)tableView:(UITableView *)tableView shouldShowMenuForRowAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];
    
    BOOL shouldShowMenu = NO;
    if (cell.cellActions.shouldShowMenu) {
        shouldShowMenu = cell.cellActions.shouldShowMenu(cell, indexPath);
    }

    else if (section.cellActions.shouldShowMenu) {
        shouldShowMenu = section.cellActions.shouldShowMenu(cell, indexPath);
    }

    else if (self.cellActions.shouldShowMenu) {
        shouldShowMenu = self.cellActions.shouldShowMenu(cell, indexPath);
    }

    return shouldShowMenu;
}

- (BOOL)tableView:(UITableView *)tableView canPerformAction:(SEL)operationAction forRowAtIndexPath:(NSIndexPath *)indexPath withSender:(id)sender
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];
    
    BOOL canPerformAction = NO;
    if (cell.cellActions.canPerformAction) {
        canPerformAction = cell.cellActions.canPerformAction(cell, indexPath, operationAction, sender);
    }

    else if (section.cellActions.shouldShowMenu) {
        canPerformAction = section.cellActions.canPerformAction(cell, indexPath, operationAction, sender);
    }

    else if (self.cellActions.shouldShowMenu) {
        canPerformAction = self.cellActions.canPerformAction(cell, indexPath, operationAction, sender);
    }

    return canPerformAction;
}

- (void)tableView:(UITableView *)tableView performAction:(SEL)operationAction forRowAtIndexPath:(NSIndexPath *)indexPath withSender:(id)sender
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
    SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (cell.cellActions.performAction) {
        cell.cellActions.performAction(cell, indexPath, operationAction, sender);
    }

    else if (section.cellActions.shouldShowMenu) {
        section.cellActions.performAction(cell, indexPath, operationAction, sender);
    }

    else if (self.cellActions.shouldShowMenu) {
        self.cellActions.performAction(cell, indexPath, operationAction, sender);
    }

}

- (void)tableView:(UITableView *)tableView 
					accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
	SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

    if (cell.cellActions.accessoryButtonTapped) {
        cell.cellActions.accessoryButtonTapped(cell, indexPath);
    }

    else if (section.cellActions.accessoryButtonTapped) {
        section.cellActions.accessoryButtonTapped(cell, indexPath);
    }

    else if (self.cellActions.accessoryButtonTapped) {
        self.cellActions.accessoryButtonTapped(cell, indexPath);
    }

    else {
        [self tableView:tableView didSelectRowAtIndexPath:indexPath];
    }

}

- (NSString *)tableView:(UITableView *)tableView 
	titleForDeleteConfirmationButtonForRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSString *deleteTitle;
	
	deleteTitle = NSLocalizedString(@"Delete", @"Delete Button Title");
	
	return deleteTitle;
}

- (void)tableView:(UITableView *)tableView willBeginEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    swipeToDeleteActive = TRUE;
}

- (void)tableView:(UITableView *)tableView didEndEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    swipeToDeleteActive = FALSE;
}

- (BOOL)tableView:(UITableView *)tableView shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    /*
     In iOS 13 calling cellForRowAtIndexPath: here was causing an Assert:

     "Attempted to call -cellForRowAtIndexPath: on the table view while it was
     in the process of updating its visible cells, which is not allowed."

     This is a bit of a kludge, but we're just going to return the default here,
     which is YES.

     dgApps
     */
//    return [tableView cellForRowAtIndexPath:indexPath].shouldIndentWhileEditing;
    return YES;
}

- (NSIndexPath *)tableView:(UITableView *)tableView targetIndexPathForMoveFromRowAtIndexPath:(NSIndexPath *)sourceIndexPath toProposedIndexPath:(NSIndexPath *)proposedDestinationIndexPath
{
    NSIndexPath *indexPath = proposedDestinationIndexPath;
    
    SCTableViewSection *section = [self sectionAtIndex:sourceIndexPath.section];
    if ([section isKindOfClass:[SCArrayOfItemsSection class]]) {
        indexPath = [(SCArrayOfItemsSection *)section targetIndexPathForMoveFromCellAtIndexPath:sourceIndexPath toProposedIndexPath:proposedDestinationIndexPath];
    }

    return indexPath;
}

#pragma mark -
#pragma mark Deferred image loading (UIScrollViewDelegate)

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView
{
    [self clearLastReturnedCellData];

    if (self.activeCell) {
        [self.activeCell resignFirstResponder];
        [self.tableView deselectRowAtIndexPath:self.activeCellIndexPath animated:YES];
        self.activeCell = nil;
    }

}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    if (self.modelActions.didScroll) {
        self.modelActions.didScroll(self);
    }

}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
    if (!decelerate) {
        NSArray *visiblePaths = [self.tableView indexPathsForVisibleRows];
        for(NSIndexPath *indexPath in visiblePaths)
        {
            SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
            SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

            if (cell.cellActions.lazyLoad) {
                cell.cellActions.lazyLoad(cell, indexPath);
            }

            else if (section.cellActions.lazyLoad) {
                section.cellActions.lazyLoad(cell, indexPath);
            }

            else if (self.cellActions.lazyLoad) {
                self.cellActions.lazyLoad(cell, indexPath);
            }

        }
    }

    if (self.modelActions.didEndDragging) {
        self.modelActions.didEndDragging(self);
    }

}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    [self clearLastReturnedCellData];
    
    NSArray *visiblePaths = [self.tableView indexPathsForVisibleRows];
    for(NSIndexPath *indexPath in visiblePaths)
    {
        SCTableViewSection *section = [self sectionAtIndex:indexPath.section];
        SCTableViewCell *cell = (SCTableViewCell *)[self.tableView cellForRowAtIndexPath:indexPath];

        if (cell.cellActions.lazyLoad) {
            cell.cellActions.lazyLoad(cell, indexPath);
        }

        else if (section.cellActions.lazyLoad) {
            section.cellActions.lazyLoad(cell, indexPath);
        }

        else if (self.cellActions.lazyLoad) {
            self.cellActions.lazyLoad(cell, indexPath);
        }

    }

    if (self.modelActions.didEndDecelerating) {
        self.modelActions.didEndDecelerating(self);
    }

}

#pragma mark -
#pragma SCInputAccessoryViewDelegate methods

- (void)inputAccessoryViewPreviousTapped:(SCInputAccessoryView *)inputAccessoryView
{
    [self moveToPreviousCellControl:inputAccessoryView.rewind];
}

- (void)inputAccessoryViewNextTapped:(SCInputAccessoryView *)inputAccessoryView
{
    [self moveToNextCellControl:inputAccessoryView.rewind];
}

- (void)inputAccessoryViewClearTapped:(SCInputAccessoryView *)inputAccessoryView
{
    if ([self.activeCell isKindOfClass:[SCControlCell class]]) {
        [(SCControlCell *)self.activeCell clearControl];
    }

    else if (self.activeCellControl) {
        if ([self.activeCellControl respondsToSelector:@selector(setText:)]) {
            [self.activeCellControl performSelector:@selector(setText:) withObject:nil];
        }

    }

}

- (void)inputAccessoryViewDoneTapped:(SCInputAccessoryView *)inputAccessoryView
{
    if (self.activeCellControl) {
        [self.activeCellControl resignFirstResponder];
    }

    else {
        [self.activeCell resignFirstResponder];
    }

    self.activeCell = nil;
}


#pragma mark -
#pragma mark Keyboard methods

- (void)removeKeyboardInset
{
    [_keyboardGeometryObserver removeFromSuperview];
    _keyboardGeometryObserver = nil;
    UITableView *table = _keyboardInsetTable;
    UIEdgeInsets content = table.contentInset;
    UIEdgeInsets indicators = table.verticalScrollIndicatorInsets;
    content.bottom -= keyboardOverlap;
    indicators.bottom -= keyboardOverlap;
    keyboardOverlap = 0;
    _keyboardInsetTable = nil;
    table.contentInset = content;
    table.verticalScrollIndicatorInsets = indicators;
}

- (void)reconcileKeyboardInset
{
    if (_reconcilingKeyboardInset) {
        return;
    }

    _reconcilingKeyboardInset = YES;
    UITableView *table = self.tableView;
    if (_keyboardInsetTable != table) {
        [self removeKeyboardInset];
    }

    UIResponder *owner = table.nextResponder;
    while(owner && ![owner isKindOfClass:[UIViewController class]]) {
        owner = owner.nextResponder;
    }

    BOOL nativeAvoidance = [owner isKindOfClass:[UITableViewController class]] &&
        [(UITableViewController *)owner tableView] == table;
    if (!keyboardShown || !self.autoResizeForKeyboard || !table || nativeAvoidance) {
        [self removeKeyboardInset];
        _reconcilingKeyboardInset = NO;
        return;
    }

    if (!_keyboardGeometryObserver) {
        /*
        hermes:
        A noninteractive constrained observer follows the viewport, not scroll
        content. Layout and safe-area callbacks recompute our inset delta;
        neither the observer nor the model writes the table's frame.
        */
        _keyboardGeometryObserver = [[SCModelGeometryObserver alloc] init];
        _keyboardGeometryObserver.userInteractionEnabled = NO;
        _keyboardGeometryObserver.translatesAutoresizingMaskIntoConstraints = NO;
        [table addSubview:_keyboardGeometryObserver];
        UILayoutGuide *viewport = table.frameLayoutGuide;
        [NSLayoutConstraint activateConstraints:@[
            [_keyboardGeometryObserver.leadingAnchor constraintEqualToAnchor:viewport.leadingAnchor],
            [_keyboardGeometryObserver.trailingAnchor constraintEqualToAnchor:viewport.trailingAnchor],
            [_keyboardGeometryObserver.topAnchor constraintEqualToAnchor:viewport.topAnchor],
            [_keyboardGeometryObserver.bottomAnchor constraintEqualToAnchor:viewport.bottomAnchor]
        ]];
        __weak typeof(self) weakSelf = self;
        _keyboardGeometryObserver.geometryDidChange = ^{ [weakSelf reconcileKeyboardInset]; };
    }

    CGRect keyboardRect = [table.superview convertRect:_keyboardEndFrame fromView:nil];
    CGRect intersection = CGRectIntersection(table.frame, keyboardRect);
    CGFloat overlap = 0;
    if (!CGRectIsNull(intersection) && CGRectGetMaxY(keyboardRect) >= CGRectGetMaxY(table.frame)) {
        overlap = CGRectGetHeight(intersection);
    }

    CGFloat automaticInset = table.adjustedContentInset.bottom - table.contentInset.bottom;
    overlap = MAX(0, overlap - automaticInset);
    UIEdgeInsets content = table.contentInset;
    UIEdgeInsets indicators = table.verticalScrollIndicatorInsets;
    content.bottom += overlap - keyboardOverlap;
    indicators.bottom += overlap - keyboardOverlap;
    keyboardOverlap = overlap;
    _keyboardInsetTable = table;
    table.contentInset = content;
    table.verticalScrollIndicatorInsets = indicators;
    _reconcilingKeyboardInset = NO;
}

- (void)scrollViewDidChangeAdjustedContentInset:(UIScrollView *)scrollView
{
    if (scrollView == _keyboardInsetTable) {
        [self reconcileKeyboardInset];
    }

}

- (void)keyboardWillChangeFrame:(NSNotification *)notification
{
    [self keyboardWillShow:notification];
}

- (void)keyboardWillShow:(NSNotification *)aNotification
{
    NSValue *frame = aNotification.userInfo[UIKeyboardFrameEndUserInfoKey];
    if (!frame) {
        return;
    }

    _keyboardEndFrame = frame.CGRectValue;
    keyboardShown = YES;
    NSDictionary *info = aNotification.userInfo;
    UIViewAnimationOptions options = UIViewAnimationOptionBeginFromCurrentState |
        ([info[UIKeyboardAnimationCurveUserInfoKey] unsignedIntegerValue] << 16);
    [UIView animateWithDuration:[info[UIKeyboardAnimationDurationUserInfoKey] doubleValue]
                          delay:0 options:options animations:^{
        [self reconcileKeyboardInset];
    } completion:^(BOOL finished) {
        [self tableAnimationEnded:nil finished:nil contextInfo:nil];
    }];
}

- (void)keyboardWillHide:(NSNotification *)aNotification
{
    keyboardShown = NO;
    NSDictionary *info = aNotification.userInfo;
    UIViewAnimationOptions options = UIViewAnimationOptionBeginFromCurrentState |
        ([info[UIKeyboardAnimationCurveUserInfoKey] unsignedIntegerValue] << 16);
    [UIView animateWithDuration:[info[UIKeyboardAnimationDurationUserInfoKey] doubleValue]
                          delay:0 options:options animations:^{
        [self removeKeyboardInset];
    } completion:nil];
}

- (void) tableAnimationEnded:(NSString*)animationID finished:(NSNumber *)finished contextInfo:(void *)context
{
	// Scroll to the active cell
    if (self.activeCell) {
        [self.tableView scrollToRowAtIndexPath:self.activeCellIndexPath atScrollPosition:UITableViewScrollPositionNone animated:YES];
        [self.tableView selectRowAtIndexPath:self.activeCellIndexPath animated:NO scrollPosition:UITableViewScrollPositionNone];
    }

}


@end







@interface SCArrayOfItemsModel () <UISearchControllerDelegate>
{
    NSMutableDictionary *_sectionsCellIdentifiers;
    __weak UITableView *_searchAttachedTable;
    __weak UIViewController *_searchAttachedController;
    NSArray<NSLayoutConstraint *> *_searchAttachmentConstraints;
    BOOL _searchHostDefinedPresentationContext;
    BOOL _searchAttachedToNavigationItem;
}

- (void)detachSearchController;
- (void)attachSearchController;

- (void)generateSections;
- (NSArray *)getSectionHeaderTitles;
- (NSString *)getHeaderTitleForItemAtIndex:(NSUInteger)index;

- (void)addNewItemToRespectiveSection:(NSObject *)newItem;

- (NSString *)safeSearchStringFromString:(NSString *)searchString;

@end



@implementation SCArrayOfItemsModel

@synthesize dataStore;
@synthesize dataFetchOptions;
@synthesize autoFetchItems;
@synthesize itemsAccessoryType;
@synthesize allowAddingItems;
@synthesize allowDeletingItems;
@synthesize allowMovingItems;
@synthesize allowEditDetailView;
@synthesize allowRowSelection;
@synthesize autoSelectNewItemCell;
@synthesize addButtonItem;
@synthesize searchBar;


- (instancetype)init
{
    if ( (self=[super init]) ) {
        tempSection = nil;
        
        dataStore = nil;
        dataFetchOptions = nil;  // will be re-initialized when dataStore is set
        
        _loadingContents = FALSE;
        sectionsInSync = FALSE;
        items = nil;
        autoFetchItems = TRUE;
        itemsInSync = FALSE;
        itemsAccessoryType = UITableViewCellAccessoryDisclosureIndicator;
        allowAddingItems = TRUE;
        allowDeletingItems = TRUE;
        allowMovingItems = FALSE;
        allowEditDetailView = TRUE;
        allowRowSelection = TRUE;
        autoSelectNewItemCell = FALSE;
        addButtonItem = nil;
		
        filteredArray = nil;
        searchBar = nil;
        _enableSearchController = FALSE;
        
        detailViewControllerOptions = nil;
        newItemDetailViewControllerOptions = nil;
        
        _sectionsCellIdentifiers = [NSMutableDictionary dictionary];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView dataStore:(SCDataStore *)store
{
    if ( (self=[self initWithTableView:tableView]) ) {
        self.dataStore = store;
    }

    return self;
}


// overrides superclass
- (UITableView *)tableView
{
    if (self.searchController && self.searchController.active && [self.searchController.searchResultsController isKindOfClass:[UITableViewController class]]) {
        return [(UITableViewController *)self.searchController.searchResultsController tableView];
    }

    //else
    return [super tableView];
}


- (void)setDataStore:(SCDataStore *)__dataStore
{
    dataStore =  __dataStore;

    if (!dataFetchOptions) {
        dataFetchOptions = [__dataStore.defaultDataDefinition generateCompatibleDataFetchOptions];
    }

    itemsInSync = FALSE;
    sectionsInSync = FALSE;
}

- (void)setDataFetchOptions:(SCDataFetchOptions *)options
{
    dataFetchOptions = options;

    if (!options.sortKey && self.dataStore) {
        options.sortKey = self.dataStore.defaultDataDefinition.keyPropertyName;
    }

    itemsInSync = FALSE;
    sectionsInSync = FALSE;
}

- (SCDetailViewControllerOptions *)detailViewControllerOptions
{
    // Conserve resources by lazy loading for only models that need it
    if (!detailViewControllerOptions) {
        detailViewControllerOptions = [[SCDetailViewControllerOptions alloc] init];
    }

    return detailViewControllerOptions;
}

- (void)setDetailViewControllerOptions:(SCDetailViewControllerOptions *)options
{
    detailViewControllerOptions = options;
}

- (SCDetailViewControllerOptions *)newItemDetailViewControllerOptions
{
    // Conserve resources by lazy loading for only models that need it
    if (!newItemDetailViewControllerOptions) {
        newItemDetailViewControllerOptions = [[SCDetailViewControllerOptions alloc] init];
    }

    return newItemDetailViewControllerOptions;
}

- (void)setNewItemDetailViewControllerOptions:(SCDetailViewControllerOptions *)options
{
    newItemDetailViewControllerOptions = options;
}


- (NSArray *)items
{
    if (!itemsInSync && self.autoFetchItems) {
        switch (self.dataStore.storeMode)
        {
            case SCStoreModeSynchronous:
                items = [[NSMutableArray alloc] initWithArray:[self.dataStore fetchObjectsWithOptions:self.dataFetchOptions]];
                itemsInSync = TRUE;
                sectionsInSync = FALSE;

                if (self.modelActions.didFetchItemsFromStore) {
                    self.modelActions.didFetchItemsFromStore(self, items);
                }

                break;
                
            case SCStoreModeAsynchronous:
                itemsInSync = TRUE;
                SCFetchItemsCell *fetchItemsCell = [SCFetchItemsCell cell];
                [fetchItemsCell startActivityIndicator];
                items = [NSMutableArray arrayWithObject:fetchItemsCell];
                
                _loadingContents = TRUE;

                [self.dataStore asynchronousFetchObjectsWithOptions:self.dataFetchOptions
                success:^(NSArray *results)
                     {
                    self->_loadingContents = FALSE; // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"

                    self->items = [NSMutableArray arrayWithArray:results];  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    self->sectionsInSync = FALSE;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    [self.tableView reloadData];
                }
                failure:^(NSError *error)
                     {
                    self->_loadingContents = FALSE;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                }
                 noConnection:^BOOL()
                    {
                        return NO;
                    }
                 ];
                break;
        }
    }

    return items;
}

- (NSMutableArray *)mutableItems
{
    return items;
}

- (void)setMutableItems:(NSMutableArray *)mutableItems
{
    items = mutableItems;
}

//override superclass
- (void)reloadBoundValues
{
    [self clearLastReturnedCellData];
    itemsInSync = FALSE;
    sectionsInSync = FALSE;

    if (filteredArray) {
        [self searchBar:self.searchBar textDidChange:self.searchBar.text];
    }

    else {
        [self generateSections];
    }

}

- (void)generateSections
{
	[self removeAllSections];
	
	NSArray *itemsArray;
    if (filteredArray) {
        itemsArray = filteredArray;
    }

    else {
        itemsArray = self.items;
    }

    NSArray *sectionHeaderTitles = [self getSectionHeaderTitles];
    for(NSString *sectionHeaderTitle in sectionHeaderTitles)
    {
        SCArrayOfItemsSection *section = [self createSectionWithHeaderTitle:sectionHeaderTitle];
        [self setPropertiesForSection:section];
        [self addSection:section];
    }
    
    for(NSUInteger i=0; i<itemsArray.count; i++)
	{
		NSString *headerTitle = [self getHeaderTitleForItemAtIndex:i];
		SCArrayOfItemsSection *section = (SCArrayOfItemsSection *)[self sectionWithHeaderTitle:headerTitle];
        if (!section) {
            section = [self createSectionWithHeaderTitle:headerTitle];
            if (!section) {
                continue;
            }

            // set the section's cellIdentifier to a unique string
            NSString *stringIndex = [NSString stringWithFormat:@"%lu", (unsigned long)i];
            NSString *sectionCellId = [_sectionsCellIdentifiers valueForKey:stringIndex];
            if (!sectionCellId) {
                sectionCellId = [[[NSUUID UUID] UUIDString] copy];
                [_sectionsCellIdentifiers setValue:sectionCellId forKey:stringIndex];
            }

            section.cellIdentifier = sectionCellId;
            
            [self setPropertiesForSection:section];
            [self addSection:section];
        }

        [[section mutableItems] addObject:[itemsArray objectAtIndex:i]];
	}
    
    for(SCArrayOfItemsSection *section in sections)
    {
        NSMutableArray *mutableItems = [section mutableItems];
        
        [section.dataFetchOptions sortMutableArray:mutableItems];

        if (section.sectionActions.didFetchItemsFromStore) {
            section.sectionActions.didFetchItemsFromStore(section, mutableItems);
        }

        else if (self.sectionActions.didFetchItemsFromStore) {
            self.sectionActions.didFetchItemsFromStore(section, mutableItems);
        }

    }
    
    sectionsInSync = TRUE;
}

- (NSArray *)getSectionHeaderTitles
{
    NSArray *sectionHeaderTitles = nil;
    if (self.modelActions.sectionHeaderTitles) {
        sectionHeaderTitles = self.modelActions.sectionHeaderTitles(self);
    }

    return sectionHeaderTitles;
}

- (NSString *)getHeaderTitleForItemAtIndex:(NSUInteger)index
{
	NSArray *itemsArray;
    if (filteredArray) {
        itemsArray = filteredArray;
    }

    else {
        itemsArray = self.items;
    }

    NSObject *item = [itemsArray objectAtIndex:index];

    if ([item isKindOfClass:[SCFetchItemsCell class]]) {
        return nil;
    }

    NSString *headerTitleName = nil;
    if (self.modelActions.sectionHeaderTitleForItem) {
        headerTitleName = self.modelActions.sectionHeaderTitleForItem(self, item, index);
    }

    return headerTitleName;
}

- (NSUInteger)getSectionIndexForItem:(NSObject *)item
{
    NSArray *itemsArray;
    if (filteredArray) {
        itemsArray = filteredArray;
    }

    else {
        itemsArray = self.items;
    }

    NSUInteger itemIndex = [itemsArray indexOfObjectIdenticalTo:item];
	NSString *sectionHeader = [self getHeaderTitleForItemAtIndex:itemIndex];

    if (!sectionHeader) {
        return 0;
    }

    NSUInteger sectionIndex = NSNotFound;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
        if ([[self sectionAtIndex:i].headerTitle isEqualToString:sectionHeader]) {
            sectionIndex = i;
            break;
        }

	}


    return sectionIndex;
}

- (SCArrayOfItemsSection *)createSectionWithHeaderTitle:(NSString *)title
{
	SCArrayOfObjectsSection *section = [SCArrayOfObjectsSection sectionWithHeaderTitle:title dataStore:self.dataStore];
    section.autoFetchItems = FALSE;
    [section setMutableItems:[NSMutableArray array]];
    
    return section;
}

- (void)setSearchBar:(UISearchBar *)sbar
{
	searchBar = sbar;
    if (sbar != self.searchController.searchBar) {
        searchBar.delegate = self;
    }

}

- (void)setTableView:(UITableView *)tableView
{
    [self detachSearchController];
    [super setTableView:tableView];
    [self attachSearchController];
}

- (void)setEnableSearchController:(BOOL)enable
{
    _enableSearchController = enable;
    if (enable) {
        if (!_searchController) {
            [self initDisplayController];
        }

        [self attachSearchController];
    }

    else {
        _searchController.active = NO;
        [self detachSearchController];
        if (_searchController) {
            self.searchBar = nil;
        }

        _searchController = nil;
    }

}

- (void)initDisplayController
{
    SCTableViewController *resultsViewController = [[SCTableViewController alloc] initWithStyle:self.trueTableView.style];
    resultsViewController.edgesForExtendedLayout = UIRectEdgeNone;
    resultsViewController.tableView.dataSource = self;
    resultsViewController.tableView.delegate = self;
    _searchController = [[UISearchController alloc] initWithSearchResultsController:resultsViewController];
    _searchController.searchResultsUpdater = self;
    _searchController.delegate = self;
    if (!self.searchBar) {
        self.searchBar = _searchController.searchBar;
    }

}

- (void)detachSearchController
{
    [NSLayoutConstraint deactivateConstraints:_searchAttachmentConstraints];
    _searchAttachmentConstraints = nil;
    if (_searchAttachedController.navigationItem.searchController == _searchController) {
        _searchAttachedController.navigationItem.searchController = nil;
    }

    if (_searchAttachedTable.tableHeaderView == _searchController.searchBar) {
        _searchAttachedTable.tableHeaderView = nil;
    }

    if (_searchAttachedTable) {
        [_searchController.searchBar removeFromSuperview];
    }

    if (_searchAttachedController) {
        _searchAttachedController.definesPresentationContext = _searchHostDefinedPresentationContext;
    }

    _searchAttachedTable = nil;
    _searchAttachedController = nil;
    _searchAttachedToNavigationItem = NO;
}

- (void)attachSearchController
{
    UITableView *table = self.trueTableView;
    if (!_enableSearchController || !_searchController || !table) {
        return;
    }

    // A supplied inline bar already has an owner and filters through its delegate.
    if (self.searchBar != _searchController.searchBar) {
        return;
    }

    UIViewController *controller = self.viewController;
    UINavigationController *navigation = controller.navigationController;
    /*
     hermes:
     A hidden navigation bar cannot host visible search. Reevaluate the mode
     on attachment without changing the application's navigation-bar policy.
     */
    BOOL useNavigationItem = !navigation.navigationBarHidden
        && [navigation.viewControllers containsObject:controller];
    if (_searchAttachedTable == table && _searchAttachedController == controller
       && _searchAttachedToNavigationItem == useNavigationItem) {
        return;
    }

    [self detachSearchController];
    _searchAttachedTable = table;
    _searchAttachedController = controller;
    _searchAttachedToNavigationItem = useNavigationItem;
    _searchHostDefinedPresentationContext = _searchAttachedController.definesPresentationContext;
    _searchAttachedController.definesPresentationContext = YES;
    UISearchBar *bar = _searchController.searchBar;
    if (useNavigationItem) {
        bar.translatesAutoresizingMaskIntoConstraints = YES;
        _searchAttachedController.navigationItem.searchController = _searchController;
    }

    else {
        bar.translatesAutoresizingMaskIntoConstraints = NO;
        table.tableHeaderView = bar;
        _searchAttachmentConstraints = @[
            [bar.widthAnchor constraintEqualToAnchor:table.widthAnchor],
            [bar.heightAnchor constraintEqualToConstant:bar.intrinsicContentSize.height]
        ];
        [NSLayoutConstraint activateConstraints:_searchAttachmentConstraints];
    }

}

- (void)setPropertiesForSection:(SCArrayOfItemsSection *)section
{
    section.dataFetchOptions = self.dataFetchOptions;
    
    section.itemsAccessoryType = self.itemsAccessoryType;
	section.allowAddingItems = self.allowAddingItems;
	section.allowDeletingItems = self.allowDeletingItems;
	section.allowMovingItems = self.allowMovingItems;
	section.allowEditDetailView = self.allowEditDetailView;
	section.allowRowSelection = self.allowRowSelection;
	section.autoSelectNewItemCell = self.autoSelectNewItemCell;
    section.addButtonItem = self.addButtonItem;
	
    section.ibNewItemViewControllerIdentifier = self.ibNewItemViewControllerIdentifier;
    [section setDetailViewControllerOptions:self.detailViewControllerOptions];
    [section setNewItemDetailViewControllerOptions:self.newItemDetailViewControllerOptions];
}

- (void)configureUsingSection:(SCArrayOfItemsSection *)section
{
    if (section.dataStore) {
        self.dataStore = section.dataStore;
    }

    self.dataFetchOptions = section.dataFetchOptions;
    
    self.itemsAccessoryType = section.itemsAccessoryType;
    self.allowAddingItems = section.allowAddingItems;
    self.allowDeletingItems = section.allowDeletingItems;
    self.allowMovingItems = section.allowMovingItems;
    self.allowEditDetailView = section.allowEditDetailView;
    self.allowRowSelection = section.allowRowSelection;
    self.autoSelectNewItemCell = section.autoSelectNewItemCell;
    self.addButtonItem = section.addButtonItem;
    
    self.ibNewItemViewControllerIdentifier = section.ibNewItemViewControllerIdentifier;
    [self setDetailViewControllerOptions:section.detailViewControllerOptions];
    [self setNewItemDetailViewControllerOptions:section.newItemDetailViewControllerOptions];
    
    self.sectionActions.cellForRowAtIndexPath = section.sectionActions.cellForRowAtIndexPath;
}

// override superclass
- (void)setItemsAccessoryType:(UITableViewCellAccessoryType)type
{
	itemsAccessoryType = type;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).itemsAccessoryType = type;
	}

}

// override superclass
- (void)setAllowAddingItems:(BOOL)allow
{
	allowAddingItems = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).allowAddingItems = allow;
	}

}

// override superclass
- (void)setAllowDeletingItems:(BOOL)allow
{
	allowDeletingItems = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).allowDeletingItems = allow;
	}

}

// override superclass
- (void)setAllowMovingItems:(BOOL)allow
{
	allowMovingItems = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).allowMovingItems = allow;
	}

}

// override superclass
- (void)setAllowEditDetailView:(BOOL)allow
{
	allowEditDetailView = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).allowEditDetailView = allow;
	}

}

// override superclass
- (void)setAllowRowSelection:(BOOL)allow
{
	allowRowSelection = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).allowRowSelection = allow;
	}

}

// override superclass
- (void)setAutoSelectNewItemCell:(BOOL)allow
{
	autoSelectNewItemCell = allow;
	for(NSUInteger i=0; i<self.sectionCount; i++) {
		((SCArrayOfItemsSection *)[self sectionAtIndex:i]).autoSelectNewItemCell = allow;
	}

}

- (void)setAddButtonItem:(UIBarButtonItem *)barButtonItem
{
	addButtonItem = barButtonItem;
	
	addButtonItem.target = self;
	addButtonItem.action = @selector(didTapAddButtonItem);
}

- (void)didTapAddButtonItem
{
    if (self.allowAddingItems) {
        [self dispatchEventAddNewItem];
    }

}

- (void)dispatchEventAddNewItem
{
    if (_loadingContents) {
        return;
    }

    // Game plan: delegate presenting the add detail view to SCArrayOfItemsSection
	
	//cancel any search in progress
    if ([self.searchBar.text length]) {
        self.searchBar.text = nil;
    }

    SCArrayOfItemsSection *section;
    if (self.sectionCount) {
        section = (SCArrayOfItemsSection *)[self sectionAtIndex:0];
    }

    else {
        if (!tempSection) {
            tempSection = [self createSectionWithHeaderTitle:nil];
            tempSection.ownerTableViewModel = self;
            [self setPropertiesForSection:tempSection];
        }

        section = tempSection;
    }

    [section dispatchEventAddNewItem];
}

- (void)dispatchEventSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [(SCArrayOfItemsSection *)[self sectionAtIndex:indexPath.section] dispatchEventSelectRowAtIndexPath:indexPath];
}

- (void)dispatchEventRemoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    [(SCArrayOfItemsSection *)[self sectionAtIndex:indexPath.section] dispatchEventRemoveRowAtIndexPath:indexPath];
}

- (void)addNewItem:(NSObject *)newItem
{
    switch (self.dataStore.storeMode)
    {
        case SCStoreModeSynchronous:
            [self.dataStore insertObject:newItem];
            
            [items addObject:newItem];
            [self addNewItemToRespectiveSection:newItem];
            break;
            
        case SCStoreModeAsynchronous:
            [self.dataStore asynchronousInsertObject:newItem
                    success:^()
                    {
                        [self->items addObject:newItem];  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                        [self addNewItemToRespectiveSection:newItem];
                    }
                    failure:^(NSError *error)
                    {
                    }
                    noConnection:^BOOL()
                    {
                        return NO;
                    }
             ];
            break;
    }
}

- (void)addNewItemToRespectiveSection:(NSObject *)newItem
{
    NSUInteger itemIndex = [self.items indexOfObjectIdenticalTo:newItem];
	
	NSString *headerTitle = [self getHeaderTitleForItemAtIndex:itemIndex];
	SCArrayOfItemsSection *section = (SCArrayOfItemsSection *)[self sectionWithHeaderTitle:headerTitle];
    if (!section) {
        // Add new section
        section = [self createSectionWithHeaderTitle:headerTitle];
        [self setPropertiesForSection:section];
        [self addSection:section];
        NSUInteger sectionIndex = [self indexForSection:section];
		
        [self.tableView insertSections:[NSIndexSet indexSetWithIndex:sectionIndex]
							 withRowAnimation:UITableViewRowAnimationLeft];
        if (self.autoGenerateSectionIndexTitles) {
            [self.tableView reloadData]; // reloadSectionIndexTitles not working!
        }

    }

    [[section mutableItems] addObject:newItem];
    if (section.dataFetchOptions.sort) {
        [section.dataFetchOptions sortMutableArray:[section mutableItems]];
    }

    [section addCellForNewItem:newItem];
}

- (void)itemModified:(NSObject *)item inSection:(SCArrayOfItemsSection *)section
{
    [self clearLastReturnedCellData];
    
	NSUInteger oldSectionIndex = [self indexForSection:section];
	NSUInteger newSectionIndex = [self getSectionIndexForItem:item];

    if (oldSectionIndex == newSectionIndex) {
        [section itemModified:item];
    }

    else {
        if (filteredArray) {
            // re-evaluate filter and refresh table view
            [self searchBar:self.searchBar textDidChange:self.searchBar.text];
        }

        else {
            // remove item from old section
            NSIndexPath *oldIndexPath =
            [NSIndexPath indexPathForRow:[[section mutableItems] indexOfObjectIdenticalTo:item]
                               inSection:oldSectionIndex];
            [[section mutableItems] removeObjectAtIndex:oldIndexPath.row];
            section.selectedCellIndexPath = nil;
            NSArray *sectionHeaderTitles = [self getSectionHeaderTitles];
            if ( [section mutableItems].count || (sectionHeaderTitles && [sectionHeaderTitles indexOfObject:section.headerTitle]!=NSNotFound) ) {
                self.activeCell = nil;
                [self.tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:oldIndexPath] withRowAnimation:UITableViewRowAnimationRight];
            }

            else {
                // Remove the empty section
                
                // Connect the addButton back to the model's target & action
                if (section.addButtonItem) {
                    self.addButtonItem = section.addButtonItem;
                }

                section.addButtonItem = nil;
                
                self.activeCell.ownerSection = nil;
                self.activeCell = nil;
                [self removeSectionAtIndex:oldSectionIndex];
                section.ownerTableViewModel = nil;
                
                [self.tableView deleteSections:[NSIndexSet indexSetWithIndex:oldSectionIndex]
                              withRowAnimation:UITableViewRowAnimationRight];
            }

            // add to respective section
            [self addNewItemToRespectiveSection:item];
        }

    }

}

- (void)itemRemoved:(NSObject *)item inSection:(SCArrayOfItemsSection *)section
{
    [(NSMutableArray *)self.items removeObjectIdenticalTo:item];
}

- (void)invalidateItems
{
    itemsInSync = FALSE;
    sectionsInSync = FALSE;
}


// Overrides superclass
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    if (!sectionsInSync) {
        [self generateSections];
    }

    return [super numberOfSectionsInTableView:tableView];
}

// Overrides superclass
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle 
	forRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self clearLastReturnedCellData];
    
	SCArrayOfItemsSection *section = (SCArrayOfItemsSection *)[self sectionAtIndex:indexPath.section];
    
    // Have the respective section remove the item
	[super tableView:tableView commitEditingStyle:editingStyle forRowAtIndexPath:indexPath];
	
	// Remove the section if empty
	NSArray *sectionHeaderTitles = [self getSectionHeaderTitles];
    if (![section mutableItems].count && (!sectionHeaderTitles || [sectionHeaderTitles indexOfObject:section.headerTitle]==NSNotFound) ) {
        [self removeSectionAtIndex:indexPath.section];
        [self.tableView deleteSections:[NSIndexSet indexSetWithIndex:indexPath.section]
							 withRowAnimation:UITableViewRowAnimationRight];
        if (self.autoGenerateSectionIndexTitles) {
            [self.tableView reloadData]; // reloadSectionIndexTitles not working!
        }

    }

}


- (NSString *)safeSearchStringFromString:(NSString *)searchString
{
    NSString *safeSearchString = [searchString stringByReplacingOccurrencesOfString:@"'" withString:@""];
    safeSearchString = [safeSearchString stringByReplacingOccurrencesOfString:@"\\" withString:@""]; // don't allow escape characters
    
    return safeSearchString;
}


#pragma mark -
#pragma mark UISearchBarDelegate methods

- (BOOL)searchBarShouldBeginEditing:(UISearchBar *)searchBar
{
    if (self.lockCellSelection) {
        return FALSE;
    }

    if (self.activeDetailModel) {
        [self.activeDetailModel dismissAllDetailViewsWithCommit:YES];
    }

    BOOL shouldBegin = TRUE;

    if (shouldBegin) {
        [SCModelCenter sharedModelCenter].keyboardIssuer = self.viewController;
    }

    return shouldBegin;
}

- (void)searchBar:(UISearchBar *)sBar selectedScopeButtonIndexDidChange:(NSInteger)selectedScope
{
	
}

- (void)searchBarBookmarkButtonClicked:(UISearchBar *)sBar
{
	
}

- (void)searchBarCancelButtonClicked:(UISearchBar *)sBar
{
	[self.searchBar resignFirstResponder];
	self.searchBar.text = nil;
	
	filteredArray = nil;
    self.addButtonItem.enabled = TRUE;
	
    sectionsInSync = FALSE;
	[self.tableView reloadData];
}

- (void)searchBarResultsListButtonClicked:(UISearchBar *)sBar
{
	
}

- (void)searchBarSearchButtonClicked:(UISearchBar *)sBar
{
	
}


#pragma mark - UISearchResultsUpdating Methods

- (void)updateSearchResultsForSearchController:(UISearchController *)searchController
{
    [self reconcileKeyboardInset];
    // Call the correspoding searchBar delegate
    [self searchBar:searchController.searchBar textDidChange:searchController.searchBar.text];
}

- (void)willPresentSearchController:(UISearchController *)searchController
{
    [self removeKeyboardInset];
}

- (void)didPresentSearchController:(UISearchController *)searchController
{
    [self reconcileKeyboardInset];
}

- (void)didDismissSearchController:(UISearchController *)searchController
{
    [self reconcileKeyboardInset];
}

@end










@implementation SCArrayOfObjectsModel

@synthesize searchPropertyName;


+ (instancetype)modelWithTableView:(UITableView *)tableView
                            items:(NSMutableArray *)items
                  itemsDefinition:(SCDataDefinition *)definition
{
	return [[[self class] alloc] initWithTableView:tableView items:items itemsDefinition:definition];
}

+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition
{
    return [[[self class] alloc] initWithTableView:tableView entityDefinition:definition];
}

+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    return [[[self class] alloc] initWithTableView:tableView entityDefinition:definition filterPredicate:predicate];
}


- (instancetype)init
{
    if ( (self=[super init]) ) {
        searchPropertyName = nil;
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
                  items:(NSMutableArray *)__items
        itemsDefinition:(SCDataDefinition *)definition
{
    SCArrayStore *store = [SCArrayStore storeWithObjectsArray:__items defaultDefiniton:definition];
    
	self = [self initWithTableView:tableView dataStore:store];
    
	return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition
{
    return [self initWithTableView:tableView entityDefinition:definition filterPredicate:nil];
}

- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext defaultEntityDefinition:definition];

    self = [self initWithTableView:tableView dataStore:store];
    if (self) {
        if (predicate) {
            self.dataFetchOptions.filter = TRUE;
            self.dataFetchOptions.filterPredicate = predicate;
        }

    }

    return self;
}




#pragma mark -
#pragma mark UISearchBarDelegate methods

- (void)searchBar:(UISearchBar *)sbar textDidChange:(NSString *)searchText
{
	NSArray *resultsArray = nil;

    if ([sbar.text length] && self.items.count) {
        NSString *safeSearchString = [self safeSearchStringFromString:sbar.text];
        
        SCDataDefinition *objDef = [self.dataStore definitionForObject:[self.items objectAtIndex:0]]; // any object
        if (!self.searchPropertyName) {
            self.searchPropertyName = objDef.titlePropertyName;
        }

        NSArray *searchProperties;
        if ([self.searchPropertyName isEqualToString:@"*"]) {
            searchProperties = [NSMutableArray arrayWithCapacity:objDef.propertyDefinitionCount];
            for(NSUInteger i=0; i<objDef.propertyDefinitionCount; i++) {
                [(NSMutableArray *)searchProperties addObject:[objDef propertyDefinitionAtIndex:i].name];
            }

        }

        else {
            searchProperties = [self.searchPropertyName componentsSeparatedByString:@";"];
        }

        NSMutableString *predicateFormat = [NSMutableString string];
        for(NSUInteger i=0; i<searchProperties.count; i++)
		{
            NSString *property = [searchProperties objectAtIndex:i];
            if (i==0) {
                [predicateFormat appendFormat:@"%@ contains[cd] '%@'", property, safeSearchString];
            }

            else {
                [predicateFormat appendFormat:@" OR %@ contains[cd] '%@'", property, safeSearchString];
            }

        }
        NSPredicate *predicate = [NSPredicate predicateWithFormat:predicateFormat];
		
        @try
		{
            resultsArray = [self.items filteredArrayUsingPredicate:predicate];
        }
        @catch (NSException * e)
		{
            // handle any unexpected property-name behavior gracefully
            resultsArray = [NSArray array]; //empty array
            
            SCDebugLog(@"Warning: Invalid search predicate: %@.", predicate);
        }
		
        // Check for custom results
        NSArray *customResultsArray = nil;
        if (self.modelActions.didComputeSearchResults) {
            customResultsArray = self.modelActions.didComputeSearchResults(self, searchText, resultsArray);
        }

        if (customResultsArray) {
            resultsArray = customResultsArray;
        }

    }

    filteredArray = resultsArray;
    self.addButtonItem.enabled = !filteredArray;
	
    sectionsInSync = FALSE;
    
    [self.tableView reloadData];  // self.tableView automatically returns the correct tableView in case a UISearchController is action
}

@end







@implementation SCArrayOfStringsModel

+ (instancetype)modelWithTableView:(UITableView *)tableView items:(NSMutableArray *)items
{
    return [[[self class] alloc] initWithTableView:tableView items:items];
}


- (instancetype)initWithTableView:(UITableView *)tableView items:(NSMutableArray *)__items
{
    SCArrayStore *stringsArrayStore = [SCArrayStore storeWithObjectsArray:__items defaultDefiniton:[SCStringDefinition definition]];
    
    self = [self initWithTableView:tableView dataStore:stringsArrayStore];
    
    return self;
}

#pragma mark -
#pragma mark UISearchBarDelegate methods

- (void)searchBar:(UISearchBar *)sbar textDidChange:(NSString *)searchText
{
	NSArray *resultsArray = nil;

    if ([sbar.text length]) {
        NSString *safeSearchString = [self safeSearchStringFromString:sbar.text];
        
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"SELF contains[cd] %@", safeSearchString];
        resultsArray = [self.items filteredArrayUsingPredicate:predicate];
		
        // Check for custom results
        NSArray *customResultsArray = nil;
        if (self.modelActions.didComputeSearchResults) {
            customResultsArray = self.modelActions.didComputeSearchResults(self, searchText, resultsArray);
        }

        if (customResultsArray) {
            resultsArray = customResultsArray;
        }

    }

    filteredArray = resultsArray;
    self.addButtonItem.enabled = !filteredArray;
	
    sectionsInSync = FALSE;
	
    [self.tableView reloadData];  // self.tableView automatically returns the correct tableView in case a UISearchController is action
}

@end








#pragma mark - SCSelectionModel





@interface SCSelectionModel ()

- (NSUInteger)itemIndexForCell:(SCTableViewCell *)cell;
- (NSUInteger)itemIndexForCellAtIndexPath:(NSIndexPath *)indexPath;
- (NSIndexPath *)indexPathForItemIndex:(NSInteger)itemIndex;
- (void)buildSelectedItemsIndexesFromString:(NSString *)string;
- (NSString *)buildStringFromSelectedItemsIndexes;

- (void)deselectLastSelectedRow;
- (void)dismissViewController;

@end



@implementation SCSelectionModel

@synthesize boundObject;
@synthesize boundObjectStore;
@synthesize boundPropertyName;
@synthesize allowMultipleSelection;
@synthesize allowNoSelection;
@synthesize maximumSelections;
@synthesize autoDismissViewController;


- (instancetype)init
{
    if ( (self=[super init]) ) {
        boundObject = nil;
        boundObjectStore = nil;
        boundPropertyName = nil;
        
        boundToNSNumber = FALSE;
        boundToNSString = FALSE;
        lastSelectedRowIndexPath = nil;
        allowAddingItems = FALSE;
        allowDeletingItems = FALSE;
        allowMovingItems = FALSE;
        allowEditDetailView = FALSE;
		
        allowMultipleSelection = FALSE;
        allowNoSelection = FALSE;
        maximumSelections = 0;
        autoDismissViewController = FALSE;
        _selectedItemsIndexes = [[NSMutableSet alloc] init];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectedIndexPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems
{
    if ( (self = [self initWithTableView:tableView items:[NSMutableArray arrayWithArray:sectionItems]]) ) {
        boundObject = object;
		
        // Only bind property name if property exists
        if ([SCUtilities propertyName:propertyName existsInObject:self.boundObject]) {
            boundPropertyName = [propertyName copy];
        }

        boundToNSNumber = TRUE;
        allowMultipleSelection = FALSE;
		
        [self reloadBoundValues];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectedIndexesPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems 
 allowMultipleSelection:(BOOL)multipleSelection
{
    if ( (self = [self initWithTableView:tableView items:[NSMutableArray arrayWithArray:sectionItems]]) ) {
        boundObject = object;
		
        // Only bind property name if property exists
        if ([SCUtilities propertyName:propertyName existsInObject:self.boundObject]) {
            boundPropertyName = [propertyName copy];
        }

        allowMultipleSelection = multipleSelection;
		
        [self reloadBoundValues];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectionStringPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems
{
    if ( (self = [self initWithTableView:tableView items:[NSMutableArray arrayWithArray:sectionItems]]) ) {
        boundObject = object;
		
        // Only bind property name if property exists
        if ([SCUtilities propertyName:propertyName existsInObject:self.boundObject]) {
            boundPropertyName = [propertyName copy];
        }

        boundToNSString = TRUE;
		
        [self reloadBoundValues];
    }

    return self;
}


- (void)setBoundValue:(id)value
{
    if (self.boundObject && self.boundPropertyName) {
        if (self.boundObjectStore) {
            [self.boundObjectStore setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            [SCUtilities setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

    }

}

- (NSObject *)boundValue
{
    if (self.boundObject && self.boundPropertyName) {
        if (self.boundObjectStore) {
            return [self.boundObjectStore valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            return [SCUtilities valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

    }

    //else
	return nil;
}

- (NSUInteger)itemIndexForCell:(SCTableViewCell *)cell
{
    return [self.items indexOfObject:cell.textLabel.text];
}

- (NSUInteger)itemIndexForCellAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewCell *cell = [self cellAtIndexPath:indexPath];
    return [self itemIndexForCell:cell];
}

- (NSIndexPath *)indexPathForItemIndex:(NSInteger)itemIndex
{
    for(NSUInteger i=0; i<self.sectionCount; i++)
    {
        SCTableViewSection *section = [self sectionAtIndex:i];
        for(NSUInteger j=0; j<section.cellCount; j++)
        {
            SCTableViewCell *cell = [section cellAtIndex:j];
            if ([self.items indexOfObject:cell.textLabel.text] == itemIndex) {
                return [NSIndexPath indexPathForRow:j inSection:i];
            }

        }
    }
    return nil;
}

- (void)buildSelectedItemsIndexesFromString:(NSString *)string
{
	NSArray *selectionStrings = [string componentsSeparatedByString:@";"];
	
	[self.selectedItemsIndexes removeAllObjects];
	for(NSString *selectionString in selectionStrings)
	{
		NSUInteger index = [self.items indexOfObject:selectionString];
        if (index != NSNotFound) {
            [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
        }

    }
}

- (NSString *)buildStringFromSelectedItemsIndexes
{
	NSMutableArray *selectionStrings = [NSMutableArray arrayWithCapacity:[self.selectedItemsIndexes count]];
	for(NSNumber *index in self.selectedItemsIndexes)
	{
		[selectionStrings addObject:[self.items objectAtIndex:[index intValue]]];
	}
	
	return [selectionStrings componentsJoinedByString:@";"];
}

// override superclass
- (void)reloadBoundValues
{
    [super reloadBoundValues];

    if (boundToNSNumber) {
        if (self.boundValue) {
            [self.selectedItemsIndexes addObject:self.boundValue];
        }

        if (self.boundObject && !self.boundValue) {
            self.boundValue = [NSNumber numberWithInt:-1];
        }

    }

    else if (boundToNSString) {
        if ([SCUtilities isStringClass:[self.boundValue class]] && self.items) {
            [self buildSelectedItemsIndexesFromString:(NSString *)self.boundValue];
        }

    }

    else {
        if (self.boundObject && !self.boundValue) {
            self.boundValue = [NSMutableSet set];   //Empty set
        }

    }

}

// override superclass method
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if ([self.selectedItemsIndexes containsObject:[NSNumber numberWithUnsignedInteger:[self itemIndexForCellAtIndexPath:indexPath]]]) {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
        cell.textLabel.textColor = [UIColor linkColor]; // dgApps
    }

    else {
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.textLabel.textColor = [UIColor labelColor]; // dgApps
    }

    cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    
    [super tableView:tableView willDisplayCell:cell forRowAtIndexPath:indexPath];
}

- (void)deselectLastSelectedRow
{
	[self.tableView deselectRowAtIndexPath:lastSelectedRowIndexPath animated:YES];
}

// override superclass method
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [super tableView:tableView didSelectRowAtIndexPath:indexPath];
    
    NSNumber *itemIndex = [NSNumber numberWithUnsignedInteger:[self itemIndexForCellAtIndexPath:indexPath]];
	UITableViewCell *selectedCell = [tableView cellForRowAtIndexPath:indexPath];
	
	lastSelectedRowIndexPath = indexPath;

    if ([self.selectedItemsIndexes containsObject:itemIndex]) {
        if (!self.allowNoSelection && self.selectedItemsIndexes.count==1) {
            [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];

            if (self.autoDismissViewController) {
                [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
            }

            return;
        }

        //uncheck cell and exit method
        [self.selectedItemsIndexes removeObject:itemIndex];
        if (boundToNSNumber) {
            self.boundValue = self.selectedItemIndex;
        }

        else if (boundToNSString) {
            self.boundValue = [self buildStringFromSelectedItemsIndexes];
        }

        selectedCell.accessoryType = UITableViewCellAccessoryNone;
        selectedCell.textLabel.textColor = [UIColor labelColor]; // dgApps
        [self valueChangedForSectionAtIndex:indexPath.section];
        [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];
        return;
    }

    // Make sure not to exceed maximumSelections
    if (self.allowMultipleSelection && self.maximumSelections!=0 && self.selectedItemsIndexes.count==self.maximumSelections) {
        [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];

        if (self.autoDismissViewController) {
            [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
        }

        return;
    }

    if (!self.allowMultipleSelection && self.selectedItemsIndexes.count) {
        NSIndexPath *oldIndexPath = [self indexPathForItemIndex:[(NSNumber *)[self.selectedItemsIndexes anyObject] intValue]];
        [self.selectedItemsIndexes removeAllObjects];
        UITableViewCell *oldCell = [tableView cellForRowAtIndexPath:oldIndexPath];
        oldCell.accessoryType = UITableViewCellAccessoryNone;
        selectedCell.textLabel.textColor = [UIColor labelColor]; // dgApps
    }

    //check selected cell
	[self.selectedItemsIndexes addObject:itemIndex];
    if (boundToNSNumber) {
        self.boundValue = self.selectedItemIndex;
    }

    else if (boundToNSString) {
        self.boundValue = [self buildStringFromSelectedItemsIndexes];
    }

    selectedCell.accessoryType = UITableViewCellAccessoryCheckmark;

    selectedCell.textLabel.textColor = [UIColor linkColor]; // dgApps

	[self valueChangedForSectionAtIndex:indexPath.section];
	
	[self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.1];

    if (self.autoDismissViewController) {
        if (!self.allowMultipleSelection || self.maximumSelections==0
		   || self.maximumSelections==self.selectedItemsIndexes.count || self.items.count==self.selectedItemsIndexes.count) {
            [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
        }

    }

}

- (void)dismissViewController
{
    if ([self.viewController isKindOfClass:[SCTableViewController class]]) {
        [(SCTableViewController *)self.viewController
		 dismissWithCancelValue:FALSE doneValue:TRUE];
    }

    else if ([self.viewController isKindOfClass:[SCViewController class]]) {
        [(SCViewController *)self.viewController
		 dismissWithCancelValue:FALSE doneValue:TRUE];
    }

}

- (NSMutableSet *)selectedItemsIndexes
{
    if (self.boundObject && !(boundToNSNumber || boundToNSString)) {
        return (NSMutableSet *)self.boundValue;
    }

    //else
	return _selectedItemsIndexes;
}

- (void)setSelectedItemIndex:(NSNumber *)number
{
	NSNumber *num = [number copy];

    if (boundToNSNumber) {
        self.boundValue = num;
    }

    [self.selectedItemsIndexes removeAllObjects];
    if ([number intValue] >= 0) {
        [self.selectedItemsIndexes addObject:num];
    }

}

- (NSNumber *)selectedItemIndex
{
	NSNumber *index = [self.selectedItemsIndexes anyObject];

    if (index) {
        return index;
    }

    //else
	return [NSNumber numberWithInt:-1];
}

@end








#pragma mark - SCObjectSelectionModel








@interface SCObjectSelectionModel ()

- (NSMutableSet *)boundMutableSet;

- (void)selectedItemsIndexesModified;

- (NSInteger)itemIndexForCell:(SCTableViewCell *)cell;
- (NSInteger)itemIndexForCellAtIndexPath:(NSIndexPath *)indexPath;
- (NSIndexPath *)indexPathForItemIndex:(NSInteger)itemIndex;

- (void)deselectLastSelectedRow;
- (void)dismissViewController;

@end



@implementation SCObjectSelectionModel

@synthesize boundObject;
@synthesize boundObjectStore;
@synthesize boundPropertyName;
@synthesize selectedItemsIndexes;
@synthesize allowMultipleSelection;
@synthesize allowNoSelection;
@synthesize maximumSelections;
@synthesize autoDismissViewController;


- (instancetype)init
{
    if ( (self=[super init]) ) {
        boundObject = nil;
        boundObjectStore = nil;
        boundPropertyName = nil;
        
        lastSelectedRowIndexPath = nil;
        itemsAccessoryType = UITableViewCellAccessoryCheckmark;
        allowAddingItems = FALSE;
        allowDeletingItems = FALSE;
        allowMovingItems = FALSE;
        allowEditDetailView = FALSE;
		
        allowMultipleSelection = FALSE;
        allowNoSelection = FALSE;
        maximumSelections = 0;
        autoDismissViewController = FALSE;
        selectedItemsIndexes = [[NSMutableSet alloc] init];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
              boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName
      selectionItemsStore:(SCDataStore *)store
{
    if ( (self = [self initWithTableView:tableView]) ) {
        boundObject = object;
        dataStore = store;
        
        // Only bind property name if property exists
        if ([SCUtilities propertyName:propertyName existsInObject:self.boundObject]) {
            boundPropertyName = [propertyName copy];
        }

        if ([self.boundValue isKindOfClass:[NSMutableSet class]]) {
            allowMultipleSelection = TRUE;
        }

        else {
            allowMultipleSelection = FALSE;
        }

        // Synchronize selectedItemsIndexes
        [self reloadBoundValues];
    }

    return self;
}

- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName
                  items:(NSArray *)selectionItems itemsDefintion:(SCDataDefinition *)definition
{
	SCArrayStore *store = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:selectionItems] defaultDefiniton:definition];
    
    self = [self initWithTableView:tableView boundObject:object selectedObjectPropertyName:propertyName selectionItemsStore:store];
    
    return self;
}


- (void)setBoundValue:(id)value
{
    if (self.boundObject && self.boundPropertyName) {
        if (self.boundObjectStore) {
            [self.boundObjectStore setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            [SCUtilities setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

    }

}

- (NSObject *)boundValue
{
    if (self.boundObject && self.boundPropertyName) {
        if (self.boundObjectStore) {
            return [self.boundObjectStore valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            return [SCUtilities valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

    }

    //else
	return nil;
}

- (NSMutableSet *)boundMutableSet
{
    return (NSMutableSet *)self.boundValue;
}


// override superclass method
- (void)reloadBoundValues
{
    [super reloadBoundValues];
    
    // Synchronize selectedItemsIndexes
    [self.selectedItemsIndexes removeAllObjects];
    if (self.allowMultipleSelection) {
        NSMutableSet *boundSet = [self boundMutableSet];  //optimize
        for(NSObject *obj in boundSet)
        {
            NSUInteger index = [self.items indexOfObjectIdenticalTo:obj];
            if (index != NSNotFound) {
                [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
            }

        }
    }

    else {
        NSObject *selectedObject = [SCUtilities valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        NSUInteger index = [self.items indexOfObjectIdenticalTo:selectedObject];
        if (index != NSNotFound) {
            [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
        }

    }

}

- (void)selectedItemsIndexesModified
{
    if (self.allowMultipleSelection) {
        NSMutableSet *boundValueSet = [self boundMutableSet];
        [boundValueSet removeAllObjects];
        for(NSNumber *index in self.selectedItemsIndexes)
        {
            [boundValueSet addObject:[self.items objectAtIndex:[index intValue]]];
        }
    }

    else {
        NSObject *selectedObject = nil;
        int index = [self.selectedItemIndex intValue];
        if (index >= 0) {
            selectedObject = [self.items objectAtIndex:index];
        }

        self.boundValue = selectedObject;
    }

}


- (NSInteger)itemIndexForCell:(SCTableViewCell *)cell
{
    return [self.items indexOfObjectIdenticalTo:cell.boundObject];
}

- (NSInteger)itemIndexForCellAtIndexPath:(NSIndexPath *)indexPath
{
    SCTableViewCell *cell = [self cellAtIndexPath:indexPath];
    return [self itemIndexForCell:cell];
}

- (NSIndexPath *)indexPathForItemIndex:(NSInteger)itemIndex
{
    for(NSUInteger i=0; i<self.sectionCount; i++)
    {
        SCTableViewSection *section = [self sectionAtIndex:i];
        for(NSUInteger j=0; j<section.cellCount; j++)
        {
            SCTableViewCell *cell = [section cellAtIndex:j];
            if ([self.items indexOfObjectIdenticalTo:cell.boundObject] == itemIndex) {
                return [NSIndexPath indexPathForRow:j inSection:i];
            }

        }
    }
    return nil;
}


// override superclass method
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if ([self.selectedItemsIndexes containsObject:[NSNumber numberWithUnsignedInteger:[self itemIndexForCellAtIndexPath:indexPath]]]) {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;

        cell.textLabel.textColor = [UIColor linkColor]; // dgApps
    }

    else {
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.textLabel.textColor = [UIColor labelColor]; // dgApps
    }

    cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    
    [super tableView:tableView willDisplayCell:cell forRowAtIndexPath:indexPath];
}

- (void)deselectLastSelectedRow
{
	[self.tableView deselectRowAtIndexPath:lastSelectedRowIndexPath animated:YES];
}

// override superclass method
- (void)dispatchEventRemoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    [super dispatchEventRemoveRowAtIndexPath:indexPath];
    
    //deselect removed row
    NSNumber *itemIndex = [NSNumber numberWithInteger:indexPath.row];
    [self.selectedItemsIndexes removeObject:itemIndex];
    [self selectedItemsIndexesModified];
}

// override superclass method
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSNumber *itemIndex = [NSNumber numberWithUnsignedInteger:[self itemIndexForCellAtIndexPath:indexPath]];
	UITableViewCell *selectedCell = [tableView cellForRowAtIndexPath:indexPath];
	
	lastSelectedRowIndexPath = indexPath;

    if ([self.selectedItemsIndexes containsObject:itemIndex]) {
        if (!self.allowNoSelection && self.selectedItemsIndexes.count==1) {
            [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];

            if (self.autoDismissViewController) {
                [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
            }

            return;
        }

        //uncheck cell and exit method
        [self.selectedItemsIndexes removeObject:itemIndex];
        [self selectedItemsIndexesModified];
        
        selectedCell.accessoryType = UITableViewCellAccessoryNone;
        selectedCell.textLabel.textColor = [UIColor labelColor]; // dgApps
        [self valueChangedForSectionAtIndex:indexPath.section];
        [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];
        
        return;
    }

    // Make sure not to exceed maximumSelections
    if (self.allowMultipleSelection && self.maximumSelections!=0 && self.selectedItemsIndexes.count==self.maximumSelections) {
        [self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.05];

        if (self.autoDismissViewController) {
            [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
        }

        return;
    }

    if (!self.allowMultipleSelection && self.selectedItemsIndexes.count) {
        NSIndexPath *oldIndexPath = [self indexPathForItemIndex:[(NSNumber *)[self.selectedItemsIndexes anyObject] intValue]];
        [self.selectedItemsIndexes removeAllObjects];
        UITableViewCell *oldCell = [tableView cellForRowAtIndexPath:oldIndexPath];
        oldCell.accessoryType = UITableViewCellAccessoryNone;
        selectedCell.textLabel.textColor = [UIColor labelColor]; // dgApps
    }

    //check selected cell
	[self.selectedItemsIndexes addObject:itemIndex];
	[self selectedItemsIndexesModified];
    
	selectedCell.accessoryType = UITableViewCellAccessoryCheckmark;

    selectedCell.textLabel.textColor = [UIColor linkColor]; // dgApps
	
	[self valueChangedForSectionAtIndex:indexPath.section];
	
	[self performSelector:@selector(deselectLastSelectedRow) withObject:nil afterDelay:0.1];

    if (self.autoDismissViewController) {
        if (!self.allowMultipleSelection || self.maximumSelections==0
		   || self.maximumSelections==self.selectedItemsIndexes.count || self.items.count==self.selectedItemsIndexes.count) {
            [self performSelector:@selector(dismissViewController) withObject:nil afterDelay:0.4];
        }

    }

}

- (void)dismissViewController
{
    if ([self.viewController isKindOfClass:[SCTableViewController class]]) {
        [(SCTableViewController *)self.viewController
		 dismissWithCancelValue:FALSE doneValue:TRUE];
    }

    else if ([self.viewController isKindOfClass:[SCViewController class]]) {
        [(SCViewController *)self.viewController
             dismissWithCancelValue:FALSE doneValue:TRUE];
    }

}

- (void)setSelectedItemIndex:(NSNumber *)number
{
	NSNumber *num = [number copy];
	
	[self.selectedItemsIndexes removeAllObjects];
    if ([number intValue] >= 0) {
        [self.selectedItemsIndexes addObject:num];
    }

}

- (NSNumber *)selectedItemIndex
{
	NSNumber *index = [self.selectedItemsIndexes anyObject];

    if (index) {
        return index;
    }

    //else
	return [NSNumber numberWithInt:-1];
}

@end






