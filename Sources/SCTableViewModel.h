/*
 *  SCTableViewModel.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
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

NS_ASSUME_NONNULL_BEGIN

@class SCEntityDefinition;
#pragma mark - SCTableViewModel
/// Coordinates a table view's sections, cells, bindings, and detail flows.
///
/// The model supplies the data source and delegate for a `UITableView` or
/// subclass. Sections contain static or data-generated cells; specialized
/// models add item lists, search, and selection. The owning view controller is
/// discovered through the table's responder chain.
///
/// UIKit measures rows and supplementary views using Auto Layout. STV's numeric
/// height settings configure content constraints rather than table row heights.
/// Keep a strong reference to the model; its table view reference is weak.

@interface SCTableViewModel : NSObject <UITableViewDataSource, UITableViewDelegate, UIScrollViewDelegate, SCInputAccessoryViewDelegate> {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    NSMutableArray<SCTableViewSection *> *sections;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCTableViewModel bound to a
/// UITableView.
///
/// Upon the model's initialization, the model sets itself as the table view's
/// dataSource and delegate, and starts providing it with its sections and
/// cells.
///
/// - Parameter tableView: The table to model, or nil for later attachment. It's
///   ok for this value to be nil if the table view is not yet available when
///   the model is created.
+ (instancetype)modelWithTableView:(nullable UITableView *)tableView;

/// Returns an initialized `SCTableViewModel` bound to a UITableView.
///
/// Upon the model's initialization, the model sets itself as the table view's
/// dataSource and delegate, and starts providing it with its sections and
/// cells.
///
/// - Parameter tableView: The table to model, or nil for later attachment. It's
///   ok for this value to be nil if the table view is not yet available when
///   the model is created.
- (instancetype)initWithTableView:(nullable UITableView *)tableView;

#pragma mark - Configuration

/// Set to TRUE to enable pull-to-refresh functionality on the table view.
/// Default: FALSE.
/// - SeeAlso: ``refreshControl``
@property (nonatomic, readwrite) BOOL enablePullToRefresh;

/// The refresh control installed when `enablePullToRefresh` is YES.
/// Initialization creates a `UIRefreshControl` and wires its value-changed
/// action. The default action reloads bound values and the table, ends
/// refreshing, then invokes `modelActions.didRefresh`. Assign nil to remove the
/// control.
@property (nonatomic, nullable, strong) UIRefreshControl *refreshControl;

/// A button whose target and action are replaced to toggle table editing. Use
/// this button or `setTableViewEditing:animated:` to coordinate editing-mode
/// section changes; directly changing UIKit's editing state bypasses that work.
/// Defaults to nil.
@property (nonatomic, nullable, strong) UIBarButtonItem *editButtonItem;

/// Adjusts content and vertical scroll-indicator insets for keyboard avoidance
/// without changing the table frame. Assigning `tableView` resets this to NO
/// for `UITableViewController` owners and YES for other owners. Plain `init`
/// leaves it NO until a table assignment.
@property (nonatomic, readwrite) BOOL autoResizeForKeyboard;

/// Content-constraint policy for cells whose `sizingMode` is Inherit. Defaults
/// to `SCTableViewCellSizingModeAutomatic`. Changing this invalidates existing
/// sizing constraints and reloads the table after pending updates complete.
/// UIKit still measures rows using Auto Layout.
@property (nonatomic, readwrite) SCTableViewCellSizingMode defaultCellSizingMode;

/// Fixed content height used when a cell resolves to Fixed sizing without its
/// own positive height. Stored default: 44 points. Assigning a positive value
/// selects the model's Fixed mode; assigning a nonpositive value selects
/// Automatic. Explicit cell sizing policy takes precedence.
@property (nonatomic, readwrite) CGFloat defaultCellHeight;

/// Optional strings displayed in the table's section index; defaults to nil.
/// They do not set section header titles. By default an index entry maps to the
/// section at the same index, clamped to the last section. Use
/// `modelActions.sectionForSectionIndexTitle` for a different mapping.
@property (nonatomic, nullable, strong) NSArray<NSString *> *sectionIndexTitles;

/// Generates index titles on access from the first character of each nonempty
/// section header instead of returning the assigned `sectionIndexTitles` value.
/// Defaults to NO. Empty titles are skipped and repeated initials are retained;
/// provide a custom index-to-section action if these require a different
/// mapping.
@property (nonatomic, readwrite) BOOL autoGenerateSectionIndexTitles;

/// Sorts existing sections by header title when enabled. Defaults to NO.
/// `addSection:` also sorts unless `modelActions.sortSections` is provided;
/// `insertSection:atIndex:` preserves its requested insertion position.
@property (nonatomic, readwrite) BOOL autoSortSections;

/// Suppresses STV-generated header labels. Defaults to NO. Custom `headerView`
/// content and explicit header-height constraints are not hidden.
@property (nonatomic, readwrite) BOOL hideSectionHeaderTitles;

/// Prevents user row selection through the model's selection delegate when YES.
/// Defaults to NO. This is not a lock on programmatic selection or active-cell
/// updates.
/// - Note: Use a cell's `selectable` property to prevent selection of
///   individual rows.
@property (nonatomic, readwrite) BOOL lockCellSelection;

/// An integer you can use to identify different table view models in your
/// application. Any detail model automatically gets its tag set to be the value
/// of its parent model's tag plus one. Default: 0.
@property (nonatomic, readwrite) NSInteger tag;

/// Optional shared STV detail controller supplied by the application; defaults
/// to nil. Use an `SCViewController` or `SCTableViewController`. Assignment
/// configures its model's master/tag relationship. Existing-item detail flows
/// can reuse this controller; the application manages its containment when it
/// is already visible. Use detail-controller actions for per-row custom UIKit
/// controllers.
@property (nonatomic, nullable, strong) UIViewController *detailViewController;

/// The set of model action blocks.
@property (nonatomic, readonly) SCModelActions *modelActions;

/// The set of section action blocks that get applied to all the model's
/// sections.
/// - Note: Section actions defined in the model's individual sections will
///   override any actions set here.
@property (nonatomic, readonly) SCSectionActions *sectionActions;

/// Fallback cell action blocks for the model's cells. For a given callback, an
/// individual cell's action takes precedence, followed by its section's cell
/// action, then the model's cell action.
@property (nonatomic, readonly) SCCellActions *cellActions;

#pragma mark - Managing Sections

/// The number of sections in the model.
@property (nonatomic, readonly) NSUInteger sectionCount;

/// Adds a section, sets its owner, applies configured sorting, and invokes
/// section-added actions. Does not reload the table.
/// - Parameter section: The nonnil section to add.
- (void)addSection:(SCTableViewSection *)section;

/// Inserts a section without automatically sorting, assigns ownership, and
/// invokes section-added actions. Does not reload the table.
/// - Parameter section: The nonnil section to insert.
/// - Parameter index: An index from zero through `sectionCount`, inclusive.
- (void)insertSection:(SCTableViewSection *)section atIndex:(NSUInteger)index;

/// Returns the section at an index, or nil if the index is out of range.
/// - Parameter index: The zero-based section index.
- (nullable SCTableViewSection *)sectionAtIndex:(NSUInteger)index;

/// Returns the first section with an equal header title, or nil if none
/// matches.
/// - Parameter title: The title to match; nil matches a section with a nil
///   title.
- (nullable SCTableViewSection *)sectionWithHeaderTitle:(nullable NSString *)title;

/// Returns the index of the specified section.
/// - Parameter section: Must be a valid non nil SCTableViewSection.
/// - Returns: If section is not found, method returns NSNotFound.
- (NSUInteger)indexForSection:(SCTableViewSection *)section;

/// Removes a section and invokes `modelActions.didRemoveSection`, if set. Does
/// not reload the table.
/// - Parameter index: An index less than `sectionCount`. Invalid indexes raise
///   an array bounds exception.
- (void)removeSectionAtIndex:(NSUInteger)index;

/// Removes all sections and clears the active cell and control. Does not reload
/// the table or invoke individual section-removal actions.
- (void)removeAllSections;

/// Appends sections for an object using its definition's property groups. Empty
/// groups are skipped; properties expanded into the current view can split a
/// group into multiple sections. Existing sections are not removed.
/// - Parameter object: The bound object, or nil for unbound generation where
///   the definition and store support it.
/// - Parameter definition: The definition describing the object; used to create
///   a compatible data store.
/// - Note: Uses detail-mode property inclusion (`newObject` is NO).
- (void)generateSectionsForObject:(nullable NSObject *)object withDefinition:(SCDataDefinition *)definition;

/// Appends sections for an object using its definition's property groups. Empty
/// groups are skipped; properties expanded into the current view can split a
/// group into multiple sections. Existing sections are not removed.
/// - Parameter object: The bound object, or nil for unbound generation where
///   the definition and store support it.
/// - Parameter definition: The definition describing the object; used to create
///   a compatible data store.
/// - Parameter newObject: YES to include creation-mode properties; NO to
///   include detail-mode properties.
- (void)generateSectionsForObject:(nullable NSObject *)object withDefinition:(SCDataDefinition *)definition newObject:(BOOL)newObject;

/// Appends sections for an object using its definition's property groups. Empty
/// groups are skipped; properties expanded into the current view can split a
/// group into multiple sections. Existing sections are not removed.
/// - Parameter object: The bound object, or nil for unbound generation where
///   the definition and store support it.
/// - Parameter store: The store supplying the object definition. Nil supplies
///   no definition and generates no sections.
/// - Note: Uses detail-mode property inclusion (`newObject` is NO).
- (void)generateSectionsForObject:(nullable NSObject *)object withDataStore:(nullable SCDataStore *)store;

/// Appends sections for an object using its definition's property groups. Empty
/// groups are skipped; properties expanded into the current view can split a
/// group into multiple sections. Existing sections are not removed.
/// - Parameter object: The bound object, or nil for unbound generation where
///   the definition and store support it.
/// - Parameter store: The store supplying the object definition. Nil supplies
///   no definition and generates no sections.
/// - Parameter newObject: YES to include creation-mode properties; NO to
///   include detail-mode properties.
- (void)generateSectionsForObject:(nullable NSObject *)object withDataStore:(nullable SCDataStore *)store newObject:(BOOL)newObject;

/// Appends sections bound to the definition's standard user-defaults object.
/// - Parameter userDefaultsDefinition: The keys, property settings, and groups
///   used to generate the sections.
- (void)generateSectionsForUserDefaultsDefinition:(SCUserDefaultsDefinition *)userDefaultsDefinition;

#pragma mark - Managing Cells

/// The current active cell. A cell becomes active if it is selected or if its
/// value changes.
@property (nonatomic, nullable, readonly) SCTableViewCell *activeCell;

/// The index path recorded when the active cell is assigned, or nil if
/// unavailable. Structural changes can invalidate this recorded path.
@property (nonatomic, nullable, readonly) NSIndexPath *activeCellIndexPath;

/// The current active input control.
@property (nonatomic, nullable, readonly) UIResponder *activeCellControl;

/// Assigns a bound object and store to cells reachable through each section's
/// current `cellCount`. Does not regenerate sections or change property names.
/// - Parameter boundObject: The object to bind, or nil to clear the binding.
/// - Parameter dataDefinition: The definition used to create a compatible
///   store.
- (void)setBoundObjectForAllCells:(nullable NSObject *)boundObject dataDefinition:(SCDataDefinition *)dataDefinition;

/// Assigns a bound object and store to cells reachable through each section's
/// current `cellCount`. Does not regenerate sections or change property names.
/// - Parameter boundObject: The object to bind, or nil to clear the binding.
/// - Parameter boundObjectStore: The binding store, or nil to use store-free
///   cell binding behavior.
- (void)setBoundObjectForAllCells:(nullable NSObject *)boundObject boundObjectStore:(nullable SCDataStore *)boundObjectStore;

/// Retrieves a model cell, generating or reusing it for array-backed sections.
/// This is not limited to visible UIKit cells and does not run `willConfigure`.
/// - Parameter indexPath: A current row path. Nil or an `NSNotFound` row
///   returns nil.
/// - Returns: The cell, or nil when unavailable. Use a valid row index for the
///   section; array-backed sections can raise for an out-of-range row.
- (nullable SCTableViewCell *)cellAtIndexPath:(nullable NSIndexPath *)indexPath;

/// Looks up a cell through the recent-cell cache and each section's lookup.
/// - Parameter cell: The cell to locate; nil returns nil.
/// - Returns: A row path, or nil when the lookup finds no match.
/// - Warning: Array-backed sections rely on UIKit's cell lookup rather than
///   searching offscreen generated cells. A detached cell can be misreported as
///   row zero of an array-backed first section; do not use this to test
///   arbitrary cell membership.
- (nullable NSIndexPath *)indexPathForCell:(nullable SCTableViewCell *)cell;

/// Returns the first matching cell in the model's `SCObjectSection` sections.
/// Other section types are not searched.
/// - Parameter boundPropertyName: The bound property name to match.
/// - Returns: The matching cell, or nil if none is found.
- (nullable SCTableViewCell *)cellWithBoundPropertyName:(NSString *)boundPropertyName;

/// Computes the next row path, advancing into the immediately following
/// section.
/// - Parameter indexPath: A valid current row path.
/// - Parameter rewind: YES to wrap from the end to row zero of section zero.
/// - Returns: The next path, or nil at the end without wrapping or for a model
///   containing exactly one section with one cell.
/// - Warning: Empty sections are not skipped and the input is not validated.
///   Verify the resulting path before retrieving a cell.
- (nullable NSIndexPath *)indexPathForCellAfterCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind;

/// Computes the next row path after locating the supplied cell.
/// - Parameter cell: A cell whose current index path is resolvable in this
///   model.
/// - Parameter rewind: YES to wrap from the end to the first section.
/// - Returns: The result of `indexPathForCellAfterCellAtIndexPath:rewind:`.
/// - Warning: This does not guard against an unresolved cell path or skip empty
///   sections.
- (nullable NSIndexPath *)indexPathForCellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Returns the UIKit cell at the next computed row path.
/// - Parameter cell: A cell whose current index path is resolvable in this
///   model.
/// - Parameter rewind: YES to wrap at the end.
/// - Returns: The cell returned by UIKit, or nil if the path or cell is
///   unavailable. This method does not generate an offscreen model cell.
/// - SeeAlso: `indexPathForCellAfterCell:rewind:`
- (nullable SCTableViewCell *)cellAfterCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Computes the previous row path, moving to the immediately preceding section
/// when the current row is zero.
/// - Parameter indexPath: A valid current row path.
/// - Parameter rewind: YES to wrap from the start to the last section's final
///   row.
/// - Returns: The previous path, or nil at the start without wrapping, when the
///   wrap target has no cells, or for one section containing one cell.
/// - Warning: Empty intermediate sections are not skipped and the input is not
///   validated. Verify the resulting path before retrieving a cell.
- (nullable NSIndexPath *)indexPathForCellBeforeCellAtIndexPath:(NSIndexPath *)indexPath rewind:(BOOL)rewind;

/// Legacy convenience lookup intended to locate the preceding row.
/// - Parameter cell: A cell whose current index path is resolvable in this
///   model.
/// - Parameter rewind: Whether the delegated traversal wraps at the end.
/// - Returns: The result of the current implementation's forward traversal.
/// - Warning: This method currently calls
///   `indexPathForCellAfterCellAtIndexPath:rewind:`. For backward traversal,
///   obtain the cell's path and call
///   `indexPathForCellBeforeCellAtIndexPath:rewind:` instead.
- (nullable NSIndexPath *)indexPathForCellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Legacy convenience lookup intended to return the preceding UIKit cell.
/// - Parameter cell: A cell whose current index path is resolvable in this
///   model.
/// - Parameter rewind: Whether the delegated traversal wraps at the end.
/// - Returns: The UIKit cell at the computed path, or nil if unavailable.
/// - Warning: This delegates to `indexPathForCellBeforeCell:rewind:`, whose
///   current implementation traverses forward. It does not generate offscreen
///   model cells.
- (nullable SCTableViewCell *)cellBeforeCell:(SCTableViewCell *)cell rewind:(BOOL)rewind;

/// Moves first responder to the next input control, scrolling as needed.
/// Custom-cell input controls are visited in tag order before changing rows. If
/// no eligible control is found, the active cell resigns and is cleared.
/// - Parameter rewind: YES to wrap at the traversal boundary; NO to stop there.
- (void)moveToNextCellControl:(BOOL)rewind;

/// Moves first responder to the previous input control, scrolling as needed.
/// Custom-cell input controls are visited in tag order before changing rows. If
/// no eligible control is found, the active cell resigns and is cleared.
/// - Parameter rewind: YES to wrap at the traversal boundary; NO to stop there.
- (void)moveToPreviousCellControl:(BOOL)rewind;

#pragma mark - Managing Detail Views

/// Recursively requests dismissal along the active-detail-model chain. For STV
/// controllers, clears the controller delegate and sends Done or Cancel flags,
/// then clears `activeDetailModel`.
/// - Parameter commit: YES to send Done; NO to send Cancel.
/// - Warning: This method does not itself call `commitChanges` or roll back
///   values. Because it clears controller delegates, it is not a substitute for
///   normal section/cell detail-commit callbacks or a transaction rollback.
- (void)dismissAllDetailViewsWithCommit:(BOOL)commit;

#pragma mark - Managing Model Values

/// TRUE if all the model's section and cell values are valid.
@property (nonatomic, readonly) BOOL valuesAreValid;

/// YES if any section reports `needsCommit`. This is not a general data-store
/// dirty flag; programmatic binding changes and selection caches are not
/// necessarily reflected here.
@property (nonatomic, readonly) BOOL needsCommit;

/// Optional button whose enabled state is set from `valuesAreValid` on
/// assignment and cell-value-change notifications. Defaults to nil. Assignment
/// does not install a commit target/action, and arbitrary programmatic binding
/// or section changes do not continuously recompute its enabled state.
@property (nonatomic, nullable, strong) UIBarButtonItem *commitButton;

/// Calls `commitCellChanges` on every section. Does not validate first or force
/// cells whose commit is already up to date. Persistence and failure handling
/// depend on the cells and their stores.
- (void)commitChanges;

/// Reloads bound values through each section and clears the recent-cell cache.
/// The base implementation also asks an explicitly supplied STV detail
/// controller to lose focus. Array-backed subclasses invalidate/refetch items
/// and regenerate sections instead. Reload the table when the visible structure
/// needs refreshing.
- (void)reloadBoundValues;

/// Coalesces an in-place row refit after pending table updates finish. Cells
/// without active input are prepared again; active editors retain their input,
/// selection, and composition. The row is not reloaded or replaced.
/// - Parameter cell: The changed cell. Nil, an unresolved cell path, or a
///   missing table makes this a no-op.
/// - Parameter animated: Whether the resulting height change is animated.
///   Coalesced requests animate if any request asks for animation.
- (void)invalidateHeightForCell:(nullable SCTableViewCell *)cell animated:(BOOL)animated;

#pragma mark - Miscellaneous

/// The weakly held table view modeled by this instance. Assignment installs the
/// model as data source and delegate, enables selection during editing, and
/// configures UIKit automatic row/header/footer sizing and estimates. In an
/// array model, the getter returns the search-results table while its search
/// controller is active; `trueTableView` remains the original table.
@property (nonatomic, nullable, weak) UITableView *tableView;

/// The first view controller in `trueTableView`'s responder chain. Returns nil
/// before the table is attached to a controller's view hierarchy.
@property (nonatomic, nullable, readonly) UIViewController *viewController;

/// The keyboard input accessory view responsible for providing keyboard
/// navigation between the different responders. Set to a valid
/// SCInputAccessoryView to enable the accessory view functionality.
@property (nonatomic, nullable, strong) SCInputAccessoryView *inputAccessoryView;

/// Removes sections and clears the active cell, control, and recorded cell
/// path. Does not reset configuration, disconnect the table, or reload it.
- (void)clear;

/// Coordinates table/controller editing with section transitions and model
/// editing actions. The should-begin/end actions can veto the transition.
/// - Parameter editing: The requested editing state.
/// - Parameter animate: Whether UIKit should animate the change.
- (void)setTableViewEditing:(BOOL)editing animated:(BOOL)animate;

/// Asks the row's section to generate/configure its detail controller without
/// presenting it. Generation can invoke detail actions and mutate detail
/// models.
/// - Parameter indexPath: A valid current row path, preferably for a visible
///   row when the section's generation depends on its UIKit cell.
/// - Returns: The detail controller, or nil if the row supplies no detail view.
- (nullable UIViewController *)detailViewControllerForCellAtIndexPath:(NSIndexPath *)indexPath;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Internal loading/scrolling gate: YES when not loading, dragging, or
/// decelerating. This does not test visibility or attachment and can be YES
/// without a table.
@property (nonatomic, readonly) BOOL live;

/// Property is used internally by the framework to determine if the table view
/// is in swipe-to-delete editing mode.
@property (nonatomic, readonly) BOOL swipeToDeleteActive;

/// Property is used internally by the framework to set the master model in a
/// master-detail relationship.
@property (nonatomic, nullable, weak) SCTableViewModel *masterModel;

/// Property is used internally by the framework to set the master boundObject
/// in a master-detail relationship.
@property (nonatomic, nullable, weak) NSObject *masterBoundObject;

/// Property is used internally by the framework to set the master boundObject
/// store in a master-detail relationship.
@property (nonatomic, nullable, weak) SCDataStore *masterBoundObjectStore;

/// Holds the currently active detail model.
/// - Warning: Property must only be set internally by the framework.
@property (nonatomic, nullable, weak) SCTableViewModel *activeDetailModel;

/// Internal hook that asks static cells to restore their initial bound values.
/// Array-backed sections are skipped; this is not a general data-store
/// rollback.
- (void)rollbackToInitialCellValues;

/// Internal hook that asks object sections to reload custom control values when
/// needed.
- (void)reloadCellsIfNeeded;

/// Internal hook that changes the active cell, dispatches selection/deselection
/// actions, records its path, and scrolls/selects it in UIKit.
/// - Parameter cell: A cell belonging to this model, or nil to clear active
///   selection.
- (void)setActiveCell:(nullable SCTableViewCell *)cell;

/// Internal hook that records the active input responder.
/// - Parameter control: The active responder, or nil to clear it.
- (void)setActiveCellControl:(nullable UIResponder *)control;

/// Internal hook that marks the model as loading, making `live` return NO.
- (void)enterLoadingMode;

/// Internal hook that clears loading state and invalidates the recent-cell
/// cache.
- (void)exitLoadingMode;

/// Internal hook that marks the cached cell unconfigured and clears its cached
/// path.
- (void)clearLastReturnedCellData;

/// Internal hook that assigns the detail model's master and parent tag plus
/// one.
/// - Parameter detailModel: The detail model to configure; nil is a no-op.
- (void)configureDetailModel:(nullable SCTableViewModel *)detailModel;

/// Internal keyboard-show hook that records the keyboard frame and reconciles
/// avoidance insets.
/// - Parameter aNotification: The corresponding UIKit keyboard notification.
- (void)keyboardWillShow:(NSNotification *)aNotification;

/// Internal hook that reconciles keyboard avoidance when the keyboard frame
/// changes.
/// - Parameter aNotification: The corresponding UIKit keyboard notification.
- (void)keyboardWillChangeFrame:(NSNotification *)aNotification;

/// Internal keyboard-hide hook that removes STV's avoidance insets.
/// - Parameter aNotification: The corresponding UIKit keyboard notification.
- (void)keyboardWillHide:(NSNotification *)aNotification;

/// Notifies the internal modification target and the applicable section
/// `valueChanged` action. Override only for subclass behavior and call super.
/// - Parameter index: The index of the changed section.
- (void)valueChangedForSectionAtIndex:(NSUInteger)index;

/// Updates the active cell and commit-button validation, then notifies the
/// internal modification target and applicable cell `valueChanged` action.
/// Override only for subclass behavior and call super.
/// - Parameter indexPath: The changed cell's current index path.
- (void)valueChangedForRowAtIndexPath:(NSIndexPath *)indexPath;

/// Internal hook that records a weak modification target and selector.
/// - Parameter _target: The receiver, or nil to stop notifications.
/// - Parameter _action: A no-argument selector supported by the target; use nil
///   only when clearing the target.
- (void)setTargetForModelModifiedEvent:(nullable id)_target action:(nullable SEL)_action;

/// Button action that toggles editing through `setTableViewEditing:animated:`.
/// Subclasses may extend this behavior and call super to retain coordination.
- (void)didTapEditButtonItem;

/// Refresh-control action that reloads bound values and the table, ends
/// refreshing, and invokes `modelActions.didRefresh`.
- (void)pullToRefreshDidStartLoading;

/// Internal hook that invokes the applicable `willConfigure` action and marks
/// the cell configured. It is not the complete binding or sizing-preparation
/// pass.
/// - Parameter cell: The cell being prepared.
/// - Parameter indexPath: Its current row path.
- (void)configureCell:(SCTableViewCell *)cell atIndexPath:(NSIndexPath *)indexPath;

// Returns the true vtable view when the model is acting as a proxy for a UISearchController
/// Internal access to the originally assigned table, even during search. Unlike
/// `tableView`, this never redirects to a search-results table.
@property (nonatomic, nullable, readonly) UITableView *trueTableView;

@end


#pragma mark - SCArrayOfItemsModel
/// Base model for store-backed item lists with generated sections and search
/// UI. Items are grouped by `modelActions.sectionHeaderTitleForItem`; without
/// that action, nonempty lists share an untitled section. Empty lists need not
/// have a section. `modelActions.sectionHeaderTitles` can precreate named
/// sections.
///
/// Use `SCArrayOfObjectsModel` or `SCArrayOfStringsModel` for concrete lists
/// and their search implementations. The base section factory creates object
/// sections; subclasses can override section creation and configuration.
/// - SeeAlso: `SCArrayOfObjectsModel`, `SCArrayOfStringsModel`
@interface SCArrayOfItemsModel : SCTableViewModel <SCTableViewControllerDelegate, UISearchBarDelegate, UISearchResultsUpdating> {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    NSArray<id> *_Nullable filteredArray;
    SCDataStore *_Nullable _dataStore;
    BOOL sectionsInSync;
    UITableViewCellAccessoryType _itemsAccessoryType;
    BOOL _allowAddingItems;
    BOOL _allowDeletingItems;
    BOOL _allowMovingItems;
    BOOL _allowEditDetailView;
}

#pragma mark - Creation and Initialization

/// Returns an initialized `SCArrayOfItemsModel` given a UITableView and a data
/// store.
///
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter store: The store supplying items, or nil until configured.
- (instancetype)initWithTableView:(nullable UITableView *)tableView dataStore:(nullable SCDataStore *)store;

#pragma mark - Configuration

/// Optional store used to fetch and persist the model's items. Assignment
/// invalidates the item cache and creates compatible fetch options only when
/// none are already assigned. Existing fetch options are retained when
/// replacing the store; configure them explicitly if the new store needs
/// different options.
@property (nonatomic, nullable, strong) SCDataStore *dataStore;

/// Optional options used for item fetching, filtering, sorting, and batching.
/// Assigning options invalidates cached items. If the assigned options have no
/// sort key and a store exists, the setter supplies the definition's key
/// property name. In-place option changes require explicit reload/invalidation.
@property (nonatomic, nullable, strong) SCDataFetchOptions *dataFetchOptions;

/// The cached items used to generate sections, before search filtering. Reading
/// this property can trigger a store fetch when `autoFetchItems` is YES. It can
/// be nil before fetching and can temporarily contain a loading cell during an
/// asynchronous fetch. Do not assume every entry is a domain object.
@property (nonatomic, nullable, readonly) NSArray<id> *items;

/// Whether stale items are fetched automatically when accessed. Defaults to
/// YES. Set to NO only when the model's items are supplied by
/// framework/subclass coordination rather than automatic store fetching.
@property (nonatomic, readwrite) BOOL autoFetchItems;

/// The accessory type assigned to ordinary generated detail rows. Defaults to
/// `UITableViewCellAccessoryDisclosureIndicator` in the base class; selection
/// and string-section specializations can override the appearance.
@property (nonatomic, readwrite) UITableViewCellAccessoryType itemsAccessoryType;

/// Allows/disables adding new cells/items to the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowAddingItems;

/// Allows/disables deleting cells/items from the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowDeletingItems;

/// Allows/disables moving cells/items from one row to another. Default: FALSE.
/// Moving also requires the data store to validate ordering changes; the flag
/// alone does not make an unordered store reorderable.
@property (nonatomic, readwrite) BOOL allowMovingItems;

/// Whether selecting an existing item opens its detail editor. Defaults to YES
/// in the base class. Selection subclasses and string sections can disable it.
/// Use `detailViewControllerOptions` for existing-item presentation and
/// `newItemDetailViewControllerOptions` for new-item content/navigation
/// settings. This flag does not disable the separate new-item creation flow.
@property (nonatomic, readwrite) BOOL allowEditDetailView;

/// Controls the selection appearance/accessory of ordinary generated rows.
/// Defaults to YES.
/// - Warning: The current implementation does not make these rows nonselectable
///   or gate direct selection dispatch. Use the model's `lockCellSelection` or
///   a cell's `selectable` property to prevent user selection.
@property (nonatomic, readwrite) BOOL allowRowSelection;

/// Whether a newly inserted row remains selected after insertion. Defaults to
/// NO. The insertion path scrolls to and briefly selects the row even when this
/// is NO. An explicitly supplied active detail controller can subsequently
/// select it.
@property (nonatomic, readwrite) BOOL autoSelectNewItemCell;

/// Optional button that starts item creation when `allowAddingItems` is YES.
/// Assignment replaces its target and action. A new item is created by the
/// store and normally edited in a generated detail controller before insertion.
/// Defaults to nil.
@property (nonatomic, nullable, strong) UIBarButtonItem *addButtonItem;

/// Optional search bar used by concrete string/object models to filter fetched
/// items in memory as the text changes. Defaults to nil. For a supplied inline
/// bar, assignment installs the model as delegate but does not place or size
/// the bar. With STV's search controller, results update through
/// `UISearchResultsUpdating`. Filtering regenerates sections and disables the
/// add button while a filtered result array is active; it does not replace the
/// store's fetch predicate.
@property (nonatomic, nullable, strong) UISearchBar *searchBar;

/// Creates and attaches a `UISearchController` when YES. Defaults to NO.
/// Creation is safe before table attachment. STV places its own search bar in a
/// visible navigation bar's navigation item, or in the table header otherwise,
/// and reuses the controller across table assignments until disabled. A
/// separately supplied inline `searchBar` keeps its existing host layout and is
/// not moved into the search controller.
@property (nonatomic, readwrite) BOOL enableSearchController;

/// The search controller used to search the model values.
///
/// - Note: `searchController` only has a value if `enableSearchController` is
///   `TRUE`.
@property (nonatomic, nullable, strong, readonly) UISearchController *searchController;

/// Options for the generated detail view controller.
@property (nonatomic, readonly) SCDetailViewControllerOptions *detailViewControllerOptions;

/// Lazily created options for new-item controller content and navigation
/// settings.
/// - Note: The current add flow chooses its presentation mode from
///   `detailViewControllerOptions.presentationMode`, treating Auto as Modal; it
///   does not use this object's `presentationMode`.
@property (nonatomic, readonly) SCDetailViewControllerOptions *newItemDetailViewControllerOptions;


#pragma mark - Manual Event Control

/// Starts the new-item flow directly; does nothing while an item fetch is
/// active or when the store cannot create an item. Unlike the add-button
/// handler, this method does not test `allowAddingItems`.
- (void)dispatchEventAddNewItem;

/// Dispatches directly to the section's row-selection behavior, such as opening
/// an item editor or toggling a selection in selection-section subclasses. This
/// does not reproduce all of UIKit's selection delegate callbacks.
/// - Parameter indexPath: A valid current row path in the owning table.
- (void)dispatchEventSelectRowAtIndexPath:(NSIndexPath *)indexPath;

/// Requests deletion through the section's `willDeleteItem` action and store,
/// then updates cached items and table rows. It does not test
/// `allowDeletingItems`. Asynchronous deletion is requested without waiting for
/// store completion.
/// - Parameter indexPath: A valid current item row, not a special cell.
- (void)dispatchEventRemoveRowAtIndexPath:(NSIndexPath *)indexPath;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/* For internal use only. */
/// Internal storyboard identifier for a supplied new-item controller. Nil or an
/// empty string uses the generated-controller path.
@property (nonatomic, nullable, copy) NSString *ibNewItemViewControllerIdentifier;

