/*
 *  SCArrayOfObjectsSection+CoreData.m
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCArrayOfObjectsSection+CoreData.h"

#import "SCCoreDataStore.h"


@implementation SCArrayOfObjectsSection (STVCoreData)

+ (instancetype)sectionWithHeaderTitle:(NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition
{
    return [[[self class] alloc] initWithHeaderTitle:sectionHeaderTitle entityDefinition:definition];
}

+ (instancetype)sectionWithHeaderTitle:(NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    return [[[self class] alloc] initWithHeaderTitle:sectionHeaderTitle entityDefinition:definition filterPredicate:predicate];
}


- (instancetype)initWithHeaderTitle:(NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition
{
    return [self initWithHeaderTitle:sectionHeaderTitle entityDefinition:definition filterPredicate:nil];
}

- (instancetype)initWithHeaderTitle:(NSString *)sectionHeaderTitle entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext defaultEntityDefinition:definition];
    
    if( (self = [self initWithHeaderTitle:sectionHeaderTitle dataStore:store]) )
    {
        if(predicate)
        {
            self.dataFetchOptions.filter = TRUE;
            self.dataFetchOptions.filterPredicate = predicate;
        }
    }
    return self;
}

@end
