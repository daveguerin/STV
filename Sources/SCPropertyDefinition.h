/*
 *  SCPropertyDefinition.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright 2019 dgApps. All rights reserved.
 *
 */


#import "SCPropertyType.h"
#import "SCPropertyAttributes.h"
#import "SCCellActions.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCPropertyDefinition
/// This class functions as a property definition for SCDataDefinition.
///
/// Every property definition should set a property type that determines which user interface element
/// should be generated for the property. In addition, the generated user interface element
/// can be further customized using the property's attributes.
///
/// - SeeAlso: ``SCPropertyType``, ``SCPropertyAttributes``, ``SCClassDefinition``, ``SCEntityDefinition``, ``SCDictionaryDefinition``.
@interface SCPropertyDefinition : NSObject {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    SCPropertyType _type;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCPropertyDefinition` given a property name.
///
/// - Parameter propertyName: The name of the property.
+ (instancetype)definitionWithName:(NSString *)propertyName;

/// Allocates and returns an initialized `SCPropertyDefinition` given a property name, title, and type.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter propertyTitle: The title of the property. Property titles are used by user interface elements to generate labels associated with the generated controls.
/// - Parameter propertyType: The property type determines which user interface element will be generated for the property.
+ (instancetype)definitionWithName:(NSString *)propertyName title:(nullable NSString *)propertyTitle type:(SCPropertyType)propertyType;

/// Returns an initialized `SCPropertyDefinition` given a property name.
///
/// - Parameter propertyName: The name of the property.
- (instancetype)initWithName:(NSString *)propertyName;

/// Returns an initialized `SCPropertyDefinition` given a property name, title, and type.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter propertyTitle: The title of the property. Property titles are used by user interface elements to generate labels associated with the generated controls.
/// - Parameter propertyType: The property type determines which user interface element will be generated for the property.
- (instancetype)initWithName:(NSString *)propertyName title:(nullable NSString *)propertyTitle type:(SCPropertyType)propertyType;

#pragma mark - Configuration

/// The owner data structure definition of the property definition.
///
/// - Warning: This property gets set automatically by the framework, you should never set this property manually.
@property (nonatomic, weak, nullable) SCDataDefinition *ownerDataStuctureDefinition;

/// The name of the property associated with the property definition.
/// Nil when created with inherited `init` instead of `initWithName:`.
@property (nonatomic, readonly, nullable) NSString *name;

/// The title of the property. Property titles are used by user
/// interface elements to generate labels associated with the generated controls.
@property (nonatomic, copy, nullable) NSString *title;

/// The type of the property. Property types determines which user interface element will be generated for the property.
@property (nonatomic, readwrite) SCPropertyType type;

/// The attibutes set associated with the type of the property. Property attributes gives the user the ability
/// to further customize the user interface element that will be generated for the given property type.
@property (nonatomic, strong, nullable) SCPropertyAttributes *attributes;

/// The set of cell action blocks.
/// Initialized by `initWithName:`; nil when using inherited `init` directly.
@property (nonatomic, readonly, nullable) SCCellActions *cellActions;

/// The type of the property while in editing mode. Property types determines which user interface element will
/// be generated for the property while in editing mode.
/// - Note: If not set (or set to SCPropertyTypeUndefined), the value set in the "type" property is used in editing mode.
@property (nonatomic, readwrite) SCPropertyType editingModeType;

/// The attibutes set associated with the editingModetype of the property. Property attributes gives the user the ability to further customize the user interface element that will be generated for the given property editingModetype.
@property (nonatomic, strong, nullable) SCPropertyAttributes *editingModeAttributes;

/// Set to TRUE if property is a required property. Default: FALSE.
@property (nonatomic, readwrite) BOOL required;

/// Set to TRUE if the property value should be automatically validated by the user interface element
/// before commiting it to the property. If the user chooses to provide custom validation
/// using either the cell's actions, or the model's SCTableViewModelDelegate, they should
/// set this property to FALSE. Default: TRUE.
@property (nonatomic, readwrite) BOOL autoValidate;

/// If set to TRUE, a user interface is generated for the property when the owning control is in Normal Mode, otherwise, no UI is generated in that mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL existsInNormalMode;

/// If set to TRUE, a user interface is generated for the property when the owning control is in Editing Mode, otherwise, no UI is generated in that mode. Default: TRUE.
@property (nonatomic, readwrite) BOOL existsInEditingMode;

/// If set to TRUE, a user interface is generated for the property when the UI is generated for a newly created object, otherwise, no UI is generated. Default: TRUE.
@property (nonatomic, readwrite) BOOL existsInCreationMode;

/// If set to TRUE, a user interface is generated for the property when the UI is generated for an existing object, otherwise, no UI is generated. Default: TRUE.
@property (nonatomic, readwrite) BOOL existsInDetailMode;

#pragma mark - Custom UI

/// The class of the custom user interface element the property will generate. Set this property to override the standard user interface element generated by the property's `type`.
///
/// - Note: No need to set if `uiElementNibName` contains a valid value.
@property (nonatomic, strong, nullable) Class uiElementClass;

/// The Nib name of the custom user interface element the property will generate. Set this property to override the standard user interface element generated by the property's `type`.
///
/// - Note: Do not include the `.xib` extension when providing the nib name.
/// - Note: If the NIB is a SwiftPM resource then the name should use a PACKAGE_TARGET_Name format.
@property (nonatomic, copy, nullable) NSString *uiElementNibName;

/// This dictionary specifies how each of the uiElementClass or uiElementNibName custom controls binds itself to the definition's properties. Each dictionary key should be the tag string value of one of the uiElement's custom controls, and the value should be the name of the definition's property that is bound to that control.
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: This property can be used interchangibly with objectBindingsString
@property (nonatomic, strong, nullable) NSDictionary<NSString *, NSString *> *objectBindings;

/// This string specifies how each of the uiElementClass or uiElementNibName custom controls binds itself to the definition's properties. The string should consist of a colon separated pair of control's tag and the property name, while each pair should be separated by a semi-colon.
///
/// Example bindings string:
///     NSString *bindingsString = @"1:firstName;2:lastName";
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: This property can be used interchangibly with objectBindings
@property (nonatomic, copy, nullable) NSString *objectBindingsString;

#pragma mark - Internal

/// This property is automatically set by the framework and should never be assigned a value directly.
@property (nonatomic, readwrite) SCDataType dataType;

/// Returns TRUE if the property's dataType is scalar.
@property (nonatomic, readonly) BOOL dataTypeScalar;

/// This property is automatically set by the framework and should never be assigned a value directly.
@property (nonatomic, readwrite) BOOL dataReadOnly;

/// Applies Interface Builder configuration values from a dictionary.
/// - Parameter ibDictionary: The Interface Builder configuration dictionary.
- (void)setAllPropertiesFromibDictionary:(nullable NSDictionary<NSString *, id> *)ibDictionary;

@end


#pragma mark - SCCustomPropertyDefinition
/// This class functions as a property definition for SCDataDefinition that will generate
/// a custom user inteface element (e.g.: custom cell).
///
/// `SCCustomPropertyDefinition` does not have to represent a property that actually exists in its class (unlike an SCPropertyDefiniton), and is often used in a structure definition as a placeholder for custom user interface element generation.
///
/// - SeeAlso: ``SCPropertyDefinition``.
@interface SCCustomPropertyDefinition : SCPropertyDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCCustomPropertyDefinition` given a property name and a class of the  custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementClass: The class of the custom user interface element that will be generated.
/// - Parameter bindings: This dictionary specifies how each of the uiElement's custom controls binds itself to the data definition's properties. Each dictionary key should be the tag string value of one of the uiElement's custom controls, and the value should be the name of the class definition's property that is bound to that control.
/// - Warning: IMPORTANT: All control tags must be greater than zero.
+ (instancetype)definitionWithName:(NSString *)propertyName uiElementClass:(nullable Class)elementClass objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings;

/// Allocates and returns an initialized `SCCustomPropertyDefinition` given a property name and a class of the  custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementClass: The class of the custom user interface element that will be generated.
/// - Parameter bindingsString: This string specifies how each of the uiElement's custom controls binds itself to the data definition's properties. The string should consist of a colon separated pair of control's tag and the property name, while each pair should be separated by a semi-colon.
///
///  Example bindings string:
///         NSString *bindingsString = @"1:firstName;2:lastName";
/// - Warning: IMPORTANT: All control tags must be greater than zero.
+ (instancetype)definitionWithName:(NSString *)propertyName uiElementClass:(nullable Class)elementClass objectBindingsString:(nullable NSString *)bindingsString;

/// Allocates and returns an initialized `SCCustomPropertyDefinition` given a property name and the name of the nib file containing the custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementNibName: The name of the nib file containing the custom user interface element that will be generated.
/// - Parameter bindings: This dictionary specifies how each of the uiElement's custom controls binds itself to the data definition's properties. Each dictionary key should be the tag string value of one of the uiElement's custom controls, and the value should be the name of the class definition's property that is bound to that control.
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: If the NIB is a SwiftPM resource then the name should use a PACKAGE_TARGET_Name format.
+ (instancetype)definitionWithName:(NSString *)propertyName uiElementNibName:(nullable NSString *)elementNibName objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings;

/// Allocates and returns an initialized `SCCustomPropertyDefinition` given a property name and the name of the nib file containing the custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementNibName: The name of the nib file containing the custom user interface element that will be generated.
/// - Parameter bindingsString: This string specifies how each of the uiElement's custom controls binds itself to the data definition's properties. The string should consist of a colon separated pair of control's tag and the property name, while each pair should be separated by a semi-colon.
///
///  Example bindings string:
///         NSString *bindingsString = @"1:firstName;2:lastName";
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: If the NIB is a SwiftPM resource then the name should use a PACKAGE_TARGET_Name format.
+ (instancetype)definitionWithName:(NSString *)propertyName uiElementNibName:(nullable NSString *)elementNibName objectBindingsString:(nullable NSString *)bindingsString;

/// Returns an initialized `SCCustomPropertyDefinition` given a property name and a class of the  custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementClass: The class of the custom user interface element that will be generated.
/// - Parameter bindings: This dictionary specifies how each of the uiElement's custom controls binds itself to the data definition's properties. Each dictionary key should be the tag string value of one of the uiElement's custom controls, and the value should be the name of the class definition's property that is bound to that control.
/// - Warning: IMPORTANT: All control tags must be greater than zero.
- (instancetype)initWithName:(NSString *)propertyName uiElementClass:(nullable Class)elementClass objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings;

/// Returns an initialized `SCCustomPropertyDefinition` given a property name and a class of the  custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementClass: The class of the custom user interface element that will be generated.
/// - Parameter bindingsString: This string specifies how each of the uiElement's custom controls binds itself to the data definition's properties. The string should consist of a colon separated pair of control's tag and the property name, while each pair should be separated by a semi-colon.
///
///  Example bindings string:
///         NSString *bindingsString = @"1:firstName;2:lastName";
/// - Warning: IMPORTANT: All control tags must be greater than zero.
- (instancetype)initWithName:(NSString *)propertyName uiElementClass:(nullable Class)elementClass objectBindingsString:(nullable NSString *)bindingsString;

/// Returns an initialized `SCCustomPropertyDefinition` given a property name and the name of the nib file containing the custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementNibName: The name of the nib file containing the custom user interface element that will be generated.
/// - Parameter bindings: This dictionary specifies how each of the uiElement's custom controls binds itself to the class definition's properties. Each dictionary key should be the tag string value of one of the uiElement's custom controls, and the value should be the name of the class definition's property that is bound to that control.
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: If the NIB is a SwiftPM resource then the name should use a PACKAGE_TARGET_Name format.
- (instancetype)initWithName:(NSString *)propertyName uiElementNibName:(nullable NSString *)elementNibName objectBindings:(nullable NSDictionary<NSString *, NSString *> *)bindings;

/// Allocates and returns an initialized `SCCustomPropertyDefinition` given a property name and a class of the  custom user interface element to generate.
///
/// - Parameter propertyName: The name of the property.
/// - Parameter elementNibName: The name of the nib file containing the custom user interface element that will be generated.
/// - Parameter bindingsString: This string specifies how each of the uiElement's custom controls binds itself to the data definition's properties. The string should consist of a colon separated pair of control's tag and the property name, while each pair should be separated by a semi-colon.
///
///  Example bindings string:
///         NSString *bindingsString = @"1:firstName;2:lastName";
/// - Warning: IMPORTANT: All control tags must be greater than zero.
/// - Note: If the NIB is a SwiftPM resource then the name should use a PACKAGE_TARGET_Name format.
- (instancetype)initWithName:(NSString *)propertyName uiElementNibName:(nullable NSString *)elementNibName objectBindingsString:(nullable NSString *)bindingsString;


@end


#pragma mark - SCPropertyGroup
/// This class functions as a way to define a property definition group that is used to generate
/// a corresponding user interface element that groups these properties (e.g.: a table view section).
///
/// Property definitions are added to this class using their NSString `name` property.
///
/// - SeeAlso: ``SCPropertyDefinition``, ``SCClassDefinition``.
@interface SCPropertyGroup : NSObject


#pragma mark - Creation and Initialization


/// Allocates and returns an initialized `SCPropertyGroup` given a header title, a footer title
///  and an NSArray of property names to be included in the group.
///
/// - Parameter groupHeaderTitle: A header title for the group.
/// - Parameter groupFooterTitle: A footer title for the group.
/// - Parameter propertyNames: The property names to include in the group.
+ (instancetype)groupWithHeaderTitle:(nullable NSString *)groupHeaderTitle footerTitle:(nullable NSString *)groupFooterTitle propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Returns an initialized `SCPropertyGroup` given a header title, a footer title
///  and an NSArray of property names to be included in the group.
///
/// - Parameter groupHeaderTitle: A header title for the group.
/// - Parameter groupFooterTitle: A footer title for the group.
/// - Parameter propertyNames: The property names to include in the group.
- (instancetype)initWithHeaderTitle:(nullable NSString *)groupHeaderTitle footerTitle:(nullable NSString *)groupFooterTitle propertyNames:(nullable NSArray<NSString *> *)propertyNames;


#pragma mark - Configuration

/// The group header title.
@property (nonatomic, copy, nullable) NSString *headerTitle;

/// The group footer title.
@property (nonatomic, copy, nullable) NSString *footerTitle;


#pragma mark - Managing Property Names

/// The count of the property names.
@property (nonatomic, readonly) NSInteger propertyNameCount;

/// 	Add the property name to the group.
/// - Parameter propertyName: The property name.
- (void)addPropertyName:(NSString *)propertyName;

/// 	Add the property name to the group at the given index.
/// - Parameter propertyName: The property name.
/// - Parameter index: The index involved in the operation.
- (void)insertPropertyName:(NSString *)propertyName atIndex:(NSInteger)index;

/// 	Return the property name at the given index.
/// - Parameter index: The index involved in the operation.
- (NSString *)propertyNameAtIndex:(NSInteger)index;

/// 	Remove the property name at the given index from the group.
/// - Parameter index: The index involved in the operation.
- (void)removePropertyNameAtIndex:(NSInteger)index;

/// 	Remove all property names from the group.
- (void)removeAllPropertyNames;

/// 	Returns TRUE if the property name exists in the group.
/// - Parameter propertyName: The property name.
- (BOOL)containsPropertyName:(NSString *)propertyName;

@end


#pragma mark - SCPropertyGroupArray
/// This class functions as an array of SCPropertyGroup objects.
///
/// - SeeAlso: ``SCPropertyGroup``.
@interface SCPropertyGroupArray : NSObject


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCPropertyGroupArray`.
+ (instancetype)groupArray;

#pragma mark - Managing Groups

/// The number of property groups in the collection.
@property (nonatomic, readonly) NSInteger groupCount;

/// 	Add a group to the array.
/// - Parameter group: The property group.
- (void)addGroup:(SCPropertyGroup *)group;

/// 	Add a group at the given index.
/// - Parameter group: The property group.
/// - Parameter index: The index involved in the operation.
- (void)insertGroup:(SCPropertyGroup *)group atIndex:(NSInteger)index;

/// 	Return group at the given index.
/// - Parameter index: The index involved in the operation.
- (SCPropertyGroup *)groupAtIndex:(NSInteger)index;

/// 	Return group with the given header title.
/// - Parameter headerTitle: The section header title.
/// - Returns: The matching group, or nil if no header title matches.
- (nullable SCPropertyGroup *)groupByHeaderTitle:(nullable NSString *)headerTitle;

/// 	Remove group at the given index.
/// - Parameter index: The index involved in the operation.
- (void)removeGroupAtIndex:(NSInteger)index;

/// 	Remove all groups.
- (void)removeAllGroups;

@end

NS_ASSUME_NONNULL_END
