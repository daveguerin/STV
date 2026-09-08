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
/// This class functions as a means to represent the definition of an NSDate class.
///
/// This class is typically used when you need to work with a collection of NSDate objects in classes like SCArrayOfObjectsSection and SCArrayOfObjectsModel. The class automatically generates only one property definition representing how the NSDate automatically generated UI should be defined.

@interface SCDateDefinition : SCDataDefinition

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCDateDefinition.
+ (instancetype)definition;

#pragma mark - Configuration

/// The property definition representing the NSDate automatically generated UI element.
@property (nonatomic, readonly) SCPropertyDefinition *datePropertyDefinition;


@end

NS_ASSUME_NONNULL_END
