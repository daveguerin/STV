/*
 *  SCGlobals.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


/*
 *	The following defines global Sesible TableView defaults. You are very much discouraged from
 *	modifying the content of these values, mainly for your product to remain forward compatable
 *	with future versions of Sensible TableView. If you think you need to change any of these 
 *	values, please consider either modifying the respective property of the Sensible TableView 
 *	object, or subclassing it.
 */
// -----------------------------------------------------------------------------
/// The subtitle style used by STV cell convenience initializers.
#define SC_DefaultCellStyle UITableViewCellStyleSubtitle
/// The initial maximum text-label width for `SCControlCell`, in points.
#define SC_DefaultMaxTextLabelWidth 200 // Default maximum width of SCTableViewCell textLabel
/// Legacy control-spacing constant, in points. Current `SCControlCell`
/// initialization uses `kDefaultControlMargin` instead.
#define SC_DefaultControlMargin 10 // Default margin between different controls
/// Legacy control-height constant, in points; current Auto Layout controls
/// are not assigned this fixed height.
#define SC_DefaultControlHeight 22 // Default control height
/// The initial control indentation for `SCControlCell`, in points.
#define SC_DefaultControlIndentation 120 // Default control indentation from table left border
/// Legacy label font-size constant, in points; not a UIKit-wide default.
#define SC_DefaultLabelFontSize 15 // Default font size of UILabel
/// Legacy text-view font-size constant, in points; not the font assigned to
/// current generated text views.
#define SC_DefaultTextViewFontSize 15 // Default font size of UITextView
/// Legacy text-field font-size constant, in points; not the font assigned to
/// current generated text fields.
#define SC_DefaultTextFieldFontSize 15 // Default font size of UITextField
/// Legacy text-field height constant, in points; current generated controls
/// use Auto Layout rather than this fixed height.
#define SC_DefaultTextFieldHeight 31 // Default height of UITextField
/// Legacy segmented-control height constant, in points; current generated
/// controls use intrinsic sizing rather than this fixed height.
#define SC_DefaultSegmentedControlHeight 29 // Default height of UISegmentedControl
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// DO NOT EDIT AFTER THIS LINE
// -----------------------------------------------------------------------------


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


// -----------------------------------------------------------------------------
/* ARC supporting functions and macros */
// -----------------------------------------------------------------------------

#ifndef __has_feature
#define __has_feature(x) 0 // For compatibility with non-clang compilers.
#endif

#if __has_feature(objc_arc)
/// Indicates that the framework is being compiled with ARC enabled.
#define ARC_ENABLED
#endif

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
/* SCDebugLog definition               */
// -----------------------------------------------------------------------------

/// Logs an STV-prefixed formatted message, receiver address, filename, line,
/// and function using `NSLog`. Requires `self` in scope and is not disabled
/// in Release builds. The first argument is an NSString format string.
#define SCDebugLog(s, ...) NSLog(@"**STV** %p (%@:%d) %s %@", self, [[NSString stringWithUTF8String:__FILE__] lastPathComponent], __LINE__, __FUNCTION__, [NSString stringWithFormat:(s), ##__VA_ARGS__])

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
/* License                             */
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------


/* Data type definitions. */
/// The data types supported by STV property definitions.
typedef NS_ENUM(NSInteger, SCDataType) {
    /// A Boolean scalar value.
    SCDataTypeBOOL,
    /// An integer scalar value.
    SCDataTypeInt,
    /// A single-precision floating-point scalar value.
    SCDataTypeFloat,
    /// A double-precision floating-point scalar value.
    SCDataTypeDouble,
    /// An `NSString` value.
    SCDataTypeNSString,
    /// An `NSNumber` value.
    SCDataTypeNSNumber,
    /// An `NSDate` value.
    SCDataTypeNSDate,
    /// An `NSMutableSet` value.
    SCDataTypeNSMutableSet,
    /// An `NSMutableOrderedSet` value.
    SCDataTypeNSMutableOrderedSet,
    /// An `NSMutableArray` value.
    SCDataTypeNSMutableArray,
    /// A general `NSObject` value.
    SCDataTypeNSObject,
    /// A dictionary-backed item value.
    SCDataTypeDictionaryItem,
    /// A Core Data transformable value.
    SCDataTypeTransformable,
    /// An unknown or unsupported value type.
    SCDataTypeUnknown
};


/// Adds STV's extended key-path accessors to all objects.
@interface NSObject (SensibleCocoa)

