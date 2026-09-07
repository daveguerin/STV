/*
 *  SCUserDefaultsDefinition.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDictionaryDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCUserDefaultsDefinition
/// This class functions as a means to define NSUserDefaults fields that the framework can automatically generate a user interface for. As with all other types of binding, once the UI is generated, the framework will also be responsible for automatically reading and writing the values to NSUserDefaults.
///
/// Sample use:
/// ```objective-c
/// // Create the user defaults definition
/// SCUserDefaultsDefinition *userDefaultsDef = [SCUserDefaultsDefinition definitionWithUserDefaultsKeyNamesString:
///     @"Login Details:(username,password):Will be automatically signed in"];
/// SCPropertyDefinition *passwordDef = [userDefaultsDef propertyDefinitionWithName:@"password"];
/// passwordDef.attributes = [SCTextFieldAttributes attributesWithPlaceholder:nil secureTextEntry:YES
///     autocorrectionType:UITextAutocorrectionTypeNo autocapitalizationType:UITextAutocapitalizationTypeNone];
///
/// // Generate the UI for the user defaults
/// [self.tableViewModel generateSectionsForUserDefaultsDefinition:userDefaultsDef];
/// ```
///
/// - SeeAlso: ``SCPropertyDefinition``.

@interface SCUserDefaultsDefinition : SCDictionaryDefinition

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names string of the user defaults.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNamesString: A string with the key names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Key names string syntax options:
///     @"key1;key2;key3;..."
///     @"group1 header:(key1, key2,...):group1 footer;group2..."
+ (instancetype)definitionWithUserDefaultsKeyNamesString:(nullable NSString *)keyNamesString;

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the user defaults key names. All array elements must be of type NSString.
+ (instancetype)definitionWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Allocates and returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults and their titles.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the user defaults key names. All array elements must be of type NSString.
/// - Parameter keyTitles: An array of titles to the keys in keyNames. All array elements must be of type NSString.
+ (instancetype)definitionWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


/// Returns an initialized `SCUserDefaultsDefinition` given the key names string of the user defaults.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNamesString: A string with the key names separated by semi-colons. Example string: @"firstName;lastName". Property groups can also be defined in the string using the following format: @"Personal Details:(firstName, lastName); Address:(street, state, country)". The group title can also be ommitted to create a group with no title. For example: @":(firstName, lastName)".
///
/// Key names string syntax options:
///     @"key1;key2;key3;..."
///     @"group1 header:(key1, key2,...):group1 footer;group2..."
- (instancetype)initWithUserDefaultsKeyNamesString:(nullable NSString *)keyNamesString;

/// Returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the user defaults key names. All array elements must be of type NSString.
- (instancetype)initWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames;

/// Returns an initialized `SCUserDefaultsDefinition` given the key names of the user defaults and their titles.
///
/// By default, all property definitions generated for the given keyNames will have a type of SCPropertyTypeTextField. This can be fully customized after initialization.
///
/// - Parameter keyNames: An array of the user defaults key names. All array elements must be of type NSString.
/// - Parameter keyTitles: An array of titles to the keys in keyNames. All array elements must be of type NSString.
- (instancetype)initWithUserDefaultsKeyNames:(nullable NSArray<NSString *> *)keyNames keyTitles:(nullable NSArray<NSString *> *)keyTitles;


@end

NS_ASSUME_NONNULL_END
