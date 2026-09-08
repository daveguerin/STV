/*
 *  SCDataStore.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright © 2019-2026 dgApps. All rights reserved.
 *
 */


#import "SCGlobals.h"
#import "SCDataDefinition.h"
#import "SCDataFetchOptions.h"


NS_ASSUME_NONNULL_BEGIN

/* Data store notifications (used internally) */
/// Posted synchronously before tracked uninserted objects are discarded.
///
/// The notification object is the store, with no userInfo dictionary. Nothing is
/// posted when the store has no tracked uninserted objects.
extern NSString *const SCDataStoreWillDiscardAllUninsertedObjectsNotification;


/// The execution modes available to an STV data store.
typedef NS_ENUM(NSInteger, SCStoreMode) {
    /// Performs data-store operations synchronously.
    SCStoreModeSynchronous,
    /// Performs data-store operations asynchronously.
    SCStoreModeAsynchronous
};
/// A block invoked after a data-store fetch succeeds.
/// Results may be nil when forwarded from a post-fetch completion handler.
typedef void (^SCDataStoreFetchSuccess_Block)(NSArray<id> *_Nullable results);
/// A block invoked after a data-store insert succeeds.
typedef void (^SCDataStoreInsertSuccess_Block)(void);
/// A block invoked after a data-store update succeeds.
typedef void (^SCDataStoreUpdateSuccess_Block)(void);
/// A block invoked after a data-store delete succeeds.
typedef void (^SCDataStoreDeleteSuccess_Block)(void);
/// A block invoked when a data-store operation fails.
/// The abstract asynchronous methods supply nil when no error is available.
typedef void (^SCDataStoreFailure_Block)(NSError *_Nullable error);
/// An optional retry decision for asynchronous store subclasses.
///
/// By convention, YES requests a retry when connected; NO requests failure. The
/// base store never calls this block or implements retries. Concrete asynchronous
/// stores define connection monitoring and callback queues.
typedef BOOL (^SCNoConnection_Block)(void);
/// Completes post-fetch processing with replacement results or an error.
///
/// A nil error selects success even if results is nil. A nonnil error selects
/// failure and ignores results. Call once per fetch.
typedef void (^SCPostFetchAsyncronousCompletionHandler_Block)(NSArray<id> *_Nullable results, NSError *_Nullable error);
/// Processes nullable fetch results before delivery to the caller.
///
/// The block receives results and a nonnull completion handler. Call completion
/// once with replacement results and a nil error, or with an error. There is no
/// implicit queue dispatch or automatic completion.
typedef void (^SCPostFetchAsyncronousAction_Block)(NSArray<id> *_Nullable results, SCPostFetchAsyncronousCompletionHandler_Block completionHandler);


#pragma mark - SCDataStore
/// SCDataStore is an abstract base class that can encapsulate any kind of data storage, providing means for the SC framework to communicate with this storage to fetch, add, update and remove data.
///
/// Bundled stores support mutable arrays, Core Data, and standard user defaults.
/// Subclass SCDataStore for other backends. Setting storeMode alone does not
/// supply asynchronous operations.
///
/// Sample use:
/// ```objective-c
/// // context is a configured main-queue context; TaskEntityDef describes its
/// // TaskEntity entity. Run this UI setup on the main thread.
/// SCCoreDataStore *coreDataStore = [SCCoreDataStore storeWithManagedObjectContext:context defaultEntityDefinition:TaskEntityDef];
/// SCArrayOfObjectsSection *tasksSection = [SCArrayOfObjectsSection sectionWithHeaderTitle:@"Task Objects" dataStore:coreDataStore];
/// [self.tableViewModel addSection:tasksSection];
/// ```
///
/// - SeeAlso: ``SCArrayStore``, ``SCCoreDataStore``, ``SCUserDefaultsStore``
///
/// - Note: Configure an appropriate data definition before creating objects.
///   A newly initialized store may have no default data definition.
///
/// - Warning: SCDataStore is an abstract base class and should not be directly instantiated.
@interface SCDataStore : NSObject {
  @protected
    // Backing storage used by subclasses; use properties for ordinary access.
    NSMutableDictionary<NSString *, SCDataDefinition *> *_dataDefinitions;
    NSMutableArray<id> *_uninsertedObjects;
    NSObject *_Nullable _boundObject;
    NSString *_Nullable _boundPropertyName;
}

