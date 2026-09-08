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
/// Posted before a data store discards all uninserted objects.
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
/// A block that determines how to handle an unavailable network connection.
typedef BOOL (^SCNoConnection_Block)(void);
/// A completion block for asynchronous post-fetch processing.
/// Supply nil for error on success; results may be nil on failure.
typedef void (^SCPostFetchAsyncronousCompletionHandler_Block)(NSArray<id> *_Nullable results, NSError *_Nullable error);
/// A block that performs asynchronous post-fetch processing.
typedef void (^SCPostFetchAsyncronousAction_Block)(NSArray<id> *_Nullable results, SCPostFetchAsyncronousCompletionHandler_Block completionHandler);


#pragma mark - SCDataStore
/// SCDataStore is an abstract base class that can encapsulate any kind of data storage, providing means for the SC framework to communicate with this storage to fetch, add, update and remove data.
///
/// Several SCDataStore subclasses have been implemented to enable the SC framework to communicate with Core Data, web services, iCloud storage, and even NSUserDefaults storage. Feel free to subclass SCDataStore to enable any kind of data storage not currently implemented by framework.
///
/// Sample use:
/// ```objective-c
/// // Display all the TaskEntity objects
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

/// Allocated and returns an initialized SCDataStore given a default SCDataDefinition of the data stored.
/// - Parameter definition: The data definition involved in the operation.
+ (instancetype)storeWithDefaultDataDefinition:(nullable SCDataDefinition *)definition;

/// Returns an initialized SCDataStore given a default SCDataDefinition of the data stored.
/// - Parameter definition: The data definition involved in the operation.
- (instancetype)initWithDefaultDataDefinition:(nullable SCDataDefinition *)definition;


#pragma mark - Configuration

/// Specifies the mode that the SC framework should use to communicate with the store. Set to SCStoreModeSynchronous to have the framework communicate synchronously, otherwise set to SCStoreModeAsynchronous for asynchronous communication.
@property (nonatomic, readwrite) SCStoreMode storeMode;

/// The data store's default data definition. To add more data definitions (other than the default one), use addDataDefinition:.
@property (nonatomic, strong, nullable) SCDataDefinition *defaultDataDefinition;

/// Whether the data store supports nil values. Default: YES.
@property (nonatomic, readwrite) BOOL supportsNilValues;

/// Adds a definition to dataDefinitions.
/// - Parameter definition: The data definition involved in the operation.
- (void)addDataDefinition:(nullable SCDataDefinition *)definition;

/// Returns the data definition for the given object.
/// - Parameter object: The object involved in the operation.
/// - Returns: The applicable definition, or nil if none is configured or found.
- (nullable SCDataDefinition *)definitionForObject:(NSObject *)object;


#pragma mark - Synchronous Data Access

/// Returns a newly allocated and initialized object based on the store's default data definition.
/// - Returns: The new object, or nil when creation is unsupported or unavailable.
/// - Warning: Any newly created objects should be later either added to the data store using insertObject:, or discarded using discardUninsertedObject:.
- (nullable NSObject *)createNewObject;

/// Returns a newly allocated and initialized object based on the given data definition.
/// - Parameter definition: The data definition that describes the object to be created.
/// - Returns: The new object, or nil when creation is unsupported or unavailable.
/// - Warning: Any newly created objects should be later either added to the data store using insertObject:, or discarded using discardUninsertedObject:.
- (nullable NSObject *)createNewObjectWithDefinition:(nullable SCDataDefinition *)definition;

/// Discards the given uninserted object.
///
/// Any object created with createNewObject: or createNewObjectWithDefinition: and not later inserted to the store using insertObject: must be discarded using this method.
///
/// - Parameter object: The uninserted newly created object to be discarded.
/// - Returns: Returns TRUE if successful.
- (BOOL)discardUninsertedObject:(NSObject *)object;

/// Inserts the given object into the data store.
/// - Parameter object: The object to be inserted.
/// - Returns: Returns TRUE if successful.
- (BOOL)insertObject:(NSObject *)object;

/// Inserts the given object into the data store at the specified order.
/// - Parameter object: The object to be inserted.
/// - Parameter order: The order index that the object should be inserted at.
/// - Returns: Returns TRUE if successful.
/// - Note: Only applicable for data stores that maintain ordered object storage.
- (BOOL)insertObject:(NSObject *)object atOrder:(NSUInteger)order;

/// Changes the order of the given object to the specified order.
/// - Parameter object: The object that the order of will change.
/// - Parameter toOrder: The new order index of the object.
/// - Parameter subsetArray: The ordered subset containing the object.
/// - Returns: Returns TRUE if successful.
/// - Note: Only applicable for data stores that maintain ordered object storage.
- (BOOL)changeOrderForObject:(NSObject *)object toOrder:(NSUInteger)toOrder subsetArray:(NSArray<id> *)subsetArray;

