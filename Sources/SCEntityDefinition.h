/*
 *  SCEntityDefinition.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <CoreData/CoreData.h>


#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCEntityDefinition
/// This class functions as a means to further extend the definition of user-defined Core Data entities.
/// Using entity definitions, classes like SCObjectCell and SCObjectSection
/// will be able to better generate user interface elements that truly represent the
/// properties of their bound objects.
///
/// SCEntityDefinition mainly consists of one or more property definitions of type SCPropertyDefinition.
/// Upon creation, SCEntityDefinition will (optionally) automatically generate all the
/// property definitions for the given entity. From there, the user will be able to customize
/// the generated property definitions, add new definitions, or remove generated definitions.
///
/// Sample use:
/// ```objective-c
/// // Extend the definition of `TaskEntity` (user defined Core Data entity)
/// SCEntityDefinition *taskDef = [SCEntityDefinition definitionWithEntityName:@"TaskEntity"
///     managedObjectContext:context
///     propertyNamesString:@"Task Details:(name,description,category,dueDate);Task Status:(completed)"];
/// SCPropertyDefinition *namePropertyDef = [taskDef propertyDefinitionWithName:@"name"];
/// namePropertyDef.required = TRUE;
/// SCPropertyDefinition *descPropertyDef = [taskDef propertyDefinitionWithName:@"description"];
/// descPropertyDef.type = SCPropertyTypeTextView;
/// SCPropertyDefinition *categoryPropertyDef = [taskDef propertyDefinitionWithName:@"category"];
/// categoryPropertyDef.type = SCPropertyTypeSelection;
/// NSArray<NSString *> *categoryItems = [NSArray arrayWithObjects:@"Home", @"Work", @"Other", nil];
/// categoryPropertyDef.attributes = [SCSelectionAttributes attributesWithItems:categoryItems
///     allowMultipleSelection:NO allowNoSelection:NO];
/// ```
///
/// - SeeAlso: ``SCPropertyDefinition``.
@interface SCEntityDefinition : SCDataDefinition


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter autoGenerate: If TRUE, `SCClassDefinition` will automatically generate all the property definitions for the given entity's attributes.
///
/// - Warning: Note: This method attempts to automatically determine the required NSManagedObjectContext by querying the current app delegate. If your app delegate does not provide an NSManagedObjectContext, or if you'd like to provide a diffrerent one, please use definitionWithEntityName:managedObjectContext:autoGeneratePropertyDefinitions: instead.
+ (instancetype)definitionWithEntityName:(NSString *)entityName autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter propertyNamesString: A string with the property names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Property names string syntax options:
///     @"property1;property2;property3;..."
///     @"group1 header:(property1, property2,...):group1 footer;group2..."
///
/// - Warning: Note: This method attempts to automatically determine the required NSManagedObjectContext by querying the current app delegate. If your app delegate does not provide an NSManagedObjectContext, or if you'd like to provide a diffrerent one, please use definitionWithEntityName:managedObjectContext:propertyNamesString: instead.
+ (instancetype)definitionWithEntityName:(NSString *)entityName propertyNamesString:(nullable NSString *)propertyNamesString;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter autoGenerate: If TRUE, `SCClassDefinition` will automatically generate all the property definitions for the given entity's attributes.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNamesString: A string with the property names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Property names string syntax options:
///     @"property1;property2;property3;..."
///     @"group1 header:(property1, property2,...):group1 footer;group2..."
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNamesString:(nullable NSString *)propertyNamesString;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and an array of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNames: An array of the names of properties to be generated. All array elements must be of type NSString.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Allocates and returns an initialized `SCEntityDefinition` given a Core Data entity name, an array of
/// the property names to generate property definitions for, and array of titles
/// for these properties.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNames: An array of the names of properties to be generated. All array elements must be of type NSString.
/// - Parameter propertyTitles: An array of titles to the properties in propertyNames. All array elements must be of type NSString.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and an SCPropertyGroupArray.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter groups: A collection of property groups.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyGroups:(nullable SCPropertyGroupArray *)groups;


/// Returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter autoGenerate: If TRUE, SCEntityDefinition will automatically generate all the property definitions for the given entity's attributes.
///
/// - Warning: Note: This method attempts to automatically determine the required NSManagedObjectContext by querying the current app delegate. If your app delegate does not provide an NSManagedObjectContext, or if you'd like to provide a diffrerent one, please use initWithEntityName:managedObjectContext:autoGeneratePropertyDefinitions: instead.
- (instancetype)initWithEntityName:(NSString *)entityName autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter propertyNamesString: A string with the property names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Property names string syntax options:
///     @"property1;property2;property3;..."
///     @"group1 header:(property1, property2,...):group1 footer;group2..."
///
/// - Warning: Note: This method attempts to automatically determine the required NSManagedObjectContext by querying the current app delegate. If your app delegate does not provide an NSManagedObjectContext, or if you'd like to provide a diffrerent one, please use initWithEntityName:managedObjectContext:propertyNamesString: instead.
- (instancetype)initWithEntityName:(NSString *)entityName propertyNamesString:(nullable NSString *)propertyNamesString;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter autoGenerate: If TRUE, SCEntityDefinition will automatically generate all the property definitions for the given entity's attributes.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context  autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNamesString: A string with the property names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Property names string syntax options:
///     @"property1;property2;property3;..."
///     @"group1 header:(property1, property2,...):group1 footer;group2..."
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNamesString:(nullable NSString *)propertyNamesString;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and an array of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNames: An array of the names of properties to be generated. All array elements must be of type NSString.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Returns an initialized SCEntityDefinition given a Core Data entity name, an array of
/// the property names to generate property definitions for, and array of titles
/// for these properties.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter propertyNames: An array of the names of properties to be generated. All array elements must be of type NSString.
/// - Parameter propertyTitles: An array of titles to the properties in propertyNames. All array elements must be of type NSString.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and an SCPropertyGroupArray.
///
/// - Parameter entityName: The name of the entity for which the definition will be extended.
/// - Parameter context: The managed object context of the entity.
/// - Parameter groups: A collection of property groups.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyGroups:(nullable SCPropertyGroupArray *)groups;

#pragma mark - Configuration

/// The entity associated with the definition.
@property (nonatomic, readonly, strong, nullable) NSEntityDescription *entity;

/// The managed object context of the entity associated with the definition.
@property (nonatomic, readonly, strong, nullable) NSManagedObjectContext *managedObjectContext;

/// 	The name of the entity attribute that will be used to store the display order of its objects.
///
/// Setting this property to a valid attribute name allows for custom re-ordering of the generated
/// user interface elements representing the Core Data objects (e.g. custom re-ordering of cells).
/// Setting this property overrides the value set for the keyPropertyName property.
/// - Warning: Important: This Core Data attribute must be of integer type.
@property (nonatomic, copy, nullable) NSString *orderAttributeName;


@end

NS_ASSUME_NONNULL_END