/// Reads a KVC key path extended with zero-based array indexes.
/// Use `[n]` for the last element and `[n-1]` for the penultimate element.
/// Detected missing brackets, non-indexable values, empty arrays, and invalid
/// indexes log and return `nil`. KVC exceptions propagate; syntax validation
/// is not exhaustive.
/// - Parameter keyPath: The key path, or `nil` to return `nil`.
/// - Returns: The resolved value (not necessarily the receiver's class), or
///   `nil` for a nil result or detected array-access error.
///
/// ```objective-c
/// id first = [customer valueForSensibleKeyPath:@"salesData.invoices[0].amount"];
/// id last = [customer valueForSensibleKeyPath:@"salesData.invoices[n].amount"];
/// id prior = [customer valueForSensibleKeyPath:@"salesData.invoices[n-1].amount"];
/// ```
- (nullable instancetype)valueForSensibleKeyPath:(nullable NSString *)keyPath;

/// Writes a KVC property, allowing bracket indexes in intermediate components
/// with the same syntax as `valueForSensibleKeyPath:`. The final component must
/// be a property key: a path ending in brackets does not replace an array item.
/// Detected intermediate array-access errors abort the assignment; KVC
/// exceptions propagate.
/// - Parameter value: The value; `nil` requires a nil-compatible target.
/// - Parameter keyPath: The key path, or `nil` to do nothing.
- (void)setValue:(nullable id)value forSensibleKeyPath:(nullable NSString *)keyPath;

@end


/// For internal framework use only
@interface UILabel (SensibleCocoa)

/// Text prepended by STV when loading a custom-cell label binding. Reading an
/// unset or nil-assigned prefix returns an empty string. Setting this alone
/// does not modify the label text.
@property (nonatomic, copy, nullable) NSString *prefix;
/// Text appended by STV when loading a custom-cell label binding. Reading an
/// unset or nil-assigned suffix returns an empty string. For numeric bindings,
/// a leading decimal-zero pattern such as `.00` requests a precision. Setting
/// this alone does not modify the label text.
@property (nonatomic, copy, nullable) NSString *suffix;

@end


/// Adds STV's keyboard-dismissal behavior to navigation controllers.
@interface UINavigationController (SensibleCocoa)

/// Returns `NO`, allowing automatic keyboard dismissal for navigation
/// controllers. This does not itself dismiss the keyboard.
- (BOOL)disablesAutomaticKeyboardDismissal;

@end


/// The standard message used when no Internet connection is available.
#define kNoInternetConnectionString @"No Internet Connection"

/// This class defines a set of internal helper methods
@interface SCUtilities : NSObject {
}

/// Returns whether the current device reports the iPad user-interface idiom.
+ (BOOL)is_iPad;

/// Checks the responder chain and controller ancestors for a presented,
/// window-attached popover that has not adapted to another presentation style.
/// - Parameter view: The view to inspect; `nil` returns `NO`.
+ (BOOL)isViewInsidePopover:(nullable UIView *)view;

/// Synchronously attempts to resolve `google.com` using the system DNS resolver.
/// - Returns: `YES` on successful resolution, which may use cached data. This
///   does not verify Internet connectivity or service reachability.
/// - Warning: This legacy helper can block its calling thread.
+ (BOOL)IsInternetConnectionAvailable;

/// Checks whether `NSURL` parses a string with both a scheme and a host.
/// Does not restrict the scheme, make a request, or test reachability.
/// - Parameter urlString: The nonnull string to inspect.
+ (BOOL)isURLValid:(NSString *)urlString;

/// Instantiates a nib with a nil owner and returns its first top-level object.
/// - Parameter nibName: A main-bundle name without underscores, or exactly
///   `PackageName_TargetName_NIBName` for a SwiftPM resource bundle. Individual
///   components must not contain underscores. `nil` returns `nil`.
/// - Returns: The first object, or `nil` for an unsupported name format,
///   unfound package bundle, or empty top-level array. Nib-loading exceptions
///   propagate; a missing nib is not guaranteed to return `nil`.
+ (nullable NSObject *)getFirstNodeInNibWithName:(nullable NSString *)nibName;

/// Uppercases the first character and inserts a space before every subsequent
/// uppercase character. A leading `~` is omitted. This is a simple name
/// transformation, not localized title generation.
/// - Parameter propertyName: The source; nil or empty input returns `@""`.
+ (NSString *)getUserFriendlyTitleFromName:(nullable NSString *)propertyName;

