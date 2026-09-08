/*
 *  SCCoreDataStore.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCEntityDefinition.h"
#import "SCDataStore.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCCoreDataStore
/// SCCoreDataStore is an SCDataStore subclass that encapsulates Core Data persistent storage, providing means for the SC framework to communicate with this storage to fetch, add, update and remove data objects.
///
/// - Note: It is very rare when you'll need to create an SCCoreDataStore instance yourself, as it's typically automatically created for you when you use the SCEntityDefinition data definition. For example, when you use the SCArrayOfObjectsSection initializer method called [SCArrayOfObjectsSection sectionWithHeaderTitle:entityDefinition:], SCArrayOfObjectsSection automatically sets its dataStore property by calling your entityDefinition's [SCDataDefinition generateCompatibleDataStore:] method.
///
/// - Note: For more information on data stores, check out the SCDataStore base class documentation.
@interface SCCoreDataStore : SCDataStore

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCCoreDataStore given a managed object context and an entity definition.
/// - Parameter context: The Core Data managed object context.
/// - Parameter definition: The entity definition of the entity objects in the data store.
+ (instancetype)storeWithManagedObjectContext:(nullable NSManagedObjectContext *)context defaultEntityDefinition:(nullable SCEntityDefinition *)definition;

/// Allocates and returns an initialized SCCoreDataStore given a managed object context and an NSMutableSet to bind the data store to.
/// - Parameter context: The Core Data managed object context.
/// - Parameter set: An NSMutableSet that the data store will bind to and fetch all its objects from.
/// - Parameter definition: The entity definition of the entity objects in the bound set.
/// - Parameter ownsStoreObjects: When TRUE, objects removed from the bound set will also be removed from the data store.
+ (instancetype)storeWithManagedObjectContext:(nullable NSManagedObjectContext *)context boundSet:(nullable NSMutableSet<NSManagedObject *> *)set boundSetEntityDefinition:(nullable SCEntityDefinition *)definition boundSetOwnsStoreObjects:(BOOL)ownsStoreObjects;

/// Returns an initialized SCCoreDataStore given a managed object context and an entity definition.
/// - Parameter context: The Core Data managed object context.
/// - Parameter definition: The entity definition of the entity objects in the data store.
- (instancetype)initWithManagedObjectContext:(nullable NSManagedObjectContext *)context defaultEntityDefinition:(nullable SCEntityDefinition *)definition;

/// Returns an initialized SCCoreDataStore given a managed object context and an NSMutableSet to bind the data store to.
/// - Parameter context: The Core Data managed object context.
/// - Parameter set: An NSMutableSet that the data store will bind to and fetch all its objects from.
/// - Parameter definition: The entity definition of the entity objects in the bound set.
/// - Parameter ownsStoreObjects: When TRUE, objects removed from the bound set will also be removed from the data store.
- (instancetype)initWithManagedObjectContext:(nullable NSManagedObjectContext *)context boundSet:(nullable NSMutableSet<NSManagedObject *> *)set boundSetEntityDefinition:(nullable SCEntityDefinition *)definition boundSetOwnsStoreObjects:(BOOL)ownsStoreObjects;


#pragma mark - Configuration

/// The managed object context associated with the store.
/// Nil until configured. A commit with no context succeeds without saving.
@property (nonatomic, strong, nullable) NSManagedObjectContext *managedObjectContext;

/// The NSMutableSet the store is bound to. This property is automatically set by bindStoreToPropertyName:forObject:withDefinition: method.
@property (nonatomic, strong, readonly, nullable) NSMutableSet<NSManagedObject *> *boundSet;

/// The NSMutableOrderedSet the store is bound to. This property is automatically set by bindStoreToPropertyName:forObject:withDefinition: method.
@property (nonatomic, strong, readonly, nullable) NSMutableOrderedSet<NSManagedObject *> *boundOrderedSet;

/// When TRUE, objects removed from boundSet will also be removed from the data store.
@property (nonatomic, readonly) BOOL boundSetOwnsStoreObjects;

/// The most recent Core Data commit error, or nil after a successful commit.
@property (nonatomic, strong, readonly, nullable) NSError *lastCommitError;

/// Optional callback used by the compatibility commitData method when a save fails.
@property (nonatomic, copy, nullable) SCDataStoreFailure_Block commitFailure;

/// Saves the managed object context and returns the failure instead of terminating the host application.
/// - Parameter error: On return, an error describing a failure, or `nil` if the operation succeeds.
///   Pass NULL when the error is not needed.
- (BOOL)commitDataWithError:(NSError *_Nullable *_Nullable)error;


@end

NS_ASSUME_NONNULL_END