#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCDataStore given a default SCDataDefinition of the data stored.
/// - Parameter definition: The default definition, or nil to leave it unset.
+ (instancetype)storeWithDefaultDataDefinition:(nullable SCDataDefinition *)definition;

/// Returns an initialized SCDataStore given a default SCDataDefinition of the data stored.
/// - Parameter definition: The default definition, or nil to leave it unset.
- (instancetype)initWithDefaultDataDefinition:(nullable SCDataDefinition *)definition;


#pragma mark - Configuration

/// Selects which data-access methods the framework calls. Defaults to synchronous.
///
/// Asynchronous mode requires subclass implementations of asynchronous methods.
/// This property does not dispatch operations to another queue.
@property (nonatomic, readwrite) SCStoreMode storeMode;

/// The retained default definition, initially nil.
///
/// Assignment also attempts to register it with addDataDefinition:. Clearing or
/// replacing the default does not remove registered definitions. Register other
/// structures using addDataDefinition:.
@property (nonatomic, strong, nullable) SCDataDefinition *defaultDataDefinition;

/// Whether the base KVC setter leaves nil values as nil. Defaults to YES.
///
/// When NO, that setter substitutes NSNull; a property defined as scalar instead
/// receives numeric zero. Subclass setters and NSUbiquitousKeyValueStore writes
/// can bypass this conversion.
@property (nonatomic, readwrite) BOOL supportsNilValues;

/// Registers a definition by its nonnil dataStructureName.
///
/// - Parameter definition: The definition to retain. Nil, unnamed definitions,
///   and definitions whose name is already registered are ignored; registration
///   does not replace an existing definition.
- (void)addDataDefinition:(nullable SCDataDefinition *)definition;

/// Looks up the definition used to interpret an object.
///
/// - Parameter object: The object to describe.
/// - Returns: The default definition in the base implementation. Array and Core
///   Data stores look up registered class or entity names instead, returning nil
///   when no definition matches.
- (nullable SCDataDefinition *)definitionForObject:(NSObject *)object;


#pragma mark - Synchronous Data Access

/// Creates an object using the store's default data definition.
///
/// Array stores allocate a supported class, dictionary, or basic value without
/// adding it to the backing array. Core Data inserts into the context and bound
/// collection immediately, tracking the object as not yet accepted by the store.
///
/// - Returns: The created object, or nil in the base implementation or for an
///   unsupported definition. Invalid Core Data configuration can raise exceptions.
/// - Warning: Accept the new object with insertObject: or discard it with
///   discardUninsertedObject: before finishing the editing workflow.
- (nullable NSObject *)createNewObject;

/// Creates an object using the store's given data definition.
///
/// Array stores allocate a supported class, dictionary, or basic value without
/// adding it to the backing array. Core Data inserts into the context and bound
/// collection immediately, tracking the object as not yet accepted by the store.
///
/// - Parameter definition: The definition to instantiate. Nil is unsupported and
///   does not substitute the default definition.
///
/// - Returns: The created object, or nil in the base implementation or for an
///   unsupported definition. Invalid Core Data configuration can raise exceptions.
/// - Warning: Accept the new object with insertObject: or discard it with
///   discardUninsertedObject: before finishing the editing workflow.
- (nullable NSObject *)createNewObjectWithDefinition:(nullable SCDataDefinition *)definition;

/// Discards an object created by the store but not accepted with insertObject:.
///
/// - Parameter object: The uninserted object to discard.
/// - Returns: YES in the base implementation, which does nothing. Concrete stores
///   remove tracking or delete/unbind the object. Core Data bound collections use
///   their ownership setting to decide whether to delete the managed object.
- (BOOL)discardUninsertedObject:(NSObject *)object;

/// Inserts an object as implemented by the concrete subclass.
///
/// Array stores append to their configured array. Core Data accepts tracked new
/// objects into its context or bound collection without saving the context.
/// - Parameter object: The object to insert.
/// - Returns: NO in the base implementation; otherwise the subclass result.
///   A YES result is not a persistence guarantee.
- (BOOL)insertObject:(NSObject *)object;

