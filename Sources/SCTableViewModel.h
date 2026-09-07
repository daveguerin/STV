/*
 *  SCTableViewModel.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"
#import "SCUserDefaultsDefinition.h"
#import "SCDataStore.h"
#import "SCTableViewSection.h"
#import "SCInputAccessoryView.h"
#import "SCDetailViewControllerOptions.h"
#import "SCModelActions.h"

@class SCEntityDefinition;
#pragma mark - SCTableViewModel
/// This class is the master mind behind all of Sensible TableView's functionality.
///
/// Sensible TableView provides an alternative easy way to create sophisticated table views very quickly.
/// The sophistication of these table views can range from simple text cells, to cells with controls, to
/// custom cells that get automatically generated from your own classes. SCTableViewModel also automatically
/// generates detail views for common tasks such as selecting cell values or creating new objects.
/// Using SCTableViewModel, you can easily create full functioning applications in a matter of minutes.
///
/// SCTableViewModel is designed to be loosely coupled with your user interface elements. What this
/// means is that you can use SCTableViewModel with Apple's default UITableView or with any of your
/// custom UITableView subclasses. Similarly, you can use SCTableViewModel with any UIViewController, or
/// any of its subclasses, including UITableViewController or your own subclasses.
///
/// Architecture:
///
/// An SCTableViewModel defines a table view model with several sections, each section being of type
/// SCTableViewSection. Each SCTableViewSection can contain several cells, each cell being of type
/// SCTableViewCell.

@interface SCTableViewModel : NSObject <UITableViewDataSource, UITableViewDelegate, UIScrollViewDelegate, SCInputAccessoryViewDelegate>
{
	//internal
    /// The index path of the most recently returned cell.
    NSIndexPath *lastReturnedCellIndexPath;     // used for optimization
    /// The most recently returned table-view cell.
    SCTableViewCell *lastReturnedCell;          // used for optimization
    /// The index path of the last visible cell.
    NSIndexPath *lastVisibleCellIndexPath;      // user for optimization
	/// The target that receives the control action.
	__weak id target;
	/// The selector sent for the control action.
	SEL action;
	/// Property is used internally by the framework to set the master model in a master-detail relationship.
	__weak SCTableViewModel *masterModel;
    /// Holds the currently active detail model.
    __weak SCTableViewModel *activeDetailModel;
	
	__weak UITableView *_tableView;
	/// When set to a valid UIBarButtonItem, SCTableViewModel automatically puts its table view in edit mode when the button is tapped. Note: Must be set if the model is to automatically show/hide editing mode sections.
	UIBarButtonItem *editButtonItem;
	/// Enables keyboard avoidance through scroll insets, never frame resizing.
	BOOL autoResizeForKeyboard;
	/// Whether the keyboard is currently shown.
	BOOL keyboardShown;
	/// The amount by which the keyboard overlaps the table view.
	CGFloat keyboardOverlap;
    SCInputAccessoryView *_inputAccessoryView;
	
	/// The model's table-view sections.
	NSMutableArray *sections;
	
	/// An array of strings that serve as the title of sections in the tableView and appear in the index list on the right side of the tableView. tableView must be in plain style for the index to appear.
	NSArray *sectionIndexTitles;
	/// If TRUE, SCTableViewModel will automatically generate the sectionIndexTitles array from the first letter of each section's header title. Default: FALSE.
	BOOL autoGenerateSectionIndexTitles;
	/// If TRUE, SCTableViewModel will automatically sort its sections alphabetically according to their header title value. Default: FALSE.
	BOOL autoSortSections;
	/// If TRUE, all section header titles will be hidden. Default: FALSE.
	BOOL hideSectionHeaderTitles;
	/// If TRUE, SCTableViewModel will prevent any cell from being selected. Default: FALSE.
	BOOL lockCellSelection;
	/// An integer that you can use to identify different table view models in your application. Any detail model automatically gets its tag set to be the value of its parent model's tag plus one. Default: 0.
	NSInteger tag;
    
    /// Set to TRUE to enable pull-to-refresh functionality on the table view. Default: FALSE.
    BOOL enablePullToRefresh;
    
    /// The current active cell. A cell becomes active if it is selected or if its value changes.
    SCTableViewCell *activeCell;
    /// The indexPath of activeCell.
    NSIndexPath *activeCellIndexPath;
    /// The current active input control.
    UIResponder *activeCellControl;
    
	/// `SCTableViewModel` will automatically enable/disable the commitButton based on the valuesAreValid property, where commitButton is enabled if valuesAreValid is TRUE.
	UIBarButtonItem *commitButton;
    /// Property is used internally by the framework to determine if the table view is in swipe-to-delete editing mode.
    BOOL swipeToDeleteActive;
    
    SCModelActions *_modelActions;
    SCSectionActions *_sectionActions;
    SCCellActions *_cellActions;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCTableViewModel bound to a UITableView.
///
/// Upon the model's initialization, the model sets itself as the table view's dataSource and delegate, and starts providing it with its sections and cells.
///
/// - Parameter tableView: The UITableView to be bound to the model. It's ok for this value to be nil if the table view is not yet available when the model is created.
+ (instancetype)modelWithTableView:(UITableView *)tableView;

/// Returns an initialized `SCTableViewModel` bound to a UITableView.
///
/// Upon the model's initialization, the model sets itself as the table view's dataSource and delegate, and starts providing it with its sections and cells.
///
/// - Parameter tableView: The UITableView to be bound to the model. It's ok for this value to be nil if the table view is not yet available when the model is created.
- (instancetype)initWithTableView:(UITableView *)tableView;

#pragma mark - Configuration

/// Set to TRUE to enable pull-to-refresh functionality on the table view. Default: FALSE.
/// - SeeAlso: ``refreshControl``
@property (nonatomic, readwrite) BOOL enablePullToRefresh;

/// Contains a UIRefreshControl that automatically provides pull-to-refresh functionality to the table view.
/// - Note: enablePullToRefresh must be set to TRUE for this view to take effect.
@property (nonatomic, strong) UIRefreshControl *refreshControl;

/// When set to a valid UIBarButtonItem, SCTableViewModel automatically puts its table view
/// in edit mode when the button is tapped. Note: Must be set if the model is to automatically
/// show/hide editing mode sections.
@property (nonatomic, strong) UIBarButtonItem *editButtonItem;

/// Adjusts content and indicator insets for keyboard avoidance without changing
/// the table frame. Defaults to NO for UITableViewController owners, which manage
/// keyboard avoidance themselves, and YES for other owners.
@property (nonatomic, readwrite) BOOL autoResizeForKeyboard;

/// The content-constraint policy inherited by cells whose sizingMode is Inherit.
/// Default: Automatic. UITableView always measures rows using Auto Layout.
@property (nonatomic, readwrite) SCTableViewCellSizingMode defaultCellSizingMode;

/// The fixed content height inherited by cells in Fixed mode. Stored default: 44.
/// Assigning a positive value selects Fixed mode and updates existing constraints;
/// assigning a nonpositive value selects Automatic. Explicit cell policy wins.
@property (nonatomic, readwrite) CGFloat defaultCellHeight;

/// An array of strings that serve as the title of sections in the tableView and
/// appear in the index list on the right side of the tableView. tableView
/// must be in plain style for the index to appear.
@property (nonatomic, strong) NSArray *sectionIndexTitles;

/// If TRUE, SCTableViewModel will automatically generate the sectionIndexTitles array from
/// the first letter of each section's header title. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoGenerateSectionIndexTitles;

/// If TRUE, SCTableViewModel will automatically sort its sections alphabetically according to their header
/// title value. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoSortSections;

/// If TRUE, all section header titles will be hidden. Default: FALSE.
@property (nonatomic, readwrite) BOOL hideSectionHeaderTitles;

/// If TRUE, SCTableViewModel will prevent any cell from being selected. Default: FALSE.
/// - Note: for preventing individual cells from being selected, use SCTableViewCell "selectable" property.
@property (nonatomic, readwrite) BOOL lockCellSelection;

/// An integer that you can use to identify different table view models in your application. Any detail model automatically gets its tag set to be the value of its parent model's tag plus one. Default: 0.
@property (nonatomic, readwrite) NSInteger tag;

/// An explicitly supplied controller for the model's generated detail views.
/// The application owns its containment and presentation arrangement.
///
/// - Warning: detailViewController must be of type SCViewController or SCTableViewController only.
@property (nonatomic, strong) UIViewController *detailViewController;

/// The set of model action blocks.
@property (nonatomic, readonly) SCModelActions *modelActions;

/// The set of section action blocks that get applied to all the model's sections.
/// - Note: Section actions defined in the model's individual sections will override any actions set here.
@property (nonatomic, readonly) SCSectionActions *sectionActions;

/// The set of cell action blocks that get applied to all the model's cells.
/// - Note: Cell actions defined in the model's individual cells will override any actions set here.
@property (nonatomic, readonly) SCCellActions *cellActions;

#pragma mark - Managing Sections

/// The number of sections in the model.
@property (nonatomic, readonly) NSUInteger sectionCount;

/// Adds a new section to the model.
/// - Parameter section: Must be a valid non nil SCTableViewSection.
- (void)addSection:(SCTableViewSection *)section;

/// Inserts a new section at the specified index.
/// - Parameter section: Must be a valid non nil SCTableViewSection.
/// - Parameter index: Must be less than the total number of sections.
- (void)insertSection:(SCTableViewSection *)section atIndex:(NSUInteger)index;

/// Returns the section at the specified index.
/// - Parameter index: Must be less than the total number of sections.
- (SCTableViewSection *)sectionAtIndex:(NSUInteger)index;

/// Returns the first section with the specified header title.
/// - Parameter title: The header title.
- (SCTableViewSection *)sectionWithHeaderTitle:(NSString *)title;

/// Returns the index of the specified section.
/// - Parameter section: Must be a valid non nil SCTableViewSection.
/// - Returns: If section is not found, method returns NSNotFound.
- (NSUInteger)indexForSection:(SCTableViewSection *)section;

/// Removes the section at the specified index from the model.
/// - Parameter index: Must be less than the total number of section.
- (void)removeSectionAtIndex:(NSUInteger)index;

/// Removes all sections from the model.
- (void)removeAllSections;

/// Generates sections using the given object and its data definition. The method fully utilizes the definition's groups feature by generating a section for each group.
/// - Parameter object: The object that the sections will be generated for.
/// - Parameter definition: The object's definition.
/// - Warning: Important: definition must be the data definition representing the given object.
- (void)generateSectionsForObject:(NSObject *)object withDefinition:(SCDataDefinition *)definition;

/// Generates sections using the given object and its data definition. The method fully utilizes the definition's groups feature by generating a section for each group.
/// - Parameter object: The object that the sections will be generated for.
/// - Parameter definition: The object's definition.
/// - Parameter newObject: Set to TRUE if the generated sections are used represent a newly created fresh object, otherwise set to FALSE.
/// - Warning: Important: definition must be the data definition representing the given object.
- (void)generateSectionsForObject:(NSObject *)object withDefinition:(SCDataDefinition *)definition newObject:(BOOL)newObject;

/// Generates sections using the given object and its data store.
/// - Parameter object: The object that the sections will be generated for.
/// - Parameter store: The object's data store.
- (void)generateSectionsForObject:(NSObject *)object withDataStore:(SCDataStore *)store;

/// Generates sections using the given object and its data store.
/// - Parameter object: The object that the sections will be generated for.
/// - Parameter store: The object's data store.
/// - Parameter newObject: Set to TRUE if the generated sections are used represent a newly created fresh object, otherwise set to FALSE.
- (void)generateSectionsForObject:(NSObject *)object withDataStore:(SCDataStore *)store newObject:(BOOL)newObject;

/// Generates sections using a user defaults definition.
/// - Parameter userDefaultsDefinition: The user defaults definition.
- (void)generateSectionsForUserDefaultsDefinition:(SCUserDefaultsDefinition *)userDefaultsDefinition;

#pragma mark - Managing Cells

/// The current active cell. A cell becomes active if it is selected or if its value changes.
@property (nonatomic, readonly) SCTableViewCell *activeCell;

/// The indexPath of activeCell.
@property (nonatomic, readonly) NSIndexPath *activeCellIndexPath;

/// The current active input control.
@property (nonatomic, readonly) UIResponder *activeCellControl;

/// Assigns the given object (given it's data definition) to be the bound object in all of the model's cells. *
/// - Parameter boundObject: The object bound to the section.
/// - Parameter dataDefinition: The data definition.
- (void)setBoundObjectForAllCells:(NSObject *)boundObject dataDefinition:(SCDataDefinition *)dataDefinition;

/// Assigns the given object (given it's data store) to be the bound object in all of the model's cells. *
/// - Parameter boundObject: The object bound to the section.
/// - Parameter boundObjectStore: The data store for the bound object.
- (void)setBoundObjectForAllCells:(NSObject *)boundObject boundObjectStore:(SCDataStore *)boundObjectStore;

/// Returns the cell at the specified indexPath.
/// - Parameter indexPath: Must be a valid non nil NSIndexPath.
- (SCTableViewCell *)cellAtIndexPath:(NSIndexPath *)indexPath;

/// Returns the index path for the specified cell.
/// - Parameter cell: Must be a valid non nil SCTableViewCell.
/// - Returns: If cell is not found, method returns NSNotFound.
- (NSIndexPath *)indexPathForCell:(SCTableViewCell *)cell;

/// Returns the first cell with the given bound property name. *
/// - Parameter boundPropertyName: The bound property name.
- (SCTableViewCell *)cellWithBoundPropertyName:(NSString *)boundPropertyName;

/// Returns the indexPath of the cell that comes after the specified cell in the model.
/// - Parameter indexPath: The indexPath of the current cell.
/// - Parameter rewind: If TRUE and cell is the very last cell in the model, method returns the indexPath of the cell at the very top.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
- (NSIndexPath *)indexPathForCellAfterCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind;

/// Returns the indexPath of the cell that comes after the specified cell in the model.
/// - Parameter cell: Must be a valid non nil SCTableViewCell.
/// - Parameter rewind: If TRUE and cell is the very last cell in the model, method returns the indexPath of the cell at the very top.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
- (NSIndexPath *)indexPathForCellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Returns the cell that comes after the specified cell in the model.
/// - Parameter cell: Must be a valid non nil SCTableViewCell.
/// - Parameter rewind: If TRUE and cell is the very last cell in the model, method returns the cell at the very top.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
- (SCTableViewCell *)cellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Returns the indexPath of the cell that comes before the specified cell in the model.
/// - Parameter indexPath: The indexPath of the current cell.
/// - Parameter rewind: If TRUE and cell is the very first cell in the model, method returns the indexPath of the last cell.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
- (NSIndexPath *)indexPathForCellBeforeCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind;

/// Returns the indexPath of the cell that comes before the specified cell in the model.
/// - Parameter cell: Must be a valid non nil SCTableViewCell.
/// - Parameter rewind: If TRUE and cell is the very first cell in the model, method returns the indexPath of the last cell.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
/// - SeeAlso: ``moveToNextCellControl:``
- (NSIndexPath *)indexPathForCellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Returns the cell that comes before the specified cell in the model.
/// - Parameter cell: Must be a valid non nil SCTableViewCell.
/// - Parameter rewind: If TRUE and cell is the very first cell in the model, method returns the last cell.
/// - Returns: Returns nil if cell is the last cell in the model and rewind is FALSE, or if cell does not exist in the model.
/// - SeeAlso: ``moveToPreviousCellControl:``
- (SCTableViewCell *)cellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Moves the first responder to the next cell control, automatically scrolling the table view as needed. If rewind is TRUE, the first responder is moved to the very first cell after the last cell has been reached.
/// - Note: This method is typically used when you're overriding the framework's automatic handling of the keyboard's `Return` button.
/// - Parameter rewind: The rewind.
- (void)moveToNextCellControl:(BOOL)rewind;

/// Moves the first responder to the previous cell control, automatically scrolling the table view as needed. If rewind is TRUE, the first responder is moved to the very last cell after the first cell has been reached.
/// - Parameter rewind: The rewind.
- (void)moveToPreviousCellControl:(BOOL)rewind;

#pragma mark - Managing Detail Views

/// Dismisses all detail views, commiting all changes when commit is TRUE, otherwise it will ignore all changes.
/// - Parameter commit: The commit.
- (void)dismissAllDetailViewsWithCommit:(BOOL)commit;

#pragma mark - Managing Model Values

/// TRUE if all the model's section and cell values are valid.
@property (nonatomic, readonly) BOOL valuesAreValid;

/// 	This property is TRUE if any of the model's cells or sections needs to be commited, otherwise it's FALSE.
@property (nonatomic, readonly) BOOL needsCommit;

/// `SCTableViewModel` will automatically enable/disable the commitButton based on the valuesAreValid property, where commitButton is enabled if valuesAreValid is TRUE.
@property (nonatomic, strong) UIBarButtonItem *commitButton;

/// Forces the commit of all section and cell values into their respective bound objects. There is usually no need to call this method manually as it's typically called by the framework when the user is ready to commit changes.
- (void)commitChanges;

/// Reload's the model's bound values in case the associated bound objects or keys valuea has changed by means other than the cells themselves (e.g. external custom code).
- (void)reloadBoundValues;

/// Coalesces an in-place row refit after pending table updates finish. Inactive
/// cells are prepared again; active editors retain their uncommitted input,
/// selection and composition. This does not reload or replace the row.
/// - Parameter cell: The cell whose content or constraints changed.
/// - Parameter animated: Whether to animate the resulting height change.
- (void)invalidateHeightForCell:(SCTableViewCell *)cell animated:(BOOL)animated;

#pragma mark - Miscellaneous

/// 	The UITableView bound to `SCTableViewModel`.
@property (nonatomic, weak) UITableView *tableView;

// dgApps remarked out the deprecated property
// /**
// The UITableView bound to 'SCTableViewModel'.
//
// Warning: This property has been deprecated. Use tableView instead.
// */
//@property (nonatomic, weak) UITableView *modeledTableView;

