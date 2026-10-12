/*
 *  SCTableViewCell.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019-2026 dgApps. All rights reserved.
 *
 */

#import "SCCellActions.h"
#import "SCDataDefinition.h"
#import "SCDataStore.h"
#import "SCViewController.h"
#import "SCTableViewController.h"
#import "SCDetailViewControllerOptions.h"
#import "SCBadgeView.h"

NS_ASSUME_NONNULL_BEGIN

@class SCTableViewModel;
@class SCTableViewSection;
@class SCEntityDefinition;

/*
STV 7 separates row-height policy from the numeric height value so UIKit's
automatic sentinel is no longer consumed as a calculated legacy height.
*/
/// The row-sizing policies available to an STV table-view cell.
typedef NS_ENUM(NSInteger, SCTableViewCellSizingMode) {
    /// Inherits the row-sizing policy from the owning model.
    SCTableViewCellSizingModeInherit,
    /// Compatibility spelling for automatic sizing; no manual layout path.
    SCTableViewCellSizingModeLegacy,
    /// Uses UIKit automatic row sizing.
    SCTableViewCellSizingModeAutomatic,
    /// Constrains contentView to the model's configured height; UIKit sizes rows.
    SCTableViewCellSizingModeFixed
};


#pragma mark - SCTableViewCell
/// A table-view cell with binding, validation, action, and detail-view support.
///
/// Subclasses display or edit a value by binding to `boundObject` and
/// `boundPropertyName`. Control cells load that value and commit valid edits
/// live by default; setting `commitChangesLive` to NO defers those commits.
/// The base cell provides binding infrastructure but no value editor.
///
/// Programmatically generated content uses Auto Layout and UIKit automatic
/// row sizing. Nib-authored content retains its own constraints; provide a
/// complete vertical constraint chain for self-sizing custom content.
@interface SCTableViewCell : UITableViewCell <SCViewControllerDelegate, SCTableViewControllerDelegate>


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCTableViewCell.
+ (instancetype)cell;

/// Allocates and returns an initialized SCTableViewCell given the cell's style.
/// - Parameter style: The table-view cell style.
+ (instancetype)cellWithStyle:(UITableViewCellStyle)style;

/// Allocates and returns an initialized SCTableViewCell given cell text.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
+ (instancetype)cellWithText:(nullable NSString *)cellText;

/// Allocates and returns an initialized SCTableViewCell given cell text and its alignment.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter textAlignment: The alignment setting for the cell's text.
+ (instancetype)cellWithText:(nullable NSString *)cellText textAlignment:(NSTextAlignment)textAlignment;

/// Allocates and returns an initialized SCTableViewCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see class overview).
/// - Parameter propertyName: The cell's bound property name (see class overview).
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object boundPropertyName:(nullable NSString *)propertyName;


/// Returns an initialized SCTableViewCell given cell text.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
- (instancetype)initWithText:(nullable NSString *)cellText;

/// Returns an initialized SCTableViewCell given cell text and its alignment.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter textAlignment: The alignment setting for the cell's text.
- (instancetype)initWithText:(nullable NSString *)cellText textAlignment:(NSTextAlignment)textAlignment;

/// Returns an initialized SCTableViewCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see class overview).
/// - Parameter propertyName: The cell's bound property name (see class overview).
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object boundPropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The owner table view model of the cell.
///
/// - Warning: This property gets set automatically by the framework, you should never set this property manually
@property (nonatomic, nullable, weak) SCTableViewModel *ownerTableViewModel;

/// The owner section of the cell.
///
/// - Warning: This property gets set automatically by the framework, you should never set this property manually
@property (nonatomic, nullable, weak) SCTableViewSection *ownerSection;

/// The set of cell action blocks.
@property (nonatomic, readonly) SCCellActions *cellActions;

/// The cell's bound object (see class overview).
@property (nonatomic, nullable, strong) NSObject *boundObject;

/// The data store of the cell's bound object.
@property (nonatomic, nullable, strong) SCDataStore *boundObjectStore;

/// The cell's bound property name (see class overview).
@property (nonatomic, nullable, copy) IBInspectable NSString *boundPropertyName;

/// Always returns `UITableViewAutomaticDimension`. Assigning a positive value
/// requests a priority-999 `contentView` height constraint, including for 44.
/// Assigning automatic dimension removes this cell-level request; a positive
/// model/section override or fixed sizing policy can still supply a height.
@property (nonatomic, readwrite) CGFloat height;

/// Sets the internal model/section height override. A nonpositive value
/// removes that override and falls back to the cell height or sizing policy.
/// - Parameter height: The resolved content height, or automatic dimension.
- (void)sc_setResolvedHeightConstraint:(CGFloat)height;

/// The row-height policy. Cells default to SCTableViewCellSizingModeInherit.
@property (nonatomic, readwrite) SCTableViewCellSizingMode sizingMode;

/// The row-height estimate supplied to UIKit. Default: 44.
/// A nib-loading factory may replace it with the nib canvas height.
@property (nonatomic, readwrite) CGFloat estimatedHeight;

/// The editable state of the cell. Default: FALSE.
@property (nonatomic, readwrite) BOOL editable;

/// The movable state of the cell. Default: FALSE.
@property (nonatomic, readwrite) BOOL movable;

/// Determines if the cell can be selected. Default: TRUE.
@property (nonatomic, readwrite) BOOL selectable;

/// Whether the model allows the cell to participate in selection and value
/// interaction. Default: YES. Control subclasses also enable/disable their
/// controls, and disabled cells report a valid value.
@property (nonatomic, readwrite) BOOL enabled;

/// The text color used by supporting control-cell subclasses when disabled.
/// Default: `UIColor.labelColor`.
@property (nonatomic, nullable, strong) UIColor *disabledTextColor;

/// A heuristic style inferred from the current labels, fonts, and colors.
/// May differ from the initialization style after customization; returns -1
/// when neither a title nor a detail label is available.
@property (nonatomic, readonly) UITableViewCellStyle cellStyle;

/// The cell's badge view. When assigned a text value, the badgeView displays a badge similar to the badge displayed by the iPhone's mail application for its mail folders.
@property (nonatomic, readonly) SCBadgeView *badgeView;

/// Whether a full leading swipe performs the first action returned by the
/// applicable `leadingSwipeActions` callback. Default: YES.
/// In a left-to-right layout this is a left-to-right swipe.
@property (nonatomic, readwrite) BOOL leadingSwipePerformsFirstActionWithFullSwipe;

/// Whether a full trailing swipe performs the first action returned by the
/// applicable `trailingSwipeActions` callback. Default: YES.
/// In a left-to-right layout this is a right-to-left swipe.
@property (nonatomic, readwrite) BOOL trailingSwipePerformsFirstActionWithFullSwipe;


/// Source image views whose images are copied to corresponding detail cells.
/// The image-view instances themselves are not installed in the detail cells.
/// Only applicable to subclasses that generate detail sections.
@property (nonatomic, nullable, strong) NSArray<UIImageView *> *detailCellsImageViews;

/// If property is TRUE, the cell gets automatically deselected whenever the user selects it. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoDeselect;