/// Tries `NSClassFromString`, then a legacy Swift mangled name formed from the
/// main bundle's `CFBundleName` and the supplied class name. This fallback is
/// not general module discovery.
/// - Parameter className: The nonnull class name.
/// - Returns: The resolved class, or `Nil` if neither lookup succeeds.
+ (nullable Class)swiftCompatibleNSClassFromString:(NSString *)className;

/// Tests whether a class responds to `availableStringEncodings`, rather than
/// checking inheritance.
/// - Parameter aClass: The class; `Nil` returns `NO`.
+ (BOOL)isStringClass:(nullable Class)aClass;

/// Tests whether a class responds to `numberWithUnsignedLongLong:`, rather
/// than checking inheritance.
/// - Parameter aClass: The class; `Nil` returns `NO`.
+ (BOOL)isNumberClass:(nullable Class)aClass;

/// Tests whether a class responds to `timeIntervalSinceReferenceDate`, rather
/// than checking inheritance.
/// - Parameter aClass: The class; `Nil` returns `NO`.
+ (BOOL)isDateClass:(nullable Class)aClass;

/// Tests whether a class responds to `dictionaryWithDictionary:`, rather than
/// checking inheritance.
/// - Parameter aClass: The class; `Nil` returns `NO`.
+ (BOOL)isDictionaryClass:(nullable Class)aClass;

/// Returns whether STV's string, number, or date class heuristics match.
/// Dictionary and collection classes are not included.
/// - Parameter aClass: The class; `Nil` returns `NO`.
+ (BOOL)isBasicDataTypeClass:(nullable Class)aClass;

/// Normalizes matching classes to `NSString`, `NSNumber`, `NSDate`, or
/// `NSDictionary`, in that order, using STV's class heuristics. Otherwise
/// returns `NSStringFromClass(aClass)`.
/// - Parameter aClass: The class; `Nil` produces `nil`.
+ (nullable NSString *)dataStructureNameForClass:(nullable Class)aClass;

/// Tests whether a property read raises an exception, not whether it has a
/// value or is a declared Objective-C property. Returns `YES` unconditionally
/// for basic types and dictionaries. Other objects use extended key-path
/// lookup, or direct key lookup for `NSUbiquitousKeyValueStore`.
/// - Parameter propertyName: The path to probe; a nil path can succeed.
/// - Parameter object: The object; `nil` returns `YES`.
/// - Returns: `NO` only when the probe throws; a nil result still succeeds.
+ (BOOL)propertyName:(nullable NSString *)propertyName existsInObject:(nullable NSObject *)object;

/// Reads one or more properties using extended key-path lookup, catching
/// lookup exceptions. `NSUbiquitousKeyValueStore` uses direct keys instead.
/// String, number, and date objects recognized as basic types are returned
/// unchanged, regardless of the name.
/// - Parameter propertyName: A key path or semicolon-separated paths. `nil`
///   returns `nil` except when the object is a basic type.
/// - Parameter object: The source, which may be `nil`.
/// - Returns: A single value, or `nil` for a missing, nil, or `NSNull` value.
///   Multiple paths return an array with `NSNull` for missing or nil values.
+ (nullable NSObject *)valueForPropertyName:(nullable NSString *)propertyName inObject:(nullable NSObject *)object;

/// Formats `valueForPropertyName:inObject:` results using `%@`. Array results
/// are flattened, skipping `NSNull`. A delimiter precedes each nonnull entry
/// whose original index is nonzero, so skipped leading entries can produce
/// a leading delimiter.
/// - Parameter propertyName: A key path or semicolon-separated paths; ignored
///   for basic-type objects.
/// - Parameter object: The source, which may be `nil`.
/// - Parameter delimiter: The separator, or `nil` to concatenate values.
/// - Returns: The formatted value, `nil` for a nil lookup result, or an empty
///   string for an empty array or one containing only `NSNull`.
+ (nullable NSString *)stringValueForPropertyName:(nullable NSString *)propertyName inObject:(nullable NSObject *)object separateValuesUsingDelimiter:(nullable NSString *)delimiter;