/// 	The UIViewController of tableView.
@property (nonatomic, readonly) UIViewController *viewController;

/// The keyboard input accessory view responsible for providing keyboard navigation between the different responders. Set to a valid SCInputAccessoryView to enable the accessory view functionality.
@property (nonatomic, strong) SCInputAccessoryView *inputAccessoryView;

/// Clears all contents of the model.
- (void)clear;

/// Sets the editing mode for tableView.
/// - Parameter editing: Whether the table is in editing mode.
/// - Parameter animate: The animate.
- (void)setTableViewEditing:(BOOL)editing animated:(BOOL)animate;

// dgApps remarked out the deprecated method
// /**
// Sets the editing mode for tableView.
//
// Warning: This method has been deprecated. Use setTableViewEditing:animated: instead.
// */
//- (void)setModeledTableViewEditing:(BOOL)editing animated:(BOOL)animate;


/// Returns the detail view controller that would normally be generated for the given cell.
///
/// - Note: This method is typically used to implement 3D Touch Peek and Pop.
/// - Parameter indexPath: The row index path.
- (UIViewController *)detailViewControllerForCellAtIndexPath:(NSIndexPath *)indexPath;


 
#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Returns true if the modeled table view is live and displaying cells.
@property (nonatomic, readonly) BOOL live;

