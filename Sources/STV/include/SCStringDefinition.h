/*
 *  SCStringDefinition.h
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataDefinition.h"


/****************************************************************************************/
/*	class SCStringDefinition	*/
/****************************************************************************************/ 
/**	
 This class functions as a means to represent the definition of an NSString class.
 
 This class is typically used when you need to work with a collection of NSString objects in classes like SCArrayOfObjectsSection and SCArrayOfObjectsModel. The class automatically generates only one property definition representing how the NSString automatically generated UI should be defined.
 */

@interface SCStringDefinition : SCDataDefinition

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Creation and Initialization
//////////////////////////////////////////////////////////////////////////////////////////

/** Allocates and returns an initialized SCStringDefinition. */
+ (instancetype)definition;

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Configuration
//////////////////////////////////////////////////////////////////////////////////////////

/** The property definition representing the string automatically generated UI element. */
@property (nonatomic, readonly) SCPropertyDefinition *stringPropertyDefinition;


@end