/* For internal use only. */
/// Configures the model using an existing array-of-items section.
/// - Parameter section: The section used to configure the model.
- (void)configureUsingSection:(SCArrayOfItemsSection *)section;

/* Used internally by the framework. */
/// Internal access to the model's mutable item cache without triggering a
/// fetch. This is framework/subclass storage access, not a separate public data
/// source.
- (nullable NSMutableArray<id> *)mutableItems;

/* Used internally by the framework. */
/// Internal hook that replaces the model's mutable item cache directly. Does
/// not set synchronization flags or update the table.
/// - Parameter mutableItems: The replacement cache, or nil.
- (void)setMutableItems:(nullable NSMutableArray<id> *)mutableItems;

/// Subclass factory hook for generated sections. The base creates an
/// `SCArrayOfObjectsSection` using the model's store, disables its automatic
/// fetch, and initializes an empty item cache.
/// - Parameter title: The generated header title, or nil for an untitled
///   section.
- (SCArrayOfItemsSection *)createSectionWithHeaderTitle:(nullable NSString *)title;

/// Internal/subclass hook that shares fetch/detail options and copies
/// item-interaction settings into a newly generated section. Call super before
/// adding subclass-specific configuration.
/// - Parameter section: The generated section to configure.
- (void)setPropertiesForSection:(SCArrayOfItemsSection *)section;

