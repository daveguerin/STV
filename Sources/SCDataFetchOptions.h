/*
 *  SCDataFetchOptions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCGlobals.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCDataFetchOptions
/// Sorting, filtering, and batch configuration consumed by data stores.
///
/// Stores interpret options differently. SCArrayStore uses the in-memory helpers;
/// SCCoreDataStore expects SCCoreDataFetchOptions for full control.
/// SCUserDefaultsStore ignores fetch options.
///
/// ```objective-c
/// SCDataFetchOptions *options = [SCDataFetchOptions
///     optionsWithSortKey:@"name" sortAscending:YES filterPredicate:nil];
/// ```
///
/// - SeeAlso: ``SCDataStore``, ``SCCoreDataFetchOptions``
@interface SCDataFetchOptions : NSObject


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCDataFetchOptions object.
+ (instancetype)options;

/// Allocates and returns an initialized SCDataFetchOptions object given the sorting and filtering configuration.
///
/// - Parameter key: The key that the data will be sorted against. Passing a non nil value for `key` also sets the sort property to TRUE.
/// - Parameter ascending: The direction when key is nonnil. With a nil key,
///   this argument is ignored and sortAscending remains YES.
/// - Parameter predicate: The NSPredicate that the data will be filtered against. Passing a non nil value for `predicate` also sets the filter property to TRUE.
+ (instancetype)optionsWithSortKey:(nullable NSString *)key sortAscending:(BOOL)ascending filterPredicate:(nullable NSPredicate *)predicate;

/// Returns an initialized SCDataFetchOptions object given the sorting and filtering configuration.
///
/// - Parameter key: The key that the data will be sorted against. Passing a non nil value for `key` also sets the sort property to TRUE.
/// - Parameter ascending: The direction when key is nonnil. With a nil key,
///   this argument is ignored and sortAscending remains YES.
/// - Parameter predicate: The NSPredicate that the data will be filtered against. Passing a non nil value for `predicate` also sets the filter property to TRUE.
- (instancetype)initWithSortKey:(nullable NSString *)key sortAscending:(BOOL)ascending filterPredicate:(nullable NSPredicate *)predicate;


#pragma mark - Configuration

/// Whether the in-memory helper sorts its array. Defaults to NO.
///
/// The helper also requires a nonnil sortKey. Core Data fetches use available
/// sort descriptors even when this flag is NO.
@property (nonatomic, readwrite) BOOL sort;

/// The sort key or semicolon-separated keys, in priority order. Defaults to nil.
///
/// Assigning this property does not enable sort. Keys must be valid for fetched
/// objects; whitespace around individual keys is not trimmed.
@property (nonatomic, copy, nullable) NSString *sortKey;

/// The direction used by all sort descriptors. Defaults to YES.
///
/// Assigning this property also enables sort, even when assigning NO. It applies
/// to both sortKey and the Core Data ordering attribute.
@property (nonatomic, readwrite) BOOL sortAscending;

/// Whether Core Data fetches apply filterPredicate. Defaults to YES.
///
/// The in-memory filterMutableArray: helper ignores this flag, so SCArrayStore
/// applies any nonnil predicate even when this property is NO.
@property (nonatomic, readwrite) BOOL filter;

/// The predicate to apply, or nil for no filtering. Defaults to nil.
///
/// Assignment does not change filter. The in-memory helper applies a nonnil
/// predicate regardless of that flag.
@property (nonatomic, strong, nullable) NSPredicate *filterPredicate;

/// The requested objects per batch, or zero for no batch limit. Defaults to 0.
///
/// SCArrayStore slices filtered, sorted results. Unbound Core Data stores apply
/// the limit separately to each registered entity; bound Core Data collections
/// do not apply batching. Batch fetches advance the current offset.
@property (nonatomic, readwrite) NSUInteger batchSize;


#pragma mark - Internal Properties & Methods (should only used by the framework or when subclassing)

/// The starting batch offset. Defaults to 0.
///
/// Increasing this beyond batchCurrentOffset advances the current offset too.
/// Decreasing it does not rewind the current offset; call resetBatchOffset.
@property (nonatomic, readwrite) NSUInteger batchStartingOffset;

/// The batch offset to use for the next fetch, initially 0.
///
/// Array and unbound Core Data batch fetches use batchCurrentOffset * batchSize
/// as the object offset, then increment the batch offset.
@property (nonatomic, readonly) NSUInteger batchCurrentOffset;

/// The value batchSize * batchCurrentOffset + batchStartingOffset.
///
/// This helper adds the starting offset separately. Array and Core Data stores
/// calculate fetch offsets directly without that extra addition.
@property (nonatomic, readonly) NSUInteger nextBatchStartIndex;


/// Sets the current batch offset.
/// - Warning: Reserved for internal framework use only.
/// - Parameter offset: The current batch offset; starting offset is unchanged.
- (void)setBatchOffset:(NSUInteger)offset;

/// Increments the current batch offset.
/// - Warning: Reserved for internal framework use only.
- (void)incrementBatchOffset;

/// Resets the current batch offset to batchStartingOffset.
/// - Warning: Reserved for internal framework use only.
- (void)resetBatchOffset;

/// Builds sort descriptors without consulting sort.
///
/// - Returns: One descriptor per semicolon-separated sort key, or an empty array
///   for a nil sortKey. Subclasses may choose a different ordering key.
- (NSArray<NSSortDescriptor *> *)sortDescriptors;

/// Sorts an array in place when both sort and sortKey are set.
///
/// Sorting exceptions are caught and logged; no rollback is performed.
/// - Parameter array: The mutable array to sort.
- (void)sortMutableArray:(NSMutableArray<id> *)array;

/// Filters an array in place whenever filterPredicate is nonnil.
///
/// This method ignores filter. Predicate exceptions are caught and logged;
/// no rollback is performed.
/// - Parameter array: The mutable array to filter.
- (void)filterMutableArray:(NSMutableArray<id> *)array;


@end

NS_ASSUME_NONNULL_END
