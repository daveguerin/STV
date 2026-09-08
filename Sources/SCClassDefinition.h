/*
 *  SCClassDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import <Foundation/Foundation.h>

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCClassDefinition
/// This class functions as a means to further extend the definition of user-defined classes.
/// Using class definitions, classes like SCObjectCell and SCObjectSection
/// will be able to better generate user interface elements that truly represent the
/// properties of their bound objects. For more information, please refer to SCDataDefinition.
///
/// Upon creation, SCClassDefinition will (optionally) automatically generate all the
/// property definitions declared directly on the class, not its superclasses.
/// Explicit lists may include inherited properties. The user can customize
/// the generated property definitions, add new definitions, or remove generated definitions.
///
/// Sample use:
/// ```objective-c
/// // Task declares name and notes strings, an NSNumber category index,
/// // an NSDate dueDate, and a Boolean completed property.
/// SCClassDefinition *taskDef = [SCClassDefinition definitionWithClass:[Task class]
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
/// - SeeAlso: ``SCDataDefinition``.
@interface SCClassDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCClassDefinition given a class and the option to auto generate property definitions for the given class.
///
/// The method will also generate user friendly property titles from the names of the generated properties. These titles can be modified by the user later as part of the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter autoGenerate: If YES, generates Objective-C runtime properties
///   declared directly on the class, not inherited properties.
+ (instancetype)definitionWithClass:(nullable Class)_cls autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Allocates and returns an initialized SCClassDefinition given a class and a string containing the property names.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
+ (instancetype)definitionWithClass:(nullable Class)_cls propertyNamesString:(nullable NSString *)propertyNamesString;

/// Allocates and returns an initialized SCClassDefinition given a class and an array of the property names to generate property definitions from.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
+ (instancetype)definitionWithClass:(nullable Class)_cls propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Allocates and returns an initialized SCClassDefinition given a class, an array of
/// the property names to generate property definitions from, and array of titles
/// for these properties.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter propertyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
+ (instancetype)definitionWithClass:(nullable Class)_cls propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Allocates and returns an initialized SCClassDefinition given a class and an SCPropertyGroupArray.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter groups: Groups whose names generate definitions. Group objects
///   are retained, not copied; nil generates no properties or groups.
+ (instancetype)definitionWithClass:(nullable Class)_cls propertyGroups:(nullable SCPropertyGroupArray *)groups;

/// Returns an initialized SCClassDefinition given a class and the option to auto generate property definitions for the given class.
///
/// The method will also generate user friendly property titles from the names of
/// the generated properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter autoGenerate: If YES, generates Objective-C runtime properties
///   declared directly on the class, not inherited properties.
- (instancetype)initWithClass:(nullable Class)_cls autoGeneratePropertyDefinitions:(BOOL)autoGenerate;

/// Returns an initialized SCClassDefinition given a class and a string containing the property names.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
- (instancetype)initWithClass:(nullable Class)_cls propertyNamesString:(nullable NSString *)propertyNamesString;

/// Returns an initialized SCClassDefinition given a class and an array of the property names to generate property definitions from.
///
/// The method will also generate user friendly property titles from the names of
/// the given properties. These titles can be modified by the user later as part of
/// the property definition customization.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
- (instancetype)initWithClass:(nullable Class)_cls propertyNames:(nullable NSArray<NSString *> *)propertyNames;

/// Returns an initialized SCClassDefinition given a class, an array of
/// the property names to generate property definitions from, and array of titles
/// for these properties.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter propertyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter propertyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
- (instancetype)initWithClass:(nullable Class)_cls propertyNames:(nullable NSArray<NSString *> *)propertyNames propertyTitles:(nullable NSArray<NSString *> *)propertyTitles;

/// Returns an initialized SCClassDefinition given a class and an SCPropertyGroupArray.
///
/// - Parameter _cls: The class to describe, or nil to leave the class unset.
///   Ordinary property definitions cannot be generated without a class.
/// - Parameter groups: Groups whose names generate definitions. Group objects
///   are retained, not copied; nil generates no properties or groups.
- (instancetype)initWithClass:(nullable Class)_cls propertyGroups:(nullable SCPropertyGroupArray *)groups;


#pragma mark - Configuration

/// The Objective-C class described by this definition, or nil when unset.
///
/// Changing it does not regenerate or revalidate existing definitions. Explicit
/// names must be runtime-visible properties or supported key paths; custom names
/// beginning with `~` bypass class-property validation.
@property (nonatomic, strong, nullable) Class cls;


@end

NS_ASSUME_NONNULL_END
