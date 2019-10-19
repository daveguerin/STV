/*
 *  SCObjectSelectionAttributes+CoreData.h
 *  Sensible TableView
 *  Version: 6.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCPropertyAttributes.h"

#import "SCEntityDefinition.h"



@interface SCObjectSelectionAttributes (STVCoreData)

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Creation and Initialization
//////////////////////////////////////////////////////////////////////////////////////////

/** Allocates and returns an initialized SCObjectSelectionAttributes.
 *
 *	@param entityDefinition The entity definition of the entity whose objects are to be presented for selection.
 *  @param predicate The predicate used to filter the selection objects. Set to nil to ignore.
 *	@param allowMultipleSel Determines if the generated selection control allows multiple selection.
 *  @warning allowMultipleSel should only be set to TRUE if relationship is many to many.
 *	@param allowNoSel Determines if the generated selection control allows no selection.
 */
+ (instancetype)attributesWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                             usingPredicate:(NSPredicate *)predicate
                     allowMultipleSelection:(BOOL)allowMultipleSel
                           allowNoSelection:(BOOL)allowNoSel;

/** Returns an initialized SCObjectSelectionAttributes.
 *
 *	@param entityDefinition The entity definition of the entity whose objects are to be presented for selection.
 *  @param predicate The predicate used to filter the selection objects. Set to nil to ignore.
 *	@param allowMultipleSel Determines if the generated selection control allows multiple selection.
 *  @warning allowMultipleSel should only be set to TRUE if relationship is many to many.
 *	@param allowNoSel Determines if the generated selection control allows no selection.
 */
- (instancetype)initWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition
                       usingPredicate:(NSPredicate *)predicate
               allowMultipleSelection:(BOOL)allowMultipleSel
                     allowNoSelection:(BOOL)allowNoSel;



@end
