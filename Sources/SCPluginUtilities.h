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

// Keys
#define kCustomClassName                @"_STV_CustomClassName"
#define kYesString                      @"Y"
#define kNoString                       @"N"
#define kibUniqueID                     @"ibUniqueID"

#define kSectionIndexKey                @"index"
#define kSectionTypeKey                 @"type"
#define kSectionDataDefIdKey            @"dataDefinitionID"
#define kSectionDataDefGroupIndexKey    @"dataDefinitionGroupIndex"
#define kSectionPredicateStringKey      @"predicateString"
#define kSectionPlaceholderTextKey      @"placeholderText"
#define kSectionPlaceholderTextAlignmentKey      @"placeholderTextAlignment"
#define kSectionAddNewItemTextKey       @"addNewItemText"
#define kSectionHideWhenEmptyKey        @"hideWhenEmpty"
#define kSectionAllowAddingKey          @"allowAdding"
#define kSectionAllowDeletingKey        @"allowDeleting"
#define kSectionAllowMovingKey          @"allowMoving"
#define kSectionAllowEditingKey         @"allowEditing"
#define kSectionBatchSizeKey            @"batchSize"
#define kSectionStringsArrayKey         @"stringsArray"
#define kSectionMasterBoundPropertyName     @"masterBoundPropertyName"
#define kSectionibNewItemViewControllerIdentifier    @"ibNewItemViewControllerIdentifier"

#define kDefaultDataDefinitionName      @"(none selected)"
#define kPropertyGroups                 @"propertyGroups"
#define kPropertyDefinitions            @"propertyDefinitions"



// UI Related
#define kTitleLabelXMargin      15
#define kDefaultTitleLabelWidth 110
#define kDefaultTitleLabelHeight    22
#define kYMargin                11
#define kDefaultControlMargin   20
#define kTextViewMargin         7
#define kTextViewXMargin        10
#define kTextViewPadding        5
#define kTextViewTitleLabelHeight   19
#define kDefaultControlHeight   22
#define kDefaultSegmentedHeight 29
#define kDefaultSliderHeight    31
#define kDefaultSwitchHeight    31
#define kDefaultSwitchWidth     51

#define kDefaultTitleLabelFontSize  16
#define kDefaultTitleLabelFontDescription      [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultTitleLabelFontSize]
#define kDefaultControlFontSize     15
#define kDefaultControlFontDescription         [IBUIFontDescription fontDescriptionWithName:@"System" family:@"Helvetica Neue" andPointSize:kDefaultControlFontSize]

#define kMinTextViewHeight  65              // 3 lines with default font
#define kMaxTextViewHeight  500

#define kSTVTagStartRange       7777777         // just a number to stay above any user-defined tag values


/* This class is used internally to provide STV plugin-related utilities */


@interface SCPluginUtilities : NSObject

+ (id)objectForPluginDictionary:(NSDictionary *)pluginDictionary;

@end


@protocol SCibInitialization <NSObject>

- (instancetype)initWithibDictionary:(NSMutableDictionary *)ibDictionary;

@end