/// Updates the given object in the data store.
/// - Parameter object: The object to be updated.
/// - Returns: Returns TRUE if successful.
- (BOOL)updateObject:(NSObject *)object;

/// Deletes the given object from the data store.
/// - Parameter object: The object to be deleted.
/// - Returns: Returns TRUE if successful.
- (BOOL)deleteObject:(NSObject *)object;

/// Fetches objects from the data store that satisfy the given fetch options.
/// - Parameter fetchOptions: Fetch settings, or nil to use the store's defaults.
/// - Returns: The fetched objects, or nil when fetching is unsupported or fails.
/// - SeeAlso: ``SCDataFetchOptions``
- (nullable NSArray<id> *)fetchObjectsWithOptions:(nullable SCDataFetchOptions *)fetchOptions;

/// Returns the value for the given property name in the given object.
/// - Parameter propertyName: The name of the property.
/// - Parameter object: The object containing propertyName.
/// - Returns: The value for the given propertyName.
- (nullable NSObject *)valueForPropertyName:(nullable NSString *)propertyName inObject:(NSObject *)object;

/// Returns the string value for the given property name in the given object.
/// - Parameter propertyName: The name of the property.
/// - Parameter object: The object containing propertyName.
/// - Parameter delimiter: If the property returns more than one value, this delimiter is used to separate the values in the returned string.
/// - Returns: The value for the given propertyName.
- (nullable NSString *)stringValueForPropertyName:(nullable NSString *)propertyName inObject:(NSObject *)object separateValuesUsingDelimiter:(nullable NSString *)delimiter;

/// Sets the value for the given property name in the given object.
/// - Parameter value: The value of the property.
/// - Parameter propertyName: The name of the property.
/// - Parameter object: The object containing propertyName.
- (void)setValue:(nullable NSObject *)value forPropertyName:(NSString *)propertyName inObject:(NSObject *)object;


#pragma mark - Asynchronous Data Access

