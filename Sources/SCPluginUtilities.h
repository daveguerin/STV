/*
 *  SCPluginUtilities.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


// ********** IMPORTANT **********
//
// This file manages shared data between STV's OS X Plugin code and it's iOS framework code.
//
// It is vital that you do not change the contents of this file for STV to be able to correctly communicate with its plugin counter-part.
//
// ********** IMPORTANT **********


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// Keys
/// Internal legacy plug-in serialization key for custom class name.
#define kCustomClassName @"_STV_CustomClassName"
/// Internal legacy plug-in string representing a Boolean true value.
#define kYesString @"Y"
/// Internal legacy plug-in string representing a Boolean false value.
#define kNoString @"N"
/// Internal legacy plug-in serialization key for Interface Builder unique identifier.
#define kibUniqueID @"ibUniqueID"

/// Internal legacy plug-in serialization key for section index.
#define kSectionIndexKey @"index"
/// Internal legacy plug-in serialization key for section type.
#define kSectionTypeKey @"type"
/// Internal legacy plug-in serialization key for section data-definition identifier.
#define kSectionDataDefIdKey @"dataDefinitionID"
/// Internal legacy plug-in serialization key for section data-definition group index.
#define kSectionDataDefGroupIndexKey @"dataDefinitionGroupIndex"
/// Internal legacy plug-in serialization key for section predicate string.
#define kSectionPredicateStringKey @"predicateString"
/// Internal legacy plug-in serialization key for section placeholder text.
#define kSectionPlaceholderTextKey @"placeholderText"
/// Internal legacy plug-in serialization key for section placeholder text alignment.
#define kSectionPlaceholderTextAlignmentKey @"placeholderTextAlignment"
/// Internal legacy plug-in serialization key for section add new item text.
#define kSectionAddNewItemTextKey @"addNewItemText"
/// Internal legacy plug-in serialization key for section hide when empty.
#define kSectionHideWhenEmptyKey @"hideWhenEmpty"
/// Internal legacy plug-in serialization key for section allow adding.
#define kSectionAllowAddingKey @"allowAdding"
/// Internal legacy plug-in serialization key for section allow deleting.
#define kSectionAllowDeletingKey @"allowDeleting"
/// Internal legacy plug-in serialization key for section allow moving.
#define kSectionAllowMovingKey @"allowMoving"
/// Internal legacy plug-in serialization key for section allow editing.
#define kSectionAllowEditingKey @"allowEditing"
/// Internal legacy plug-in serialization key for section batch size.
#define kSectionBatchSizeKey @"batchSize"
/// Internal legacy plug-in serialization key for section strings array.
#define kSectionStringsArrayKey @"stringsArray"
/// Internal legacy plug-in serialization key for the section's master bound property name.
#define kSectionMasterBoundPropertyName @"masterBoundPropertyName"
/// Internal legacy plug-in serialization key for the section's Interface Builder new-item view-controller identifier.
#define kSectionibNewItemViewControllerIdentifier @"ibNewItemViewControllerIdentifier"

/// Internal legacy plug-in sentinel for no selected data definition.
#define kDefaultDataDefinitionName @"(none selected)"
/// Internal legacy plug-in serialization key for property groups.
#define kPropertyGroups @"propertyGroups"
/// Internal legacy plug-in serialization key for property definitions.
#define kPropertyDefinitions @"propertyDefinitions"


// UI Related
/// Internal legacy plug-in layout constant for title label x margin.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kTitleLabelXMargin 15
/// Internal legacy plug-in layout constant for title label width.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultTitleLabelWidth 110
/// Internal legacy plug-in layout constant for title label height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultTitleLabelHeight 22
/// Internal legacy plug-in layout constant for y margin.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kYMargin 11
/// Internal initial `SCControlCell.controlMargin`, in points; also retained
/// for legacy plug-in compatibility. Not a public customization switch.
#define kDefaultControlMargin 20
/// Internal legacy plug-in layout constant for text view margin.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kTextViewMargin 7
/// Internal legacy plug-in layout constant for text view x margin.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kTextViewXMargin 10
/// Internal legacy plug-in layout constant for text view padding.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kTextViewPadding 5
/// Internal legacy plug-in layout constant for text view title label height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kTextViewTitleLabelHeight 19
/// Internal legacy plug-in layout constant for control height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultControlHeight 22
/// Internal legacy plug-in layout constant for segmented height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultSegmentedHeight 29
/// Internal legacy plug-in layout constant for slider height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultSliderHeight 31
/// Internal legacy plug-in layout constant for switch height.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultSwitchHeight 31
/// Internal legacy plug-in layout constant for switch width.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultSwitchWidth 51

/// Internal legacy plug-in layout constant for title label font size.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultTitleLabelFontSize 16
/// Internal legacy plug-in font-description expression. Requires the plug-in
/// class `IBUIFontDescription`; it is not a UIKit font factory.
#define kDefaultTitleLabelFontDescription [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultTitleLabelFontSize]
/// Internal legacy plug-in layout constant for control font size.
/// Not a current UIKit sizing guarantee or a public customization switch.
#define kDefaultControlFontSize 15
/// Internal legacy plug-in font-description expression. Requires the plug-in
/// class `IBUIFontDescription`; it is not a UIKit font factory.
#define kDefaultControlFontDescription [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultControlFontSize]

/// Internal initial `SCTextViewCell.minimumHeight`, in points. The legacy
/// three-line estimate is not a guarantee for every font or content size.
#define kMinTextViewHeight 65 // 3 lines with default font
/// Internal initial `SCTextViewCell.maximumHeight`, in points.
#define kMaxTextViewHeight 500

/// Internal baseline for generated custom-control tags. STV also checks the
/// existing direct-subview tags; this number alone does not reserve all tags.
#define kSTVTagStartRange 7777777 // just a number to stay above any user-defined tag values


/* This class is used internally to provide STV plugin-related utilities */


/// Internal compatibility helpers for decoding legacy STV Interface Builder
/// plug-in data. These are not a supported application-customization interface.
@interface SCPluginUtilities : NSObject

/// Instantiates the class named by `kCustomClassName` in trusted legacy data.
/// Classes conforming to `SCibInitialization` receive a mutable shallow copy
/// with the class-name key removed. Other classes receive `init`, then KVC
/// assignments for every original key, including the class-name key.
/// - Parameter pluginDictionary: A trusted, nonnull legacy configuration.
/// - Returns: The initialized object, or `nil` if the class is unresolved or
///   initialization returns nil. Recognized missing definition classes instead
///   return an internal missing-framework placeholder.
/// - Warning: This performs runtime class lookup and KVC without validation or
///   exception handling. It is not a decoder for untrusted input.
+ (nullable id)objectForPluginDictionary:(NSDictionary<NSString *, id> *)pluginDictionary;

@end


/// Internal initialization contract for legacy STV Interface Builder data.
@protocol SCibInitialization <NSObject>

/// Initializes an object from trusted legacy configuration.
/// - Parameter ibDictionary: A mutable dictionary that the implementation may
///   consume or modify. The utility caller removes `kCustomClassName` first.
/// - Returns: The initialized object, or `nil` on initialization failure.
- (nullable instancetype)initWithibDictionary:(NSMutableDictionary<NSString *, id> *)ibDictionary;

@end

NS_ASSUME_NONNULL_END
