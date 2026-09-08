/*
 *  SCPropertyAttributes.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCGlobals.h"


@class SCDataDefinition;
@class SCDataStore;
@class SCDataFetchOptions;
@class SCSectionActions;
@class SCEntityDefinition;


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCPropertyAttributes
/// Common attributes used to customize UI generated from a property definition.
///
/// Assign an instance to `SCPropertyDefinition.attributes`, using a subclass
/// for settings specific to that property's UI type. The base class provides
/// images and inline-expansion configuration.
@interface SCPropertyAttributes : NSObject


/// The source image view for the generated cell's image. Default: nil.
/// The cell copies its `image`; the image-view instance is not installed.
@property (nonatomic, strong, nullable) UIImageView *imageView;

/// Source image views for generated detail rows or expanded sections.
/// Their images are copied to the corresponding cells. Default: nil.
/// Applicable to selection, object-selection, object, and object-array content.
@property (nonatomic, strong, nullable) NSArray<UIImageView *> *imageViewArray;

/// When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view. Default: FALSE.
///
/// - Note: Not applicable for all property definition types.
@property (nonatomic) BOOL expandContentInCurrentView;

/// A set of section actions for the section containing the expanded content.
///
/// - Note: Only applicable when expandContentInCurrentView is TRUE.
@property (nonatomic, readonly) SCSectionActions *expandedContentSectionActions;

@end

#pragma mark - SCTextViewAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeTextView, thus allowing further customization of the generated control by the user.
@interface SCTextViewAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextViewAttributes`.
///
/// - Parameter minHeight: The editor minimum height in points. A nonpositive value leaves the cell setting unchanged.
/// - Parameter maxHeight: The editor maximum height in points. A nonpositive value leaves the cell setting unchanged.
/// - Parameter _autoResize: Determines whether the generated UITextView control will auto resize.
/// - Parameter _editable: Determines whether the generated UITextView control will be editable.
+ (instancetype)attributesWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight autoResize:(BOOL)_autoResize editable:(BOOL)_editable;

/// Returns an initialized `SCTextViewAttributes`.
///
/// - Parameter minHeight: The editor minimum height in points. A nonpositive value leaves the cell setting unchanged.
/// - Parameter maxHeight: The editor maximum height in points. A nonpositive value leaves the cell setting unchanged.
/// - Parameter _autoResize: Determines whether the generated UITextView control will auto resize.
/// - Parameter _editable: Determines whether the generated UITextView control will be editable.
- (instancetype)initWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight autoResize:(BOOL)_autoResize editable:(BOOL)_editable;

#pragma mark - Configuration

/// The minimum editor height applied by `SCTextViewCell`, in points.
/// Values at or below zero leave the cell setting unchanged. Default: `FLT_MIN`.
/// The positive default is applied by the current cell implementation.
@property (nonatomic, readwrite) CGFloat minimumHeight;

/// The maximum editor height applied by `SCTextViewCell`, in points.
/// Values at or below zero leave the cell setting unchanged. Default: `FLT_MIN`.
/// The positive default is applied by the current cell implementation; set
/// explicit limits when configuring a text-view cell.
@property (nonatomic, readwrite) CGFloat maximumHeight;

/// Whether a generated text-view editor grows with its content. Default: YES.
/// When NO, the programmatic editor is constrained to its minimum height.
/// Longer text remains editable by scrolling; authored nib constraints are retained.
@property (nonatomic, readwrite) BOOL autoResize;

/// Whether the generated text view is editable. Default: YES.
/// Not applied to a control created in Interface Builder.
@property (nonatomic, readwrite) BOOL editable;

@end


#pragma mark - SCTextFieldAttributes
/// Customizes a generated text-field control for `SCPropertyDefinition`.
/// `SCTextFieldCell` applies these settings to programmatic controls, not
/// controls created in Interface Builder. Numeric text-field attributes inherit
/// these settings.
@interface SCTextFieldAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
+ (instancetype)attributesWithPlaceholder:(nullable NSString *)_placeholder;

/// Allocates and returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
/// - Parameter secure: Identifies whether the text field should hide the text being entered.
/// - Parameter autocorrection: The auto-correction style for the text field.
/// - Parameter autocapitalization: The auto-capitalization style for the text field.
+ (instancetype)attributesWithPlaceholder:(nullable NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization;

/// Returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
- (instancetype)initWithPlaceholder:(nullable NSString *)_placeholder;

/// Returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
/// - Parameter secure: Identifies whether the text field should hide the text being entered.
/// - Parameter autocorrection: The auto-correction style for the text field.
/// - Parameter autocapitalization: The auto-capitalization style for the text field.
- (instancetype)initWithPlaceholder:(nullable NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization;

#pragma mark - Configuration

/// The placeholder of the generated UITextField control. Set to nil to ignore.
@property (nonatomic, copy, nullable) NSString *placeholder;

/// Identifies whether the text field should hide the text being entered. Default: FALSE.
@property (nonatomic, readwrite) BOOL secureTextEntry;

/// The auto-correction style for the text field. Default: UITextAutocorrectionTypeDefault.
@property (nonatomic, readwrite) UITextAutocorrectionType autocorrectionType;

/// The auto-capitalization style for the text field. Default: UITextAutocapitalizationTypeSentences.
@property (nonatomic, readwrite) UITextAutocapitalizationType autocapitalizationType;

@end


#pragma mark - SCNumericTextFieldAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeNumericTextField, thus allowing further customization of the generated control by the user.
@interface SCNumericTextFieldAttributes : SCTextFieldAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
+ (instancetype)attributesWithMinimumValue:(nullable NSNumber *)minValue maximumValue:(nullable NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat;

/// Allocates and returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
/// - Parameter _placeholder: The placeholder of the generated numeric text field control. Set to nil to ignore.
+ (instancetype)attributesWithMinimumValue:(nullable NSNumber *)minValue maximumValue:(nullable NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat placeholder:(nullable NSString *)_placeholder;

/// Returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
- (instancetype)initWithMinimumValue:(nullable NSNumber *)minValue maximumValue:(nullable NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat;

/// Returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
/// - Parameter _placeholder: The placeholder of the generated numeric text field control. Set to nil to ignore.
- (instancetype)initWithMinimumValue:(nullable NSNumber *)minValue maximumValue:(nullable NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat placeholder:(nullable NSString *)_placeholder;

#pragma mark - Configuration

/// The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
@property (nonatomic, copy, nullable) NSNumber *minimumValue;

/// The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
@property (nonatomic, copy, nullable) NSNumber *maximumValue;

/// Whether the generated numeric field's formatter permits floating-point
/// input. Default: YES.
@property (nonatomic, readwrite) BOOL allowFloatValue;

/// The decimal-style formatter supplied to generated numeric cells.
/// The same formatter instance is retained by a cell, which reapplies its
/// minimum, maximum, and floating-point policy before display or parsing.
@property (nonatomic, readonly) NSNumberFormatter *numberFormatter;

@end


#pragma mark - SCSliderAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeSlider, thus allowing further customization of the generated control by the user.
@interface SCSliderAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSliderAttributes`.
///
/// - Parameter minValue: The slider minimum. A negative value leaves the cell setting unchanged.
/// - Parameter maxValue: The slider maximum. A negative value leaves the cell setting unchanged.
+ (instancetype)attributesWithMinimumValue:(float)minValue maximumValue:(float)maxValue;

/// Returns an initialized `SCSliderAttributes`.
///
/// - Parameter minValue: The slider minimum. A negative value leaves the cell setting unchanged.
/// - Parameter maxValue: The slider maximum. A negative value leaves the cell setting unchanged.
- (instancetype)initWithMinimumValue:(float)minValue maximumValue:(float)maxValue;

#pragma mark - Configuration

/// The slider minimum. Nonnegative values are applied to programmatic controls;
/// negative values leave the current minimum unchanged. Default: `FLT_MIN`.
/// This positive default is applied, not ignored.
@property (nonatomic, readwrite) float minimumValue;

/// The slider maximum. Nonnegative values are applied to programmatic controls;
/// negative values leave the current maximum unchanged. Default: `FLT_MIN`.
/// Set an explicit range: the positive default is applied, not ignored.
@property (nonatomic, readwrite) float maximumValue;


@end


#pragma mark - SCSegmentedAttributes
/// Configures segment titles for a generated segmented control.
/// `SCSegmentedCell` preserves Interface Builder segments when the authored
/// control already has at least two segments.
@interface SCSegmentedAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSegmentedAttributes`.
///
/// - Parameter titles: The segment titles of the generated UISegmentedControl. Set to nil to ignore.
+ (instancetype)attributesWithSegmentTitlesArray:(nullable NSArray<NSString *> *)titles;

/// Returns an initialized `SCSegmentedAttributes`.
///
/// - Parameter titles: The segment titles of the generated UISegmentedControl. Set to nil to ignore.
- (instancetype)initWithSegmentTitlesArray:(nullable NSArray<NSString *> *)titles;

#pragma mark - Configuration

/// The segment titles of the generated UISegmentedControl. Set to nil to ignore.
@property (nonatomic, strong, nullable) NSArray<NSString *> *segmentTitlesArray;

@end


#pragma mark - SCDateAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeDate, thus allowing further customization of the generated control by the user.
@interface SCDateAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
+ (instancetype)attributesWithDateFormatter:(nullable NSDateFormatter *)formatter;

/// Allocates and returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
/// - Parameter mode: The date picker mode.
/// - Parameter displayDatePickerAsInputAccessoryView: Set to TRUE to display the date picker as a keyboard-replacement input view, instead of displaying it inside the cell.
+ (instancetype)attributesWithDateFormatter:(nullable NSDateFormatter *)formatter datePickerMode:(UIDatePickerMode)mode displayDatePickerAsInputAccessoryView:(BOOL)displayDatePickerAsInputAccessoryView;

/// Returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
- (instancetype)initWithDateFormatter:(nullable NSDateFormatter *)formatter;

/// Returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
/// - Parameter mode: The date picker mode.
/// - Parameter displayDatePickerAsInputAccessoryView: Set to TRUE to display the date picker as a keyboard-replacement input view, instead of displaying it inside the cell.
- (instancetype)initWithDateFormatter:(nullable NSDateFormatter *)formatter datePickerMode:(UIDatePickerMode)mode displayDatePickerAsInputAccessoryView:(BOOL)displayDatePickerAsInputAccessoryView;

#pragma mark - Configuration

/// The date formatter used to display the date of generated control. Set to nil to ignore.
@property (nonatomic, strong, nullable) NSDateFormatter *dateFormatter;

/// The date picker mode. Default:UIDatePickerModeDateAndTime.
@property (nonatomic, readwrite) UIDatePickerMode datePickerMode;

/// Requests a keyboard-replacement picker input view instead of an embedded
/// picker. Default: NO. Applying NO does not turn off an already-enabled
/// input-view setting on `SCDateCell`. The input accessory toolbar is separate.
@property (nonatomic, readwrite) BOOL displayDatePickerAsInputAccessoryView;

@end


#pragma mark - SCSelectionAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeSelection, thus allowing further customization of the generated control by the user.
@interface SCSelectionAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The string items copied into a new array store. Nil supplies no initial items.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithItems:(nullable NSArray<id> *)_items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

/// Allocates and returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The string items copied into a new array store. Nil supplies no initial items.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
/// - Parameter autoDismiss: Set to TRUE to automatically dismiss the selection detail view when an item is selected.
/// - Parameter hideNavBar: Set to TRUE to hide the detail view's navigation bar.
/// - Note: `hideNavBar` only applies when `autoDismiss` is TRUE.
+ (instancetype)attributesWithItems:(nullable NSArray<id> *)_items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss hideDetailViewNavigationBar:(BOOL)hideNavBar;

/// Returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The string items copied into a new array store. Nil supplies no initial items.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithItems:(nullable NSArray<id> *)_items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The string items copied into a new array store. Nil supplies no initial items.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
/// - Parameter autoDismiss: Set to TRUE to automatically dismiss the selection detail view when an item is selected.
/// - Parameter hideNavBar: Set to TRUE to hide the detail view's navigation bar.
/// - Note: `hideNavBar` only applies when `autoDismiss` is TRUE.
- (instancetype)initWithItems:(nullable NSArray<id> *)_items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss hideDetailViewNavigationBar:(BOOL)hideNavBar;


/// The selection-items store. Default: nil. Assigning it replaces
/// `selectionItemsFetchOptions` with options generated by its default definition.
@property (nonatomic, strong, nullable) SCDataStore *selectionItemsStore;

/// Options used when fetching selection items. Initially nil; assigning
/// `selectionItemsStore` replaces them with compatible defaults.
@property (nonatomic, strong, nullable) SCDataFetchOptions *selectionItemsFetchOptions;

/// Synchronously fetches selection items using `selectionItemsFetchOptions`.
/// Returns nil when there is no store or the store returns no result.
@property (nonatomic, readonly, nullable) NSArray<id> *items;

/// Whether the generated selection control permits multiple selections. Default: NO.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// Whether the generated selection control permits no selection. Default: NO.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
/// - Note: Only applicable when allowMultipleSelection is TRUE.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// Whether selecting an item automatically dismisses its detail view. Default: NO.
@property (nonatomic, readwrite) BOOL autoDismissDetailView;

/// Whether to hide the owner's navigation bar during selection detail display.
/// Default: NO. Applies to the cell detail flow only when `autoDismissDetailView`
/// is YES; it does not hide every custom or modal controller's navigation bar.
@property (nonatomic, readwrite) BOOL hideDetailViewNavigationBar;

/// 	Determines if the generated control allows adding new items. Default: FALSE.
@property (nonatomic) BOOL allowAddingItems;

/// 	Determines if the generated control allows deleting existing items. Default: FALSE.
@property (nonatomic) BOOL allowDeletingItems;

/// 	Determines if the generated control allows moving items. Default: FALSE.
@property (nonatomic) BOOL allowMovingItems;

/// 	Determines if the generated control allows editing items. Default: FALSE.
@property (nonatomic) BOOL allowEditingItems;

/// The placeholder displayed when no items exist, typically an `SCTableViewCell`.
/// Nil leaves the generated section or cell placeholder unchanged. Default: nil.
@property (nonatomic, strong, nullable) NSObject *placeholderuiElement;

/// The row used to add a new object when tapped, typically an `SCTableViewCell`.
/// Nil leaves the generated section or cell add-item row unchanged. Default: nil.
@property (nonatomic, strong, nullable) NSObject *addNewObjectuiElement;

@end


#pragma mark - SCObjectSelectionAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeObjectSelection, thus allowing further customization of the generated control by the user.
@interface SCObjectSelectionAttributes : SCSelectionAttributes

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCObjectSelectionAttributes`.
///
/// - Parameter objects: The objects copied into a new array store for selection; nil supplies no initial objects.
/// - Parameter definition: The data structure definition of the objects.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithSelectionObjects:(nullable NSArray<id> *)objects objectsDefinition:(nullable SCDataDefinition *)definition allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized `SCObjectSelectionAttributes`.
///
/// - Parameter objects: The objects copied into a new array store for selection; nil supplies no initial objects.
/// - Parameter definition: The data structure definition of the objects.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithSelectionObjects:(nullable NSArray<id> *)objects objectsDefinition:(nullable SCDataDefinition *)definition allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

/// Allocates and returns an initialized SCObjectSelectionAttributes.
///
/// - Parameter entityDefinition: The entity definition of the entity whose objects are to be presented for selection.
/// - Parameter predicate: The predicate used to filter the selection objects. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Warning: Use multiple selection for a to-many relationship, not a to-one relationship.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithObjectsEntityDefinition:(nullable SCEntityDefinition *)entityDefinition usingPredicate:(nullable NSPredicate *)predicate allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized SCObjectSelectionAttributes.
///
/// - Parameter entityDefinition: The entity definition of the entity whose objects are to be presented for selection.
/// - Parameter predicate: The predicate used to filter the selection objects. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Warning: Use multiple selection for a to-many relationship, not a to-one relationship.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithObjectsEntityDefinition:(nullable SCEntityDefinition *)entityDefinition usingPredicate:(nullable NSPredicate *)predicate allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel;

#pragma mark - Configuration

/// Reserved configuration for an intermediate relationship entity. Default: nil.
/// The generated object-selection cell does not implement intermediate-entity
/// loading or committing. Leave nil for supported direct relationships.
@property (nonatomic, strong, nullable) SCDataDefinition *intermediateEntityDefinition;


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/// The Interface Builder identifier for the selection-objects definition.
@property (nonatomic, strong, nullable) NSString *objectsDefinitionibUniqueId;
/// The Interface Builder predicate string.
@property (nonatomic, strong, nullable) NSString *ibPredicateString;
/// The Interface Builder placeholder text.
@property (nonatomic, strong, nullable) NSString *ibPlaceholderText;
/// The Interface Builder placeholder-text alignment.
@property (nonatomic) NSTextAlignment ibPlaceholderTextAlignment;
/// The Interface Builder text for the add-new-object row.
@property (nonatomic, strong, nullable) NSString *ibAddNewObjectText;

@end


#pragma mark - SCObjectAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type
/// SCPropertyTypeObject, thus allowing further customization of the generated control by the user.
@interface SCObjectAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
+ (instancetype)attributesWithObjectDefinition:(nullable SCDataDefinition *)definition;

/// Allocates and returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
+ (instancetype)attributesWithObjectDefinition:(nullable SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent;

/// Returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
- (instancetype)initWithObjectDefinition:(nullable SCDataDefinition *)definition;

/// Returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
- (instancetype)initWithObjectDefinition:(nullable SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent;

#pragma mark - Configuration

/// 	The definition for the generated object control. Set to nil to ignore.
@property (nonatomic, strong, nullable) SCDataDefinition *objectDefinition;


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/// The Interface Builder identifier for the object definition.
@property (nonatomic, strong, nullable) NSString *objectDefinitionibUniqueId;

@end


#pragma mark - SCArrayOfObjectsAttributes
/// This class is used to extend the definition of an `SCPropertyDefinition` instance of type SCPropertyTypeArrayOfObjects, thus allowing further customization of the generated control by the user.
@interface SCArrayOfObjectsAttributes : SCPropertyAttributes


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
+ (instancetype)attributesWithObjectDefinition:(nullable SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving;


/// Allocates and returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The class definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
/// - Parameter placeholderUI: The uiElement that is displayed when no items are available. This object is typically of type SCTableViewCell.
/// - Parameter newObjectUI: The uiElement that is used to add new objects when tapped. This object is typically of type SCTableViewCell.
/// - Parameter existsInNormalMode: Determines if the add-new-object row is displayed in `Normal Mode`.
/// - Parameter existsInEditingMode: Determines if the add-new-object row is displayed in `Editing Mode`.
+ (instancetype)attributesWithObjectDefinition:(nullable SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving expandContentInCurrentView:(BOOL)expandContent placeholderuiElement:(nullable NSObject *)placeholderUI addNewObjectuiElement:(nullable NSObject *)newObjectUI addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode;

/// Returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
- (instancetype)initWithObjectDefinition:(nullable SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving;

/// Returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
/// - Parameter placeholderUI: The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell.
/// - Parameter newObjectUI: The uiElement that is used to add new objects when tapped. This object is typically of type SCTableViewCell.
/// - Parameter existsInNormalMode: Determines if the add-new-object row is displayed in `Normal Mode`.
/// - Parameter existsInEditingMode: Determines if the add-new-object row is displayed in `Editing Mode`.
- (instancetype)initWithObjectDefinition:(nullable SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving expandContentInCurrentView:(BOOL)expandContent placeholderuiElement:(nullable NSObject *)placeholderUI addNewObjectuiElement:(nullable NSObject *)newObjectUI addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode;


#pragma mark - Configuration

/// The default definition for the objects. Assigning it replaces
/// `objectsFetchOptions` with compatible options generated by the definition.
/// Default: nil.
@property (nonatomic, strong, nullable) SCDataDefinition *defaultObjectsDefinition;

/// The fetch options for the objects. Initially nil; assigning
/// `defaultObjectsDefinition` replaces them with compatible defaults.
@property (nonatomic, strong, nullable) SCDataFetchOptions *objectsFetchOptions;

/// 	Determines if the generated control allows adding new objects. Default: TRUE.
@property (nonatomic) BOOL allowAddingItems;

/// 	Determines if the generated control allows deleting objects. Default: TRUE.
@property (nonatomic) BOOL allowDeletingItems;

/// 	Determines if the generated control allows moving objects. Default: TRUE.
@property (nonatomic) BOOL allowMovingItems;

/// 	Determines if the generated control allows editing objects. Default: TRUE.
@property (nonatomic) BOOL allowEditingItems;

/// The row used to add a new object when tapped, typically an `SCTableViewCell`.
/// Nil leaves the generated section or cell add-item row unchanged. Default: nil.
@property (nonatomic, strong, nullable) NSObject *addNewObjectuiElement;

/// The placeholder displayed when no items exist, typically an `SCTableViewCell`.
/// Nil leaves the generated section or cell placeholder unchanged. Default: nil.
@property (nonatomic, strong, nullable) NSObject *placeholderuiElement;

/// Determines if the add-new-object row is displayed in `Normal Mode`. Default: TRUE.
@property (nonatomic) BOOL addNewObjectuiElementExistsInNormalMode;

/// Determines if the add-new-object row is displayed in `Editing Mode`. Default: TRUE.
@property (nonatomic) BOOL addNewObjectuiElementExistsInEditingMode;

/// The actions of the generated SCArrayOfObjectsSection.
@property (nonatomic, readonly) SCSectionActions *sectionActions;


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/// The Interface Builder identifier for the default objects definition.
@property (nonatomic, strong, nullable) NSString *defaultObjectsDefinitionibUniqueId;
/// The Interface Builder predicate string.
@property (nonatomic, strong, nullable) NSString *ibPredicateString;
/// The Interface Builder placeholder text.
@property (nonatomic, strong, nullable) NSString *ibPlaceholderText;
/// The Interface Builder placeholder-text alignment.
@property (nonatomic) NSTextAlignment ibPlaceholderTextAlignment;
/// The Interface Builder text for the add-new-object row.
@property (nonatomic, strong, nullable) NSString *ibAddNewObjectText;


@end

NS_ASSUME_NONNULL_END