/// If property is TRUE, the cell automatically dismisses the keyboard (if applicable) when another cell is selected or when the value of another cell is changed. Default: TRUE.
@property (nonatomic, readwrite) BOOL autoResignFirstResponder;

/// The cell's editing style.
@property (nonatomic, readwrite) UITableViewCellEditingStyle cellEditingStyle;

/// Requests a nonempty value from subclasses that implement required-value
/// validation. Default: NO. Custom validation actions override built-in rules;
/// the base cell does not impose a required-value check.
@property (nonatomic, readwrite) IBInspectable BOOL valueRequired;

/// Whether to use subclass validation when no `valueIsValid` action exists.
/// Default: YES. Actions on the cell, section, or model take precedence
/// regardless of this flag; NO skips built-in validation and required checks.
@property (nonatomic, readwrite) BOOL autoValidateValue;

/// Whether the cell currently passes validation. Disabled cells return YES.
/// Otherwise, the first available `valueIsValid` action on the cell, section,
/// or model determines the result. Without an action, `autoValidateValue`
/// selects subclass `getValueIsValid` rules; when NO, the result is YES.
@property (nonatomic, readonly) BOOL valueIsValid;

/// Options for the generated detail view controller (only applicable to subclasses with a detail view controller).
@property (nonatomic, readonly) SCDetailViewControllerOptions *detailViewControllerOptions;

#pragma mark - Managing Cell Value

/// Whether `cellValueChanged` immediately calls `commitChanges`. Default: YES.
/// When NO, commit through the cell, section, or model explicitly.
/// Invalid edits can remain pending even with live commits enabled. Object
/// detail editors have their own dismissal-time commit lifecycle.
@property (nonatomic, readwrite) BOOL commitChangesLive;

/// Whether a value-change notification has marked the cell as pending commit.
/// Default: NO. It remains YES if validation prevents a control-cell commit;
/// some subclasses also retain it after committing.
@property (nonatomic, readonly) BOOL needsCommit;

/// Commits pending control edits to the bound object in supporting subclasses.
/// Control-cell implementations normally skip invalid or unchanged values.
/// The base implementation only clears `needsCommit`; this is not a general
/// data-store save operation.
- (void)commitChanges;

/// Reloads the cell's bound value in case the associated boundProperty value has changed
/// by means other than the cell itself (e.g. external custom code).
- (void)reloadBoundValue;

#pragma mark - Misc. Properties

/// Is TRUE if the cell is a special cell. Special cells are cells like addNewItemCell or placeholderCell and typically exist within an SCArrayOfItemsSection.
@property (nonatomic, readonly) BOOL isSpecialCell;

#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Property should only be used internally by framework.
@property (nonatomic, readwrite) SCDataType boundPropertyDataType;

/// The value read through the cell's binding and value-loading actions.
/// A nonnil `calculatedValue` result takes precedence over the bound property.
/// Assigning writes through the store or property utilities after the applicable
/// `willCommitBoundValue` action; it does not validate or mark edits pending.
/// Missing bindings and placeholder property names can yield nil.
@property (nonatomic, nullable, strong) NSObject *boundValue;

/// Property used internally by framework to change reuseIdentifier after the cell has been created.
@property (nonatomic, nullable, copy) NSString *reuseId;

/// Property used internally.
@property (nonatomic, readwrite) BOOL configured;

/// Property used internally by framework to determine if cell is being dequed and reused.
@property (nonatomic, readwrite) BOOL beingReused;

/// Property used internally by framework to determine if cell is a custom cell.
@property (nonatomic, readwrite) BOOL customCell;

/// For internal use only.
@property (nonatomic) BOOL cellCreatedInIB;

/// For internal use only.
@property (nonatomic, nullable, copy) NSString *ibDetailViewControllerIdentifier;

/// For internal use only.
@property (nonatomic, readonly) BOOL generatesDetailView;

/// Clones a table cell by archiving and decoding it as the receiving STV class.
/// The decoder's class substitution is scoped to that decoder instance.
/// - Parameter cell: The cell to clone; the original is not modified.
/// - Returns: A decoded cell of the receiving class, or nil after an
///   archive/decode failure, exception, or unexpected decoded type.
+ (nullable instancetype)cellWithCell:(UITableViewCell *)cell;


/// Method should be overridden by subclasses to perform any required initialization.
/// - Warning: Subclasses must call [super performInitialization] from within the method's implementation.
- (void)performInitialization;

/// Restores the saved pre-edit bound value, if any. The base cell records it
/// on the first bound-value write after the object or property changes, not
/// when the binding is assigned. Custom controls maintain per-property values.
- (void)rollbackToInitialBoundValue;

/// Method called internally.
/// - Parameter custom: Whether the property uses custom binding.
- (void)setIsCustomBoundProperty:(BOOL)custom;

/// Method called internally.
/// - Parameter needsCommit: Whether the pending value needs to be committed.
- (void)setNeedsCommit:(BOOL)needsCommit;

/// Marks edits pending, attempts a live commit when enabled, notifies the
/// owning model, and requests row-height invalidation.
/// Use when implementing a cell subclass; use `cellActions.valueChanged`
/// to observe changes instead.
- (void)cellValueChanged;

/// Subclass hook called before display; the base implementation does nothing.
/// Control subclasses load bindings here. Use `cellActions.willDisplay` to
/// observe display without subclassing.
- (void)willDisplay;

/// Subclass selection hook. The base implementation deselects the row when
/// `autoDeselect` is YES. Use `cellActions.didSelect` to observe selection.
- (void)didSelectCell;

/// 	Method gets called internally whenever the cell is about to be deselected.
- (void)willDeselectCell;

/// 	Method gets called internally whenever the cell is deselected.
- (void)didDeselectCell;

/// Marks the cell as a special cell.
- (void)markCellAsSpecial;

/// Applies property attributes to the cell. The base implementation copies
/// the image and detail-image array, clearing them when attributes is nil.
/// Subclasses apply the settings supported by their attribute class.
/// - Parameter attributes: The attributes to apply, or nil.
- (void)setAttributesTo:(nullable SCPropertyAttributes *)attributes;

/// Returns subclass-specific validation. The base implementation returns YES.
/// Override in a cell subclass, or supply a `valueIsValid` action on the cell,
/// section, or model for custom validation.
- (BOOL)getValueIsValid;

/// Temporarily clears the owning STV controller's delegate and, when present,
/// locks selection in the master model before detail presentation.
- (void)prepareCellForDetailViewAppearing;

/// Restores the saved owning STV controller delegate and unlocks selection
/// in the master model as detail presentation ends.
- (void)prepareCellForDetailViewDisappearing;

/// Replaces the detail options used by the cell.
/// - Parameter options: The options, or nil to discard them. The next read of
///   `detailViewControllerOptions` lazily creates fresh defaults.
- (void)setDetailViewControllerOptions:(nullable SCDetailViewControllerOptions *)options;

/// Returns the table model of an `SCTableViewController` or `SCViewController`.
/// - Parameter viewController: The controller to inspect, or nil.
/// - Returns: Its table model, or nil for other controller types or nil input.
- (nullable SCTableViewModel *)modelForViewController:(nullable UIViewController *)viewController;

/// Subclasses should override this method to change the returned default detail view controller navigation bar type.
- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType;