/// Inserts an object at an explicit position in an ordered store.
///
/// - Parameter object: The object to insert.
/// - Parameter order: A valid insertion index for the backing collection.
/// - Returns: NO in the base implementation. SCArrayStore inserts at this index;
///   invalid indexes raise an exception. SCCoreDataStore does not implement this
///   insertion variant.
- (BOOL)insertObject:(NSObject *)object atOrder:(NSUInteger)order;

/// Moves an object within an ordered store.
///
/// - Parameter object: The object to move.
/// - Parameter toOrder: A valid destination index in the applicable ordering.
/// - Parameter subsetArray: The displayed subset for Core Data attribute-based
///   ordering. Array stores and ordered Core Data sets ignore it and use backing
///   collection indexes instead.
/// - Returns: NO in the base implementation; otherwise the subclass result.
/// - Warning: Invalid collection indexes can raise an exception.
- (BOOL)changeOrderForObject:(NSObject *)object toOrder:(NSUInteger)toOrder subsetArray:(NSArray<id> *)subsetArray;

/// Subclass hook to update an object in storage.
///
/// The base implementation does nothing. Bundled stores inherit this method;
/// changes to bound objects are made by property setters instead.
/// - Parameter object: The object to update.
/// - Returns: NO in the base implementation.
- (BOOL)updateObject:(NSObject *)object;

/// Deletes an object according to the concrete store's collection semantics.
///
/// Array stores remove the identical object from the backing array. Core Data
/// unbinds it and, depending on collection ownership, may mark it for deletion.
/// - Parameter object: The object to delete.
/// - Returns: NO in the base implementation; otherwise the subclass result.
///   A YES result does not imply persistent changes have been saved.
- (BOOL)deleteObject:(NSObject *)object;

/// Fetches objects according to the concrete store's options handling.
///
/// - Parameter fetchOptions: Options, or nil for no array transformations or for
///   the Core Data definition's default options. User-defaults stores ignore it.
/// - Returns: Nil in the base implementation. Concrete stores return their fetched
///   collection; consult the subclass for error handling and batching behavior.
/// - SeeAlso: ``SCDataFetchOptions``
- (nullable NSArray<id> *)fetchObjectsWithOptions:(nullable SCDataFetchOptions *)fetchOptions;

/// Reads property values, with optional defaults for a missing single value.
///
/// Basic string, number, and date objects return themselves regardless of the
/// name. Other objects use sensible key-path lookup, or objectForKey: for
/// NSUbiquitousKeyValueStore. Lookup exceptions are caught and logged.
/// - Parameter propertyName: A key path, semicolon-separated paths, or nil.
/// - Parameter object: The object to read.
/// - Returns: A single value or nil, or an array for multiple paths with NSNull
///   placeholders for missing values. A missing single value may be supplied by
///   defaultsDictionary. Nil names on nonbasic objects return nil.
- (nullable NSObject *)valueForPropertyName:(nullable NSString *)propertyName inObject:(NSObject *)object;

/// Formats property values using their object descriptions.
///
/// - Parameter propertyName: A key path, semicolon-separated paths, or nil.
/// - Parameter object: The object to read; basic values are formatted directly.
/// - Parameter delimiter: Separator for array values, or nil to concatenate.
/// - Returns: Nil if lookup returns nil; otherwise a string. NSNull array entries
///   are skipped, but a later nonnull entry receives a delimiter when its source
///   index is not zero, so leading delimiters are possible.
- (nullable NSString *)stringValueForPropertyName:(nullable NSString *)propertyName inObject:(NSObject *)object separateValuesUsingDelimiter:(nullable NSString *)delimiter;

/// Writes a property value using the concrete store's binding behavior.
///
/// The base implementation ignores basic values and unavailable properties. For
/// ordinary KVC writes, nil becomes NSNull if supportsNilValues is NO, or numeric
/// zero for a property defined as scalar. Setter exceptions propagate. Array
/// stores replace a basic value in their backing array instead. Core Data can
/// create missing intermediate objects along a to-one relationship key path.
/// - Parameter value: The new value, including nil when the binding supports it.
/// - Parameter propertyName: The property key path to set.
/// - Parameter object: The object containing the property.
- (void)setValue:(nullable NSObject *)value forPropertyName:(NSString *)propertyName inObject:(NSObject *)object;


