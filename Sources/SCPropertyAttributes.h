/*
 *  SCPropertyAttributes.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCGlobals.h"


@class SCDataDefinition;
@class SCDataStore;
@class SCDataFetchOptions;
@class SCSectionActions;
@class SCEntityDefinition;


#pragma mark - SCPropertyAttributes
/// This class is an abstract base class that represents a set of SCPropertyDefinition attributes.
///
/// Each subclass of `SCPropertyAttributes` is used to extend the definition of an SCPropertyDefinition
/// instance according to its respective type. Set the attributes property of an SCPropertyDefinition instance
/// to a subclass to be able to further customize the user interface element that will be generated for
/// this property definition.
///
/// - Note: You should never make instances of this class. Use subclasses instead.
@interface SCPropertyAttributes : NSObject 
{
	/// The image view assigned to the generated UI element.
	UIImageView *imageView;
	/// The array of image views assigned to the detail elements of the generated UI element.
	NSArray *imageViewArray;
    
    /// When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view. Default: FALSE.
    BOOL expandContentInCurrentView;
    SCSectionActions *_expandedContentSectionActions;
}

/// The image view assigned to the generated UI element.
@property (nonatomic, strong) UIImageView *imageView;

/// The array of image views assigned to the detail elements of the generated UI element.
///
/// The property is applicable to property definitions of type SCPropertyTypeSelection,
/// SCPropertyTypeObject, and SCPropertyTypeArrayOfObjects.
@property (nonatomic, strong) NSArray *imageViewArray;

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
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeTextView, thus allowing further customization of the generated control by the user.
@interface SCTextViewAttributes : SCPropertyAttributes
{
	/// The minimum height of the generated UITextView control. Set to a negative value to ignore.
	CGFloat minimumHeight;
	/// The maximum height of the generated UITextView control. Set to a negative value to ignore.
	CGFloat maximumHeight;
	/// Determines whether the generated UITextView control will be editable.
	BOOL editable;
	/// Determines whether the generated UITextView control will auto resize.
	BOOL autoResize;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextViewAttributes`.
///
/// - Parameter minHeight: The minimum height of the generated UITextView control. Set to a negative value to ignore.
/// - Parameter maxHeight: The maximum height of the generated UITextView control. Set to a negative value to ignore.
/// - Parameter _autoResize: Determines whether the generated UITextView control will auto resize.
/// - Parameter _editable: Determines whether the generated UITextView control will be editable.
+ (instancetype)attributesWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight
					   autoResize:(BOOL)_autoResize editable:(BOOL)_editable;

/// Returns an initialized `SCTextViewAttributes`.
///
/// - Parameter minHeight: The minimum height of the generated UITextView control. Set to a negative value to ignore.
/// - Parameter maxHeight: The maximum height of the generated UITextView control. Set to a negative value to ignore.
/// - Parameter _autoResize: Determines whether the generated UITextView control will auto resize.
/// - Parameter _editable: Determines whether the generated UITextView control will be editable.
- (instancetype)initWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight
				 autoResize:(BOOL)_autoResize editable:(BOOL)_editable;

#pragma mark - Configuration

/// The minimum height of the generated UITextView control. Set to a negative value to ignore.
@property (nonatomic, readwrite) CGFloat minimumHeight;

/// The maximum height of the generated UITextView control. Set to a negative value to ignore.
@property (nonatomic, readwrite) CGFloat maximumHeight;

/// Determines whether the generated UITextView control will auto resize.
@property (nonatomic, readwrite) BOOL autoResize;

/// Determines whether the generated UITextView control will be editable.
@property (nonatomic, readwrite) BOOL editable;

@end



#pragma mark - SCTextFieldAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeTextField, thus allowing further customization of the generated control by the user.
@interface SCTextFieldAttributes : SCPropertyAttributes
{
	/// The placeholder of the generated UITextField control. Set to nil to ignore.
	NSString *placeholder;
    /// Identifies whether the text field should hide the text being entered. Default: FALSE.
    BOOL secureTextEntry;
    /// The auto-correction style for the text field. Default: UITextAutocorrectionTypeDefault.
    UITextAutocorrectionType autocorrectionType;
    /// The auto-capitalization style for the text field. Default: UITextAutocapitalizationTypeSentences.
    UITextAutocapitalizationType autocapitalizationType;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
+ (instancetype)attributesWithPlaceholder:(NSString *)_placeholder;

/// Allocates and returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
/// - Parameter secure: Identifies whether the text field should hide the text being entered.
/// - Parameter autocorrection: The auto-correction style for the text field.
/// - Parameter autocapitalization: The auto-capitalization style for the text field.
+ (instancetype)attributesWithPlaceholder:(NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization;

/// Returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
- (instancetype)initWithPlaceholder:(NSString *)_placeholder;

/// Returns an initialized `SCTextFieldAttributes`.
///
/// - Parameter _placeholder: The placeholder of the generated UITextField control. Set to nil to ignore.
/// - Parameter secure: Identifies whether the text field should hide the text being entered.
/// - Parameter autocorrection: The auto-correction style for the text field.
/// - Parameter autocapitalization: The auto-capitalization style for the text field.
- (instancetype)initWithPlaceholder:(NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization;

#pragma mark - Configuration

/// The placeholder of the generated UITextField control. Set to nil to ignore.
@property (nonatomic, copy) NSString *placeholder;

/// Identifies whether the text field should hide the text being entered. Default: FALSE.
@property (nonatomic, readwrite) BOOL secureTextEntry;

/// The auto-correction style for the text field. Default: UITextAutocorrectionTypeDefault.
@property (nonatomic, readwrite) UITextAutocorrectionType autocorrectionType;

/// The auto-capitalization style for the text field. Default: UITextAutocapitalizationTypeSentences.
@property (nonatomic, readwrite) UITextAutocapitalizationType autocapitalizationType;

@end



#pragma mark - SCNumericTextFieldAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeNumericTextField, thus allowing further customization of the generated control by the user.
@interface SCNumericTextFieldAttributes : SCTextFieldAttributes
{
	/// The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
	NSNumber *minimumValue;
	/// The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
	NSNumber *maximumValue;
	/// Determines if the generated numeric text field control allows float values.
	BOOL allowFloatValue;
    /// The number formatter responsible for converting the numeric value to a string and vice versa.
    NSNumberFormatter *numberFormatter;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
+ (instancetype)attributesWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue
				 allowFloatValue:(BOOL)allowFloat;

/// Allocates and returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
/// - Parameter _placeholder: The placeholder of the generated numeric text field control. Set to nil to ignore.
+ (instancetype)attributesWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue
				 allowFloatValue:(BOOL)allowFloat placeholder:(NSString *)_placeholder;

/// Returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
- (instancetype)initWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue
				 allowFloatValue:(BOOL)allowFloat;

/// Returns an initialized `SCNumericTextFieldAttributes`.
///
/// - Parameter minValue: The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
/// - Parameter allowFloat: Determines if the generated numeric text field control allows float values.
/// - Parameter _placeholder: The placeholder of the generated numeric text field control. Set to nil to ignore.
- (instancetype)initWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue
				 allowFloatValue:(BOOL)allowFloat placeholder:(NSString *)_placeholder;

#pragma mark - Configuration

/// The minimum value allowed for the generated numeric text field control. Set to nil to ignore.
@property (nonatomic, copy) NSNumber *minimumValue;

/// The maximum value allowed for the generated numeric text field control. Set to nil to ignore.
@property (nonatomic, copy) NSNumber *maximumValue;

/// Determines if the generated numeric text field control allows float values.
@property (nonatomic, readwrite) BOOL allowFloatValue;

/// The number formatter responsible for converting the numeric value to a string and vice versa.
@property (nonatomic, readonly) NSNumberFormatter *numberFormatter;

@end



#pragma mark - SCSliderAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeSlider, thus allowing further customization of the generated control by the user.
@interface SCSliderAttributes : SCPropertyAttributes
{
	/// The minimum value of the generated UISlider control. Set to nil to ignore.
	float minimumValue;
	/// The maximum value of the generated UISlider control. Set to nil to ignore.
	float maximumValue;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSliderAttributes`.
///
/// - Parameter minValue: The minimum value of the generated UISlider control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value of the generated UISlider control. Set to nil to ignore.
+ (instancetype)attributesWithMinimumValue:(float)minValue maximumValue:(float)maxValue;

/// Returns an initialized `SCSliderAttributes`.
///
/// - Parameter minValue: The minimum value of the generated UISlider control. Set to nil to ignore.
/// - Parameter maxValue: The maximum value of the generated UISlider control. Set to nil to ignore.
- (instancetype)initWithMinimumValue:(float)minValue maximumValue:(float)maxValue;

#pragma mark - Configuration

/// The minimum value of the generated UISlider control. Set to nil to ignore.
@property (nonatomic, readwrite) float minimumValue;

/// The maximum value of the generated UISlider control. Set to nil to ignore.
@property (nonatomic, readwrite) float maximumValue;


@end



#pragma mark - SCSegmentedAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeSegmented, thus allowing further customization of the generated control by the user.
@interface SCSegmentedAttributes : SCPropertyAttributes
{
	/// The segment titles of the generated UISegmentedControl. Set to nil to ignore.
	NSArray *segmentTitlesArray;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSegmentedAttributes`.
///
/// - Parameter titles: The segment titles of the generated UISegmentedControl. Set to nil to ignore.
+ (instancetype)attributesWithSegmentTitlesArray:(NSArray *)titles;

/// Returns an initialized `SCSegmentedFieldAttributes`.
///
/// - Parameter titles: The segment titles of the generated UISegmentedControl. Set to nil to ignore.
- (instancetype)initWithSegmentTitlesArray:(NSArray *)titles;

#pragma mark - Configuration

/// The segment titles of the generated UISegmentedControl. Set to nil to ignore.
@property (nonatomic, strong) NSArray *segmentTitlesArray;

@end



#pragma mark - SCDateAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeDate, thus allowing further customization of the generated control by the user.
@interface SCDateAttributes : SCPropertyAttributes
{
	/// The date formatter used to display the date of generated control. Set to nil to ignore.
	NSDateFormatter *dateFormatter;
	/// The date picker mode. Default:UIDatePickerModeDateAndTime.
	UIDatePickerMode datePickerMode;
	/// Set to TRUE to display the date picker as a keyboard input accessory view that appears from the bottom of the screen, instead of displaying it inside the cell. Default: FALSE.
	BOOL displayDatePickerAsInputAccessoryView;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
+ (instancetype)attributesWithDateFormatter:(NSDateFormatter *)formatter;

/// Allocates and returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
/// - Parameter mode: The date picker mode.
/// - Parameter displayDatePickerAsInputAccessoryView: Set to TRUE to display the date picker as a keyboard input accessory view that appears from the bottom of the screen, instead of displaying it inside the cell.
+ (instancetype)attributesWithDateFormatter:(NSDateFormatter *)formatter
				   datePickerMode:(UIDatePickerMode)mode
	displayDatePickerAsInputAccessoryView:(BOOL)displayDatePickerAsInputAccessoryView;

/// Returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
- (instancetype)initWithDateFormatter:(NSDateFormatter *)formatter;

/// Returns an initialized `SCDateAttributes`.
///
/// - Parameter formatter: The date formatter used to display the date of generated control. Set to nil to ignore.
/// - Parameter mode: The date picker mode.
/// - Parameter displayDatePickerAsInputAccessoryView: Set to TRUE to display the date picker as a keyboard input accessory view that appears from the bottom of the screen, instead of displaying it inside the cell.
- (instancetype)initWithDateFormatter:(NSDateFormatter *)formatter
			 datePickerMode:(UIDatePickerMode)mode
	displayDatePickerAsInputAccessoryView:(BOOL)displayDatePickerAsInputAccessoryView;

#pragma mark - Configuration

/// The date formatter used to display the date of generated control. Set to nil to ignore.
@property (nonatomic, strong) NSDateFormatter *dateFormatter;

/// The date picker mode. Default:UIDatePickerModeDateAndTime.
@property (nonatomic, readwrite) UIDatePickerMode datePickerMode;

/// Set to TRUE to display the date picker as a keyboard input accessory view that appears from the bottom of the screen, instead of displaying it inside the cell. Default: FALSE.
@property (nonatomic, readwrite) BOOL displayDatePickerAsInputAccessoryView;

@end



#pragma mark - SCSelectionAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeSelection, thus allowing further customization of the generated control by the user.
@interface SCSelectionAttributes : SCPropertyAttributes
{
	/// The data store for the selection items.
	SCDataStore *selectionItemsStore;
    /// The fetch options for the selection items.
    SCDataFetchOptions *selectionItemsFetchOptions;
    
    /// Determines if the generated selection control allows multiple selection.
    BOOL allowMultipleSelection;
	/// Determines if the generated selection control allows no selection.
	BOOL allowNoSelection;
	/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
	NSUInteger maximumSelections;
	/// Set to TRUE to automatically dismiss the selection detail view when an item is selected.
	BOOL autoDismissDetailView;
	/// Set to TRUE to hide the detail view's navigation bar.
	BOOL hideDetailViewNavigationBar;
    /// Determines if the generated control allows adding new items. Default: FALSE.
    BOOL allowAddingItems;
	/// Determines if the generated control allows deleting existing items. Default: FALSE.
	BOOL allowDeletingItems;
	/// Determines if the generated control allows moving items. Default: FALSE.
	BOOL allowMovingItems;
	/// Determines if the generated control allows editing items. Default: FALSE.
	BOOL allowEditingItems;
    /// The user interface element that is used to add new objects when selected. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
    NSObject *placeholderuiElement;
    /// The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
    NSObject *addNewObjectuiElement;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The items of the generated selection control. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithItems:(NSArray *)_items allowMultipleSelection:(BOOL)allowMultipleSel
		 allowNoSelection:(BOOL)allowNoSel;

/// Allocates and returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The items of the generated selection control. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
/// - Parameter autoDismiss: Set to TRUE to automatically dismiss the selection detail view when an item is selected.
/// - Parameter hideNavBar: Set to TRUE to hide the detail view's navigation bar.
/// - Note: `hideNavBar` only applies when `autoDismiss` is TRUE.
+ (instancetype)attributesWithItems:(NSArray *)_items allowMultipleSelection:(BOOL)allowMultipleSel
		 allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss
			hideDetailViewNavigationBar:(BOOL)hideNavBar;

/// Returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The items of the generated selection control. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithItems:(NSArray *)_items allowMultipleSelection:(BOOL)allowMultipleSel
		 allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized `SCSelectionAttributes`.
///
/// - Parameter _items: The items of the generated selection control. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
/// - Parameter autoDismiss: Set to TRUE to automatically dismiss the selection detail view when an item is selected.
/// - Parameter hideNavBar: Set to TRUE to hide the detail view's navigation bar.
/// - Note: `hideNavBar` only applies when `autoDismiss` is TRUE.
- (instancetype)initWithItems:(NSArray *)_items allowMultipleSelection:(BOOL)allowMultipleSel
		 allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss
			hideDetailViewNavigationBar:(BOOL)hideNavBar;


/// The data store for the selection items.
@property (nonatomic, strong) SCDataStore *selectionItemsStore;

/// The fetch options for the selection items.
@property (nonatomic, strong) SCDataFetchOptions *selectionItemsFetchOptions;

/// The items stored in selectionItemsStore.
@property (nonatomic, readonly) NSArray *items;

/// Determines if the generated selection control allows multiple selection.
@property (nonatomic, readwrite) BOOL allowMultipleSelection;

/// Determines if the generated selection control allows no selection.
@property (nonatomic, readwrite) BOOL allowNoSelection;

/// The maximum number of items that can be selected. Set to zero to allow an infinite number of selections. Default: 0.
/// - Note: Only applicable when allowMultipleSelection is TRUE.
@property (nonatomic, readwrite) NSUInteger maximumSelections;

/// Set to TRUE to automatically dismiss the selection detail view when an item is selected.
@property (nonatomic, readwrite) BOOL autoDismissDetailView;

/// Set to TRUE to hide the detail view's navigation bar.
/// - Note: This property only applies when `autoDismissDetailView` is TRUE.
@property (nonatomic, readwrite) BOOL hideDetailViewNavigationBar;

/// 	Determines if the generated control allows adding new items. Default: FALSE.
@property (nonatomic) BOOL allowAddingItems;

/// 	Determines if the generated control allows deleting existing items. Default: FALSE.
@property (nonatomic) BOOL allowDeletingItems;

/// 	Determines if the generated control allows moving items. Default: FALSE.
@property (nonatomic) BOOL allowMovingItems;

/// 	Determines if the generated control allows editing items. Default: FALSE.
@property (nonatomic) BOOL allowEditingItems;

/// The user interface element that is used to add new objects when selected. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
@property (nonatomic, strong) NSObject *placeholderuiElement;

/// The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
@property (nonatomic, strong) NSObject *addNewObjectuiElement;

@end







#pragma mark - SCObjectSelectionAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeObjectSelection, thus allowing further customization of the generated control by the user.
@interface SCObjectSelectionAttributes : SCSelectionAttributes

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCObjectSelectionAttributes`.
///
/// - Parameter objects: The array of objects that will be used to select from.
/// - Parameter definition: The data structure definition of the objects.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithSelectionObjects:(NSArray *)objects
                   objectsDefinition:(SCDataDefinition *)definition
              allowMultipleSelection:(BOOL)allowMultipleSel
                    allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized `SCObjectSelectionAttributes`.
///
/// - Parameter objects: The array of objects that will be used to select from.
/// - Parameter definition: The data structure definition of the objects.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithSelectionObjects:(NSArray *)objects
             objectsDefinition:(SCDataDefinition *)definition
        allowMultipleSelection:(BOOL)allowMultipleSel
              allowNoSelection:(BOOL)allowNoSel;

/// Allocates and returns an initialized SCObjectSelectionAttributes.
///
/// - Parameter entityDefinition: The entity definition of the entity whose objects are to be presented for selection.
/// - Parameter predicate: The predicate used to filter the selection objects. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Warning: allowMultipleSel should only be set to TRUE if relationship is many to many.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
+ (instancetype)attributesWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                             usingPredicate:(NSPredicate *)predicate
                     allowMultipleSelection:(BOOL)allowMultipleSel
                           allowNoSelection:(BOOL)allowNoSel;

/// Returns an initialized SCObjectSelectionAttributes.
///
/// - Parameter entityDefinition: The entity definition of the entity whose objects are to be presented for selection.
/// - Parameter predicate: The predicate used to filter the selection objects. Set to nil to ignore.
/// - Parameter allowMultipleSel: Determines if the generated selection control allows multiple selection.
/// - Warning: allowMultipleSel should only be set to TRUE if relationship is many to many.
/// - Parameter allowNoSel: Determines if the generated selection control allows no selection.
- (instancetype)initWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                       usingPredicate:(NSPredicate *)predicate
               allowMultipleSelection:(BOOL)allowMultipleSel
                     allowNoSelection:(BOOL)allowNoSel;

#pragma mark - Configuration

/// Set this to the definition of the intermediate entity between the property's definition and the objectsDefinition. This is useful in complex many-to-many relationships where you have created an intermediate entity between you main two entities. Default: nil.
@property (nonatomic, strong) SCDataDefinition *intermediateEntityDefinition;


//////////////////////////////////////////////////////////////////////////////////////////
/// Internal framework properties
//////////////////////////////////////////////////////////////////////////////////////////

@property (nonatomic, strong) NSString *objectsDefinitionibUniqueId;
/// The Interface Builder predicate string.
@property (nonatomic, strong) NSString *ibPredicateString;
/// The Interface Builder placeholder text.
@property (nonatomic, strong) NSString *ibPlaceholderText;
/// The Interface Builder placeholder-text alignment.
@property (nonatomic) NSTextAlignment ibPlaceholderTextAlignment;
/// The Interface Builder text for the add-new-object row.
@property (nonatomic, strong) NSString *ibAddNewObjectText;

@end





#pragma mark - SCObjectAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type
/// SCPropertyTypeObject, thus allowing further customization of the generated control by the user.
@interface SCObjectAttributes : SCPropertyAttributes
{
	/// The definition for the generated object control. Set to nil to ignore.
	SCDataDefinition *objectDefinition;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition;

/// Allocates and returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent;

/// Returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition;

/// Returns an initialized `SCObjectAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent;

#pragma mark - Configuration

/// 	The definition for the generated object control. Set to nil to ignore.
@property (nonatomic, strong) SCDataDefinition *objectDefinition;


//////////////////////////////////////////////////////////////////////////////////////////
/// Internal framework properties
//////////////////////////////////////////////////////////////////////////////////////////

@property (nonatomic, strong) NSString *objectDefinitionibUniqueId;

@end






#pragma mark - SCArrayOfObjectsAttributes
/// This class is used to extend the definition of an `SCPropertyAttributes` instance of type SCPropertyTypeArrayOfObjects, thus allowing further customization of the generated control by the user.
@interface SCArrayOfObjectsAttributes : SCPropertyAttributes
{
    /// The default data definition for the objects.
    SCDataDefinition *defaultObjectsDefinition;
    /// The fetch options for the objects.
    SCDataFetchOptions *objectsFetchOptions;
    
	/// Determines if the generated control allows adding new objects. Default: TRUE.
	BOOL allowAddingItems;
	/// Determines if the generated control allows deleting objects. Default: TRUE.
	BOOL allowDeletingItems;
	/// Determines if the generated control allows moving objects. Default: TRUE.
	BOOL allowMovingItems;
	/// Determines if the generated control allows editing objects. Default: TRUE.
	BOOL allowEditingItems;
    /// The user interface element that is used to add new objects when selected. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
    NSObject *placeholderuiElement;
    /// The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
    NSObject *addNewObjectuiElement;
    /// Determines if uiElement is displayed in `Normal Mode`. Default: TRUE.
    BOOL addNewObjectuiElementExistsInNormalMode;
    /// Whether the add-new-object UI element is available in editing mode.
    BOOL addNewObjectuiElementExistsInEdtingMode;
    
    SCSectionActions *_sectionActions;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition
						 allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting
						 allowMovingItems:(BOOL)allowMoving;



/// Allocates and returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The class definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
/// - Parameter placeholderUI: The uiElement that is displayed when no items are available. This object is typically of type SCTableViewCell.
/// - Parameter newObjectUI: The uiElement that is used to add new objects when tapped. This object is typically of type SCTableViewCell.
/// - Parameter existsInNormalMode: Determines if uiElement is displayed in `Normal Mode`.
/// - Parameter existsInEditingMode: Determines if uiElement is displayed in `Editing Mode`.
+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition
						 allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving 
               expandContentInCurrentView:(BOOL)expandContent
                     placeholderuiElement:(NSObject *)placeholderUI
                    addNewObjectuiElement:(NSObject *)newObjectUI
  addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode 
 addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode;

/// Returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition
				   allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting
				   allowMovingItems:(BOOL)allowMoving;

/// Returns an initialized `SCArrayOfObjectsAttributes`.
///
/// - Parameter definition: The definition for the generated object control. Set to nil to ignore.
/// - Parameter allowAdding: Determines if the generated control allows adding new objects.
/// - Parameter allowDeleting: Determines if the generated control allows deleting objects.
/// - Parameter allowMoving: Determines if the generated control allows moving objects.
/// - Parameter expandContent: When TRUE, the generated user interface element will expand its content in the current view, instead of generating its own detail view.
/// - Parameter placeholderUI: The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell.
/// - Parameter newObjectUI: The uiElement that is used to add new objects when tapped. This object is typically of type SCTableViewCell.
/// - Parameter existsInNormalMode: Determines if uiElement is displayed in `Normal Mode`.
/// - Parameter existsInEditingMode: Determines if uiElement is displayed in `Editing Mode`.
- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition
						 allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving 
               expandContentInCurrentView:(BOOL)expandContent
                     placeholderuiElement:(NSObject *)placeholderUI
                    addNewObjectuiElement:(NSObject *)newObjectUI
  addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode 
 addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode;


#pragma mark - Configuration

/// The default data definition for the objects.
@property (nonatomic, strong) SCDataDefinition *defaultObjectsDefinition;

/// The fetch options for the objects.
@property (nonatomic, strong) SCDataFetchOptions *objectsFetchOptions;

/// 	Determines if the generated control allows adding new objects. Default: TRUE.
@property (nonatomic) BOOL allowAddingItems;

/// 	Determines if the generated control allows deleting objects. Default: TRUE.
@property (nonatomic) BOOL allowDeletingItems;

/// 	Determines if the generated control allows moving objects. Default: TRUE.
@property (nonatomic) BOOL allowMovingItems;

/// 	Determines if the generated control allows editing objects. Default: TRUE.
@property (nonatomic) BOOL allowEditingItems;

/// The uiElement that is displayed when no items are available to display. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
@property (nonatomic, strong) NSObject *addNewObjectuiElement;

/// The user interface element that is used to add new objects when selected. This object is typically of type SCTableViewCell. Set to nil to ignore. Default:nil.
@property (nonatomic, strong) NSObject *placeholderuiElement;

/// Determines if uiElement is displayed in `Normal Mode`. Default: TRUE.
@property (nonatomic) BOOL addNewObjectuiElementExistsInNormalMode;

/// Determines if uiElement is displayed in `Editing Mode`. Default: TRUE.
@property (nonatomic) BOOL addNewObjectuiElementExistsInEditingMode;

/// The actions of the generated SCArrayOfObjectsSection.
@property (nonatomic, readonly) SCSectionActions *sectionActions;


//////////////////////////////////////////////////////////////////////////////////////////
/// Internal framework properties
//////////////////////////////////////////////////////////////////////////////////////////

@property (nonatomic, strong) NSString *defaultObjectsDefinitionibUniqueId;
/// The Interface Builder predicate string.
@property (nonatomic, strong) NSString *ibPredicateString;
/// The Interface Builder placeholder text.
@property (nonatomic, strong) NSString *ibPlaceholderText;
/// The Interface Builder placeholder-text alignment.
@property (nonatomic) NSTextAlignment ibPlaceholderTextAlignment;
/// The Interface Builder text for the add-new-object row.
@property (nonatomic, strong) NSString *ibAddNewObjectText;


@end