/// Resolves a custom detail controller or creates an STV controller, then
/// configures it and builds/configures its detail model.
/// - Parameter cell: The source cell used for the fallback detail title.
/// - Parameter indexPath: The source row index, or nil for a detached cell.
/// - Parameter allowUITableViewController: Selects `SCTableViewController`
///   rather than `SCViewController` for the generated fallback; it does not
///   restrict the class returned by a custom callback.
/// - Returns: The configured controller. Non-STV custom controllers may have
///   no detail model, so model hooks must accept nil.
- (UIViewController *)getDetailViewControllerForCell:(SCTableViewCell *)cell forRowAtIndexPath:(nullable NSIndexPath *)indexPath allowUITableViewControllerSubclass:(BOOL)allowUITableViewController;

/// Subclasses should call this method to present their detail view controller.
/// - Parameter detailViewController: The detail view controller.
/// - Parameter cell: The cell to configure.
/// - Parameter indexPath: The row index path.
/// - Parameter mode: The presentation mode.
- (void)presentDetailViewController:(UIViewController *)detailViewController forCell:(SCTableViewCell *)cell forRowAtIndexPath:(nullable NSIndexPath *)indexPath withPresentationMode:(SCPresentationMode)mode;

/// Subclasses should override this method to configure the generated detail view controller.
/// - Parameter detailViewController: The detail view controller.
- (void)configureDetailViewController:(UIViewController *)detailViewController;

/// Subclass hook for generating the detail model's content. The base does nothing.
/// - Parameter detailModel: The detail model, or nil for a custom controller
///   without an STV table model.
- (void)buildDetailModel:(nullable SCTableViewModel *)detailModel;

/// Configures the detail model's master bound object and store.
/// Subclasses can extend this method to configure generated detail content.
/// - Parameter detailModel: The detail model, possibly nil for custom controllers.
- (void)configureDetailModel:(nullable SCTableViewModel *)detailModel;

/// Subclass hook for committing detail changes after noncancelled dismissal.
/// The base implementation does nothing.
/// - Parameter detailModel: The detail model, possibly nil for custom controllers.
- (void)commitDetailModelChanges:(nullable SCTableViewModel *)detailModel;

/// Subclasses should override this method to handle their detail view controller willPresent. Superclass method must be called.
/// - Parameter detailViewController: The detail view controller.
- (void)handleDetailViewControllerWillPresent:(UIViewController *)detailViewController;

/// Subclasses should override this method to handle their detail view controller didPresent. Superclass method must be called.
/// - Parameter detailViewController: The detail view controller.
- (void)handleDetailViewControllerDidPresent:(UIViewController *)detailViewController;

/// Subclasses should override this method to handle their detail view controller shouldDismiss. Superclass method must be called.
/// - Parameter detailViewController: The detail view controller.
/// - Parameter cancelTapped: Whether the cancel button was tapped.
/// - Parameter doneTapped: Whether the done button was tapped.
- (BOOL)handleDetailViewControllerShouldDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Subclasses should override this method to handle their detail view controller willDismiss. Superclass method must be called.
/// - Parameter detailViewController: The detail view controller.
/// - Parameter cancelTapped: Whether the cancel button was tapped.
/// - Parameter doneTapped: Whether the done button was tapped.
- (void)handleDetailViewControllerWillDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Subclasses should override this method to handle their detail view controller didDismiss. Superclass method must be called.
/// - Parameter detailViewController: The detail view controller.
/// - Parameter cancelTapped: Whether the cancel button was tapped.
/// - Parameter doneTapped: Whether the done button was tapped.
- (void)handleDetailViewControllerDidDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

/// Subclass hook for creating or loading a configured detail controller.
/// The base implementation returns nil.
/// - Parameter indexPath: The row index path, or nil for a detached cell.
/// - Returns: A detail controller, or nil when none is generated.
- (nullable UIViewController *)generatedDetailViewController:(nullable NSIndexPath *)indexPath;

@end


#pragma mark - SCCustomCell
/// Base class for user-defined cells with tagged, automatically bound controls.
///
/// Binding discovery and control lookup inspect direct `contentView` subviews,
/// not nested view hierarchies. Positive tag strings map to object property names.
/// Supported value-loading controls include:
///
/// - `UILabel`: object descriptions, including strings, numbers, and dates.
/// - `UITextField` and `UITextView`: values converted to strings.
/// - `UISlider`, `UISegmentedControl`, and `UISwitch`: numeric values.
/// - `UIButton`: a string-converted title.
/// - `UIImageView`: a `UIImage`, image-name string, or URL string.
///
/// Editable text and value controls commit through the bindings. Button and
/// image bindings load values; they do not automatically write them back.
/// Supply constraints for custom content to participate in automatic row sizing.
@interface SCCustomCell : SCTableViewCell <UITextViewDelegate, UITextFieldDelegate> {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    BOOL _pauseControlEvents;
    BOOL _autoResize;
}


#pragma mark - Creation and Initialization

/// Creates a custom cell with bindings that can receive a bound object later.
/// - Parameter cellText: The optional title-label text.
/// - Parameter bindings: Optional string-tag-to-property-name bindings for
///   direct content subviews. Tags must be positive.
/// - Parameter nibName: Optional nib name. Its first top-level object should
///   be an `SCCustomCell`; a regular `UITableViewCell` is cloned as one.
///   Nil constructs the receiving class programmatically. See
///   `SCPropertyDefinition.uiElementNibName` for resource-name rules.
/// - Returns: The configured cell, or nil for an unsuitable nib object or
///   failed cell conversion. Missing or invalid nib resources may raise.
/// - Note: A nib canvas height is an estimate; its constraints size the row.
+ (nullable instancetype)cellWithText:(nullable NSString *)cellText objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings nibName:(nullable NSString *)nibName;

/// Creates a custom cell from a bindings string and optional nib.
/// - Parameter cellText: The optional title-label text.
/// - Parameter bindingsString: Semicolon-separated `tag:propertyName` pairs,
///   such as `@"1:firstName;2:lastName"`. Nil supplies no explicit bindings.
/// - Parameter nibName: Optional nib name. The first top-level object must be
///   an `SCCustomCell` or a convertible `UITableViewCell`. Nil constructs the
///   receiving class programmatically. See `SCPropertyDefinition.uiElementNibName`
///   for resource-name rules.
/// - Returns: The configured cell, or nil for an unsuitable nib object or
///   failed conversion. Missing or invalid nib resources may raise.
/// - Warning: Control tags must be positive; only direct content subviews bind.
+ (nullable instancetype)cellWithText:(nullable NSString *)cellText objectBindingsString:(nullable NSString *)bindingsString nibName:(nullable NSString *)nibName;

