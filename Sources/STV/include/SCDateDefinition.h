/*
 *  SCDateDefinition.h
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


/****************************************************************************************/
/*	class SCDateDefinition	*/
/****************************************************************************************/ 
/**	
 This class functions as a means to represent the definition of an NSDate class.
 
 This class is typically used when you need to work with a collection of NSDate objects in classes like SCArrayOfObjectsSection and SCArrayOfObjectsModel. The class automatically generates only one property definition representing how the NSDate automatically generated UI should be defined.
 */

@interface SCDateDefinition : SCDataDefinition

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Creation and Initialization
//////////////////////////////////////////////////////////////////////////////////////////

/** Allocates and returns an initialized SCDateDefinition. */
+ (instancetype)definition;

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Configuration
//////////////////////////////////////////////////////////////////////////////////////////

/** The property definition representing the NSDate automatically generated UI element. */
@property (nonatomic, readonly) SCPropertyDefinition *datePropertyDefinition;


@end
