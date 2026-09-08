/*
 *  SCTableViewSection.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019-2026 dgApps. All rights reserved.
 *
 */

#import "SCGlobals.h"
#import "SCDataDefinition.h"
#import "SCDataStore.h"
#import "SCSectionActions.h"
#import "SCTableViewCell.h"
#import "SCTableViewController.h"
#import "SCDetailViewControllerOptions.h"
#import "SCExpandCollapseCell.h"
#import "SCFetchItemsCell.h"

NS_ASSUME_NONNULL_BEGIN

@class SCEntityDefinition;


#pragma mark - SCTableViewSection
/// A table-model section containing static cells or subclass-generated rows.
/// Add sections through `SCTableViewModel` so ownership is configured. Static
/// cell mutations do not reload UIKit automatically; coordinate table updates
/// with the model's new structure.
@interface SCTableViewSection : NSObject {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    __weak SCTableViewModel *_Nullable _ownerTableViewModel;
    NSObject *_Nullable _boundObject;
    SCDataStore *_Nullable _boundObjectStore;
    NSString *_Nullable _boundPropertyName;
    NSMutableArray<id> *_Nullable _cells;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCTableViewSection.
+ (instancetype)section;

/// Allocates and returns an initialized SCTableViewSection given a header
/// title.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle;

/// Allocates and returns an initialized SCTableViewSection given a header and a
/// footer title.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter sectionFooterTitle: A footer title for the section, or nil.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle footerTitle:(nullable NSString *)sectionFooterTitle;

/// Returns an initialized SCTableViewSection given a header title.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle;

/// Returns an initialized SCTableViewSection given a header and a footer title.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter sectionFooterTitle: A footer title for the section, or nil.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle footerTitle:(nullable NSString *)sectionFooterTitle;

#pragma mark - Configuration

/// The owner table view model of the section.
///
/// - Warning: This property gets set automatically by the section's owner, you
///   should never set this property manually
@property (nonatomic, nullable, weak) SCTableViewModel *ownerTableViewModel;

/// Optional title rendered in uppercase by STV's generated header label.
/// Defaults to nil; a custom `headerView` supplies its own content instead.
@property (nonatomic, nullable, copy) NSString *headerTitle;

/// Optional fixed constraint on the header container's content height, in
/// points. Defaults to `UITableViewAutomaticDimension`. Nonnegative values
/// install a constraint at priority 999; negative values leave content
/// self-sizing. UIKit still measures the supplementary view using Auto Layout.
@property (nonatomic) CGFloat headerHeight;

/// Custom header content with intrinsic size or a complete constraint chain.
/// UIKit measures it in a header/footer container; frame height is not sizing
/// policy. A UITableViewHeaderFooterView is used directly.
@property (nonatomic, nullable, strong) UIView *headerView;

/// Optional title rendered by STV's generated footer label. Defaults to nil; a
/// custom `footerView` supplies its own content instead.
@property (nonatomic, nullable, copy) NSString *footerTitle;

/// Optional fixed constraint on the footer container's content height, in
/// points. Defaults to `UITableViewAutomaticDimension`. Nonnegative values
/// install a constraint at priority 999; negative values leave content
/// self-sizing. UIKit still measures the supplementary view using Auto Layout.
@property (nonatomic) CGFloat footerHeight;

/// Custom footer content with intrinsic size or a complete constraint chain.
/// UIKit measures it in a header/footer container; frame height is not sizing
/// policy. A UITableViewHeaderFooterView is used directly.
@property (nonatomic, nullable, strong) UIView *footerView;

/// The set of section action blocks.
@property (nonatomic, readonly) SCSectionActions *sectionActions;

/// The set of cell action blocks that get applied to all the section's cells.
/// - Note: Cell actions defined in the section's individual cells will override
///   any actions set here.
@property (nonatomic, readonly) SCCellActions *cellActions;

/// Optional row-indexed image sources; defaults to nil. During cell
/// preparation, the model copies each corresponding image view's `image` into
/// the cell's existing `imageView`. It does not install the supplied image view
/// itself. Rows beyond the array are unchanged.
@property (nonatomic, nullable, strong) NSArray<UIImageView *> *cellsImageViews;

/// Options for the generated detail view controller (only applicable to
/// subclasses with a detail view controller).
@property (nonatomic, readonly) SCDetailViewControllerOptions *detailViewControllerOptions;

/// Lazily created options for new-item controller content/navigation settings
/// in supporting subclasses.
/// - Note: Array sections currently choose the add flow's presentation mode
///   from `detailViewControllerOptions.presentationMode`, not this object's
///   mode.
@property (nonatomic, readonly) SCDetailViewControllerOptions *newItemDetailViewControllerOptions;

#pragma mark - Configuring Special Cells

/// Optional controlling row for the section's expanded/collapsed state.
/// Assignment inserts it as the first static row; array-backed sections insert
/// it during special-cell assembly. Do not also add it manually. Defaults to
/// nil, but assigning nil to remove an installed cell is unsupported. Set the
/// cell's `ownerSectionExpanded` property to change state.
@property (nonatomic, nullable, strong) SCExpandCollapseCell *expandCollapseCell;

/// Assigns the controlling cell. The getter is nil before configuration, but
/// assigning nil is unsupported because the setter inserts the supplied cell.
/// - Parameter cell: The expand/collapse cell to install.
- (void)setExpandCollapseCell:(SCExpandCollapseCell *)cell;

#pragma mark - Managing Cells

/// The number of rows exposed by the section, including special rows. A
/// collapsed section with an expand/collapse cell reports one row. In array
/// sections, querying the count can trigger item fetching.
@property (nonatomic, readonly) NSUInteger cellCount;

/// Appends a static cell and assigns its owners and live-commit policy. Does
/// not insert a UIKit row automatically.
/// - Parameter cell: The nonnil cell to append.
- (void)addCell:(SCTableViewCell *)cell;

/// Inserts a static cell and assigns its owners and live-commit policy. Does
/// not insert a UIKit row automatically.
/// - Parameter cell: The nonnil cell to insert.
/// - Parameter index: An index from zero through the stored cell count,
///   inclusive. Use an expanded section when computing this from `cellCount`.
- (void)insertCell:(SCTableViewCell *)cell atIndex:(NSUInteger)index;

/// Retrieves a cell by its row index. The base implementation returns nil
/// outside `cellCount`. Array-backed subclasses can generate/reuse a cell and
/// require an in-range item index.
/// - Parameter index: The zero-based row index.
/// - Returns: The cell, or nil when unavailable; array-backed subclasses may
///   raise for out-of-range indexes instead of returning nil.
- (nullable SCTableViewCell *)cellAtIndex:(NSUInteger)index;

/// Removes a stored cell, asking it to resign first responder and clearing the
/// owner's active cell if it matches. Does not delete a UIKit row
/// automatically.
/// - Parameter index: An index within the stored cell array; invalid indexes
///   raise an array bounds exception.
- (void)removeCellAtIndex:(NSUInteger)index;

/// Removes the cell resolved by `indexForCell:` if that lookup succeeds. Does
/// not update UIKit's row structure automatically.
/// - Parameter cell: The cell to locate and remove.
- (void)removeCellIdenticalTo:(SCTableViewCell *)cell;

/// Clears the stored cells without updating UIKit or individually resigning
/// cells. For array-backed subclasses this clears the item cache, not the data
/// store.
- (void)removeAllCells;

/// Returns a cell's index using identity in the base implementation.
/// - Parameter cell: The cell to locate.
/// - Returns: The stored index, or `NSNotFound` when not found.
/// - Warning: Array-backed subclasses instead consult UIKit. A cell without a
///   UIKit index path can currently be misreported as row zero when this is the
///   model's first section. Use this lookup only for attached generated cells.
- (NSUInteger)indexForCell:(SCTableViewCell *)cell;

#pragma mark - Managing Cell Values

/// This property is TRUE if all the section cells' values are valid, otherwise
/// it's FALSE.
@property (nonatomic, readonly) BOOL valuesAreValid;

/// YES if any stored cell reports `needsCommit`. This does not separately track
/// changes to an object-selection index cache or arbitrary external changes to
/// bound objects.
@property (nonatomic, readonly) BOOL needsCommit;

/// Sets live committing for existing static cells and cells subsequently added
/// through `addCell:` or `insertCell:atIndex:`. Defaults to YES. With NO,
/// commit those cells using `commitCellChanges` or the owner's `commitChanges`.
/// `SCObjectSelectionSection` also uses this flag for selection changes;
/// string/index selection sections update their bindings directly.
@property (nonatomic, readwrite) BOOL commitCellChangesLive;

/// Calls `commitChanges` on each stored cell without first validating it.
/// `SCObjectSelectionSection` also writes its cached object selection to the
/// binding. Use this for deferred commits when `commitCellChangesLive` is NO.
- (void)commitCellChanges;

/// Overrides optimization and sets all cells as needing to be committed.
- (void)invalidateCellCommits;

/// Reloads stored cells from their bindings without reloading the table.
/// Array-backed subclasses instead clear cached items and reset batching so the
/// next access can refetch. Selection subclasses also read the bound selection.
- (void)reloadBoundValues;

#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Optional bound object used by binding-aware section subclasses. In
/// `SCObjectSection`, assignment regenerates cells in normal mode.
@property (nonatomic, nullable, strong) NSObject *boundObject;

/// The store used by subclasses to access the section's bound object. When nil,
/// `boundValue` uses STV's store-free property access.
@property (nonatomic, nullable, strong) SCDataStore *boundObjectStore;

/// The optional property name used by a binding-aware section subclass.
@property (nonatomic, nullable, readonly) NSString *boundPropertyName;

/// Reads or writes the section's bound property through its store, or through
/// STV property access when no store is supplied. Without both a bound object
/// and property name, reads return nil and writes do nothing.
@property (nonatomic, nullable, strong) NSObject *boundValue;

/// Compatibility sizing query; always returns UITableViewAutomaticDimension.
/// Row measurement belongs to UIKit. Numeric sizing callbacks are applied as
/// content constraints during cell preparation, not from this query.
/// - Parameter indexPath: The row index path.
- (CGFloat)heightForCellAtIndexPath:(NSIndexPath *)indexPath;

/// Method called internally whenever the owner table view's editing mode is
/// about to change.
- (void)editingModeWillChange;

/// Method called internally whenever the owner table view's editing mode has
/// changed.
- (void)editingModeDidChange;

/// Subclass hook for applying supported property attributes to section
/// settings. The base implementation does nothing.
/// - Parameter attributes: The attributes to apply, or nil.
- (void)setAttributesTo:(nullable SCPropertyAttributes *)attributes;

/// Internal hook that replaces the stored detail options.
/// - Parameter options: The options to retain, or nil to recreate default
///   options on the next getter access.
- (void)setDetailViewControllerOptions:(nullable SCDetailViewControllerOptions *)options;

/// Internal hook that replaces the stored detail options.
/// - Parameter options: The options to retain, or nil to recreate default
///   options on the next getter access.
- (void)setNewItemDetailViewControllerOptions:(nullable SCDetailViewControllerOptions *)options;

/// Internal row-update hook used after the controlling cell changes state. Set
/// `expandCollapseCell.ownerSectionExpanded` instead of calling this directly.
/// - Parameter expanded: YES to insert the content rows; NO to remove them.
- (void)setExpanded:(BOOL)expanded;

/// Internal hook that asks the indexed cell to generate its detail controller.
/// Array-backed subclasses generate an item detail controller instead.
/// - Parameter indexPath: A valid current row path.
/// - Returns: The detail controller, or nil if no controller is available.
- (nullable UIViewController *)generatedDetailViewControllerForCellAtIndexPath:(NSIndexPath *)indexPath;

/// Internal hook that asks static cells to restore their initial bound values.
/// Array-backed sections are skipped; this is not a general data-store
/// rollback.
- (void)rollbackToInitialCellValues;

@end


#pragma mark - SCObjectSection
/// Generates cells for a bound object's property group using a data definition.
/// When no definition is supplied, a class definition is inferred from the
/// object's runtime class. Auto-detection handles supported strings, numbers,
/// dates, scalars, and object/array properties; explicit definitions configure
/// types, attributes, actions, and editing-mode visibility.
///
/// Each generated cell's tag is the property's index in `propertyGroup`, not
/// the runtime object's property order. Unsupported or excluded properties can
/// leave gaps in tags.
/// - SeeAlso: `SCArrayOfObjectsSection`, `SCObjectCell`
@interface SCObjectSection : SCTableViewSection


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCObjectSection` given a header title
/// and a bound object.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object;

/// Allocates and returns an initialized `SCObjectSection` given a header title,
/// a bound object and its data definition.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter definition: The object definition, or nil to infer a class
///   definition from the object.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition;

/// Allocates and returns an initialized `SCObjectSection` given a header title,
/// a bound object and its data definition along with a property group.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter definition: The object definition, or nil to infer a class
///   definition from the object.
/// - Parameter group: The property group to render, or nil to use the
///   definition's default group.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition propertyGroup:(nullable SCPropertyGroup *)group;

/// Allocates and returns an initialized `SCObjectSection` given a header title,
/// a bound object and its data store along with a property group.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter store: The store supplying the object definition. Nil supplies
///   no definition and generates no property cells.
/// - Parameter group: The property group to render, or nil to use the
///   definition's default group.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectStore:(nullable SCDataStore *)store propertyGroup:(nullable SCPropertyGroup *)group;


/// Returns an initialized `SCObjectSection` given a header title and a bound
/// object.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object;

/// Returns an initialized `SCObjectSection` given a header title, a bound
/// object and its data definition.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter definition: The object definition, or nil to infer a class
///   definition from the object.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition;

/// Returns an initialized `SCObjectSection` given a header title, a bound
/// object and its data definition along with a property group.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter definition: The object definition, or nil to infer a class
///   definition from the object.
/// - Parameter group: The property group to render, or nil to use the
///   definition's default group.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition propertyGroup:(nullable SCPropertyGroup *)group;

/// Returns an initialized `SCObjectSection` given a header title, a bound
/// object and its data store along with a property group.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter object: The object to bind, or nil for unbound cells when a
///   usable definition/store is supplied.
/// - Parameter store: The store supplying the object definition. Nil supplies
///   no definition and generates no property cells.
/// - Parameter group: The property group to render, or nil to use the
///   definition's default group.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object boundObjectStore:(nullable SCDataStore *)store propertyGroup:(nullable SCPropertyGroup *)group;

#pragma mark - Cell Management

/// Returns the first cell whose bound property name equals the supplied name.
/// - Parameter propertyName: The property name to match; nil matches no cell.
/// - Returns: The matching generated or manually added cell, or nil.
- (nullable SCTableViewCell *)cellForPropertyName:(nullable NSString *)propertyName;

#pragma mark - Other

/// The ordered property group used for cell generation. Initializers use the
/// definition's default group when no group is supplied. Assigning this
/// property alone does not regenerate existing cells.
@property (nonatomic, nullable, strong) SCPropertyGroup *propertyGroup;

#pragma mark - Internal Methods (should only be used by the framework or when subclassing)

// internal
/// Internal hook that changes the object and store together.
/// - Parameter boundObject: The new bound object, or nil.
/// - Parameter boundObjectStore: The corresponding store, or nil.
/// - Parameter autoGenerateCells: YES to regenerate cells in normal mode; NO to
///   retain cells and bind only those that currently have no bound object.
- (void)setBoundObject:(nullable NSObject *)boundObject withStore:(nullable SCDataStore *)boundObjectStore autoGenerateCells:(BOOL)autoGenerateCells;

/// Configures the cell for the given property definition.
/// - Parameter cell: The cell to configure.
/// - Parameter propertyDefinition: The property definition.
/// - Parameter editing: Whether the table is in editing mode.
- (void)configureCell:(SCTableViewCell *)cell forPropertyDefinition:(SCPropertyDefinition *)propertyDefinition inEditingMode:(BOOL)editing;

/// Removes existing cells and regenerates them from `propertyGroup` and the
/// bound object's store definition for the requested editing state.
/// - Parameter editing: YES to use editing-mode visibility and settings.
- (void)generateCellsForEditingState:(BOOL)editing;

/// Internal hook that asks each custom cell to reload control values when
/// needed.
- (void)reloadCellsIfNeeded;

@end


#pragma mark - SCArrayOfItemsSection
/// Base section for store-backed item lists and generated item detail flows.
/// Concrete subclasses build detail-model contents. Item creation uses the
/// store's `createNewObject`, followed by `sectionActions.didCreateItem` (or
/// the owner's fallback action) for customization. Use `addButtonItem` or
/// `addNewItemCell` to expose adding in the UI.
///
/// The section handles fetching, row reuse, and item insert/update/delete/move
/// operations. Implement `buildDetailTableModel:forItem:` in custom subclasses
/// to populate the supplied detail model; it does not return a model.
/// - SeeAlso: `SCArrayOfStringsSection`, `SCArrayOfObjectsSection`
@interface SCArrayOfItemsSection : SCTableViewSection <SCViewControllerDelegate, SCTableViewControllerDelegate> {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    NSObject *_Nullable tempItem;
    UITableViewCellAccessoryType _itemsAccessoryType;
    BOOL _allowAddingItems;
    BOOL _allowDeletingItems;
    BOOL _allowMovingItems;
    BOOL _allowEditDetailView;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCArrayOfItemsSection` given a data
/// store.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter store: The store supplying items, or nil until configured.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle dataStore:(nullable SCDataStore *)store;


/// Returns an initialized `SCArrayOfItemsSection` given a data store.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter store: The store supplying items, or nil until configured.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle dataStore:(nullable SCDataStore *)store;


#pragma mark - Configuration

/// Optional store used to fetch and persist the section's items. Assignment
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

/// The current fetched item list, including any configured special cells.
/// Reading it can trigger fetching or special-cell insertion. It is not a
/// cell-only array or a domain-object-only array; do not mutate it directly.
@property (nonatomic, nullable, readonly) NSArray<id> *items;

/// Whether stale items are fetched automatically when accessed. Defaults to
/// YES. Set to NO only when the section's items are supplied by
/// framework/subclass coordination rather than automatic store fetching.
@property (nonatomic, readwrite) BOOL autoFetchItems;

/// YES while an asynchronous item fetch is outstanding. Defaults to NO.
/// Synchronous fetches do not set this flag.
@property (nonatomic, readonly) BOOL isFetchingItems;

/// The accessory type assigned to ordinary generated detail rows. Defaults to
/// `UITableViewCellAccessoryDisclosureIndicator` in the base class; selection
/// and string-section specializations can override the appearance.
@property (nonatomic, readwrite) UITableViewCellAccessoryType itemsAccessoryType;

/// Suppresses the section's header/footer when it exposes no rows. Defaults to
/// NO. It does not remove the section from the model. Placeholder, add, fetch,
/// and expand/collapse rows count as rows and prevent this suppression.
@property (nonatomic, readwrite) BOOL hideWhenEmpty;

/// Allows/disables adding new cells/items to the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowAddingItems;

/// Allows/disables deleting cells/items from the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowDeletingItems;

/// Allows/disables moving cells/items from one row to another. Default: TRUE.
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


/// Inserts a newly created item without presenting a detail editor. Defaults to
/// NO. Use `sectionActions.didCreateItem` to populate the item before
/// insertion; `willInsertItem` receives a nil item model on this path.
@property (nonatomic, readwrite) BOOL skipNewItemDetailView;

/// Whether a newly inserted row remains selected after insertion. Defaults to
/// NO. The insertion path scrolls to and briefly selects the row even when this
/// is NO. An explicitly supplied active detail controller can subsequently
/// select it.
@property (nonatomic, readwrite) BOOL autoSelectNewItemCell;

/// Enables the normal detail-processing path to commit cell values and request
/// item insertion/update. Defaults to YES. Set to NO when managing that path
/// manually. This does not disable individual live commits or every store
/// write, such as the separate `skipNewItemDetailView` insertion path.
@property (nonatomic, readwrite) BOOL autoCommitDetailModelChanges;


/// Optional button that starts item creation when `allowAddingItems` is YES.
/// Assignment replaces its target and action. A new item is created by the
/// store and normally edited in a generated detail controller before insertion.
/// Defaults to nil.
@property (nonatomic, nullable, strong) UIBarButtonItem *addButtonItem;


#pragma mark - Configuring Special Cells

/// Special loading/batch-fetch row; initialized with the text "Load more...".
/// It is shown while initially fetching asynchronously and, for batched
/// fetches, when the batch/auto-hide rules indicate more items may be
/// available. Nil removes this loading-row affordance.
@property (nonatomic, nullable, strong) SCFetchItemsCell *fetchItemsCell;

/// Optional nonselectable placeholder for an empty item list. Defaults to nil.
/// Special-cell assembly inserts it only if the list is empty after adding an
/// expand/collapse cell, so an expand/collapse cell suppresses the placeholder.
@property (nonatomic, nullable, strong) SCTableViewCell *placeholderCell;

/// Optional special row appended when adding is allowed and the current mode's
/// visibility flag is enabled. Tapping it starts item creation. Defaults to
/// nil. Assign this property rather than manually appending the row.
/// - SeeAlso: `addButtonItem`
@property (nonatomic, nullable, strong) SCTableViewCell *addNewItemCell;

/// When TRUE, addNewItemCell will be displayed in Normal Mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInNormalMode;

/// When TRUE, addNewItemCell will be displayed in Editing Mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInEditingMode;


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

/// Internal path of the existing item being edited, or nil for a new-item flow.
/// It is not always identical to UIKit's currently highlighted row.
@property (nonatomic, nullable, strong) NSIndexPath *selectedCellIndexPath;

/// Internal default reuse identifier for generated rows. Initialization creates
/// a unique UUID string; row-specific reuse-identifier actions can override it.
@property (nonatomic, copy) NSString *cellIdentifier;

/* For internal use only. */
/// Internal storyboard identifier for a supplied new-item controller. Nil or an
/// empty string uses the generated-controller path.
@property (nonatomic, nullable, copy) NSString *ibNewItemViewControllerIdentifier;

/// Internal access to the section's mutable item cache without triggering a
/// fetch. This is framework/subclass storage access, not a separate public data
/// source.
- (nullable NSMutableArray<id> *)mutableItems;

/// Internal hook that replaces the section's mutable item cache directly. Does
/// not set synchronization flags or update the table.
/// - Parameter mutableItems: The replacement cache, or nil.
- (void)setMutableItems:(nullable NSMutableArray<id> *)mutableItems;

/// Internal hook that fetches items using the current store and batch options.
/// - Parameter sender: The fetch initiator, or nil. Passing the section itself
///   suppresses explicit table updates for a synchronous fetch.
- (void)fetchItems:(nullable id)sender;

/// Internal fetch-completion hook that invokes fetch actions, replaces or
/// appends cached items, restores special cells, and updates UIKit when
/// required.
/// - Parameter fetchedItems: The returned items; nil is treated as an empty
///   batch.
/// - Parameter sender: The original fetch initiator, or nil.
- (void)didFetchItems:(nullable NSArray<id> *)fetchedItems sender:(nullable id)sender;

/// Internal hook that inserts configured expand/collapse, placeholder, fetch,
/// and add rows into the item cache, then invokes `didAddSpecialCells`. It does
/// not first remove existing special rows.
- (void)addSpecialCellsToItems;

/// Internal hook that removes configured special-cell instances from the item
/// cache.
- (void)removeSpecialCellsFromItems;

/// Subclass factory hook for a generated cell when neither reuse nor a custom
/// cell action provides one. The base creates an `SCTableViewCell` and assigns
/// ownership.
/// - Parameter index: The item index.
/// - Parameter cellId: The reuse identifier, or nil.
- (SCTableViewCell *)createCellAtIndex:(NSUInteger)index usingCellId:(nullable NSString *)cellId;

/// Internal/subclass hook that assigns ownership, item bindings, text,
/// accessories, and editing/moving flags to a newly created or reused row.
/// - Parameter cell: The cell to configure.
/// - Parameter index: A valid item index.
- (void)setPropertiesForCell:(SCTableViewCell *)cell withIndex:(NSUInteger)index;

/// Returns the item's definition-based title, or a dictionary-value fallback
/// when no definition is available. Subclasses can supply custom text.
/// - Parameter index: A valid item index.
/// - Returns: The title text, or nil when no text is available.
- (nullable NSString *)textForCellAtIndex:(NSUInteger)index;

/// Returns the definition's description text when it has a title property name.
/// Subclasses can supply custom detail text.
/// - Parameter index: A valid item index.
/// - Returns: The description, or nil when unavailable.
- (nullable NSString *)detailTextForCellAtIndex:(NSUInteger)index;

/// Method is called internally when the generated cells are selected
/// - Parameter indexPath: The row index path.
- (void)didSelectCellAtIndexPath:(NSIndexPath *)indexPath;

/// Internal hook that processes an active detail model's pending changes and
/// reloads the row before deselection.
/// - Parameter indexPath: The row being deselected.
- (void)willDeselectCellAtIndexPath:(NSIndexPath *)indexPath;

/// Dispatches insertion for `UITableViewCellEditingStyleInsert`; other styles
/// dispatch deletion.
/// - Parameter editingStyle: The requested editing operation.
/// - Parameter indexPath: The affected row path.
- (void)commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forCellAtIndexPath:(NSIndexPath *)indexPath;

/// Constrains interactive moves to this section and before its trailing add
/// row.
/// - Parameter sourceIndexPath: The current item row.
/// - Parameter proposedIndexPath: UIKit's proposed destination.
/// - Returns: The source for a cross-section proposal, the preceding row when
///   targeting the trailing add row, or the proposed path otherwise.
- (NSIndexPath *)targetIndexPathForMoveFromCellAtIndexPath:(NSIndexPath *)sourceIndexPath toProposedIndexPath:(NSIndexPath *)proposedIndexPath;

/// Internal hook that reorders cached items and asks the store to persist
/// ordering for same-section moves. Normal interactive moves are constrained to
/// one section.
/// - Parameter fromIndexPath: The current item path.
/// - Parameter toIndexPath: The destination path.
/// - Warning: Calling this across sections removes the item from the source
///   store and inserts it into the destination cache, not the destination
///   store.
- (void)moveCellAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath;

/// Add-button action that dispatches item creation only when `allowAddingItems`
/// is YES. Subclasses may extend this behavior.
- (void)didTapAddButtonItem;

/// Subclass hook that asks `dataStore` to create a new object.
/// - Returns: The new object, or nil if creation is unavailable. The framework
///   later inserts or discards the uninserted object.
- (nullable NSObject *)createNewItem;

/// Subclass hook for populating a generated detail model; the base does
/// nothing.
/// - Parameter detailTableModel: The model to populate. It can be nil for a
///   plain UIKit custom controller; do not assume an existing custom model is
///   empty.
/// - Parameter item: The existing or already-created new item to edit. Nil is
///   not the signal for creating a new item.
- (void)buildDetailTableModel:(nullable SCTableViewModel *)detailTableModel forItem:(NSObject *)item;

/// Configures STV controller delegates, navigation buttons, and editing cancel
/// options. Subclasses may extend this behavior rather than replace it.
/// - Parameter detailViewController: The generated or supplied controller.
/// - Parameter item: The item being edited.
/// - Parameter newItem: YES for an already-created new item; NO for an existing
///   one.
- (void)configureDetailViewController:(UIViewController *)detailViewController item:(NSObject *)item newItem:(BOOL)newItem;

/// Subclass hook to configure a detail model after generation or storyboard
/// loading. The base implementation does nothing.
/// - Parameter detailTableModel: The model to configure, or nil when a custom
///   controller has no STV model.
/// - Parameter item: The item whose values the detail model represents.
- (void)configureDetailTableModel:(nullable SCTableViewModel *)detailTableModel forItem:(NSObject *)item;

/// Returns the applicable new-item or existing-item navigation-bar option.
/// Subclasses can override this for item-specific editing requirements.
/// - Parameter item: The object represented by the detail controller.
/// - Parameter newItem: YES for new-item options; NO for existing-item options.
- (SCNavigationBarType)getDetailViewNavigationBarTypeForItem:(NSObject *)item newItem:(BOOL)newItem;

/// Internal hook that inserts a new item through the owner array model or
/// store. For basic value types, a detail control's value can replace the
/// original item.
/// - Parameter newItem: The created object to insert, not a Boolean flag.
/// - Parameter detailModel: The detail model supplying edited values, or nil.
- (void)commitNewItem:(NSObject *)newItem detailModel:(nullable SCTableViewModel *)detailModel;

/// Internal hook that displays an already inserted item, handles the
/// placeholder and collapsed state, scrolls to the row, and invokes the
/// insertion action.
/// - Parameter newItem: The object already present in the current item list.
- (void)addCellForNewItem:(NSObject *)newItem;

/// Internal hook that re-sorts and refreshes an edited item's row, updating its
/// selected path if ordering changes. Basic-value items trigger a refetch
/// because the value may have been replaced.
/// - Parameter item: The item that was edited.
- (void)itemModified:(NSObject *)item;

/// Subclass hook called after an item is removed. The base implementation does
/// nothing; selection subclasses adjust cached indexes and bindings.
/// - Parameter index: The former item index.
- (void)itemRemovedAtIndex:(NSInteger)index;

/// Internal hook that checks `autoCommitDetailModelChanges` and insert/update
/// actions, commits detail values, and requests the corresponding store
/// operation.
/// - Parameter detailModel: The detail model to commit, or nil for a custom
///   flow.
- (void)commitAndProcessChangesForDetailModel:(nullable SCTableViewModel *)detailModel;

@end


#pragma mark - SCArrayOfObjectsSection
/// Generates object-list rows and property-based detail editors from a data
/// store. Objects can have different types if the store resolves a definition
/// for each one. Register additional definitions with `dataStore`'s
/// `addDataDefinition:` method.
/// - SeeAlso: `SCObjectSection`, `SCObjectCell`
@interface SCArrayOfObjectsSection : SCArrayOfItemsSection

#pragma mark - Creation and Initialization

/// Creates a `SCArrayOfObjectsSection` backed by an `SCArrayStore`.
/// - Parameter sectionHeaderTitle: The header title, or nil for no title.
/// - Parameter sectionItems: The mutable backing array, retained rather than
///   copied. Nil leaves the store without a backing array; provide a mutable
///   array before expecting persistent additions.
/// - Parameter definition: The default object definition. Register additional
///   definitions with the resulting data store when items have different types.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle items:(nullable NSMutableArray<id> *)sectionItems itemsDefinition:(SCDataDefinition *)definition;

/// Creates a `SCArrayOfObjectsSection` backed by an `SCArrayStore`.
/// - Parameter sectionHeaderTitle: The header title, or nil for no title.
/// - Parameter sectionItems: The mutable backing array, retained rather than
///   copied. Nil leaves the store without a backing array; provide a mutable
///   array before expecting persistent additions.
/// - Parameter definition: The default object definition. Register additional
///   definitions with the resulting data store when items have different types.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle items:(nullable NSMutableArray<id> *)sectionItems itemsDefinition:(SCDataDefinition *)definition;

/// Allocates and returns an initialized `SCArrayOfObjectsSection` given a
/// header title and an entity definition.
///
/// - Note: Fetches objects of the specified entity in its managed object
///   context, not every entity in the context. Fetch options can further
///   restrict results.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter definition: The entity definition of the objects to fetch.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition;

/// Allocates and returns an initialized `SCArrayOfObjectsSection` given a
/// header title, an entity definition, and an NSPredicate.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter definition: The entity definition of the objects to fetch.
/// - Parameter predicate: The predicate used to filter fetched objects, or nil
///   for no additional predicate.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition filterPredicate:(nullable NSPredicate *)predicate;

/// Returns an initialized `SCArrayOfObjectsSection` given a header title and an
/// entity definition.
///
/// - Note: Fetches objects of the specified entity in its managed object
///   context, not every entity in the context. Fetch options can further
///   restrict results.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter definition: The entity definition of the objects to fetch.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition;

/// Returns an initialized `SCArrayOfObjectsSection` given a header title, an
/// entity definition, and an NSPredicate.
///
/// - Parameter sectionHeaderTitle: A header title for the section, or nil.
/// - Parameter definition: The entity definition of the objects to fetch.
/// - Parameter predicate: The predicate used to filter fetched objects, or nil
///   for no additional predicate.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition filterPredicate:(nullable NSPredicate *)predicate;

@end


#pragma mark - SCArrayOfStringsSection
/// An object-list section specialized for `NSString` items. Its string-array
/// initializer creates an `SCArrayStore` with `SCStringDefinition`, disables
/// existing-item detail editing, and selects no item accessory. Set
/// `allowEditDetailView` explicitly if existing strings should be editable. The
/// inherited new-item flow remains available.

@interface SCArrayOfStringsSection : SCArrayOfObjectsSection

/// Creates a `SCArrayOfStringsSection` backed by an `SCArrayStore`.
/// - Parameter sectionHeaderTitle: The header title, or nil for no title.
/// - Parameter sectionItems: The mutable backing array, retained rather than
///   copied. Nil leaves the store without a backing array; provide a mutable
///   array before expecting persistent additions. Entries must be NSString
///   values.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle items:(nullable NSMutableArray<id> *)sectionItems;

/// Creates a `SCArrayOfStringsSection` backed by an `SCArrayStore`.
/// - Parameter sectionHeaderTitle: The header title, or nil for no title.
/// - Parameter sectionItems: The mutable backing array, retained rather than
///   copied. Nil leaves the store without a backing array; provide a mutable
///   array before expecting persistent additions. Entries must be NSString
///   values.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle items:(nullable NSMutableArray<id> *)sectionItems;

@end


#pragma mark - SCSelectionSection
/// Presents string items as a checkmarked single- or multiple-selection
/// section. Bind a writable NSNumber property to an item index, an NSMutableSet
/// of NSNumber indexes to selections, or an NSString to selected item text
/// separated by semicolons. Alternatively, configure cached indexes directly.
/// Adding, deleting, moving, and detail editing default to NO.
///
/// Indexes refer to the section's `items`. String bindings resolve selected
/// values by equality, so duplicate item strings map to the first equal item.
/// - SeeAlso: `SCSelectionCell`
@interface SCSelectionSection : SCArrayOfStringsSection


#pragma mark - Creation and Initialization

/// Creates a `SCSelectionSection` with index selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSNumber property storing a zero-based
///   item index, or -1 for no selection. A missing bound value is initialized
///   to -1. Nil or an unrecognized property name leaves the instance without a
///   property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedIndexPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;

/// Creates a `SCSelectionSection` with index-set selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A property holding an NSMutableSet of NSNumber
///   item indexes. A missing value is initialized to an empty mutable set when
///   binding is available. Nil or an unrecognized property name leaves the
///   instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Parameter multipleSelection: Whether more than one item can be selected.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedIndexesPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems allowMultipleSelection:(BOOL)multipleSelection;

/// Creates a `SCSelectionSection` with string selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSString property containing selected
///   item text, separated by semicolons for multiple selections. Nil or an
///   unrecognized property name leaves the instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Note: Semicolons in item text are not escaped. Multiple selections are
///   serialized in set enumeration order, not guaranteed item order.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectionStringPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;


/// Creates a `SCSelectionSection` with index selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSNumber property storing a zero-based
///   item index, or -1 for no selection. A missing bound value is initialized
///   to -1. Nil or an unrecognized property name leaves the instance without a
///   property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedIndexPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;

/// Creates a `SCSelectionSection` with index-set selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A property holding an NSMutableSet of NSNumber
///   item indexes. A missing value is initialized to an empty mutable set when
///   binding is available. Nil or an unrecognized property name leaves the
///   instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Parameter multipleSelection: Whether more than one item can be selected.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedIndexesPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems allowMultipleSelection:(BOOL)multipleSelection;

/// Creates a `SCSelectionSection` with string selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: A writable NSString property containing selected
///   item text, separated by semicolons for multiple selections. Nil or an
///   unrecognized property name leaves the instance without a property binding.
/// - Parameter sectionItems: Selectable NSString values, copied into a mutable
///   backing array. Nil supplies no initial items.
/// - Note: Semicolons in item text are not escaped. Multiple selections are
///   serialized in set enumeration order, not guaranteed item order.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectionStringPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems;

#pragma mark - Configuration

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

/// Requests Done-style dismissal from an `SCTableViewController` owner after
/// selection handling. Defaults to NO. A newly checked item dismisses for
/// single selection, an unlimited selection count, a reached maximum, or all
/// items selected. Tapping an already-selected final item that cannot be
/// unchecked, or attempting to exceed the maximum, also requests dismissal.
/// Unchecking an item does not.
@property (nonatomic, readwrite) BOOL autoDismissViewController;

/// Text color assigned to selected rows. Defaults to `UIColor.linkColor`.
@property (nonatomic, nullable, strong) UIColor *selectedCellTextColor;

/// Text color assigned to unselected rows. Defaults to `UIColor.labelColor`.
@property (nonatomic, nullable, strong) UIColor *deselectedCellTextColor;

/// Optional view assigned directly as the selected row's accessory instead of a
/// checkmark. Defaults to nil. The same view instance is assigned, not copied;
/// a UIView cannot remain installed in several selected rows simultaneously.
/// Use per-cell configuration for distinct accessories in multiple-selection
/// lists.
@property (nonatomic, nullable, strong) UIView *selectedCellAccessoryView;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Configures a generated cell immediately before display.
/// - Parameter cell: The cell to configure.
/// - Parameter index: The item index.
- (void)configureCellForDisplay:(SCTableViewCell *)cell atIndex:(NSUInteger)index;

@end


#pragma mark - SCObjectSelectionSection
/// Presents store-backed objects as a checkmarked selection section. Single
/// selection binds an object-valued property; multiple selection uses an
/// existing mutable set of selected objects. Initializers infer multiple
/// selection from an `NSMutableSet` bound value. Items are matched by identity
/// when bound values are reloaded. Adding, deleting, moving, and detail editing
/// default to NO.
@interface SCObjectSelectionSection : SCArrayOfObjectsSection


#pragma mark - Creation and Initialization

/// Creates a `SCObjectSelectionSection` with object selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter store: The store supplying selectable objects, or nil until
///   configured.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItemsStore:(nullable SCDataStore *)store;

/// Creates a `SCObjectSelectionSection` with object selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter sectionItems: Selectable objects, copied into a mutable backing
///   array. Nil supplies no initial items.
/// - Parameter definition: The default definition for selectable objects.
///   Register other object definitions with the resulting data store as needed.
+ (instancetype)sectionWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems itemsDefintion:(SCDataDefinition *)definition;


/// Creates a `SCObjectSelectionSection` with object selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter store: The store supplying selectable objects, or nil until
///   configured.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItemsStore:(nullable SCDataStore *)store;

/// Creates a `SCObjectSelectionSection` with object selection binding.
/// - Parameter sectionHeaderTitle: The section header title, or nil for no
///   title.
/// - Parameter object: The binding object, or nil for an unbound selection.
/// - Parameter propertyName: An object-valued property for single selection, or
///   a property holding an existing mutable set of objects for multiple
///   selection. The initializer infers the mode from the current value. Nil or
///   an unrecognized property name leaves the instance without a property
///   binding.
/// - Parameter sectionItems: Selectable objects, copied into a mutable backing
///   array. Nil supplies no initial items.
/// - Parameter definition: The default definition for selectable objects.
///   Register other object definitions with the resulting data store as needed.
- (instancetype)initWithHeaderTitle:(nullable NSString *)sectionHeaderTitle boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)sectionItems itemsDefintion:(SCDataDefinition *)definition;