/// Property is used internally by the framework to determine if the table view is in swipe-to-delete editing mode.
@property (nonatomic, readonly) BOOL swipeToDeleteActive;

/// Property is used internally by the framework to set the master model in a master-detail relationship.
@property (nonatomic, weak) SCTableViewModel *masterModel;

/// Property is used internally by the framework to set the master boundObject in a master-detail relationship.
@property (nonatomic, weak) NSObject *masterBoundObject;

/// Property is used internally by the framework to set the master boundObject store in a master-detail relationship.
@property (nonatomic, weak) SCDataStore *masterBoundObjectStore;

/// Holds the currently active detail model.
/// - Warning: Property must only be set internally by the framework.
@property (nonatomic, weak) SCTableViewModel *activeDetailModel;

/// Called internally to rollback to initial cell bound values when their bound object was first assigned.
- (void)rollbackToInitialCellValues;

/// Method called internally by framework to reload cells values, if needed.
- (void)reloadCellsIfNeeded;

/// Warning: Method must only be called internally by the framework.
/// - Parameter cell: The cell to configure.
- (void)setActiveCell:(SCTableViewCell *)cell;

/// Warning: Method must only be called internally by the framework.
/// - Parameter control: The control to initialize.
- (void)setActiveCellControl:(UIResponder *)control;

