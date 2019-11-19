/*
 *  SCCoreDataFetchOptions.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDataFetchOptions.h"

/****************************************************************************************/
/*	class SCCoreDataFetchOptions	*/
/****************************************************************************************/ 
/**	
 SCCoreDataFetchOptions further extends the SCDataFetchOptions subclass to control how data is fetched from SCCoreDataStore. 
 
 @note For more information on fetch options, check out the SCDataFetchOptions documentation.
 */

@interface SCCoreDataFetchOptions : SCDataFetchOptions
{
    NSString *_orderAttributeName;
}

/**	The name of the attribute that will be used to determine the fetch order of the objects. 
 
 @note This value is automatically set by the framework to the value in [SCEntityDefinition orderAttributeName], which makes you rarely need to set this value yourself.
 */
@property (nonatomic, copy) NSString *orderAttributeName;

@end
