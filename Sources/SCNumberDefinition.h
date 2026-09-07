/*
 *  SCNumberDefinition.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCNumberDefinition
/// This class functions as a means to represent the definition of an NSNumber class.
///
/// This class is typically used when you need to work with a collection of NSNumber objects in classes like SCArrayOfObjectsSection and SCArrayOfObjectsModel. The class automatically generates only one property definition representing how the NSNumber automatically generated UI should be defined.

@interface SCNumberDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCNumberDefinition.
+ (instancetype)definition;

#pragma mark - Configuration

/// The property definition representing the NSDate automatically generated UI element.
@property (nonatomic, readonly) SCPropertyDefinition *numberPropertyDefinition;


@end

NS_ASSUME_NONNULL_END