/// Add-button action that dispatches item creation only when `allowAddingItems`
/// is YES. Subclasses may extend this behavior.
- (void)didTapAddButtonItem;

/// Method called internally by framework when the model should add a new item.
/// - Parameter newItem: The newly created object to add.
- (void)addNewItem:(NSObject *)newItem;

/// Internal hook that refreshes a modified item and, when its grouping title
/// changes, moves it between generated sections or recomputes search results.
/// - Parameter item: The existing modified item.
/// - Parameter section: The item's previous section.
- (void)itemModified:(NSObject *)item inSection:(SCArrayOfItemsSection *)section;

/// Internal notification that removes an already-deleted item from the model's
/// item cache by identity. It does not issue another store deletion.
/// - Parameter item: The removed object.
/// - Parameter section: The section that removed it.
- (void)itemRemoved:(NSObject *)item inSection:(SCArrayOfItemsSection *)section;

/// Internal hook that marks both items and generated sections out of sync. The
/// next automatic fetch/section-generation access refreshes them; this method
/// does not reload the table itself.
- (void)invalidateItems;

/// Internal lookup of an item's grouping section.
/// - Parameter item: An item present in the current filtered or unfiltered item
///   array.
/// - Returns: Zero for an untitled group, the matching section index, or
///   `NSNotFound` if a titled group has no matching section.
/// - Warning: Passing an item absent from the array can raise an index
///   exception.
- (NSUInteger)getSectionIndexForItem:(NSObject *)item;