/// Asynchronously inserts the given object into the data store.
/// - Parameter object: The object to be inserted.
/// - Parameter success_block: The code block called after the object has been successfully inserted.
///
/// SCDataStoreInsertSuccess_Block syntax:
///     ^()
///     {
///         // Your code here
///     }
///
/// - Parameter failure_block: The code block called in case the object could not be inserted.
///
/// SCDataStoreFailure_Block syntax:
///     ^(NSError *error)
///     {
///         // Your code here
///     }
///
/// - Parameter noConnection_block: The code block called in case no connection could be established to data store.
///
/// SCNoConnection_Block syntax:
///     ^BOOL()
///     {
///         // Return YES to try operation again when connection is established, otherwise return NO to call failure_block
///     }
- (void)asynchronousInsertObject:(NSObject *)object success:(nullable SCDataStoreInsertSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Asynchronously updates the given object into the data store.
/// - Parameter object: The object to be updated.
/// - Parameter success_block: The code block called after the object has been successfully updated.
///
/// SCDataStoreUpdateSuccess_Block syntax:
///     ^()
///     {
///         // Your code here
///     }
///
/// - Parameter failure_block: The code block called in case the object could not be updated.
///
/// SCDataStoreFailure_Block syntax:
///     ^(NSError *error)
///     {
///         // Your code here
///     }
///
/// - Parameter noConnection_block: The code block called in case no connection could be established to data store.
///
/// SCNoConnection_Block syntax:
///     ^BOOL()
///     {
///         // Return YES to try operation again when connection is established, otherwise return NO to call failure_block
///     }
- (void)asynchronousUpdateObject:(NSObject *)object success:(nullable SCDataStoreUpdateSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Asynchronously deletes the given object from the data store.
/// - Parameter object: The object to be deleted.
/// - Parameter success_block: The code block called after the object has been successfully deleted.
///
/// SCDataStoreDeleteSuccess_Block syntax:
///     ^()
///     {
///         // Your code here
///     }
///
/// - Parameter failure_block: The code block called in case the object could not be deleted.
///
/// SCDataStoreFailure_Block syntax:
///     ^(NSError *error)
///     {
///         // Your code here
///     }
///
/// - Parameter noConnection_block: The code block called in case no connection could be established to data store.
///
/// SCNoConnection_Block syntax:
///     ^BOOL()
///     {
///         // Return YES to try operation again when connection is established, otherwise return NO to call failure_block
///     }
- (void)asynchronousDeleteObject:(NSObject *)object success:(nullable SCDataStoreDeleteSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Asynchronously fetches objects from the data store that satisfy the given fetch options.
/// - Parameter fetchOptions: The fetch options that the data store must satisfy when returning the objects.
/// - Parameter success_block: The code block called after the data has been successfully fetched.
///
/// SCDataStoreFetchSuccess_Block syntax:
///     ^(NSArray<id> * _Nullable results)
///     {
///         // Your code here
///     }
/// Where `results` is the fetched data array.
/// - Parameter failure_block: The code block called in case the data could not be fetched.
///
/// SCDataStoreFailure_Block syntax:
///     ^(NSError *error)
///     {
///         // Your code here
///     }
///
/// - Parameter noConnection_block: The code block called in case no connection could be established to data store.
///
/// SCNoConnection_Block syntax:
///     ^BOOL()
///     {
///         // Return YES to try operation again when connection is established, otherwise return NO to call failure_block
///     }
///
/// - SeeAlso: ``SCDataFetchOptions``
- (void)asynchronousFetchObjectsWithOptions:(nullable SCDataFetchOptions *)fetchOptions success:(nullable SCDataStoreFetchSuccess_Block)success_block failure:(nullable SCDataStoreFailure_Block)failure_block noConnection:(nullable SCNoConnection_Block)noConnection_block;

/// Action gets called right after asynchronousFetchObjectsWithOptions has successfully finished.
///
/// This action is typically used to asynchronously load further objects or data in addition to the ones fetched in asynchronousFetchObjectsWithOptions.
///
/// The block must call its completion handler with the updated results and any error.
///
/// Example:
///
/// ```objective-c
/// // Objective-C
/// myDataStore.postAsynchronousFetchObjectsAction = ^(NSArray<id> * _Nullable results, SCPostFetchAsyncronousCompletionHandler_Block completionHandler)
/// {
///     // ... load your data here asynchronously ...
///
///     NSError *error = nil;
///     completionHandler(myUpdatedResultsArray, error);
/// };
/// ```
@property (nonatomic, copy, nullable) SCPostFetchAsyncronousAction_Block postAsynchronousFetchObjectsAction;


#pragma mark - Data Validation

/// Returns TRUE if the given object can be inserted into the data store, otherwise returns FALSE.
/// - Parameter object: The object involved in the operation.
- (BOOL)validateInsertForObject:(NSObject *)object;

/// Returns TRUE if the given object can be updated into the data store, otherwise returns FALSE.
/// - Parameter object: The object involved in the operation.
- (BOOL)validateUpdateForObject:(NSObject *)object;

/// Returns TRUE if the given object can be deleted from the data store, otherwise returns FALSE.
/// - Parameter object: The object involved in the operation.
- (BOOL)validateDeleteForObject:(NSObject *)object;

/// Returns TRUE if the order of the given object can be changed in the data store, otherwise returns FALSE.
/// - Parameter object: The object involved in the operation.
- (BOOL)validateOrderChangeForObject:(NSObject *)object;


#pragma mark - Data Management

/// Commits the data store objects to the persistent store. This method is only applicable to data stores that store their objects in memory before persisting them to a permenant persistant storage.
- (void)commitData;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// Should be used by subclasses to store their data when heap memory storage is needed.
@property (nonatomic, strong, nullable) NSObject *storedData;

/// Should be used by subclasses to store any default values needed.
@property (nonatomic, strong, nullable) NSDictionary<NSString *, id> *defaultsDictionary;

/// Binds the store to a property name that will be used to fetch all the store's data.
///
/// Should only be used by the framework all must be implemented by all subclasses.
/// - Parameter propertyName: The property name.
/// - Parameter object: The object involved in the operation.
/// - Parameter definition: The data definition involved in the operation.
- (void)bindStoreToPropertyName:(nullable NSString *)propertyName forObject:(nullable NSObject *)object withDefinition:(nullable SCDataDefinition *)definition;

/// This method is typically called internally by the framework when all unadded objects must be discarded. The method will issue the `SCDataStoreWillDiscardAllUnaddedObjectsNotification` notification to inform all classes using the store that this will happen.
- (void)forceDiscardAllUnaddedObjects;

/// Method called when the application is about to leave the background state. Subclasses should override this method when any initialization is needed at this point.
- (void)applicationWillEnterForeground;

// Internally checks if the 'postAsynchronousFetchObjectsAction' property has been set before calling success_block
/// Completes a fetch operation with either the success or failure callback.
/// - Parameter objects: The fetched objects.
/// - Parameter success_block: The success_block.
/// - Parameter failure_block: The failure_block.
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

/// The placeholder definition for an unavailable framework.
@property (nonatomic, readonly, nullable) SCMissingFrameworkDataDefinition *missingFrameworkDataDefinition;

@end

NS_ASSUME_NONNULL_END
