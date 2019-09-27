/*
 *  SCUserDefaultsStore.h
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDataStore.h"


/****************************************************************************************/
/*	class SCUserDefaultsStore	*/
/****************************************************************************************/ 
/**	
 SCUserDefaultsStore is an SCDataStore subclass that encapsulates the NSUserDefaults storage, providing means for the SC framework to communicate with this storage to fetch, add, update and remove user defaults.
 
 @note It is very rare when you'll need to create an SCUserDefaultsStore instance yourself, as it's typically automatically created for you when you use SCUserDefaultsDefinition. For example, when you use the SCTableViewModel method called [SCTableViewModel generateSectionsForUserDefaultsDefinition:], the model automatically sets its sections' dataStore property by calling your userDefaultsDefinition's [SCDataDefinition generateCompatibleDataStore:] method.
 
 @note For more information on data stores, check out the SCDataStore base class documentation.
 */
@interface SCUserDefaultsStore : SCDataStore

/** The user defaults object managed by the data store. */
@property (nonatomic, readonly) NSUserDefaults *standardUserDefaultsObject;

@end