/// Internal hook that replaces the stored detail options.
/// - Parameter options: The options to retain, or nil to recreate default
///   options on the next getter access.
- (void)setDetailViewControllerOptions:(nullable SCDetailViewControllerOptions *)options;

/// Internal hook that replaces the stored detail options.
/// - Parameter options: The options to retain, or nil to recreate default
///   options on the next getter access.
- (void)setNewItemDetailViewControllerOptions:(nullable SCDetailViewControllerOptions *)options;

@end


#pragma mark - SCArrayOfObjectsModel
/// Generates object-list rows and property-based detail editors from a data
/// store. The store must be able to resolve a definition for each object's
/// type. Add additional definitions with `dataStore`'s `addDataDefinition:`
/// method. Grouping follows `SCArrayOfItemsModel`'s section-header actions.
@interface SCArrayOfObjectsModel : SCArrayOfItemsModel


#pragma mark - Creation and Initialization

/// Creates a `SCArrayOfObjectsModel` backed by an `SCArrayStore`.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter items: The mutable backing array, retained rather than copied.
///   Nil leaves the store without a backing array; provide a mutable array
///   before expecting persistent additions.
/// - Parameter definition: The default object definition. Register additional
///   definitions with the resulting data store when items have different types.
+ (instancetype)modelWithTableView:(nullable UITableView *)tableView items:(nullable NSMutableArray<id> *)items itemsDefinition:(SCDataDefinition *)definition;


