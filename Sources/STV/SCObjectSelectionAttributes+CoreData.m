/*
 *  SCObjectSelectionAttributes+CoreData.m
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCObjectSelectionAttributes+CoreData.h"

#import "SCCoreDataStore.h"


@implementation SCObjectSelectionAttributes (STVCoreData)

+ (instancetype)attributesWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                             usingPredicate:(NSPredicate *)predicate
                     allowMultipleSelection:(BOOL)allowMultipleSel
                           allowNoSelection:(BOOL)allowNoSel
{
	return [[[self class] alloc] initWithObjectsEntityDefinition:entityDefinition
                                                                 usingPredicate:(NSPredicate *)predicate
                                                         allowMultipleSelection:allowMultipleSel
                                                               allowNoSelection:allowNoSel];
}

- (instancetype)initWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                       usingPredicate:(NSPredicate *)predicate
               allowMultipleSelection:(BOOL)allowMultipleSel
                     allowNoSelection:(BOOL)allowNoSel
{
	if( (self=[self init]) )
	{
		self.selectionItemsStore = [SCCoreDataStore storeWithManagedObjectContext:entityDefinition.managedObjectContext defaultEntityDefinition:entityDefinition];
        
        if(predicate)
        {
            self.selectionItemsFetchOptions.filter = YES;
            self.selectionItemsFetchOptions.filterPredicate = predicate;
        }
		self.allowMultipleSelection = allowMultipleSel;
		self.allowNoSelection = allowNoSel;
	}
	return self;
}

@end
