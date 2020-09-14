/*
 *  SCArrayOfObjectsCell+CoreData.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCArrayOfObjectsCell+CoreData.h"

#import "SCCoreDataStore.h"



@implementation SCArrayOfObjectsCell (STVCoreData)

+ (instancetype)cellWithEntityDefinition:(SCEntityDefinition *)definition
{
	return [[[self class] alloc] initWithEntityDefinition:definition];
}

+ (instancetype)cellWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects
{
	return [[[self class] alloc] initWithBoundItemsSet:cellItemsSet boundSetEntityDefinition:definition boundSetOwnsObjects:ownsObjects];
}

- (instancetype)initWithEntityDefinition:(SCEntityDefinition *)definition
{
	SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext defaultEntityDefinition:definition];
	
	self = [self initWithDataStore:store];
    
    return self;
}

- (instancetype)initWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects
{
	SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext boundSet:cellItemsSet boundSetEntityDefinition:definition boundSetOwnsStoreObjects:ownsObjects];
    
	self = [self initWithDataStore:store];
    
    return self;
}

@end