#pragma mark - Configuration

/// One cached selected item index, or `@(-1)` when no indexes are selected.
/// With multiple selection, the getter returns an arbitrary member. Assignment
/// clears the cache and adds a nonnegative index, but does not commit the bound
/// object selection or refresh visible checkmarks.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// The mutable cache of selected `NSNumber` indexes into `items`. Direct
/// mutation does not itself commit or reload the table. Call
/// `commitCellChanges` to write a manually changed cache to the binding, and
/// reload UIKit to refresh checkmarks. `reloadBoundValues` instead discards
/// cached indexes and reconstructs them from bound objects by identity.
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

/// Requests Done-style dismissal from an `SCTableViewController` owner after
/// selection handling. Defaults to NO. A newly checked item dismisses for
/// single selection, an unlimited selection count, a reached maximum, or all
/// items selected. Tapping an already-selected final item that cannot be
/// unchecked, or attempting to exceed the maximum, also requests dismissal.
/// Unchecking an item does not.
@property (nonatomic, readwrite) BOOL autoDismissViewController;

/// Text color assigned to selected rows. Defaults to `UIColor.linkColor`.
@property (nonatomic, nullable, strong) UIColor *selectedCellTextColor;

/// Text color assigned to unselected rows. Defaults to `UIColor.labelColor`.
@property (nonatomic, nullable, strong) UIColor *deselectedCellTextColor;

/// Optional view assigned directly as the selected row's accessory instead of a
/// checkmark. Defaults to nil. The same view instance is assigned, not copied;
/// a UIView cannot remain installed in several selected rows simultaneously.
/// Use per-cell configuration for distinct accessories in multiple-selection
/// lists.
@property (nonatomic, nullable, strong) UIView *selectedCellAccessoryView;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Configures a generated cell immediately before display.
/// - Parameter cell: The cell to configure.
/// - Parameter index: The item index.
- (void)configureCellForDisplay:(SCTableViewCell *)cell atIndex:(NSUInteger)index;


#pragma mark - Other

/// Reserved definition for an intermediate relationship entity; defaults to
/// nil. The current implementation stores this value (including from
/// attributes), but its selection reload/commit paths still compare and bind
/// the items directly. Setting it does not create, traverse, or synchronize
/// intermediary entities.
@property (nonatomic, nullable, strong) SCDataDefinition *intermediateEntityDefinition;


@end

NS_ASSUME_NONNULL_END
