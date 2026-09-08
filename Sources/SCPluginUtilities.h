/*
 *  SCPluginUtilities.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
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
/// The legacy plug-in dictionary key for custom class name.
#define kCustomClassName @"_STV_CustomClassName"
/// The legacy plug-in string representing a Boolean true value.
#define kYesString @"Y"
/// The legacy plug-in string representing a Boolean false value.
#define kNoString @"N"
/// The legacy plug-in dictionary key for interface builder unique id.
#define kibUniqueID @"ibUniqueID"

/// The legacy plug-in dictionary key for section index key.
#define kSectionIndexKey @"index"
/// The legacy plug-in dictionary key for section type key.
#define kSectionTypeKey @"type"
/// The legacy plug-in dictionary key for section data def identifier key.
#define kSectionDataDefIdKey @"dataDefinitionID"
/// The legacy plug-in dictionary key for section data def group index key.
#define kSectionDataDefGroupIndexKey @"dataDefinitionGroupIndex"
/// The legacy plug-in dictionary key for section predicate string key.
#define kSectionPredicateStringKey @"predicateString"
/// The legacy plug-in dictionary key for section placeholder text key.
#define kSectionPlaceholderTextKey @"placeholderText"
/// The legacy plug-in dictionary key for section placeholder text alignment key.
#define kSectionPlaceholderTextAlignmentKey @"placeholderTextAlignment"
/// The legacy plug-in dictionary key for section add new item text key.
#define kSectionAddNewItemTextKey @"addNewItemText"
/// The legacy plug-in dictionary key for section hide when empty key.
#define kSectionHideWhenEmptyKey @"hideWhenEmpty"
/// The legacy plug-in dictionary key for section allow adding key.
#define kSectionAllowAddingKey @"allowAdding"
/// The legacy plug-in dictionary key for section allow deleting key.
#define kSectionAllowDeletingKey @"allowDeleting"
/// The legacy plug-in dictionary key for section allow moving key.
#define kSectionAllowMovingKey @"allowMoving"
/// The legacy plug-in dictionary key for section allow editing key.
#define kSectionAllowEditingKey @"allowEditing"
/// The legacy plug-in dictionary key for section batch size key.
#define kSectionBatchSizeKey @"batchSize"
/// The legacy plug-in dictionary key for section strings array key.
#define kSectionStringsArrayKey @"stringsArray"
/// The legacy plug-in dictionary key for the section's master bound property name.
#define kSectionMasterBoundPropertyName @"masterBoundPropertyName"
/// The legacy plug-in dictionary key for the section's Interface Builder new-item view-controller identifier.
#define kSectionibNewItemViewControllerIdentifier @"ibNewItemViewControllerIdentifier"

/// The legacy plug-in default for default data definition name.
#define kDefaultDataDefinitionName @"(none selected)"
/// The legacy plug-in dictionary key for property groups.
#define kPropertyGroups @"propertyGroups"
/// The legacy plug-in dictionary key for property definitions.
#define kPropertyDefinitions @"propertyDefinitions"


// UI Related
/// The legacy plug-in default for title label x margin.
#define kTitleLabelXMargin 15
/// The legacy plug-in default for default title label width.
#define kDefaultTitleLabelWidth 110
/// The legacy plug-in default for default title label height.
#define kDefaultTitleLabelHeight 22
/// The legacy plug-in default for y margin.
#define kYMargin 11
/// The legacy plug-in default for default control margin.
#define kDefaultControlMargin 20
/// The legacy plug-in default for text view margin.
#define kTextViewMargin 7
/// The legacy plug-in default for text view x margin.
#define kTextViewXMargin 10
/// The legacy plug-in default for text view padding.
#define kTextViewPadding 5
/// The legacy plug-in default for text view title label height.
#define kTextViewTitleLabelHeight 19
/// The legacy plug-in default for default control height.
#define kDefaultControlHeight 22
/// The legacy plug-in default for default segmented height.
#define kDefaultSegmentedHeight 29
/// The legacy plug-in default for default slider height.
#define kDefaultSliderHeight 31
/// The legacy plug-in default for default switch height.
#define kDefaultSwitchHeight 31
/// The legacy plug-in default for default switch width.
#define kDefaultSwitchWidth 51

/// The legacy plug-in default for default title label font size.
#define kDefaultTitleLabelFontSize 16
/// The legacy plug-in default for default title label font description.
#define kDefaultTitleLabelFontDescription [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultTitleLabelFontSize]
/// The legacy plug-in default for default control font size.
#define kDefaultControlFontSize 15
/// The legacy plug-in default for default control font description.
#define kDefaultControlFontDescription [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultControlFontSize]

/// 3 lines with default font.
#define kMinTextViewHeight 65 // 3 lines with default font
/// The legacy plug-in default for max text view height.
#define kMaxTextViewHeight 500

/// just a number to stay above any user-defined tag values.
#define kSTVTagStartRange 7777777 // just a number to stay above any user-defined tag values


/* This class is used internally to provide STV plugin-related utilities */


/// Provides helpers used to decode legacy STV Interface Builder plug-in data.
@interface SCPluginUtilities : NSObject

/// Creates the STV object represented by a plug-in dictionary.
/// - Parameter pluginDictionary: The plugin dictionary.
+ (nullable id)objectForPluginDictionary:(NSDictionary<NSString *, id> *)pluginDictionary;

@end


/// Defines initialization from legacy STV Interface Builder configuration data.
@protocol SCibInitialization <NSObject>

/// Initializes an object from an Interface Builder configuration dictionary.
/// - Parameter ibDictionary: The Interface Builder configuration dictionary.
- (nullable instancetype)initWithibDictionary:(NSMutableDictionary<NSString *, id> *)ibDictionary;

@end

NS_ASSUME_NONNULL_END