/// Assigns a value after STV's property-existence probe succeeds. Does nothing
/// for basic-type objects. Ubiquitous stores use direct key assignment; other
/// objects use ordinary KVC key paths, not bracket indexing. Assignment
/// exceptions are not caught.
/// - Parameter value: The value. Nil becomes zero only for direct properties
///   with runtime encodings `c`, `i`, `f`, or `d`; other scalar encodings and
///   scalar key paths are not covered by this fallback.
/// - Parameter propertyName: A nonnull property name or ordinary KVC key path.
/// - Parameter object: The destination, or `nil` to do nothing.
+ (void)setValue:(nullable NSObject *)value forPropertyName:(NSString *)propertyName inObject:(nullable NSObject *)object;

/// Performs limited conversion for string, number, and date types. Strings
/// use `%@`; numbers parse strings with a decimal `NSNumberFormatter`; dates
/// parse strings with an otherwise unconfigured `NSDateFormatter`. Existing
/// number/date instances are preserved; nonstring alternatives are rejected.
/// - Parameter dataType: The target. All other types, including scalar types,
///   return the value unchanged rather than validating it.
/// - Parameter value: The source; `nil` returns `nil`.
/// - Returns: The converted or unchanged value, or `nil` if conversion fails.
+ (nullable NSObject *)getValueCompatibleWithDataType:(SCDataType)dataType fromValue:(nullable NSObject *)value;

/// Parses semicolon-separated `key:value` pairs and trims whitespace/newlines.
/// Pairs without exactly one colon are logged and skipped. Later duplicate
/// keys overwrite earlier values; delimiter characters cannot be escaped.
/// - Parameter bindingsString: The text; nil or empty produces an empty dictionary.
/// - Returns: A dictionary of string keys and values, possibly empty.
+ (NSDictionary<NSString *, NSString *> *)bindingsDictionaryForBindingsString:(nullable NSString *)bindingsString;

/// Joins entries as semicolon-separated `key:value` pairs. Ordering is
/// unspecified and delimiter characters are not escaped.
/// - Parameter bindingsDictionary: The dictionary to encode.
/// - Returns: The text, or `nil` for a nil or empty dictionary.
+ (nullable NSString *)bindingsStringForBindingsDictionary:(nullable NSDictionary<NSString *, NSString *> *)bindingsDictionary;


/// Encodes UTF-8 bytes as standard Base64 with `=` padding and no line breaks.
/// This is encoding, not encryption.
/// - Parameter string: The nonnull input; an empty string produces `@""`.
+ (NSString *)base64EncodedStringFromString:(NSString *)string;

/// Instantiates a storyboard controller. If lookup throws, retries after
/// replacing every `UIViewController` substring with `UITableViewController`
/// in the identifier for legacy storyboard compatibility. Retry exceptions
/// propagate.
/// - Parameter identifier: The nonnull identifier.
/// - Parameter storyboard: The storyboard, or `nil` to return `nil`.
/// - Returns: The controller, or `nil` when the storyboard is nil.
+ (nullable UIViewController *)instantiateViewControllerWithIdentifier:(NSString *)identifier usingStoryboard:(nullable UIStoryboard *)storyboard;

/// Reconstructs a date from year, month, and day using the current calendar
/// and its time zone, not UTC truncation.
/// - Parameter date: The nonnull source date.
/// - Returns: The reconstructed date, or `nil` if the calendar cannot form it.
+ (nullable NSDate *)stripTimeFromDate:(NSDate *)date;

@end


@class SCTableViewModel;

/// An internal registry that routes keyboard notifications to table models.
/// Models are not retained and must unregister before deallocation.
@interface SCModelCenter : NSObject


/// The weak controller used to route keyboard show, frame-change, and hide
/// notifications to registered models with the same `viewController`.
/// Defaults to `nil`, which disables forwarding.
@property (nonatomic, weak, nullable) UIViewController *keyboardIssuer;

/// Returns the shared STV model center.
+ (SCModelCenter *)sharedModelCenter;

/// Registers a model by pointer identity without retaining it. Re-registering
/// the same model does not add a duplicate.
/// - Parameter model: The nonnull model; unregister it before deallocation.
- (void)registerModel:(SCTableViewModel *)model;
/// Removes a model from the registry; an unregistered model is a no-op.
/// - Parameter model: The nonnull model to remove.
- (void)unregisterModel:(SCTableViewModel *)model;

/// Finds a registered model with a pointer-equal `viewController`. If several
/// match, the returned model is unspecified.
/// - Parameter viewController: The controller; `nil` can match a model whose
///   `viewController` is also nil.
/// - Returns: A matching model, or `nil` if no registered model matches.
- (nullable SCTableViewModel *)modelForViewController:(nullable UIViewController *)viewController;

@end

NS_ASSUME_NONNULL_END
