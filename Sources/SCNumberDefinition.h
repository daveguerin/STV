/*
 *  SCNumberDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCNumberDefinition
/// Describes NSNumber values for generated collection and detail UI.
///
/// Initialization creates one synthetic property named `number` with type
/// `SCPropertyTypeNumericTextField`. It represents the value itself, not a KVC property
/// on NSNumber. Compatible stores are SCArrayStore instances with no backing
/// array until configured.
///
/// Titles use the SCNumericTextFieldAttributes numberFormatter when supplied,
/// otherwise stringValue. Description values are always nil.
@interface SCNumberDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCNumberDefinition.
+ (instancetype)definition;

#pragma mark - Configuration

/// The retained, customizable definition of the synthetic `number` property.
///
/// Initially uses SCDataTypeNSNumber and SCPropertyTypeNumericTextField.
/// The property reference is read-only, but its display configuration is mutable.
@property (nonatomic, readonly) SCPropertyDefinition *numberPropertyDefinition;


@end

NS_ASSUME_NONNULL_END
