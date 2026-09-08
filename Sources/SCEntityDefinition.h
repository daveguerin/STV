/*
 *  SCEntityDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
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
/// // TaskEntity has name and notes strings, an integer category index,
/// // a date dueDate, and a Boolean completed attribute. Use its context queue.
/// SCEntityDefinition *taskDef = [SCEntityDefinition definitionWithEntityName:@"TaskEntity"
///     managedObjectContext:context
///     propertyNamesString:@"Task Details:(name,notes,category,dueDate);Task Status:(completed)"];
/// SCPropertyDefinition *namePropertyDef = [taskDef propertyDefinitionWithName:@"name"];
/// namePropertyDef.required = TRUE;
/// SCPropertyDefinition *descPropertyDef = [taskDef propertyDefinitionWithName:@"notes"];
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
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter autoGenerate: If YES, generates the entity's modeled properties,
///   including relationships and fetched properties, not just attributes.
///
/// - Warning: Queries the application delegate's `managedObjectContext` selector.
///   It does not create a context or recover when none is available. Prefer
///   `definitionWithEntityName:managedObjectContext:autoGeneratePropertyDefinitions:` with an explicitly configured context.
+ (instancetype)definitionWithEntityName:(NSString *)entityName autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
///
/// - Warning: Queries the application delegate's `managedObjectContext` selector.
///   It does not create a context or recover when none is available. Prefer
///   `definitionWithEntityName:managedObjectContext:propertyNamesString:` with an explicitly configured context.
+ (instancetype)definitionWithEntityName:(NSString *)entityName propertyNamesString:(nullable NSString *)propertyNamesString;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter autoGenerate: If YES, generates the entity's modeled properties,
///   including relationships and fetched properties, not just attributes.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNamesString:(nullable NSString *)propertyNamesString;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and an array of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Allocates and returns an initialized `SCEntityDefinition` given a Core Data entity name, an array of
/// the property names to generate property definitions for, and array of titles
/// for these properties.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter propertyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
///   Custom names beginning with `~` use generated titles.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Allocates and returns an initialized SCEntityDefinition given a Core Data entity name and an SCPropertyGroupArray.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter groups: Groups whose names generate definitions. Group objects
///   are retained, not copied; nil generates no properties or groups.
+ (instancetype)definitionWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyGroups:(nullable SCPropertyGroupArray *)groups;


/// Returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter autoGenerate: If YES, generates the entity's modeled properties,
///   including relationships and fetched properties, not just attributes.
///
/// - Warning: Queries the application delegate's `managedObjectContext` selector.
///   It does not create a context or recover when none is available. Prefer
///   `initWithEntityName:managedObjectContext:autoGeneratePropertyDefinitions:` with an explicitly configured context.
- (instancetype)initWithEntityName:(NSString *)entityName autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
///
/// - Warning: Queries the application delegate's `managedObjectContext` selector.
///   It does not create a context or recover when none is available. Prefer
///   `initWithEntityName:managedObjectContext:propertyNamesString:` with an explicitly configured context.
- (instancetype)initWithEntityName:(NSString *)entityName propertyNamesString:(nullable NSString *)propertyNamesString;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and the option to auto generate property definitions for the given entity's properties.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter autoGenerate: If YES, generates the entity's modeled properties,
///   including relationships and fetched properties, not just attributes.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and a string of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNamesString:(nullable NSString *)propertyNamesString;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and an array of the property names to generate property definitions for.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Returns an initialized SCEntityDefinition given a Core Data entity name, an array of
/// the property names to generate property definitions for, and array of titles
/// for these properties.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter propertyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
///   Custom names beginning with `~` use generated titles.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Returns an initialized SCEntityDefinition given a Core Data entity name and an SCPropertyGroupArray.
///
/// - Parameter entityName: The modeled entity name. An unresolved entity may leave
///   entity nil; initialization does not provide an NSError failure result.
/// - Parameter context: A configured context whose model contains entityName.
///   Call on the context's queue; initialization performs no queue dispatch.
/// - Parameter groups: Groups whose names generate definitions. Group objects
///   are retained, not copied; nil generates no properties or groups.
- (instancetype)initWithEntityName:(NSString *)entityName managedObjectContext:(NSManagedObjectContext *)context propertyGroups:(nullable SCPropertyGroupArray *)groups;

#pragma mark - Configuration

/// The resolved entity, or nil when no entity was configured or found.
@property (nonatomic, readonly, strong, nullable) NSEntityDescription *entity;

/// The retained context used to resolve the entity and generate its store.
///
/// Nil for plain init. Entity-based initializers require a usable context.
/// Unbound Core Data stores fetch using each registered definition's context.
@property (nonatomic, readonly, strong, nullable) NSManagedObjectContext *managedObjectContext;

/// The attribute storing custom display order. Defaults to nil.
///
/// Compatible Core Data options use it instead of sortKey; setting it does not
/// change keyPropertyName. Invalid or nonnumeric names are ignored. Assigning
/// nil does not clear a previously accepted ordering attribute.
///
/// - Warning: Use an integer Core Data attribute. The setter accepts other numeric
///   types, but the store reads and writes ordering values as integers.
@property (nonatomic, copy, nullable) NSString *orderAttributeName;


@end

NS_ASSUME_NONNULL_END
