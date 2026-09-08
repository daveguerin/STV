/*
 *  SCDataDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <Foundation/Foundation.h>

#import "SCPropertyDefinition.h"
#import "SCPluginUtilities.h"

@class SCDataStore;


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCDataDefinition
/// Describes properties and grouping for automatically generated object UI.
///
/// Definitions hold an ordered collection of SCPropertyDefinition objects and
/// optional property groups. Before generating sections, the framework rebuilds
/// defaultPropertyGroup with properties not assigned to an explicit group.
/// A newly initialized definition has no properties or explicit groups.
///
/// - SeeAlso: ``SCClassDefinition``, ``SCDictionaryDefinition``, ``SCEntityDefinition``
/// - Warning: Use a concrete subclass. The base class supplies shared configuration
///   but does not inspect a data structure or generate a usable data store.
@interface SCDataDefinition : NSObject <SCibInitialization> {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    NSMutableArray<SCPropertyDefinition *> *propertyDefinitions;
}

#pragma mark - Configuration

/// Whether generated property cells require editing mode to be enabled.
///
/// Defaults to NO. When YES, object sections disable generated cells outside
/// editing mode except expand/collapse cells. Generated object detail flows
/// request an Edit button for existing items; merely generating sections in a
/// custom controller does not install a navigation button.
@property (nonatomic, readwrite) BOOL requireEditingModeToEditPropertyValues;

/// The property used as the sort key in generated fetch options.
///
/// Initially nil. Default setup selects the first property accepted by
/// isValidPropertyName:. Invalid assignments are ignored. An accepted change
/// also changes titlePropertyName if the title is empty or matches the old key.
/// Changing the key does not update existing fetch options.
@property (nonatomic, copy, nullable) NSString *keyPropertyName;

/// The property name or semicolon-separated names used to form object titles.
///
/// Initially nil. Default setup sets it to keyPropertyName after choosing the
/// first valid property. Multiple values use titlePropertyNameDelimiter.
/// Basic-value definitions override title lookup to use the value itself.
@property (nonatomic, copy, nullable) NSString *titlePropertyName;

/// The delimiter for multiple title values. Defaults to a single space.
///
/// Nil concatenates values without a delimiter. Missing values are skipped, but
/// a later value can receive a leading delimiter based on its source index.
@property (nonatomic, copy, nullable) NSString *titlePropertyNameDelimiter;

/// The property name or semicolon-separated names used for description text.
///
/// Defaults to nil. Multiple values are separated by a space. Basic string,
/// number, and date definitions return nil for descriptions regardless of this
/// setting.
@property (nonatomic, copy, nullable) NSString *descriptionPropertyName;

/// Default cell actions merged into generated property cells.
///
/// The action set is created empty and is mutable. During configuration, property
/// and existing cell actions take precedence; these defaults fill only unset
/// actions. Changing the defaults does not automatically reconfigure existing cells.
@property (nonatomic, readonly) SCCellActions *cellActions;

#pragma mark - Managing Property Definitions

/// The number of property definitions.
@property (nonatomic, readonly) NSUInteger propertyDefinitionCount;

/// Appends a property definition with the supplied display configuration.
///
/// - Parameter propertyName: A nonempty name. Nil or empty names return NO. Names
///   beginning with `~` create SCCustomPropertyDefinition instances.
/// - Parameter propertyTitle: The title, or nil to leave it unset.
/// - Parameter propertyType: The requested type; ignored for custom names.
/// - Returns: YES if insertion succeeds. Subclasses can reject invalid properties.
- (BOOL)addPropertyDefinitionWithName:(nullable NSString *)propertyName title:(nullable NSString *)propertyTitle type:(SCPropertyType)propertyType;

/// Appends a property definition and assigns this definition as its owner.
///
/// - Parameter propertyDefinition: The definition to retain, not copy.
/// - Returns: YES in the base implementation. Subclasses may reject a definition,
///   for example when a noncustom class property does not exist.
- (BOOL)addPropertyDefinition:(SCPropertyDefinition *)propertyDefinition;

/// Inserts a property definition and assigns this definition as its owner.
///
/// - Parameter propertyDefinition: The definition to retain, not copy.
/// - Parameter index: Zero through propertyDefinitionCount, inclusive.
/// - Returns: YES in the base implementation; subclasses may reject a definition.
/// - Warning: An invalid index raises an exception when insertion is attempted.
- (BOOL)insertPropertyDefinition:(SCPropertyDefinition *)propertyDefinition atIndex:(NSInteger)index;

/// Removes the property definition at the given index.
/// - Parameter index: Must be less than the total number of property definitions.
- (void)removePropertyDefinitionAtIndex:(NSUInteger)index;

/// Removes the first property with the given name; no match is a no-op.
/// - Parameter propertyName: The name to remove, or nil for no match.
- (void)removePropertyDefinitionWithName:(nullable NSString *)propertyName;

/// Returns the property definition at the given index.
/// - Parameter index: Must be less than the total number of property definitions.
/// - Warning: An out-of-range index raises an exception; it does not return nil.
- (SCPropertyDefinition *)propertyDefinitionAtIndex:(NSUInteger)index;

/// Returns the property definition with the given name.
/// - Parameter propertyName: The name to find, or nil for no match.
/// - Returns: The first matching definition, or nil when no property matches.
- (nullable SCPropertyDefinition *)propertyDefinitionWithName:(nullable NSString *)propertyName;

/// Returns the index for the property definition with the given name.
/// - Parameter propertyName: The name to find, or nil for no match.
/// - Returns: The first matching index, or NSNotFound.
- (NSUInteger)indexOfPropertyDefinitionWithName:(nullable NSString *)propertyName;

#pragma mark - Managing Property Definition Groups

/// The group used for properties not assigned to an explicit property group.
///
/// Created empty. generateDefaultPropertyGroupProperties rebuilds its names in
/// definition order before generated sections are added. A nonempty default group
/// is rendered ahead of explicit groups. Customize its header and footer, not
/// its names: automatic rebuilding replaces any manually assigned names.
@property (nonatomic, readonly) SCPropertyGroup *defaultPropertyGroup;

/// The mutable collection of explicit groups, initially empty.
///
/// Groups render in collection order after any nonempty default group. The
/// separately managed defaultPropertyGroup is not an entry in this collection.
@property (nonatomic, readonly) SCPropertyGroupArray *propertyGroups;

#pragma mark - Internal properties & methods (should only be used by the framework or when subclassing)


/// The unique identifier supplied by Interface Builder.
@property (nonatomic, readonly, nullable) NSString *ibUniqueID;
/// The name supplied by Interface Builder.
@property (nonatomic, readonly, nullable) NSString *ibName;

/// The structure name used as the registration key in compatible data stores.
///
/// The abstract base returns nil. Class and basic-value definitions normalize
/// class names; entity definitions use the entity name. Definitions sharing a
/// name compete for the same store registration.
@property (nonatomic, readonly, nullable) NSString *dataStructureName;

/// Initializes the definition and applies Interface Builder configuration.
///
/// Subclasses may extract structure-specific settings before applying the rest.
/// - Parameter ibDictionary: Mutable configuration; consumed keys may be removed.
- (instancetype)initWithibDictionary:(NSMutableDictionary<NSString *, id> *)ibDictionary;

/// Appends configured groups and properties, then applies remaining values by KVC.
///
/// Also sets the key to the first property if no key is configured.
/// - Parameter ibDictionary: Mutable configuration. The groups entry is removed
///   after processing; unrecognized remaining keys may raise KVC errors.
- (void)setAllPropertiesFromibDictionary:(NSMutableDictionary<NSString *, id> *)ibDictionary;

/// Resolves definition references in object and collection property attributes.
///
/// Also converts configured predicate strings into selection and collection fetch
/// predicates. Invalid predicate syntax can raise an exception.
/// - Parameter dictionary: Definitions indexed by their Interface Builder IDs.
- (void)resolveibRelationshipsUsingDictionary:(NSDictionary<NSString *, id> *)dictionary;

/// Selects the first valid property as key, then sets the title to that key.
///
/// Existing customization can be overwritten. With no valid property the current
/// key is retained and still copied to titlePropertyName.
- (void)setupDefaultConfiguration;

/// Determines the storage type of a named property.
///
/// - Parameter propertyName: The property to inspect.
/// - Returns: SCDataTypeUnknown in the base implementation; subclasses inspect
///   their underlying data structure.
- (SCDataType)propertyDataTypeForPropertyWithName:(NSString *)propertyName;

/// Asks whether a property name is accepted by this definition.
///
/// The base implementation and dictionary definitions always return YES, even for
/// nil. Class and entity definitions inspect their underlying structures; this
/// method is not a general-purpose nil or configuration validator.
/// - Parameter propertyName: The candidate name or supported key path.
- (BOOL)isValidPropertyName:(nullable NSString *)propertyName;

/// Formats an object's title using the configured title names and delimiter.
///
/// - Parameter object: An object described by this definition.
/// - Returns: The title, or nil for an unset or unavailable single value. Missing
///   entries in multiple values are skipped. Basic-value subclasses use the value
///   itself and may apply a configured formatter.
- (nullable NSString *)titleValueForObject:(NSObject *)object;

/// Finds the first object with an exactly matching formatted title.
///
/// - Parameter title: The title to match, or nil for no match.
/// - Parameter objectsArray: Objects described by this definition; nil has no matches.
/// - Returns: The first matching object, or nil. Matching is case-sensitive.
- (nullable NSObject *)objectWithTitle:(nullable NSString *)title inObjectsArray:(nullable NSArray<id> *)objectsArray;

/// Formats the configured description values, separated by a space.
///
/// - Parameter object: An object described by this definition.
/// - Returns: The description, or nil for an unset or unavailable single value.
///   Basic string, number, and date definitions always return nil.
- (nullable NSString *)descriptionValueForObject:(NSObject *)object;

/// Subclass hook to append definitions from parallel name and title arrays.
///
/// The base implementation does nothing.
/// - Parameter propertyNamesArray: Ordered names; nil generates nothing.
/// - Parameter propertyTitlesArray: Optional titles matched by index. Concrete
///   generators derive missing titles and ignore extra entries.
- (void)generatePropertiesFromPropertyNamesArray:(nullable NSArray<NSString *> *)propertyNamesArray propertyTitlesArray:(nullable NSArray<NSString *> *)propertyTitlesArray;

/// Parses property names and groups, then calls the array-based generation hook.
///
/// Groups are appended; existing properties and groups are not cleared. Whitespace
/// around names is trimmed.
/// - Parameter propertyNamesString: Semicolon-separated names, optionally grouped
///   as `@"Header:(firstName,lastName):Footer"`. Header and footer may be empty.
///   Nil or empty strings generate nothing; names inside groups must be nonempty.
- (void)generatePropertiesFromPropertyNamesString:(nullable NSString *)propertyNamesString;

/// Rebuilds default-group names from properties not in any explicit group.
///
/// Existing names are cleared. New names follow definition order; the group's
/// header and footer configuration is preserved.
- (void)generateDefaultPropertyGroupProperties;

/// Creates a store suited to this definition's underlying data structure.
///
/// - Returns: Nil in the base implementation. Concrete definitions return a new
///   store with this definition as its default. Array-backed definitions leave
///   the backing array unset until supplied by the caller or framework.
- (nullable SCDataStore *)generateCompatibleDataStore;

/// Creates fresh fetch options with sortKey set to keyPropertyName.
///
/// The base implementation leaves sorting disabled, filtering enabled with no
/// predicate, and batching disabled. Entity definitions return Core Data options
/// with their ordering attribute too. Existing options are not updated.
- (SCDataFetchOptions *)generateCompatibleDataFetchOptions;

@end

NS_ASSUME_NONNULL_END
