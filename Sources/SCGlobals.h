/*
 *  SCGlobals.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
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
/// The default cell style.
#define     SC_DefaultCellStyle                 UITableViewCellStyleSubtitle
/// Default maximum width of SCTableViewCell textLabel.
#define		SC_DefaultMaxTextLabelWidth			200		// Default maximum width of SCTableViewCell textLabel
/// Default margin between different controls.
#define		SC_DefaultControlMargin				10		// Default margin between different controls
/// Default control height.
#define		SC_DefaultControlHeight				22		// Default control height
/// Default control indentation from table left border.
#define		SC_DefaultControlIndentation		120		// Default control indentation from table left border
/// Default font size of UILabel.
#define		SC_DefaultLabelFontSize             15		// Default font size of UILabel
/// Default font size of UITextView.
#define		SC_DefaultTextViewFontSize			15		// Default font size of UITextView
/// Default font size of UITextField.
#define		SC_DefaultTextFieldFontSize			15		// Default font size of UITextField
/// Default height of UITextField.
#define		SC_DefaultTextFieldHeight			31		// Default height of UITextField
/// Default height of UISegmentedControl.
#define		SC_DefaultSegmentedControlHeight	29		// Default height of UISegmentedControl
// -----------------------------------------------------------------------------






// -----------------------------------------------------------------------------
// DO NOT EDIT AFTER THIS LINE
// -----------------------------------------------------------------------------


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>



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

//#define SCDebugLog(...) NSLog(@"**STV** %@", [NSString stringWithFormat:__VA_ARGS__]) // dgApps
/// Logs a formatted STV debug message with source context.
#define SCDebugLog( s, ... ) NSLog(@"**STV** %p (%@:%d) %s %@", self, [[NSString stringWithUTF8String:__FILE__] lastPathComponent], __LINE__, __FUNCTION__, [NSString stringWithFormat:(s), ##__VA_ARGS__] )

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
/* License                             */
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------