/// Creates a bound custom cell using tagged controls and an optional nib.
/// - Parameter cellText: The optional title-label text.
/// - Parameter object: The bound object, or nil to assign one later.
/// - Parameter bindings: Optional string-tag-to-property-name bindings for
///   direct content subviews. Tags must be positive.
/// - Parameter nibName: Optional nib name. The first top-level object must be
///   an `SCCustomCell` or a convertible `UITableViewCell`. Nil constructs the
///   receiving class programmatically. See `SCPropertyDefinition.uiElementNibName`
///   for resource-name rules.
/// - Returns: The configured cell, or nil for an unsuitable nib object or
///   failed conversion. Missing or invalid nib resources may raise.
/// - Note: A nib canvas height is an estimate; its constraints size the row.
+ (nullable instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings nibName:(nullable NSString *)nibName;


#pragma mark - Configuration

/// This dictionary specifies how each of the cell's custom controls binds itself to the boundObject's properties.
///
/// Each dictionary key
/// should be the tag string value of one of the cell's custom controls, and the value should be the
/// name of the boundObject's property that is bound to that control.
///
/// - Warning: All control tags must be greater than zero.
/// - Note: This property can be used interchangeably with objectBindingsString
@property (nonatomic, readonly) NSMutableDictionary<NSString *, NSString *> *objectBindings;

/// A semicolon-separated list of `tag:propertyName` control bindings.
/// Setting merges entries into `objectBindings`, replacing matching keys but
/// retaining others; nil or an empty string does not clear existing bindings.
/// The getter returns nil for an empty dictionary; pair ordering is unspecified.
///
/// ```objective-c
/// NSString *bindingsString = @"1:firstName;2:lastName";
/// ```
/// - Warning: Tags must be positive and identify direct content subviews.
/// - SeeAlso: ``objectBindings``.
@property (nonatomic, nullable, copy) NSString *objectBindingsString;

/// A content-resizing preference, YES by default. `SCTextViewCell` uses it
/// to grow its programmatic editor between the configured height limits;
/// NO constrains that editor to its minimum height. It does not disable
/// UIKit self-sizing or replace constraints on arbitrary custom or nib views.
@property (nonatomic, readwrite) BOOL autoResize;

/// Whether the model's input accessory shows its Clear button for this cell.
/// Default: NO for custom cells; `SCDateCell` sets YES.
@property (nonatomic, readwrite) BOOL showClearButtonInInputAccessoryView;


#pragma mark - Managing Custom Control Values

/// Adds or replaces a tagged-control binding.
/// - Parameter propertyName: The bound property name; nil removes the binding.
/// - Parameter controlTag: A positive tag identifying a direct content subview.
- (void)setObjectBindingsPropertyName:(nullable NSString *)propertyName forControlWithTag:(NSInteger)controlTag;

/// Finds a direct `contentView` subview with the supplied positive tag.
/// - Parameter controlTag: The tag to find.
/// - Returns: The matching direct subview, or nil if absent or the tag is
///   less than one. Nested subviews are not searched.
- (nullable UIView *)controlWithTag:(NSInteger)controlTag;

/// Reads the object property mapped to the supplied tag.
/// - Parameter controlTag: The tag key in `objectBindings`.
/// - Returns: The property value, or nil if the object, mapping, or property
///   is unavailable. This method does not look up the control view itself.
- (nullable NSObject *)boundValueForControlWithTag:(NSInteger)controlTag;

/// Commits the bound value for the control with the given tag value.
/// - Parameter controlTag: The tag identifying the custom control.
/// - Parameter controlValue: The value to commit for the custom control.
- (void)commitValueForControlWithTag:(NSInteger)controlTag value:(nullable NSObject *)controlValue;


#pragma mark - Internal Methods (should only be used by the framework or when subclassing)

/// The text fields and text views that are direct `contentView` subviews.
/// Despite the name, the current implementation returns subview order: its
/// nonmutating sort result is discarded. Do not rely on tag ordering.
@property (nonatomic, readonly) NSArray<UIView *> *inputControlsSortedByTag;

/// 	Method must be called by subclasses to configure any added custom controls for automatic binding.
- (void)configureCustomControls;

/// Method called internally by framework.
- (void)loadBindingsIntoCustomControls;

/// Method called internally by framework to reload control values, if needed.
- (void)reloadControlValuesIfNeeded;

/// Method gets called internally whenever the value of a UITextField control is changed.
/// - Parameter sender: The object sending the action.
- (void)textFieldEditingChanged:(id)sender;

/// Method gets called internally whenever the value of a UISlider control is changed.
/// - Parameter sender: The object sending the action.
- (void)sliderValueChanged:(id)sender;

/// Method gets called internally whenever the value of a UISegmentedControl is changed.
/// - Parameter sender: The object sending the action.
- (void)segmentedControlValueChanged:(id)sender;

/// Method gets called internally whenever the value of a UISwitch control is changed.
/// - Parameter sender: The object sending the action.
- (void)switchControlChanged:(id)sender;

/// Method gets called internally whenever the a custom button is tapped.
/// - Parameter sender: The object sending the action.
- (void)customButtonTapped:(id)sender;

// Call designated first responder actions
/// Invokes the actions associated with becoming first responder.
- (void)callDidBecomeFirstResponderActions;
/// Invokes the actions associated with resigning first responder.
- (void)callDidResignFirstResponderActions;

// Used to set initial control values so that they could be rolled back later on if the user issues a 'Cancel' operation
/// Sets a control's initial value when one has not already been recorded.
/// - Parameter value: The value to assign or convert.
/// - Parameter propertyName: The property name.
- (void)setInitialControlValueIfNeeded:(nullable id)value propertyName:(NSString *)propertyName;

@end


#pragma mark - SCControlCell
/// This class functions as a base class for all cells that require user controls to be
/// present inside their content view.

@interface SCControlCell : SCCustomCell {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    BOOL _boundValueLoaded;
    UIColor *_Nullable _initialControlColor;
}


#pragma mark - Configuration

/// The associated control. Assigning a compatible control removes the previous
/// control and configures the replacement, adding it to `contentView` if it
/// has no superview. Nil removes the control. An incompatible class is ignored.
/// The type is `UIView` because editors such as `UITextView` are not UIControls.
@property (nonatomic, nullable, strong) IBOutlet UIView *control;

/// The current control value, or nil for an empty or unsupported control.
@property (nonatomic, nullable, readonly) NSObject *controlValue;

/// The title-label width cap, in points, for programmatic side-by-side layout.
/// Default: 200 points. This is a maximum, not a fixed width; available space
/// and the neighboring control's layout requirements can further limit it.
/// Programmatic titles yield before standard controls and images when space
/// is insufficient; callers can override horizontal compression resistance.
/// Changing the cap schedules constraint and layout updates. Nib-authored
/// layout and stacked text editors use their own constraints.
@property (nonatomic, readwrite) CGFloat maxTextLabelWidth;

/// The preferred minimum leading offset of a programmatic control from its
/// layout group when a title is visible. Default: 120 points.
/// This directional priority-750 constraint adapts to available width; it is
/// not a fixed distance from the cell's left edge. Nib and stacked text-view
/// layouts do not use this side-by-side constraint.
@property (nonatomic, readwrite) CGFloat controlIndentation;

/// The minimum title-to-control gap in programmatic side-by-side layout.
/// Default: 20 points. The actual gap can grow to satisfy other constraints;
/// nib-authored and stacked text-view layouts use their own spacing.
@property (nonatomic, readwrite) CGFloat controlMargin;

/// Subclass hook for clearing the editor. The base implementation does nothing.
/// Text fields and text views clear their text without notifying a value
/// change; the date-cell override also calls `cellValueChanged`.
- (void)clearControl;

#pragma mark - Internal Methods (should only be used by the framework or when subclassing)

/// Returns TRUE if the cell's control has been created in Interface Builder.
@property (nonatomic, readonly) BOOL controlCreatedInIB;

/* Internal */
/// The control label supplied by Interface Builder.
@property (nonatomic, nullable, readonly) UILabel *ibControlLabel;

/// 	Method should be overridden by subclasses to return the expected class of the cell's control. Default: returns [UIView class]
- (Class)expectedControlClass;

/// 	Method should be overridden by subclasses to configure the cell's control.
- (void)configureControl;

/// 	Method should be overridden by subclasses to load the cell's bound value into its control.
- (void)loadBoundValueIntoControl;


@end


#pragma mark - SCLabelCell
/// This class functions as a cell with a UILabel control. The bound property of this cell can be of any type the descends from NSObject.

@interface SCLabelCell : SCControlCell {
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCLabelCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UILabel's text. The value is displayed using its object description and may be readonly.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object labelTextPropertyName:(nullable NSString *)propertyName;


/// Returns an initialized `SCLabelCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UILabel's text. The value is displayed using its object description and may be readonly.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object labelTextPropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UILabel control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UILabel *label;

#pragma mark - Internal Methods (should only be used by the framework or when subclassing)

/// Method should be overridden by subclasses (instead of loadBoundValueIntoControl) to load the label's bound value.
- (void)loadBoundValueIntoLabel;

@end


#pragma mark - SCTextViewCell
/// This class functions as a cell with a UITextView control. The bound property of this cell must be of type NSString.

@interface SCTextViewCell : SCControlCell


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextViewCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UITextView's text. Property must be a readwrite property of type NSString.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object textViewTextPropertyName:(nullable NSString *)propertyName;


/// Returns an initialized `SCTextViewCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UITextView's text. Property must be a readwrite property of type NSString.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object textViewTextPropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UITextView control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UITextView *textView;

/// The minimum programmatic text-editor height in points. Default: 65.
/// Negative assignments become zero. This is not the entire row height;
/// title and vertical spacing are additional. Nib constraints remain authored.
@property (nonatomic, readwrite) CGFloat minimumHeight;

/// The maximum programmatic text-editor height in points. Default: 500.
/// With `autoResize` enabled, a positive value caps the viewport at least at
/// `minimumHeight`; a nonpositive value removes the cap. With `autoResize`
/// disabled, the viewport is constrained to `minimumHeight` instead. Longer
/// text remains editable by scrolling. Nib constraints remain authored.
@property (nonatomic, readwrite) CGFloat maximumHeight;


@end


#pragma mark - SCTextFieldCell
/// This class functions as a cell with a UITextField control. The bound property of this cell must be of type NSString.

@interface SCTextFieldCell : SCControlCell {
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextFieldCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter placeholder: The placeholder string for UITextField.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UITextField's text. Property must be a readwrite property of type NSString.
+ (instancetype)cellWithText:(nullable NSString *)cellText placeholder:(nullable NSString *)placeholder boundObject:(nullable NSObject *)object textFieldTextPropertyName:(nullable NSString *)propertyName;


/// Returns an initialized `SCTextFieldCell` given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter placeholder: The placeholder string for UITextField.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UITextField's text. Property must be a readwrite property of type NSString.
- (instancetype)initWithText:(nullable NSString *)cellText placeholder:(nullable NSString *)placeholder boundObject:(nullable NSObject *)object textFieldTextPropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UITextField control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UITextField *textField;


@end


#pragma mark - SCNumericTextFieldCell
/// This class functions as a cell with a UITextField control with numeric values. The bound property of this cell must be of type NSNumber, int, float, or double. SCNumericTextFieldCell defines its own validation rules according to the following criteria:
/// - An empty value is valid unless `valueRequired` is YES.
/// - Nonempty values must be parsed by `numberFormatter` with the configured limits.
/// - If minimumValue is not nil, then value is only valid if it's greater than or equal to this value.
/// - If maximumValue is not nil, then value is only valid if it's less than or equal to this value.
/// - `allowFloatValue` sets the formatter's `allowsFloats` parsing option.

@interface SCNumericTextFieldCell : SCTextFieldCell


#pragma mark - Configuration

/// The minimum valid cell value.
@property (nonatomic, nullable, copy) NSNumber *minimumValue;

/// The maximum valid cell value.
@property (nonatomic, nullable, copy) NSNumber *maximumValue;

/// Whether the number formatter permits floating-point input. Default: YES.
@property (nonatomic, readwrite) BOOL allowFloatValue;

/// If TRUE, an empty space is displayed if the bound value equals zero. Default: FALSE.
@property (nonatomic, readwrite) BOOL displayZeroAsBlank;

/// The decimal-style formatter used for display, parsing, and validation.
/// The cell reapplies `minimumValue`, `maximumValue`, and `allowFloatValue`
/// before using it. Numeric attributes may supply a shared formatter.
@property (nonatomic, readonly) NSNumberFormatter *numberFormatter;


@end


#pragma mark - SCSliderCell
/// A cell with a UISlider bound to an NSNumber or KVC-compatible numeric scalar.
/// Programmatic sliders default to noncontinuous value-change events.
/// A bound-object initializer writes the slider's initial value when the bound
/// value is nil and live committing is enabled.
@interface SCSliderCell : SCControlCell {
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCSliderCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UISlider's value. Use a writable NSNumber property or a compatible numeric scalar.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object sliderValuePropertyName:(nullable NSString *)propertyName;


/// Returns an initialized SCSliderCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the UISlider's value. Use a writable NSNumber property or a compatible numeric scalar.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object sliderValuePropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UISlider control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UISlider *slider;


@end


#pragma mark - SCSegmentedCell
/// A cell with a segmented control bound to a numeric index or string title.
/// Numeric properties may be NSNumber or KVC-compatible scalar values.
/// String-title commits require `boundPropertyDataType` to be
/// `SCDataTypeNSString`; generated definitions configure this automatically.
/// A bound-object initializer writes -1 when the bound value is nil.
@interface SCSegmentedCell : SCControlCell {
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCSegmentedCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The writable numeric-index property, or string-title property when `boundPropertyDataType` is `SCDataTypeNSString`.
/// - Parameter cellSegmentTitlesArray: An array containing title strings for UISegmentedControl's segments.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedSegmentIndexPropertyName:(nullable NSString *)propertyName segmentTitlesArray:(nullable NSArray<NSString *> *)cellSegmentTitlesArray;


/// Returns an initialized SCSegmentedCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The writable numeric-index property, or string-title property when `boundPropertyDataType` is `SCDataTypeNSString`.
/// - Parameter cellSegmentTitlesArray: An array containing title strings for UISegmentedControl's segments.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedSegmentIndexPropertyName:(nullable NSString *)propertyName segmentTitlesArray:(nullable NSArray<NSString *> *)cellSegmentTitlesArray;


#pragma mark - Configuration

/// The UISegmentedControl associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UISegmentedControl *segmentedControl;

/// Removes existing segments, then creates segments from the supplied titles.
/// - Parameter segmentTitlesArray: An array of strings; nil leaves no segments.
- (void)createSegmentsUsingArray:(nullable NSArray<NSString *> *)segmentTitlesArray;


@end


#pragma mark - SCSwitchCell
/// A cell with a UISwitch bound to an NSNumber or BOOL property.
/// A bound-object initializer writes the switch's initial value when the bound
/// value is nil and live committing is enabled.
@interface SCSwitchCell : SCControlCell {
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCSwitchCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The writable NSNumber or BOOL property corresponding to the switch's on value.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object switchOnPropertyName:(nullable NSString *)propertyName;


/// Returns an initialized SCSwitchCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The writable NSNumber or BOOL property corresponding to the switch's on value.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object switchOnPropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UISwitch control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, nullable, readonly) UISwitch *switchControl;


@end


#pragma mark - SCDateCell
/// This class functions as a cell that provides the end-user with a UIDatePicker to choose a date with. The bound property of this cell must be of type NSDate.
@interface SCDateCell : SCLabelCell <SCViewControllerDelegate>


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCDateCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the cell's date selection. Property must be a readwrite property of type NSDate.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object datePropertyName:(nullable NSString *)propertyName;


/// Returns an initialized SCDateCell given cell text, bound object, and a bound property name.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the cell's date selection. Property must be a readwrite property of type NSDate.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object datePropertyName:(nullable NSString *)propertyName;


#pragma mark - Configuration

/// The UIDatePicker control associated with the cell. Even though this property is readonly, feel free to customize any of the control's properties.
@property (nonatomic, readonly) UIDatePicker *datePicker;

/// Formats the date displayed by the label. Initially uses
/// `@"MMM d  hh:mm a"`. Nil provides no formatted date string.
@property (nonatomic, nullable, strong) NSDateFormatter *dateFormatter;

/// Whether the date label is visible when its bound value is loaded.
/// Default: YES. The inherited programmatic label is right-aligned.
@property (nonatomic, readwrite) BOOL displaySelectedDate;

/// A placeholder string that appears when no date is selected. Default: nil.
@property (nonatomic, nullable, copy) NSString *placeholder;

/// Shows the picker as a keyboard-replacement `inputView` when YES, rather
/// than expanding it inside the row. Default: NO. The model's accessory
/// toolbar, if used, is separate from the picker.
@property (nonatomic, readwrite) BOOL displayDatePickerAsInputAccessoryView;

@end


#pragma mark - SCSelectionCell
/// A cell with a generated detail list for selecting strings.
///
/// Use a writable `NSNumber` binding for a single index, a writable `NSString`
/// binding for selected titles separated by semicolons, or an initialized
/// `NSMutableSet` of NSNumber indexes for multiple selections. An existing
/// mutable set can be exposed by a readonly property because commits mutate
/// its contents. Supply an item store directly or through the item initializers.
///
/// `allowMultipleSelection` and `allowNoSelection` configure detail-list
/// interaction; required-value validation is separate. Summary labels use
/// `delimeter`, not the semicolon used by string bindings.
///
/// When `allowMultipleSelection` is YES and `delimeter` is exactly `@"\n"`,
/// summary refreshes set `label.numberOfLines` to zero. Automatically sized
/// rows can then grow to show the selected titles on separate lines. This also
/// applies to `SCObjectSelectionCell`; explicit fixed cell heights still apply.
/// Leaving this mode restores the label's previous line count unless the caller
/// has since assigned a nonzero count. Other separators do not enable this mode.
///
/// Changing either property refreshes the summary and requests an automatic
/// row-height update without committing or changing the selected indexes.
///
/// - SeeAlso: ``SCSelectionSection``.
@interface SCSelectionCell : SCLabelCell


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCSelectionCell given cell text, bound object, an NSNumber bound property name, and an array of selection items.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected item index. Property must be of an NSNumber type and cannot be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedIndexPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems;

/// Allocates and returns an initialized SCSelectionCell given cell text, bound object, a bound property name, an array of selection items, and whether to allow multiple selection.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected items indexes set. Property must be of an NSMutableSet type and can be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
/// - Parameter multipleSelection: Determines if multiple selection is allowed.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedIndexesPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems allowMultipleSelection:(BOOL)multipleSelection;

/// Allocates and returns an initialized SCSelectionCell given cell text, bound object, an NSString bound property name, and an array of selection items.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected item string. Property must be of an NSString type and cannot be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectionStringPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems;


/// Returns an initialized SCSelectionCell given cell text, bound object, an NSNumber bound property name, and an array of selection items.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected item index. Property must be of an NSNumber type and cannot be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedIndexPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems;

/// Returns an initialized SCSelectionCell given cell text, bound object, a bound property name, an array of selection items, and whether to allow multiple selection.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected items indexes set. Property must be of an NSMutableSet type and can be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
/// - Parameter multipleSelection: Determines if multiple selection is allowed.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedIndexesPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems allowMultipleSelection:(BOOL)multipleSelection;

/// Returns an initialized SCSelectionCell given cell text, bound object, an NSString bound property name, and an array of selection items.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the selected item string. Property must be of an NSString type and cannot be a readonly property.
/// - Parameter cellItems: An array of the items that the user will choose from. All items must be of an NSString type.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectionStringPropertyName:(nullable NSString *)propertyName items:(nullable NSArray<id> *)cellItems;


#pragma mark - Configuration

/// The selection-items store. Assigning it regenerates fetch options,
/// invalidates cached items, and reloads selection and summary values.
/// Default: nil. Supply a store to preserve items across bound-value reloads.
@property (nonatomic, nullable, strong) SCDataStore *selectionItemsStore;

/// The options used to fetch selection items. Assigning invalidates the
/// cache and reloads selection and summary values. Setting `selectionItemsStore`
/// replaces these options with defaults from its definition.
@property (nonatomic, nullable, strong) SCDataFetchOptions *selectionItemsFetchOptions;

/// The cached selection items, fetched from `selectionItemsStore` as needed.
/// An asynchronous fetch can return the previous cache while loading.
/// Assigning also replaces an existing `SCArrayStore`'s objects, but does not
/// create a store. Without a store, manually assigned items are lost on a later
/// bound-value reload. Use the item initializers or configure a store.
@property (nonatomic, nullable, strong) NSArray<id> *items;

/// A selected zero-based item index, or -1 when the selection set is empty.
/// Assigning replaces `selectedItemsIndexes`; a negative index clears it.
/// With multiple selections the getter returns an arbitrary member, not the
/// lowest index. Assignments do not reload the summary or notify/commit edits;
/// call `cellValueChanged` after a programmatic change when that is desired.
@property (nonatomic, copy) NSNumber *selectedItemIndex;

/// The mutable set of selected zero-based item indexes.
/// Mutating it does not reload the summary or notify/commit edits; call
/// `cellValueChanged` after a programmatic change when that is desired.
/// Bound-value reloads rebuild this set from the binding.
@property (nonatomic, readonly) NSMutableSet<NSNumber *> *selectedItemsIndexes;

/// Whether the generated detail list allows multiple selections. Default: NO.
/// With `delimeter` equal to `@"\n"`, summary refreshes enable unlimited lines
/// on `label`. Disabling restores the previously managed line count.
/// Changing this property refreshes the summary and requests automatic sizing;
/// it does not commit changes or reduce an existing set to a single selection.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// If TRUE, the cell allows no selection at all. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
/// - Note: Only applicable when allowMultipleSelection is TRUE.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// If TRUE, the detail view is automatically dismissed when an item is selected. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoDismissDetailView;

/// Whether to hide the owner's navigation bar while detail is presented.
/// Default: NO. Only used when `autoDismissDetailView` is YES; this does not
/// configure a separate modal navigation controller's bar.
@property (nonatomic, readwrite) BOOL hideDetailViewNavigationBar;

/// 	Allows/disables adding new objects to the items array. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowAddingItems;

/// Allows/disables deleting existing objects from the items array. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowDeletingItems;

/// Allows/disables moving an item from one row to another. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowMovingItems;

/// Allows/disables editing detail views for items. Default: FALSE.
@property (nonatomic, readwrite) BOOL allowEditDetailView;

/// Whether to build the selected-item summary in the cell's label.
/// Default: YES. The inherited programmatic label is right-aligned.
@property (nonatomic, readwrite) BOOL displaySelection;

/// The separator between displayed selected-item titles. Default: `@", "`.
/// With `allowMultipleSelection` set to YES, exactly `@"\n"` enables unlimited
/// summary-label lines. Leaving that mode restores the previous line count
/// unless the caller has since assigned a nonzero count. Existing custom line
/// counts are otherwise preserved; `SCLabelCell` defaults are unchanged.
/// Changing the separator refreshes the summary and requests automatic sizing,
/// without committing edits. `displaySelection` controls whether text is shown.
/// String bindings still serialize selections with semicolons instead.
@property (nonatomic, copy) NSString *delimeter;


#pragma mark - Configuring Special Cells

/// When set to a valid cell object, `placeholderCell` will be displayed when no items exist in the generated section. As soon as any items are added, this cell automatically disappears. Default: nil.
@property (nonatomic, nullable, strong) SCTableViewCell *placeholderCell;

/// When set to a valid cell object, `addNewItemCell` will be displayed as the last cell of the generated section, and will add a new item to the section whenever it gets tapped by the user. This cell can be used as an alternative to addButtonItem. Default: nil.
@property (nonatomic, nullable, strong) SCTableViewCell *addNewItemCell;

/// When TRUE, addNewItemCell will be displayed in Normal Mode. Default: FALSE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInNormalMode;

/// When TRUE, addNewItemCell will be displayed in Editing Mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInEditingMode;


@end


#pragma mark - SCObjectSelectionCell
/// A cell with a generated detail list for selecting objects.
///
/// Single selection binds to a writable object property. Multiple selection
/// uses a mutable set or ordered-set relationship, with the property's collection
/// data type resolved through `boundObjectStore` and its definition. Set
/// `allowMultipleSelection` explicitly when configuring a cell manually.
/// With `boundPropertyDataType` set to `SCDataTypeNSString`, the cell instead
/// stores semicolon-separated object titles. Object titles come from the
/// selection store's data definitions. Direct object matching uses identity.
///
/// - SeeAlso: ``SCObjectSelectionSection``.
@interface SCObjectSelectionCell : SCSelectionCell


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCObjectSelectionCell given cell text, bound object, a bound property name, and a selection items store.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the object selection. Use a writable object property for single selection, or a mutable set/ordered-set relationship with a bound-object store definition for multiple selection.
/// - Parameter store: The store containing the selection objects.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItemsStore:(nullable SCDataStore *)store;

/// Allocates and returns an initialized SCObjectSelectionCell given cell text, bound object, a bound property name, and an array of selection objects.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the object selection. Use a writable object property for single selection, or a mutable set/ordered-set relationship with a bound-object store definition for multiple selection.
/// - Parameter items: An array of the objects that the user will choose from. All objects must be of an NSObject type and must be instances of the same class.
/// - Parameter definition: The definition of the selection objects.
+ (instancetype)cellWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItems:(nullable NSArray<id> *)items itemsDefintion:(SCDataDefinition *)definition;


/// Returns an initialized SCObjectSelectionCell given cell text, bound object, a bound property name, and a selection items store.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the object selection. Use a writable object property for single selection, or a mutable set/ordered-set relationship with a bound-object store definition for multiple selection.
/// - Parameter store: The store containing the selection objects.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItemsStore:(nullable SCDataStore *)store;

/// Returns an initialized SCObjectSelectionCell given cell text, bound object, a bound property name, and an array of selection objects.
/// - Parameter cellText: The text that will appear in the cell's textLabel.
/// - Parameter object: The cell's bound object (see SCTableViewCell class overview).
/// - Parameter propertyName: The cell's bound property name corresponding to the object selection. Use a writable object property for single selection, or a mutable set/ordered-set relationship with a bound-object store definition for multiple selection.
/// - Parameter items: An array of the objects that the user will choose from. All objects must be of an NSObject type and must be instances of the same class.
/// - Parameter definition: The definition of the selection objects.
- (instancetype)initWithText:(nullable NSString *)cellText boundObject:(nullable NSObject *)object selectedObjectPropertyName:(nullable NSString *)propertyName selectionItems:(nullable NSArray<id> *)items itemsDefintion:(SCDataDefinition *)definition;
;

#pragma mark - Configuration

/// Reserved configuration for intermediate-entity relationships. Default: nil.
/// Intermediate-entity selection loading and committing are not implemented
/// in this cell. Leave nil for supported direct relationships.
@property (nonatomic, nullable, strong) SCDataDefinition *intermediateEntityDefinition;


@end


#pragma mark - SCObjectCell
/// A cell that generates an editable detail view for a bound object.
///
/// Automatic detail generation requires a `boundObjectStore` that can resolve
/// a data definition. The initializer taking a definition creates a compatible
/// store; the object-only initializer does not infer a definition or store.
/// Set up the store before expecting generated property rows.
///
/// Generated detail sections defer edits until noncancelled dismissal. Their
/// changes are committed then regardless of this cell's `commitChangesLive`.
///
/// - SeeAlso: ``SCObjectSection``, ``SCArrayOfObjectsSection``.
@interface SCObjectCell : SCCustomCell


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCObjectCell given a bound object.
///
/// - Parameter object: The object that SCObjectCell will use to generate its detail view cells.
+ (instancetype)cellWithBoundObject:(nullable NSObject *)object;

/// Allocates and returns an initialized SCObjectCell given a bound object and a class definition.
///
/// - Parameter object: The object that SCObjectCell will use to generate its detail view cells.
/// - Parameter definition: The class definition for the object.
+ (instancetype)cellWithBoundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition;

/// Allocates and returns an initialized SCObjectCell given a bound object and a data store.
///
/// - Parameter object: The object that SCObjectCell will use to generate its detail view cells.
/// - Parameter store: The data store containing the object.
+ (instancetype)cellWithBoundObject:(nullable NSObject *)object boundObjectStore:(nullable SCDataStore *)store;

/// Returns an initialized `SCObjectCell` given a bound object.
///
/// - Parameter object: The object that `SCObjectCell` will use to generate its detail view cells.
- (instancetype)initWithBoundObject:(nullable NSObject *)object;

/// Returns an initialized `SCObjectCell` given a bound object and a class definition.
///
/// - Parameter object: The object that `SCObjectCell` will use to generate its detail view cells.
/// - Parameter definition: The class definition for the object.
- (instancetype)initWithBoundObject:(nullable NSObject *)object boundObjectDefinition:(nullable SCDataDefinition *)definition;

/// Returns an initialized SCObjectCell given a bound object and a data store.
///
/// - Parameter object: The object that SCObjectCell will use to generate its detail view cells.
/// - Parameter store: The data store containing the object.
- (instancetype)initWithBoundObject:(nullable NSObject *)object boundObjectStore:(nullable SCDataStore *)store;


#pragma mark - Configuration

/// The definition resolved by `boundObjectStore` for `boundObject`, or nil
/// without a store/definition. Assigning changes the store's default definition;
/// it does not create a store when `boundObjectStore` is nil.
@property (nonatomic, nullable, strong) SCDataDefinition *objectDefinition;

/// An explicit title for the cell's `textLabel`, or nil to use
/// `objectDefinition.titlePropertyName` through `titleValueForObject:`.
/// Without either source, the existing title is left unchanged.
@property (nonatomic, nullable, copy) NSString *boundObjectTitleText;


@end


#pragma mark - SCArrayOfObjectsCell
/// A cell that displays a generated object-list detail view when tapped.
///
/// The list is backed by `dataStore`, either supplied directly or created by
/// an array/entity initializer. Normal-mode selection presents the detail;
/// the current editing-mode selection path does not generate an object editor.
///
/// - SeeAlso: ``SCObjectCell``, ``SCObjectSection``, ``SCArrayOfObjectsSection``.
@interface SCArrayOfObjectsCell : SCCustomCell


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCArrayOfObjectsCell given a data store.
///
/// - Parameter store: The data store containing the cell's objects.
+ (instancetype)cellWithDataStore:(nullable SCDataStore *)store;

/// Allocates and returns an initialized SCArrayOfObjectsCell given an array of objects and their class definition.
///
/// - Parameter cellItems: An array of objects that the cell will use to generate its detail cells.
/// - Parameter definition: The definition of the objects in the objects array. Register additional object definitions with `dataStore` using `addDataDefinition:`.
+ (instancetype)cellWithItems:(nullable NSMutableArray<id> *)cellItems itemsDefinition:(SCDataDefinition *)definition;


/// Returns an initialized SCArrayOfObjectsCell given a data store.
///
/// - Parameter store: The data store containing the cell's objects.
- (instancetype)initWithDataStore:(nullable SCDataStore *)store;

/// Returns an initialized SCArrayOfObjectsCell given an array of objects and their class definition.
///
/// - Parameter cellItems: An array of objects that the cell will use to generate its detail cells.
/// - Parameter definition: The class definition of the class or entity of the objects in the objects array. Register additional object definitions with `dataStore` using `addDataDefinition:`.
- (instancetype)initWithItems:(nullable NSMutableArray<id> *)cellItems itemsDefinition:(SCDataDefinition *)definition;

/// Allocates and returns an initialized SCArrayOfObjectsCell given an entity definition.
///
/// - Note: Creates a Core Data store for the definition's entity and context. Use `dataFetchOptions` to filter this cell's fetched items or a bound-set initializer for a relationship.
///
/// - Parameter definition: The entity definition of the objects in the objects set.
+ (instancetype)cellWithEntityDefinition:(SCEntityDefinition *)definition;

/// Allocates and returns an initialized SCArrayOfObjectsCell given a mutable set of objects.
///
/// - Warning: This method should only be used to create a cell with the contents of a Core Data relationship.
///
/// - Parameter cellItemsSet: A mutable set of objects that the cell will use to generate its cells.
/// - Parameter definition: The class definition of the entity of the objects in the objects set.
/// - Parameter ownsObjects: Whether the relationship owns the objects in the set.
+ (instancetype)cellWithBoundItemsSet:(NSMutableSet<id> *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects;

/// Returns an initialized SCArrayOfObjectsCell given an entity definition.
///
/// - Note: Creates a Core Data store for the definition's entity and context. Use `dataFetchOptions` to filter this cell's fetched items or a bound-set initializer for a relationship.
///
/// - Parameter definition: The entity definition of the objects in the objects set.
- (instancetype)initWithEntityDefinition:(SCEntityDefinition *)definition;

/// Returns an initialized SCArrayOfObjectsCell given a mutable set of objects.
///
/// - Warning: This method should only be used to create a cell with the contents of a Core Data relationship.
///
/// - Parameter cellItemsSet: A mutable set of objects that the cell will use to generate its cells.
/// - Parameter definition: The class definition of the entity of the objects in the objects set.
/// - Parameter ownsObjects: Whether the relationship owns the objects in the set.
- (instancetype)initWithBoundItemsSet:(NSMutableSet<id> *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects;


#pragma mark - Configuration

/// The data store that's used to store and fetch the cell's objects.
@property (nonatomic, nullable, strong) SCDataStore *dataStore;

/// Options used by this cell's `items` fetch and count badge.
/// Defaults are generated when assigning a store if no options are set.
/// The current detail-generation path does not forward these options to its
/// new section/model; configure that detail explicitly when it needs filtering.
@property (nonatomic, nullable, strong) SCDataFetchOptions *dataFetchOptions;

/// The items fetched from the dataStore.
@property (nonatomic, nullable, readonly) NSArray<id> *items;

/// 	Allows/disables adding new objects to the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowAddingItems;

/// Allows/disables deleting existing objects from the items array. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowDeletingItems;

/// Allows/disables moving objects from one row to another. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowMovingItems;

/// Allows/disables editing detail views for objects. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowEditDetailView;

/// Allows/disables row selection. Default: TRUE.
@property (nonatomic, readwrite) BOOL allowRowSelection;

/// Allows/disables automatic cell selection of newly created items. Default: FALSE.
@property (nonatomic, readwrite) BOOL autoSelectNewItemCell;

/// Whether layout updates the badge with the fetched item count. Default: YES.
/// Asynchronous stores show `-` while fetching. Setting NO stops updates but
/// does not clear existing badge text; clear `badgeView.text` to remove it.
@property (nonatomic, readwrite) BOOL displayItemsCountInBadgeView;

/// The set of actions for the cell's automatically generated detail section.
@property (nonatomic, readonly) SCSectionActions *detailSectionActions;


#pragma mark - Configuring Special Cells

/// When set to a valid cell object, `placeholderCell` will be displayed when no items exist in the generated section. As soon as any items are added, this cell automatically disappears. Default: nil.
@property (nonatomic, nullable, strong) SCTableViewCell *placeholderCell;

/// When set to a valid cell object, `addNewItemCell` will be displayed as the last cell of the generated section, and will add a new item to the section whenever it gets tapped by the user. This cell can be used as an alternative to the section's addButtonItem. Default: nil.
@property (nonatomic, nullable, strong) SCTableViewCell *addNewItemCell;

/// When TRUE, addNewItemCell will be displayed in Normal Mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInNormalMode;

/// When TRUE, addNewItemCell will be displayed in Editing Mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL addNewItemCellExistsInEditingMode;


@end

NS_ASSUME_NONNULL_END
