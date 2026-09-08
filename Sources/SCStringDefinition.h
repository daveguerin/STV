/*
 *  SCStringDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCStringDefinition
/// Describes NSString values for generated collection and detail UI.
///
/// Initialization creates one synthetic property named `string` with type
/// `SCPropertyTypeTextField`. It represents the value itself, not a KVC property
/// on NSString. Compatible stores are SCArrayStore instances with no backing
/// array until configured.
///
/// The string itself supplies its title. Description values are always nil.
@interface SCStringDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCStringDefinition.
+ (instancetype)definition;

#pragma mark - Configuration

/// The retained, customizable definition of the synthetic `string` property.
///
/// Initially uses SCDataTypeNSString and SCPropertyTypeTextField.
/// The property reference is read-only, but its display configuration is mutable.
@property (nonatomic, readonly) SCPropertyDefinition *stringPropertyDefinition;


@end

NS_ASSUME_NONNULL_END