/* Data type definitions. */
/// The data types supported by STV property definitions.
typedef NS_ENUM(NSInteger, SCDataType)
{
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

/// This method allows accessing array values in a regular key-path string by placing the array item index between brakets.
///
/// - Parameter keyPath: A key path that may contain array indexes in brackets.
///
/// Examples:
/// ```objective-c
/// firstSaleAmount = [customer valueForSensibleKeyPath:@"salesData.invoices[0].amount"];
/// lastSaleAmount = [customer valueForSensibleKeyPath:@"salesData.invoices[n].amount"];
/// saleBeforeLastAmount = [customer valueForSensibleKeyPath:@"salesData.invoices[n-1].amount"];
/// ```
- (instancetype)valueForSensibleKeyPath:(NSString *)keyPath;

/// Similar to valueForSensibleKeyPath, this method uses a Sensible keyPath to set a value.
/// - Parameter value: The value to set.
/// - Parameter keyPath: A key path that may contain array indexes in brackets.
- (void)setValue:(id)value forSensibleKeyPath:(NSString *)keyPath;

@end


/// For internal framework use only
@interface UILabel (SensibleCocoa)

/// Text prepended to the label's value.
@property (nonatomic, copy) NSString *prefix;
/// Text appended to the label's value.
@property (nonatomic, copy) NSString *suffix;

@end



/// Adds STV's keyboard-dismissal behavior to navigation controllers.
@interface UINavigationController (SensibleCocoa)

/// Returns whether the navigation controller prevents automatic keyboard dismissal.
- (BOOL)disablesAutomaticKeyboardDismissal;

@end







/// The standard message used when no Internet connection is available.
#define  kNoInternetConnectionString @"No Internet Connection"

/// This class defines a set of internal helper methods
@interface SCUtilities : NSObject
{
}

/// Method determines if code is being run on an iPad
+ (BOOL)is_iPad;

/// Method determines if the given view is inside a popover view
/// - Parameter view: The view to inspect.
+ (BOOL)isViewInsidePopover:(UIView *)view;

/// Returns whether an Internet connection is currently available.
+ (BOOL)IsInternetConnectionAvailable;

/// Returns whether the supplied URL string is valid.
/// - Parameter urlString: The URL string to validate.
+ (BOOL)isURLValid:(NSString *)urlString;

/// Loads and returns the first top-level object in the named nib.
/// - Parameter nibName: The nib name to load.
+ (NSObject *)getFirstNodeInNibWithName:(NSString *)nibName;

/// Returns a user-facing title derived from a property name.
/// - Parameter propertyName: The property name.
+ (NSString *)getUserFriendlyTitleFromName:(NSString *)propertyName;

/// Returns the class identified by a name using Swift-compatible lookup rules.
/// - Parameter className: The class name to resolve.
+ (Class)swiftCompatibleNSClassFromString:(NSString *)className;

/// Returns whether a class represents a string type.
/// - Parameter aClass: The class to inspect.
+ (BOOL)isStringClass:(Class)aClass;

/// Returns whether a class represents a number type.
/// - Parameter aClass: The class to inspect.
+ (BOOL)isNumberClass:(Class)aClass;

/// Returns whether a class represents a date type.
/// - Parameter aClass: The class to inspect.
+ (BOOL)isDateClass:(Class)aClass;

/// Returns whether a class represents a dictionary type.
/// - Parameter aClass: The class to inspect.
+ (BOOL)isDictionaryClass:(Class)aClass;

/// Returns whether a class represents an STV basic data type.
/// - Parameter aClass: The class to inspect.
+ (BOOL)isBasicDataTypeClass:(Class)aClass;

/// Returns the STV data-structure name associated with a class.
/// - Parameter aClass: The class to inspect.
+ (NSString *)dataStructureNameForClass:(Class)aClass;

/// Returns whether an object exposes the specified property name.
/// - Parameter propertyName: The property name.
/// - Parameter object: The object involved in the operation.
+ (BOOL)propertyName:(NSString *)propertyName existsInObject:(NSObject *)object;

///  Returns the the value for the given property in the given object.
/// - Parameter propertyName: The name of the property whose value is requested. propertyName can be
///   given in a key-path format (e.g.: "subObject1.subObject2.subObject3"). More than one property
///   can be given, separated by `;` (e.g.: "property1;property2;property3").
/// - Parameter object: The owner object of the given property.
/// - Returns: Returns the value of the given property. If more than one property name is given
///   (separated by `;`), the return value is an NSArray. Returns nil if property does not exist
///   in object (or NSNull if more than one property name is given).
+ (NSObject *)valueForPropertyName:(NSString *)propertyName inObject:(NSObject *)object;

/// Returns a string representation of an object's property value.
/// - Parameter propertyName: The property name.
/// - Parameter object: The object involved in the operation.
/// - Parameter delimiter: The delimiter used to separate multiple values.
+ (NSString *)stringValueForPropertyName:(NSString *)propertyName inObject:(NSObject *)object
			separateValuesUsingDelimiter:(NSString *)delimiter;

/// Sets an object's property to the supplied value.
/// - Parameter value: The value to assign or convert.
/// - Parameter propertyName: The property name.
/// - Parameter object: The object involved in the operation.
+ (void)setValue:(NSObject *)value forPropertyName:(NSString *)propertyName inObject:(NSObject *)object;

/// Returns a value converted for the requested STV data type.
/// - Parameter dataType: The target STV data type.
/// - Parameter value: The value to assign or convert.
+ (NSObject *)getValueCompatibleWithDataType:(SCDataType)dataType fromValue:(NSObject *)value;

/// Returns a bindings dictionary parsed from its string representation.
/// - Parameter bindingsString: The bindings string to parse.
+ (NSDictionary *)bindingsDictionaryForBindingsString:(NSString *)bindingsString;

/// Returns the string representation of a bindings dictionary.
/// - Parameter bindingsDictionary: The bindings dictionary to encode.
+ (NSString *)bindingsStringForBindingsDictionary:(NSDictionary *)bindingsDictionary;


/// Returns the Base64 representation of a string.
/// - Parameter string: The source string.
+ (NSString *)base64EncodedStringFromString:(NSString *)string;

/// We need our own instantiator for cases where we need to port Xcode 6 view controller id naming conventions to Xcode 7
/// - Parameter identifier: The identifier.
/// - Parameter storyboard: The storyboard.
+ (UIViewController *)instantiateViewControllerWithIdentifier:(NSString *)identifier usingStoryboard:(UIStoryboard *)storyboard;

/// Returns a date with its time components removed.
/// - Parameter date: The source date.
+ (NSDate *)stripTimeFromDate:(NSDate *)date;

@end




@class SCTableViewModel;

/// This class defines a tabel view model center.
/// IMPORTANT: This class is usually only used internally by the framework.
@interface SCModelCenter : NSObject
{
	/// The registered table-view models.
	CFMutableSetRef modelsSet;
}

/// The view controller currently responsible for keyboard input.
@property (nonatomic, weak) UIViewController *keyboardIssuer;

/// Returns the shared STV model center.
+ (SCModelCenter *)sharedModelCenter;

/// Registers a table-view model with the model center.
/// - Parameter model: The table-view model.
- (void)registerModel:(SCTableViewModel *)model;
/// Unregisters a table-view model from the model center.
/// - Parameter model: The table-view model.
- (void)unregisterModel:(SCTableViewModel *)model;

/// Returns the table-view model associated with a view controller.
/// - Parameter viewController: The view controller associated with the operation.
- (SCTableViewModel *)modelForViewController:(UIViewController *)viewController;

@end


