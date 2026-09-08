/*
 *  SCDateDefinition.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCDateDefinition
/// Describes NSDate values for generated collection and detail UI.
///
/// Initialization creates one synthetic property named `date` with type
/// `SCPropertyTypeDate`. It represents the value itself, not a KVC property
/// on NSDate. Compatible stores are SCArrayStore instances with no backing
/// array until configured.
///
/// Titles use the SCDateAttributes dateFormatter when supplied, otherwise the
/// object description. Description values are always nil.
@interface SCDateDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCDateDefinition.
+ (instancetype)definition;

#pragma mark - Configuration

/// The retained, customizable definition of the synthetic `date` property.
///
/// Initially uses SCDataTypeNSDate and SCPropertyTypeDate.
/// The property reference is read-only, but its display configuration is mutable.
@property (nonatomic, readonly) SCPropertyDefinition *datePropertyDefinition;


@end

NS_ASSUME_NONNULL_END
