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
/// A synchronous store for Core Data objects and mutable relationship collections.
///
/// SCEntityDefinition creates this store through generateCompatibleDataStore.
/// Unbound fetches concatenate results for every registered entity, using each
/// definition's context. Bound fetches use the bound set or ordered set instead.
/// Use SCCoreDataFetchOptions for sorting, filtering, and batching: ordinary
/// SCDataFetchOptions only contributes its predicate to the generated defaults.
/// Ordered bound sets preserve their own order. Bound collections ignore batching;
/// unbound batch limits apply separately to each entity, not the combined array.
/// Core Data sort descriptors are used even when the options' sort flag is NO.
/// Fetch errors are not exposed through an NSError result.
///
/// Creating an object immediately inserts it into the context and any bound
/// collection, but tracks it as unaccepted until insertObject: is called. Finish
/// or discard new-object editing before saving. Saves can trigger discarding of
/// tracked uninserted objects.
///
/// - Warning: Except for commitDataWithError:, operations do not dispatch to a
///   context queue. Use consistent contexts and obey their queue confinement.
///   The store observes will-save notifications from all contexts, not just its
///   own, and discards its pending objects on the notification's posting queue.
/// - SeeAlso: ``SCDataStore``, ``SCCoreDataFetchOptions``
@interface SCCoreDataStore : SCDataStore

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCCoreDataStore given a managed object context and an entity definition.
/// - Parameter context: The context for creation, deletion, and saving, or nil
///   to configure later. Keep it consistent with the entity definitions;
///   unbound fetches use each definition's context instead.
/// - Parameter definition: The default entity definition, or nil to configure later.
+ (instancetype)storeWithManagedObjectContext:(nullable NSManagedObjectContext *)context defaultEntityDefinition:(nullable SCEntityDefinition *)definition;

/// Allocates and returns an initialized SCCoreDataStore given a managed object context and an NSMutableSet to bind the data store to.
/// - Parameter context: The context for creation, deletion, and saving, or nil
///   to configure later. Keep it consistent with the entity definitions;
///   unbound fetches use each definition's context instead.
/// - Parameter set: The retained mutable set to fetch and modify, or nil for
///   unbound entity fetching. The set is not copied.
/// - Parameter definition: The bound objects' entity definition, or nil to
///   configure before creating objects.
/// - Parameter ownsStoreObjects: Whether deletion through this store also deletes
///   the object from the context. Direct set mutations are not observed.
+ (instancetype)storeWithManagedObjectContext:(nullable NSManagedObjectContext *)context boundSet:(nullable NSMutableSet<NSManagedObject *> *)set boundSetEntityDefinition:(nullable SCEntityDefinition *)definition boundSetOwnsStoreObjects:(BOOL)ownsStoreObjects;

/// Returns an initialized SCCoreDataStore given a managed object context and an entity definition.
/// - Parameter context: The context for creation, deletion, and saving, or nil
///   to configure later. Keep it consistent with the entity definitions;
///   unbound fetches use each definition's context instead.
/// - Parameter definition: The default entity definition, or nil to configure later.
- (instancetype)initWithManagedObjectContext:(nullable NSManagedObjectContext *)context defaultEntityDefinition:(nullable SCEntityDefinition *)definition;

/// Returns an initialized SCCoreDataStore given a managed object context and an NSMutableSet to bind the data store to.
/// - Parameter context: The context for creation, deletion, and saving, or nil
///   to configure later. Keep it consistent with the entity definitions;
///   unbound fetches use each definition's context instead.
/// - Parameter set: The retained mutable set to fetch and modify, or nil for
///   unbound entity fetching. The set is not copied.
/// - Parameter definition: The bound objects' entity definition, or nil to
///   configure before creating objects.
/// - Parameter ownsStoreObjects: Whether deletion through this store also deletes
///   the object from the context. Direct set mutations are not observed.
- (instancetype)initWithManagedObjectContext:(nullable NSManagedObjectContext *)context boundSet:(nullable NSMutableSet<NSManagedObject *> *)set boundSetEntityDefinition:(nullable SCEntityDefinition *)definition boundSetOwnsStoreObjects:(BOOL)ownsStoreObjects;


#pragma mark - Configuration

/// The context used for creating, deleting, and saving managed objects.
///
/// Initially nil. Unbound fetches use the registered entity definitions' contexts,
/// so configure consistent contexts. A commit with no context succeeds without
/// saving. Changing this property does not update existing entity definitions.
@property (nonatomic, strong, nullable) NSManagedObjectContext *managedObjectContext;

/// The retained mutable set supplied at initialization or obtained by binding.
///
/// Nil when unbound or bound to an ordered set. Store fetches and collection writes
/// use this set; direct mutations do not invoke the store's deletion logic.
@property (nonatomic, strong, readonly, nullable) NSMutableSet<NSManagedObject *> *boundSet;

/// The mutable ordered relationship set obtained by binding, or nil.
///
/// Fetched results preserve this set's order rather than applying sort descriptors.
/// It is populated by bindStoreToPropertyName:forObject:withDefinition: for a
/// supported ordered relationship.
@property (nonatomic, strong, readonly, nullable) NSMutableOrderedSet<NSManagedObject *> *boundOrderedSet;

/// Whether store-driven deletion also deletes a bound object from its context.
///
/// Applies to either bound collection. Defaults to NO, or the initializer's value.
/// Binding sets it to YES for SCArrayOfObjectsAttributes and NO for other
/// attributes. Direct collection removal is not observed. When unbound,
/// deleteObject: deletes from the context regardless of this flag.
@property (nonatomic, readonly) BOOL boundSetOwnsStoreObjects;

/// The error from the most recent commitDataWithError: call, initially nil.
///
/// Updated by commitData too. Cleared on success, including no-context and
/// no-change commits. Saves made directly on a context do not update it.
@property (nonatomic, strong, readonly, nullable) NSError *lastCommitError;

/// Optional callback invoked by commitData after a failed save. Defaults to nil.
///
/// The block receives the save error on the queue that called commitData, after
/// the context save has finished. Calling commitDataWithError: directly does not
/// invoke this callback. Avoid strongly capturing the store in its retained block.
@property (nonatomic, copy, nullable) SCDataStoreFailure_Block commitFailure;

/// Synchronously saves pending context changes and records the save error.
///
/// Uses performBlockAndWait: on the context. No context or no changes is a
/// successful no-op. Updates lastCommitError but does not invoke commitFailure.
/// A save can trigger the store's pending-object discard notification handling.
/// - Parameter error: Optional output receiving the save error, or nil on success.
///   Pass NULL when the error is not needed.
/// - Returns: YES on success or a no-op, NO if the context save fails.
- (BOOL)commitDataWithError:(NSError *_Nullable *_Nullable)error;


@end

NS_ASSUME_NONNULL_END
