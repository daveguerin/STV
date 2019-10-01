/*
 *  SCArrayOfObjectsModel+CoreData.h
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */



#import "SCTableViewModel.h"

#import "SCEntityDefinition.h"


@interface SCArrayOfObjectsModel (STVCoreData)

/** 
 Allocates and returns an initialized SCArrayOfObjectsModel given a UITableView 
 and an entity definition. 
 
 @note This method creates a model with all the objects that exist in the definition's managedObjectContext. 
 
 @param tableView The UITableView to be bound to the model. 
 @param definition The entity definition of the objects.
 */
+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;

/** Allocates and returns an initialized 'SCArrayOfObjectsModel' given a UITableView 
 and an entity definition. 
 *
 *	@param tableView The UITableView to be bound to the model.
 *	@param definition The entity definition of the objects.
 *	@param predicate The predicate that will be used to filter the fetched objects.
 */
+ (instancetype)modelWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate;


/** 
 Returns an initialized SCArrayOfObjectsModel given a UITableView 
 and an entity definition. 
 
 @note This method creates a model with all the objects that exist in the definition's managedObjectContext. 
 
 @param tableView The UITableView to be bound to the model. 
 @param definition The entity definition of the objects.
 */
- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition;


/** Returns an initialized 'SCArrayOfObjectsModel' given a UITableView 
 and an entity definition. 
 *
 *	@param tableView The UITableView to be bound to the model. 
 *	@param definition The entity definition of the objects.
 *	@param predicate The predicate that will be used to filter the fetched objects.
 */
- (instancetype)initWithTableView:(UITableView *)tableView entityDefinition:(SCEntityDefinition *)definition filterPredicate:(NSPredicate *)predicate;

@end