/// Creates a `SCArrayOfObjectsModel` backed by an `SCArrayStore`.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter items: The mutable backing array, retained rather than copied.
///   Nil leaves the store without a backing array; provide a mutable array
///   before expecting persistent additions.
/// - Parameter definition: The default object definition. Register additional
///   definitions with the resulting data store when items have different types.
- (instancetype)initWithTableView:(nullable UITableView *)tableView items:(nullable NSMutableArray<id> *)items itemsDefinition:(SCDataDefinition *)definition;

/// Allocates and returns an initialized SCArrayOfObjectsModel given a
/// UITableView and an entity definition.
///
/// - Note: Fetches objects of the specified entity in its managed object
///   context, not every entity in the context. Fetch options can further
///   restrict results.
///
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter definition: The entity definition of the objects.
+ (instancetype)modelWithTableView:(nullable UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;

/// Allocates and returns an initialized `SCArrayOfObjectsModel` given a
/// UITableView and an entity definition.
///
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter definition: The entity definition of the objects.
/// - Parameter predicate: The predicate used to filter fetched objects, or nil
///   for no additional predicate.
+ (instancetype)modelWithTableView:(nullable UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(nullable NSPredicate *)predicate;

/// Returns an initialized SCArrayOfObjectsModel given a UITableView and an
/// entity definition.
///
/// - Note: Fetches objects of the specified entity in its managed object
///   context, not every entity in the context. Fetch options can further
///   restrict results.
///
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter definition: The entity definition of the objects.
- (instancetype)initWithTableView:(nullable UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;

/// Returns an initialized `SCArrayOfObjectsModel` given a UITableView and an
/// entity definition.
///
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter definition: The entity definition of the objects.
/// - Parameter predicate: The predicate used to filter fetched objects, or nil
///   for no additional predicate.
- (instancetype)initWithTableView:(nullable UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(nullable NSPredicate *)predicate;


#pragma mark - Configuration

/// Property names used for case- and diacritic-insensitive substring matching
/// on the fetched items. Separate names with semicolons, for example
/// `@"firstName;lastName"`. `@"*"` uses every property name in the first item's
/// definition. The stored default is nil. On the first nonempty search with
/// items, nil is replaced with the first item's definition's
/// `titlePropertyName`.
/// - Warning: Choose valid searchable values for every item. The wildcard does
///   not restrict itself to string properties. Predicate-evaluation exceptions
///   produce no matches; malformed property-name expressions can fail during
///   predicate construction. Apostrophes and backslashes are removed from the
///   search text.
@property (nonatomic, nullable, copy) NSString *searchPropertyName;

@end


#pragma mark - SCArrayOfStringsModel
/// An object-list model specialized for `NSString` items and string search. Its
/// array initializer creates an `SCArrayStore` with `SCStringDefinition`.
/// Search matches string contents case- and diacritic-insensitively; section
/// grouping follows `SCArrayOfItemsModel`'s actions. Unlike the standalone
/// string-section initializer, this model's generated object sections allow
/// detail editing by default.

@interface SCArrayOfStringsModel : SCArrayOfObjectsModel

/// Creates a `SCArrayOfStringsModel` backed by an `SCArrayStore`.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter items: The mutable backing array, retained rather than copied.
///   Nil leaves the store without a backing array; provide a mutable array
///   before expecting persistent additions. Entries must be NSString values.
+ (instancetype)modelWithTableView:(nullable UITableView *)tableView items:(nullable NSMutableArray<id> *)items;

/// Creates a `SCArrayOfStringsModel` backed by an `SCArrayStore`.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter items: The mutable backing array, retained rather than copied.
///   Nil leaves the store without a backing array; provide a mutable array
///   before expecting persistent additions. Entries must be NSString values.
- (instancetype)initWithTableView:(nullable UITableView *)tableView items:(nullable NSMutableArray<id> *)items;

@end


#pragma mark - SCSelectionModel
/// Presents string items as a checkmarked single- or multiple-selection model.
/// Bind a writable `NSNumber` property to an item index, an `NSMutableSet` of
/// `NSNumber` indexes to selections, or an `NSString` to selected item text
/// separated by semicolons. Alternatively, configure the cached indexes
/// directly. Adding, deleting, moving, and detail editing default to NO.
///
/// Indexes refer to `items`, not necessarily visible row positions after
/// grouping or filtering. String matching uses equality; duplicate item titles
/// cannot be distinguished reliably by the model's title-based lookup.
/// - SeeAlso: `SCSelectionCell`
@interface SCSelectionModel : SCArrayOfStringsModel


#pragma mark - Creation and Initialization


/// Creates a `SCSelectionModel` with index selection binding.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSNumber property storing a zero-based
///   item index, or -1 for no selection. A missing bound value is initialized
///   to -1. Nil or an unrecognized property name leaves the instance without a
///   property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
- (instancetype)initWithTableView:(nullable UITableView *)tableView boundObject:(nullable NSObject *)object selectedIndexPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;

/// Creates a `SCSelectionModel` with index-set selection binding.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A property holding an NSMutableSet of NSNumber
///   item indexes. A missing value is initialized to an empty mutable set when
///   binding is available. Nil or an unrecognized property name leaves the
///   instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Parameter multipleSelection: Whether more than one item can be selected.
- (instancetype)initWithTableView:(nullable UITableView *)tableView boundObject:(nullable NSObject *)object selectedIndexesPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems allowMultipleSelection:(BOOL)multipleSelection;

/// Creates a `SCSelectionModel` with string selection binding.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSString property containing selected
///   item text, separated by semicolons for multiple selections. Nil or an
///   unrecognized property name leaves the instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Note: Semicolons in item text are not escaped. Multiple selections are
///   serialized in set enumeration order, not guaranteed item order.
- (instancetype)initWithTableView:(nullable UITableView *)tableView boundObject:(nullable NSObject *)object selectionStringPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;


#pragma mark - Configuration

/// The model's bound object.
@property (nonatomic, nullable, readonly) NSObject *boundObject;

/// The model's bound object store.
@property (nonatomic, nullable, strong) SCDataStore *boundObjectStore;

/// The model's bound property name.
@property (nonatomic, nullable, readonly) NSString *boundPropertyName;

/// Reads or writes the bound property through `boundObjectStore`, or through
/// STV property access when no store is supplied. Without a bound object and
/// property name, reads return nil and writes do nothing. Assignment does not
/// rebuild the cached selection.
/// - Warning: In NSNumber-binding mode, the current `reloadBoundValues` adds
///   the bound index without first clearing older cached indexes. Use
///   `selectedItemIndex` for programmatic single-index changes rather than
///   relying on reload to replace the selection.
@property (nonatomic, nullable, strong) NSObject *boundValue;

/// One selected item index, or `@(-1)` when the selected-index set is empty.
/// With multiple selections, the returned index is an arbitrary member, not
/// necessarily the lowest. Assignment clears the previous indexes and adds the
/// supplied nonnegative index; an NSNumber-bound instance also writes that
/// binding. It does not refresh visible checkmarks or serialize an NSString
/// binding.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// The mutable set of selected `NSNumber` item indexes. For a set-bound object
/// this is the bound set itself and may be nil if the bound value is nil;
/// otherwise it is an internal selection cache. Direct mutation does not update
/// NSNumber/NSString bindings or reload the table. Use only valid indexes into
/// `items`.
@property (nonatomic, nullable, readonly) NSMutableSet<NSNumber *> *selectedItemsIndexes;

/// Allows the user to select more than one item. Plain initialization defaults
/// to NO; the set-binding initializer uses its `multipleSelection` argument.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// Allows the user to uncheck the final selected item. Defaults to NO. NO does
/// not automatically select an item when the initial selection is empty.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// User-interaction limit for multiple selection. Zero means no limit; default:
/// 0. This does not validate or trim programmatically assigned indexes. It
/// applies only when `allowMultipleSelection` is YES.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// Requests Done-style dismissal from an `SCViewController` or
/// `SCTableViewController` owner after selection handling. Defaults to NO. A
/// newly checked item dismisses for single selection, an unlimited selection
/// count, a reached maximum, or all items selected. Tapping an already-selected
/// final item that cannot be unchecked, or attempting to exceed the maximum,
/// also requests dismissal. Unchecking an item does not.
@property (nonatomic, readwrite) BOOL autoDismissViewController;


@end


#pragma mark - SCObjectSelectionModel
/// Presents store-backed objects as a checkmarked selection model. Single
/// selection binds an object-valued property; multiple selection uses an
/// existing mutable set of selected objects. Initializers infer multiple
/// selection from an `NSMutableSet` bound value. Items are matched by identity
/// when bound values are reloaded. Adding, deleting, moving, and detail editing
/// default to NO.
@interface SCObjectSelectionModel : SCArrayOfObjectsModel


#pragma mark - Creation and Initialization


/// Creates a `SCObjectSelectionModel` with object selection binding.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter store: The store supplying selectable objects, or nil until
///   configured.
- (instancetype)initWithTableView:(nullable UITableView *)tableView boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItemsStore:(nullable SCDataStore *)store;

/// Creates a `SCObjectSelectionModel` with object selection binding.
/// - Parameter tableView: The table to model, or nil for later attachment.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter selectionItems: Selectable objects, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Parameter definition: The default definition for selectable objects.
///   Register other object definitions with the resulting data store as needed.
- (instancetype)initWithTableView:(nullable UITableView *)tableView boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)selectionItems itemsDefintion:(SCDataDefinition *)definition;


#pragma mark - Configuration


/// The model's bound object.
@property (nonatomic, nullable, strong) NSObject *boundObject;

/// The model's bound object store.
@property (nonatomic, nullable, strong) SCDataStore *boundObjectStore;

/// The model's bound property name.
@property (nonatomic, nullable, copy) NSString *boundPropertyName;

/// Reads or writes the bound property through `boundObjectStore`, or through
/// STV property access when no store is supplied. Without a bound object and
/// property name, reads return nil and writes do nothing. Assigning a value
/// does not itself rebuild the cached selection; call `reloadBoundValues` when
/// needed.
@property (nonatomic, nullable, strong) NSObject *boundValue;

/// One cached selected item index, or `@(-1)` when no indexes are selected.
/// With multiple selection, the getter returns an arbitrary member. Assignment
/// clears the cache and adds a nonnegative index, but does not commit the bound
/// object selection or refresh visible checkmarks.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// The mutable cache of selected `NSNumber` indexes into `items`. Direct
/// mutation does not itself persist the object selection or refresh the table.
/// To change the persisted selection programmatically, update the bound object
/// value/set, then call `reloadBoundValues` and refresh UIKit. The base model's
/// `commitChanges` does not serialize this cache.
@property (nonatomic, readonly) NSMutableSet<NSNumber *> *selectedItemsIndexes;

/// Enables selection of multiple objects. Plain initialization defaults to NO;
/// binding initializers set it to YES when the current bound value is an
/// `NSMutableSet`. When enabling this manually, provide an existing mutable
/// bound set; this property does not create one.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// Allows the user to uncheck the final selected object. Defaults to NO. NO
/// does not automatically choose an object for an initially empty selection.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// User-interaction limit for multiple selection. Zero means no limit; default:
/// 0. This does not validate or trim programmatically assigned indexes. It
/// applies only when `allowMultipleSelection` is YES.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// Requests Done-style dismissal from an `SCViewController` or
/// `SCTableViewController` owner after selection handling. Defaults to NO. A
/// newly checked item dismisses for single selection, an unlimited selection
/// count, a reached maximum, or all items selected. Tapping an already-selected
/// final item that cannot be unchecked, or attempting to exceed the maximum,
/// also requests dismissal. Unchecking an item does not.
@property (nonatomic, readwrite) BOOL autoDismissViewController;


@end

NS_ASSUME_NONNULL_END