/// Warning: Method must only be called internally by the framework.
- (void)enterLoadingMode;

/// Warning: Method must only be called internally by the framework.
- (void)exitLoadingMode;

/// Warning: Method must only be called internally by the framework.
- (void)clearLastReturnedCellData;

/// Warning: Method must only be called internally by the framework.
/// - Parameter detailModel: The detail table-view model.
- (void)configureDetailModel:(SCTableViewModel *)detailModel;

/// Warning: Method must only be called internally by the framework.
/// - Parameter aNotification: The a notification.
- (void)keyboardWillShow:(NSNotification *)aNotification;

/// Reconciles keyboard avoidance when only the keyboard's frame changes.
- (void)keyboardWillChangeFrame:(NSNotification *)aNotification;

/// Warning: Method must only be called internally by the framework.
/// - Parameter aNotification: The a notification.
- (void)keyboardWillHide:(NSNotification *)aNotification;

/// Method gets called internally whenever the value of a section changes. This method
/// should only be used when subclassing `SCTableViewModel`. If what you want is to get notified
/// when a section value changes, consider using SCTableViewModelDelegate methods.
///
/// When subclassing `SCTableViewModel`, you can override this method to define custom behaviour when a
/// section value changes. However, you should always call "[super valueChangedForSectionAtIndex:]"
/// somewhere in your subclassed method.
///
/// - Parameter index: Index of the section changed.
- (void)valueChangedForSectionAtIndex:(NSUInteger)index;

/// Method gets called internally whenever the value of a cell changes. This method
/// should only be used when subclassing `SCTableViewModel`. If what you want is to get notified
/// when a cell value changes, consider using either SCTableViewModelDelegate or
/// the cell's actions.
///
/// When subclassing `SCTableViewModel`, you can override this method to define custom behaviour when a
/// cell value changes. However, you should always call "[super valueChangedForRowAtIndexPath:]"
/// somewhere in your subclassed method.
///
/// - Parameter indexPath: Index path of the cell changed.
- (void)valueChangedForRowAtIndexPath:(NSIndexPath *)indexPath;

/// Method used internally by the framework to monitor model modification events.
/// - Parameter _target: The target.
/// - Parameter _action: The action.
- (void)setTargetForModelModifiedEvent:(id)_target action:(SEL)_action;

