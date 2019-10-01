/*
 *  SCArrayOfObjectsCell+CoreData.h
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCTableViewCell.h"

#import "SCEntityDefinition.h"

@interface SCArrayOfObjectsCell (STVCoreData)

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Creation and Initialization
//////////////////////////////////////////////////////////////////////////////////////////

/** 
 Allocates and returns an initialized SCArrayOfObjectsCell given a header title and an entity definition. 
 
 @note This method creates a cell with all the objects that exist in classDefinition's entity's managedObjectContext. To create a cell with only a subset of these objects, consider using the other cell initializers.
 
 @param definition The entity definition of the objects in the objects set.
 */
+ (instancetype)cellWithEntityDefinition:(SCEntityDefinition *)definition;

/** Allocates and returns an initialized SCArrayOfObjectsCell given a mutable set of objects. 
 
 @warning This method should only be used to create a cell with the contents of a Core Data relationship.
 
 @param cellItemsSet A mutable set of objects that the cell will use to generate its cells.
 @param definition The class definition of the entity of the objects in the objects set.
 */
+ (instancetype)cellWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects;


/** 
 Returns an initialized SCArrayOfObjectsCell given a header title and an entity definition. 
 
 @note This method creates a cell with all the objects that exist in classDefinition's entity's managedObjectContext. To create a cell with only a subset of these objects, consider using the other cell initializers.
 
 @param definition The entity definition of the objects in the objects set.
 */
- (instancetype)initWithEntityDefinition:(SCEntityDefinition *)definition;

/** Returns an initialized SCArrayOfObjectsCell given a mutable set of objects. 
 
 @warning This method should only be used to create a cell with the contents of a Core Data relationship.
 
 @param cellItemsSet A mutable set of objects that the cell will use to generate its cells.
 @param definition The class definition of the entity of the objects in the objects set.
 */
- (instancetype)initWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects;

@end


