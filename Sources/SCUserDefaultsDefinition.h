/*
 *  SCUserDefaultsDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDictionaryDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCUserDefaultsDefinition
/// Describes standard user-defaults keys for generated property-editing UI.
///
/// The generated store reads and writes standardUserDefaults. Register defaults
/// and choose suitable property types for nonstring values before generating UI.
/// Preferences must be property-list values; NSUserDefaults is not secure storage.
/// Use Keychain rather than storing passwords or authentication tokens here.
///
/// ```objective-c
/// SCUserDefaultsDefinition *preferences = [SCUserDefaultsDefinition
///     definitionWithUserDefaultsKeyNamesString:
///         @"Display:(displayName,showHints)"];
/// [preferences propertyDefinitionWithName:@"showHints"].type = SCPropertyTypeSwitch;
/// [self.tableViewModel generateSectionsForUserDefaultsDefinition:preferences];
/// ```
///
/// - SeeAlso: ``SCPropertyDefinition``, ``SCUserDefaultsStore``
@interface SCUserDefaultsDefinition : SCDictionaryDefinition

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names string of the user defaults.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
+ (instancetype)definitionWithUserDefaultsKeyNamesString:(nullable NSString *)keyNamesString;

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
+ (instancetype)definitionWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults and their titles.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter keyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
+ (instancetype)definitionWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


/// Returns an initialized `SCUserDefaultsDefinition` given the key names string of the user defaults.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNamesString: Semicolon-separated names, or nil to generate
///   no properties. Groups use `@"Details:(firstName,lastName):Footer"`;
///   an empty header before the colon creates an untitled group. Names must
///   be nonempty. Delimiters are syntax and cannot be escaped.
///
/// Examples: `@"firstName;lastName"` and
/// `@"Details:(firstName,lastName):Footer;address"`.
- (instancetype)initWithUserDefaultsKeyNamesString:(nullable NSString *)keyNamesString;

/// Returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
- (instancetype)initWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults and their titles.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter keyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
- (instancetype)initWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


@end

NS_ASSUME_NONNULL_END
