/*
 *  SCDictionaryDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCDictionaryDefinition
/// This class functions as a means to further extend the key definitions of an NSMutableDictionary.
///
/// Using dictionary definitions, classes like SCObjectCell and SCObjectSection
/// will be able to better generate user interface elements that truly represent the
/// keys of their bound mutable dictionaries.
///
/// Sample use:
/// ```objective-c
/// // Define the dictionary using SCDictionaryDefinition
/// SCDictionaryDefinition *dictionaryDef = [SCDictionaryDefinition definitionWithDictionaryKeyNamesString:@"key1;key2;key3"];
/// [dictionaryDef propertyDefinitionWithName:@"key2"].type = SCPropertyTypeSwitch;
/// [dictionaryDef propertyDefinitionWithName:@"key3"].type = SCPropertyTypeSlider;
///
/// // Create some sample data
/// NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
/// [dictionary setValue:@"Text" forKey:@"key1"];
/// [dictionary setValue:[NSNumber numberWithBool:YES] forKey:@"key2"];
/// [dictionary setValue:[NSNumber numberWithFloat:0.7f] forKey:@"key3"];
///
/// // Generate sections for the dictionary
/// [self.tableViewModel generateSectionsForObject:dictionary withDefinition:dictionaryDef];
/// ```
///
/// - SeeAlso: ``SCPropertyDefinition``.

@interface SCDictionaryDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized `SCDictionaryDefinition` given the key names string of the mutable dictionary to be defined.
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
+ (instancetype)definitionWithDictionaryKeyNamesString:(nullable NSString *)keyNamesString;

/// Allocates and returns an initialized `SCDictionaryDefinition` given the key names of the mutable dictionary to be defined.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
+ (instancetype)definitionWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Allocates and returns an initialized `SCDictionaryDefinition` given the key names and titles of the mutable dictionary to be defined.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter keyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
+ (instancetype)definitionWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


/// Returns an initialized `SCDictionaryDefinition` given the key names string of the mutable dictionary to be defined.
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
- (instancetype)initWithDictionaryKeyNamesString:(nullable NSString *)keyNamesString;

/// Returns an initialized `SCDictionaryDefinition` given the key names of the mutable dictionary to be defined.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
- (instancetype)initWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Returns an initialized `SCDictionaryDefinition` given the key names and titles of the mutable dictionary to be defined.
///
/// Ordinary names generate SCPropertyTypeTextField definitions; names beginning
/// with `~` generate custom definitions. Types can be customized afterward.
///
/// - Parameter keyNames: Ordered, nonempty string names; nil generates nothing.
/// - Parameter keyTitles: Titles matched by index. Nil or missing entries use
///   generated titles; extra entries are ignored.
- (instancetype)initWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;

@end

NS_ASSUME_NONNULL_END