/// Subclasses should override this method to handle when editButtonItem is tapped.
- (void)didTapEditButtonItem;

/// Method called by refreshControl to initiate refresh.
- (void)pullToRefreshDidStartLoading;

/// Method called internally.
/// - Parameter cell: The cell to configure.
/// - Parameter indexPath: The row index path.
- (void)configureCell:(SCTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath;

// Returns the true vtable view when the model is acting as a proxy for a UISearchController
/// The underlying table view managed by the model.
@property (nonatomic, readonly) UITableView *trueTableView;

@end










#pragma mark - SCArrayOfItemsModel
/// This class subclasses SCTableViewModel to represent an array of any kind of items and will automatically generate its cells from these items. `SCArrayOfItemsModel` will automatically generate a set of SCArrayOfItemsSection(s) if the sectionHeaderTitleForItem modelAction is implemented, otherwise it will only generate a single SCArrayOfItemsSection.
///
/// - Warning: This is an abstract base class, you should never make any direct instances of it.
///
/// - SeeAlso: ``SCArrayOfStringsModel``, ``SCArrayOfObjectsModel``, ``SCArrayOfStringsSection``, ``SCArrayOfObjectsSection``.
@interface SCArrayOfItemsModel : SCTableViewModel <SCTableViewControllerDelegate, UISearchBarDelegate, UISearchResultsUpdating>
{
	/// The section temporarily being edited.
	SCArrayOfItemsSection *tempSection;		//internal
	/// The items currently retained by the active filter.
	NSArray *filteredArray;					//internal
	
	/// The data store that's used to store and fetch the model's items.
	SCDataStore *dataStore;
    /// The options used to fetch the model's items from dataStore.
    SCDataFetchOptions *dataFetchOptions;
    
    BOOL _loadingContents;
    /// Whether generated sections match the backing items.
    BOOL sectionsInSync;
    /// The items fetched from dataStore.
    NSMutableArray *items;
    /// Set to FALSE to disable the section from automatically fetching its items from dataStore. Default: TRUE.
    BOOL autoFetchItems;
    /// Whether the current items match the data store.
    BOOL itemsInSync;
    /// The accessory type of the generated cells.
    UITableViewCellAccessoryType itemsAccessoryType;
	/// Allows/disables adding new cells/items to the items array. Default: TRUE.
	BOOL allowAddingItems;
	/// Allows/disables deleting new cells/items from the items array. Default: TRUE.
	BOOL allowDeletingItems;
	/// Allows/disables moving cells/items from one row to another. Default: FALSE.
	BOOL allowMovingItems;
	/// Allows/disables a detail view for editing items' values. Default: TRUE.
	BOOL allowEditDetailView;
	/// Allows/disables row selection. Default: TRUE.
	BOOL allowRowSelection;
	/// Allows/disables automatic cell selection of newly created items. Default: TRUE.
	BOOL autoSelectNewItemCell;

	/// Options for the generated detail view controller.
	SCDetailViewControllerOptions *detailViewControllerOptions;
    /// Options for the generated detail view controller for new items.
    SCDetailViewControllerOptions *newItemDetailViewControllerOptions;
    
	/// The search bar associated with the model. Once set to a valid UISearchBar, the model will automatically filter its items based on the user's typed search term.
	UISearchBar *searchBar;
}

#pragma mark - Creation and Initialization

/// Returns an initialized `SCArrayOfItemsModel` given a UITableView and a data store.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter store: The data store containing the model's items.
- (instancetype)initWithTableView:(UITableView *)tableView dataStore:(SCDataStore *)store;

#pragma mark - Configuration

/// The data store that's used to store and fetch the model's items.
@property (nonatomic, strong) SCDataStore *dataStore;

/// The options used to fetch the model's items from dataStore.
@property (nonatomic, strong) SCDataFetchOptions *dataFetchOptions;

/// The items fetched from dataStore.
@property (nonatomic, readonly) NSArray *items;

/// Set to FALSE to disable the section from automatically fetching its items from dataStore. Default: TRUE.
@property (nonatomic, readwrite) BOOL autoFetchItems;

/// The accessory type of the generated cells.
@property (nonatomic, readwrite) UITableViewCellAccessoryType itemsAccessoryType;

/// Allows/disables adding new cells/items to the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowAddingItems;

/// Allows/disables deleting new cells/items from the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowDeletingItems;

/// Allows/disables moving cells/items from one row to another. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowMovingItems;

/// Allows/disables a detail view for editing items' values. Default: TRUE.
///
/// Detail views are automatically generated for editing new items. You can control wether the
/// view appears as a modal view or gets pushed to the navigation stack using the detailViewModal
/// property. Modal views have the added feature of giving the end user a Cancel and Done buttons.
/// The Cancel button cancels all user's actions, while the Done button commits them. Also, if the
/// cell's validation is enabled, the Done button will remain disabled until all cells' values
/// are valid.
@property (nonatomic, readwrite) BOOL allowEditDetailView;

/// Allows/disables row selection. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowRowSelection;

/// Allows/disables automatic cell selection of newly created items. Default: TRUE.
@property (nonatomic, readwrite) BOOL autoSelectNewItemCell;

/// Set this property to a valid UIBarButtonItem. When addButtonItem is tapped and allowAddingItems
/// is TRUE, a detail view is automatically generated for the user to enter the new items
/// properties. If the properties are commited, a new item is added to the array.
@property (nonatomic, strong) UIBarButtonItem *addButtonItem;

/// The search bar associated with the model. Once set to a valid UISearchBar, the model will
/// automatically filter its items based on the user's typed search term.
@property (nonatomic, strong) UISearchBar *searchBar;

/// Set to TRUE to use a search controller to perform searches. Default: FALSE.
/// Creation is safe before table attachment. The same controller moves with the
/// model until disabled; supplied inline search bars retain their host layout.
@property (nonatomic, readwrite) BOOL enableSearchController;

/// The search controller used to search the model values.
///
/// - Note: `searchController` only has a value if `enableSearchController` is `TRUE`.
@property (nonatomic, strong, readonly) UISearchController *searchController;

/// Options for the generated detail view controller.
@property (nonatomic, readonly) SCDetailViewControllerOptions *detailViewControllerOptions;

/// Options for the generated detail view controller for new items.
@property (nonatomic, readonly) SCDetailViewControllerOptions *newItemDetailViewControllerOptions;


#pragma mark - Manual Event Control

/// User can call this method to dispatch an AddNewItem event, the same event dispached when the end-user taps addButtonItem.
- (void)dispatchEventAddNewItem;

/// User can call this method to dispatch a SelectRow event, the same event dispached when the end-user selects a cell.
/// - Parameter indexPath: The row index path.
- (void)dispatchEventSelectRowAtIndexPath:(NSIndexPath *)indexPath;

/// User can call this method to dispatch a RemoveRow event, the same event dispached when the end-user taps the delete button on a cell.
/// - Parameter indexPath: The row index path.
- (void)dispatchEventRemoveRowAtIndexPath:(NSIndexPath *)indexPath;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/* For internal use only. */
/// The Interface Builder identifier for the new-item view controller.
@property (nonatomic, copy) NSString *ibNewItemViewControllerIdentifier;

/* For internal use only. */
/// Configures the model using an existing array-of-items section.
/// - Parameter section: The section used to configure the model.
- (void)configureUsingSection:(SCArrayOfItemsSection *)section;

/* Used internally by the framework. */
/// Returns the model's mutable backing items.
- (NSMutableArray *)mutableItems;

/* Used internally by the framework. */
/// Replaces the model's mutable backing items.
/// - Parameter mutableItems: The replacement mutable items.
- (void)setMutableItems:(NSMutableArray *)mutableItems;

/// Subclasses should override this method to handle section creation.
/// - Parameter title: The title.
- (SCArrayOfItemsSection *)createSectionWithHeaderTitle:(NSString *)title;

/// Subclasses should override this method to set additional section properties after creation.
/// - Parameter section: The section used to configure the model.
- (void)setPropertiesForSection:(SCArrayOfItemsSection *)section;

/// Subclasses should override this method to handle when addButtonItem is tapped.
- (void)didTapAddButtonItem;

/// Method called internally by framework when the model should add a new item.
/// - Parameter newItem: Whether the item is newly created.
- (void)addNewItem:(NSObject *)newItem;

/// Method called internally by framework when a model item has been modified.
/// - Parameter item: The item involved in the operation.
/// - Parameter section: The section used to configure the model.
- (void)itemModified:(NSObject *)item inSection:(SCArrayOfItemsSection *)section;

/// Method called internally by framework when a model item has been removed.
/// - Parameter item: The item involved in the operation.
/// - Parameter section: The section used to configure the model.
- (void)itemRemoved:(NSObject *)item inSection:(SCArrayOfItemsSection *)section;

/// Method called internally by framework when the model's items are out of sync with the data store.
- (void)invalidateItems;

/// Method called internally by framework.
/// - Parameter item: The item involved in the operation.
- (NSUInteger)getSectionIndexForItem:(NSObject *)item;

/// Method called internally.
/// - Parameter options: The detail-view-controller options.
- (void)setDetailViewControllerOptions:(SCDetailViewControllerOptions *)options;

/// Method called internally.
/// - Parameter options: The detail-view-controller options.
- (void)setNewItemDetailViewControllerOptions:(SCDetailViewControllerOptions *)options;

@end






#pragma mark - SCArrayOfObjectsModel
/// This class functions as a table view model that is able to represent an array of any kind of objects and automatically generate its cells from these objects. In addition, `SCArrayOfObjectsModel` generates its detail views from the properties of the corresponding object in its items array. Objects in the items array need not all be of the same object type, but they must all decend from NSObject.
///
/// `SCArrayOfItemsModel` will automatically generate a set of SCArrayOfObjectsSection(s) if the sectionHeaderTitleForItem modelAction is implemented, otherwise it will only generate a single SCArrayOfObjectsSection.
@interface SCArrayOfObjectsModel : SCArrayOfItemsModel
{
	/// The name of the object's property that the value of which will be used to search the items array when the user types a search term inside the model's associated search bar. To search more than one property value, separate the property names by a semi-colon (e.g.: @"firstName;lastName"). To search all properties in the object's class definition, set the property to an astrisk (e.g.: @"*"). If the property is not set, it defaults to the value of the object's class definition titlePropertyName property.
	NSString *searchPropertyName;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCArrayOfObjectsModel` given a UITableView and an array of objects.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter items: An array of objects that the model will use to generate its cells.
///   This array must be of type NSMutableArray, as it must support the model's add, delete, and
///   move operations.
/// - Parameter definition: The definition of the objects in the objects array.
+ (instancetype)modelWithTableView:(UITableView *)tableView items:(NSMutableArray *)items itemsDefinition:(SCDataDefinition *)definition;
   

/// Returns an initialized `SCArrayOfObjectsModel` given a UITableView and an array of objects.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter items: An array of objects that the model will use to generate its cells.
///   This array must be of type NSMutableArray, as it must support the model's add, delete, and
///   move operations.
/// - Parameter definition: The definition of the objects in the objects array.
- (instancetype)initWithTableView:(UITableView *)tableView items:(NSMutableArray *)items itemsDefinition:(SCDataDefinition *)definition;

/// Allocates and returns an initialized SCArrayOfObjectsModel given a UITableView
/// and an entity definition.
///
/// - Note: This method creates a model with all the objects that exist in the definition's managedObjectContext.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter definition: The entity definition of the objects.
+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;

/// Allocates and returns an initialized `SCArrayOfObjectsModel` given a UITableView
/// and an entity definition.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter definition: The entity definition of the objects.
/// - Parameter predicate: The predicate that will be used to filter the fetched objects.
+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate;

/// Returns an initialized SCArrayOfObjectsModel given a UITableView
/// and an entity definition.
///
/// - Note: This method creates a model with all the objects that exist in the definition's managedObjectContext.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter definition: The entity definition of the objects.
- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;

/// Returns an initialized `SCArrayOfObjectsModel` given a UITableView
/// and an entity definition.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter definition: The entity definition of the objects.
/// - Parameter predicate: The predicate that will be used to filter the fetched objects.
- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate;


#pragma mark - Configuration

/// The name of the object's property that the value of which will be used to search the items array
/// when the user types a search term inside the model's associated search bar. To search more than one property
/// value, separate the property names by a semi-colon (e.g.: @"firstName;lastName"). To search all
/// properties in the object's class definition, set the property to an astrisk (e.g.: @"*").
/// If the property is not set, it defaults to the value of the object's class definition titlePropertyName property.
@property (nonatomic, copy) NSString *searchPropertyName;

@end







#pragma mark - SCArrayOfStringsModel
/// This class functions as a table view model that is able to represent an array
/// of string items and automatically generate its cells from these items. The class inherits
/// all its funtionality from its superclass: SCArrayOfItemsModel, except that its items
/// array can only contain items of type NSString. `SCArrayOfStringsModel`
/// will automatically generate a set of SCArrayOfStringsSection(s) if the sectionHeaderTitleForItem modelAction is implemented, otherwise it will only generate a single SCArrayOfStringsSection.

@interface SCArrayOfStringsModel : SCArrayOfObjectsModel

/// Allocates and returns an initialized `SCArrayOfStringsModel` given a UITableView and an array of NSString objects.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter items: An array of NSStrings that the model will use to generate its cells.
///   This array must be of type NSMutableArray, as it must support the model's add, delete, and
///   move operations.
+ (instancetype)modelWithTableView:(UITableView *)tableView items:(NSMutableArray *)items;

/// Returns an initialized `SCArrayOfStringsModel` given a UITableView and an array of NSString objects.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter items: An array of NSStrings that the model will use to generate its cells.
///   This array must be of type NSMutableArray, as it must support the model's add, delete, and
///   move operations.
- (instancetype)initWithTableView:(UITableView *)tableView items:(NSMutableArray *)items;

@end









#pragma mark - SCSelectionModel
/// This class functions as a model that is able to provide selection functionality.
/// The cells in this model represent different items that the end-user can select from, and they
/// are generated from NSStrings in its items array. Once a cell is selected, a checkmark appears next
/// to it, similar to Apple's Settings application where a user selects a Ringtone for their
/// iPhone. The section can be configured to allow multiple selection and to allow no selection at all.
///
/// Since this model is based on SCArrayOfStringsModel, it supports automatically generated sections and automatic search functionality.
///
/// There are three ways to set/retrieve the section's selection:
/// - Through binding an object to the model, and specifying a property name to bind the selection index
/// result to. The bound property must be of type NSMutableSet if multiple selection is allowed, otherwise
/// it must be of type NSNumber or NSString.
/// - Through the selectedItemsIndexes or selectedItemIndex properties.
///
/// - SeeAlso: ``SCSelectionSection``.
@interface SCSelectionModel : SCArrayOfStringsModel
{	
    //internal
	/// Whether the value is bound to an `NSNumber` object.
	BOOL boundToNSNumber;	
	/// Whether the value is bound to an `NSString` object.
	BOOL boundToNSString;	
	/// The index path of the most recently selected row.
	NSIndexPath *lastSelectedRowIndexPath; 
	
    /// The model's bound object.
    NSObject *boundObject;
    /// The model's bound object store.
    SCDataStore *boundObjectStore;
	/// The model's bound property name.
	NSString *boundPropertyName;
    
	/// If TRUE, the section allows multiple selection. Default: FALSE.
	BOOL allowMultipleSelection;
	/// If TRUE, the section allows no selection at all. Default: FALSE.
	BOOL allowNoSelection;
	/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
	NSUInteger maximumSelections;
	/// If TRUE, the section automatically dismisses the current view controller when a value is selected. Default: FALSE.
	BOOL autoDismissViewController;
	NSMutableSet *_selectedItemsIndexes;
}

#pragma mark - Creation and Initialization


/// Returns an initialized `SCSelectionModel` given a table view, a bound object,
/// an NSNumber bound property name, and an array of selection items.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter object: The object the model will bind to.
/// - Parameter propertyName: The property name present in the bound object that the section will bind to and
///   will automatically change the value of to reflect the model's current selection. This property must
///   be of type NSNumber and can't be a readonly property. The model will also initialize its selection
///   from the value present in this property.
/// - Parameter sectionItems: An array of the items that the user will choose from. All items must be of
///   an NSString type.
- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectedIndexPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems;

/// Returns an initialized `SCSelectionModel` given a table view, a bound object,
/// a bound property name, an array of selection items, and whether to allow multiple selection.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter object: The object the model will bind to.
/// - Parameter propertyName: The property name present in the bound object that the model will bind to and
///   will automatically change the value of to reflect the model's current selection(s). This property must
///   be of type NSMutableSet. The model will also initialize its selection(s) from the value present
///   in this property. Every item in this set must be an NSNumber that represent the index of the selected cell(s).
/// - Parameter sectionItems: An array of the items that the user will choose from. All items must be of
///   an NSString type.
/// - Parameter multipleSelection: Determines if multiple selection is allowed.
- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectedIndexesPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems 
 allowMultipleSelection:(BOOL)multipleSelection;

/// Returns an initialized `SCSelectionModel` given a table view, a bound object,
/// an NSString bound property name, and an array of selection items.
///
/// - Parameter tableView: The UITableView to be bound to the model.
/// - Parameter object: The object the model will bind to.
/// - Parameter propertyName: The property name present in the bound object that the model will bind to and
///   will automatically change the value of to reflect the model's current selection. This property must
///   be of type NSString and can't be a readonly property. The model will also initialize its selection
///   from the value present in this property.
/// - Parameter sectionItems: An array of the items that the user will choose from. All items must be of
///   an NSString type.
- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object 
    selectionStringPropertyName:(NSString *)propertyName 
                  items:(NSArray *)sectionItems;


#pragma mark - Configuration

/// The model's bound object.
@property (nonatomic, readonly) NSObject *boundObject;

/// The model's bound object store.
@property (nonatomic, strong) SCDataStore *boundObjectStore;

/// The model's bound property name.
@property (nonatomic, readonly) NSString *boundPropertyName;

/// The model's bound value.
@property (nonatomic, strong) NSObject *boundValue;

/// This property reflects the current section's selection. You can set this property
/// to define the section's selection.
///
/// - Note: If you have bound this section to an object or a key, you can define the section's selection
///   using either the bound property value or the key value, respectively.
/// - Note: In case of no selection, this property will be set to an NSNumber of value -1.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// This property reflects the current section's selection(s). You can add index(es) to the set
/// to define the section's selection.
///
/// - Note: If you have bound this section to an object or a key, you can define the section's selection
///   using either the bound property value or the key value, respectively.
@property (nonatomic, readonly) NSMutableSet *selectedItemsIndexes;

/// If TRUE, the section allows multiple selection. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// If TRUE, the section allows no selection at all. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
/// - Note: Only applicable when `allowMultipleSelection` is `TRUE`.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// If TRUE, the section automatically dismisses the current view controller when a value is selected. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoDismissViewController;


@end













#pragma mark - SCObjectSelectionModel
/// This class functions as a model that provides the end-user with an automatically generated
/// list of objects to choose from.
///
/// The selection items are provided in the form of an array of
/// NSObjects, called the items array. `SCObjectSelectionModel` can be configured to allow multiple
/// selection and to allow no selection at all. If allow multiple selection is disabled, then
/// the bound property name of this model must be of type NSObject, otherwise
/// it must be of type NSMutableSet.
///
/// - SeeAlso: ``SCObjectSelectionSection``.
@interface SCObjectSelectionModel : SCArrayOfObjectsModel
{
	/// The index path of the most recently selected row.
	NSIndexPath *lastSelectedRowIndexPath; //internal
    
    /// The model's bound object.
    NSObject *boundObject;
    /// The model's bound object store.
    SCDataStore *boundObjectStore;
	/// The model's bound property name.
	NSString *boundPropertyName;
	
    /// This property reflects the current model selection(s). You can add index(es) to the set to define the section's selection.
    NSMutableSet *selectedItemsIndexes;
	/// If TRUE, the model allows multiple selection. Default: FALSE.
	BOOL allowMultipleSelection;
	/// If TRUE, the model allows no selection at all. Default: FALSE.
	BOOL allowNoSelection;
	/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
	NSUInteger maximumSelections;
	/// If TRUE, the model automatically dismisses the current view controller when a value is selected. Default: FALSE.
	BOOL autoDismissViewController;
}

#pragma mark - Creation and Initialization


/// Returns an initialized `SCObjectSelectionModel` given a table view, bound object, a bound property name, and a selection items store.
/// - Parameter tableView: The model's table view.
/// - Parameter object: The object the model will bind to.
/// - Parameter propertyName: The model's bound property name corresponding to the object selection. If multiple selection is allowed, then property must be of an NSMutableSet type, otherwise, property must be of type NSObject and cannot be a readonly property.
/// - Parameter store: The store containing the selection objects.
- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName
    selectionItemsStore:(SCDataStore *)store;

/// Returns an initialized `SCObjectSelectionModel` given a table view, bound object, a bound property name, and an array of selection items.
/// - Parameter tableView: The model's table view.
/// - Parameter object: The object the model will bind to.
/// - Parameter propertyName: The model's bound property name corresponding to the object selection. If multiple selection is allowed, then property must be of an NSMutableSet type, otherwise, property must be of type NSObject and cannot be a readonly property.
/// - Parameter selectionItems: An array of the items that the user will choose from. All items must be of an NSObject type and all items must be instances of the same class.
/// - Parameter definition: The definition of the selection items.
- (instancetype)initWithTableView:(UITableView *)tableView
            boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName
                  items:(NSArray *)selectionItems itemsDefintion:(SCDataDefinition *)definition;


#pragma mark - Configuration


/// The model's bound object.
@property (nonatomic, strong) NSObject *boundObject;

/// The model's bound object store.
@property (nonatomic, strong) SCDataStore *boundObjectStore;

/// The model's bound property name.
@property (nonatomic, copy) NSString *boundPropertyName;

/// The model's bound value.
@property (nonatomic, strong) NSObject *boundValue;

/// This property reflects the current model selection. You can set this property
/// to define the model's selection.
///
/// - Note: In case of no selection, this property will be set to an NSNumber of value -1.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// This property reflects the current model selection(s). You can add index(es) to the set
/// to define the section's selection.
@property (nonatomic, readonly) NSMutableSet *selectedItemsIndexes;

/// If TRUE, the model allows multiple selection. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// If TRUE, the model allows no selection at all. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
/// - Note: Only applicable when allowMultipleSelection is TRUE.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// If TRUE, the model automatically dismisses the current view controller when a value is selected. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoDismissViewController;


@end




