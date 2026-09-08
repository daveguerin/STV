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
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNamesString: A string with the key names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Key names string syntax options:
///     @"key1;key2;key3;..."
///     @"group1 header:(key1, key2,...):group1 footer;group2..."
+ (instancetype)definitionWithDictionaryKeyNamesString:(nullable NSString *)keyNamesString;

/// Allocates and returns an initialized `SCDictionaryDefinition` given the key names of the mutable dictionary to be defined.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the dictionary key names. All array elements must be of type NSString.
+ (instancetype)definitionWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Allocates and returns an initialized `SCDictionaryDefinition` given the key names and titles of the mutable dictionary to be defined.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the dictionary key names. All array elements must be of type NSString.
/// - Parameter keyTitles: An array of titles to the keys in keyNames. All array elements must be of type NSString.
+ (instancetype)definitionWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


/// Returns an initialized `SCDictionaryDefinition` given the key names string of the mutable dictionary to be defined.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNamesString: A string with the key names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Key names string syntax options:
///     @"key1;key2;key3;..."
///     @"group1 header:(key1, key2,...):group1 footer;group2..."
- (instancetype)initWithDictionaryKeyNamesString:(nullable NSString *)keyNamesString;

/// Returns an initialized `SCDictionaryDefinition` given the key names of the mutable dictionary to be defined.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the dictionary key names. All array elements must be of type NSString.
- (instancetype)initWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Returns an initialized `SCDictionaryDefinition` given the key names and titles of the mutable dictionary to be defined.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the dictionary key names. All array elements must be of type NSString.
/// - Parameter keyTitles: An array of titles to the keys in keyNames. All array elements must be of type NSString.
- (instancetype)initWithDictionaryKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;

@end

NS_ASSUME_NONNULL_END
