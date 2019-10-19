/*
 *  SCArrayOfObjectsModel+CoreData.m
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCArrayOfObjectsModel+CoreData.h"

#import "SCCoreDataStore.h"


@implementation SCArrayOfObjectsModel (STVCoreData)

+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition
{
    return [[[self class] alloc] initWithTableView:tableView entityDefinition:definition];
}

+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    return [[[self class] alloc] initWithTableView:tableView entityDefinition:definition filterPredicate:predicate];
}


- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition
{
    return [self initWithTableView:tableView entityDefinition:definition filterPredicate:nil];
}

- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate
{
    SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext defaultEntityDefinition:definition];
    
    self = [self initWithTableView:tableView dataStore:store];
    if(self)
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