#pragma mark - Asynchronous Data Access

/// Subclass hook for an asynchronous insert operation.
///
/// The base immediately calls a nonnil failure block with nil error on the calling
/// queue. It never calls success or noConnection. Concrete asynchronous subclasses
/// define scheduling and retry behavior.
///
/// - Parameter object: The object to insert.
/// - Parameter success_block: Optional success callback with no arguments.
/// - Parameter failure_block: Optional failure callback receiving a nullable error.
/// - Parameter noConnection_block: Optional retry decision. By convention, YES
///   requests retry when connected and NO requests failure; unused by the base.
- (void)asynchronousInsertObject:(NSObject *)object success:(nullable SCDataStoreInsertSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Subclass hook for an asynchronous update operation.
///
/// The base immediately calls a nonnil failure block with nil error on the calling
/// queue. It never calls success or noConnection. Concrete asynchronous subclasses
/// define scheduling and retry behavior.
///
/// - Parameter object: The object to update.
/// - Parameter success_block: Optional success callback with no arguments.
/// - Parameter failure_block: Optional failure callback receiving a nullable error.
/// - Parameter noConnection_block: Optional retry decision. By convention, YES
///   requests retry when connected and NO requests failure; unused by the base.
- (void)asynchronousUpdateObject:(NSObject *)object success:(nullable SCDataStoreUpdateSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Subclass hook for an asynchronous delete operation.
///
/// The base immediately calls a nonnil failure block with nil error on the calling
/// queue. It never calls success or noConnection. Concrete asynchronous subclasses
/// define scheduling and retry behavior.
///
/// - Parameter object: The object to delete.
/// - Parameter success_block: Optional success callback with no arguments.
/// - Parameter failure_block: Optional failure callback receiving a nullable error.
/// - Parameter noConnection_block: Optional retry decision. By convention, YES
///   requests retry when connected and NO requests failure; unused by the base.
- (void)asynchronousDeleteObject:(NSObject *)object success:(nullable SCDataStoreDeleteSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Subclass hook for an asynchronous fetch operation.
///
/// The base immediately calls a nonnil failure block with nil error on the calling
/// queue. It never calls success or noConnection. Concrete asynchronous subclasses
/// define scheduling and retry behavior.
///
/// - Parameter fetchOptions: Options, or nil for subclass defaults.
/// - Parameter success_block: Optional success callback receiving nullable results.
/// - Parameter failure_block: Optional failure callback receiving a nullable error.
/// - Parameter noConnection_block: Optional retry decision. By convention, YES
///   requests retry when connected and NO requests failure; unused by the base.
- (void)asynchronousFetchObjectsWithOptions:(nullable SCDataFetchOptions *)fetchOptions success:(nullable SCDataStoreFetchSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Optional processing performed by fetchObjectsSuccessful:successBlock:failure:.
///
/// The helper passes nullable results and a nonnull completion block. Call
/// completion exactly once with updated results and nil error, or a nonnil error
/// to select failure. Delivery occurs on the queue that calls completion. There
/// is no automatic dispatch or completion. Subclasses must use the helper for
/// this action to run; synchronous fetching does not invoke it.
///
/// ```objective-c
/// myDataStore.postAsynchronousFetchObjectsAction =
///     ^(NSArray<id> * _Nullable results,
///       SCPostFetchAsyncronousCompletionHandler_Block completionHandler) {
///         // Replace this pass-through with application-specific processing.
///         completionHandler(results, nil);
///     };
/// ```
///
/// Defaults to nil. Avoid capturing the store strongly in its retained block.
@property (nonatomic, copy, nullable) SCPostFetchAsyncronousAction_Block postAsynchronousFetchObjectsAction;


#pragma mark - Data Validation

/// Checks whether the store considers an object valid for insertion.
///
/// - Parameter object: The candidate object.
/// - Returns: NO in the base implementation, YES for SCArrayStore. Core Data calls
///   the managed object's validateForInsert: without returning its error. This check
///   does not perform or persist the operation.
- (BOOL)validateInsertForObject:(NSObject *)object;

/// Checks whether the store considers an object valid for updating.
///
/// - Parameter object: The candidate object.
/// - Returns: NO in the base implementation, YES for SCArrayStore. Core Data calls
///   the managed object's validateForUpdate: without returning its error. This check
///   does not perform or persist the operation.
- (BOOL)validateUpdateForObject:(NSObject *)object;

/// Checks whether the store considers an object valid for deletion.
///
/// - Parameter object: The candidate object.
/// - Returns: NO in the base implementation, YES for SCArrayStore. Core Data calls
///   the managed object's validateForDelete: without returning its error. This check
///   does not perform or persist the operation.
- (BOOL)validateDeleteForObject:(NSObject *)object;

/// Checks whether the store has ordering support for an object.
///
/// - Parameter object: The candidate object.
/// - Returns: NO in the base implementation, YES for SCArrayStore. Core Data returns
///   YES for a bound ordered set or a registered entity definition with a nonnil
///   orderAttributeName. This does not validate destination indexes or membership.
- (BOOL)validateOrderChangeForObject:(NSObject *)object;


#pragma mark - Data Management

/// Requests persistence of pending changes, where supported by the subclass.
///
/// The base implementation does nothing. Every store registers this method for
/// application-background and termination notifications. Core Data saves its
/// context and reports failures through its error API and compatibility callback;
/// user-defaults stores call synchronize without exposing its result.
- (void)commitData;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Retained backing storage for subclasses that keep data in memory.
///
/// Defaults to nil. The base implementation imposes no storage type. SCArrayStore
/// accepts only NSMutableArray and clears storage for other assigned types.
@property (nonatomic, strong, nullable) NSObject *storedData;

/// Fallback values for missing single-property reads. Defaults to nil.
///
/// Retained, not copied. SCUserDefaultsStore also registers assigned defaults with
/// standardUserDefaults without overwriting stored preferences. Pass a valid
/// dictionary when registering defaults.
@property (nonatomic, strong, nullable) NSDictionary<NSString *, id> *defaultsDictionary;

/// Records an object, property, and definition for collection-backed subclasses.
///
/// The base only retains binding information. Array and Core Data subclasses
/// additionally bind their backing collection to a supported property value.
/// - Parameter propertyName: Collection property, or nil to clear the name.
/// - Parameter object: Owning object, or nil to clear the object.
/// - Parameter definition: The owner's definition, or nil when unavailable.
- (void)bindStoreToPropertyName:(nullable NSString *)propertyName forObject:(nullable NSObject *)object withDefinition:(nullable SCDataDefinition *)definition;

/// Asks the subclass to discard tracked uninserted objects in reverse order.
///
/// When tracking is nonempty, first posts
/// SCDataStoreWillDiscardAllUninsertedObjectsNotification synchronously with this
/// store as the notification object. The helper does not itself clear tracking.
- (void)forceDiscardAllUnaddedObjects;

/// Handles the application-will-enter-foreground notification.
///
/// The base implementation does nothing. Subclasses may restore transient state.
- (void)applicationWillEnterForeground;

// Internally checks if the 'postAsynchronousFetchObjectsAction' property has been set before calling success_block
/// Delivers successful results, optionally running the post-fetch action.
///
/// Without an action, calls success immediately on the calling queue. Otherwise,
/// the action's completion selects success for nil error or failure for nonnil
/// error. Callbacks run on the completion's queue. Nil callbacks are skipped.
/// - Parameter objects: Fetched results, which may be nil.
/// - Parameter success_block: Optional callback for original or updated results.
/// - Parameter failure_block: Optional callback for a post-processing error.
- (void)fetchObjectsSuccessful:(nullable NSArray<id> *)objects successBlock:(nullable SCDataStoreFetchSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block;

@end


/* Missing framework classes (internal) */

/// Represents a data definition whose required framework is unavailable.
@interface SCMissingFrameworkDataDefinition : SCDataDefinition

/// The message explaining which required framework is unavailable.
@property (nonatomic, copy, nullable) NSString *missingFrameworkMessage;

@end


/// Represents a data store whose required framework is unavailable.
@interface SCMissingFrameworkDataStore : SCDataStore

/// The default definition cast to a missing-framework definition.
///
/// Nil if no default is configured. Callers must configure the expected type;
/// the getter does not validate the cast.
@property (nonatomic, readonly, nullable) SCMissingFrameworkDataDefinition *missingFrameworkDataDefinition;

@end

NS_ASSUME_NONNULL_END
