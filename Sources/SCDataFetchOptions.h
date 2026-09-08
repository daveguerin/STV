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
/// This class is used to control how data is fetched from SCDataStore. SCDataFetchOptions can be configured to have the data sorted, filtered, and/or returned in batches.
///
/// Sample use:
/// ```objective-c
/// SCArrayOfObjectsModel *myObjectsModel = ...;
/// myObjectsModel.dataFetchOptions.sort = TRUE;
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
/// - Parameter ascending: Whether the results should be sorted in ascending order.
/// - Parameter predicate: The NSPredicate that the data will be filtered against. Passing a non nil value for `predicate` also sets the filter property to TRUE.
+ (instancetype)optionsWithSortKey:(nullable NSString *)key sortAscending:(BOOL)ascending filterPredicate:(nullable NSPredicate *)predicate;

/// Returns an initialized SCDataFetchOptions object given the sorting and filtering configuration.
///
/// - Parameter key: The key that the data will be sorted against. Passing a non nil value for `key` also sets the sort property to TRUE.
/// - Parameter ascending: Whether the results should be sorted in ascending order.
/// - Parameter predicate: The NSPredicate that the data will be filtered against. Passing a non nil value for `predicate` also sets the filter property to TRUE.
- (instancetype)initWithSortKey:(nullable NSString *)key sortAscending:(BOOL)ascending filterPredicate:(nullable NSPredicate *)predicate;


#pragma mark - Configuration

/// Set to TRUE to enable data sorting, otherwise set to FALSE. Default: FALSE.
/// - Note: Only applicable when sortKey is set to a valid key.
@property (nonatomic, readwrite) BOOL sort;

/// The key that the data will be sorted against. Default: nil.
@property (nonatomic, copy, nullable) NSString *sortKey;

/// Set to TRUE to have the sorting done in ascending order, otherwise set to FALSE. Default: TRUE.
/// - Note: Only applicable when sortKey is set to a valid key.
@property (nonatomic, readwrite) BOOL sortAscending;

/// Set to TRUE to enable data filtering, otherwise set to FALSE. Default: TRUE.
/// - Note: Only applicable when filterPredicate is set to a valid NSPredicate.
@property (nonatomic, readwrite) BOOL filter;

/// The NSPredicate that the data will be filtered against. Default: nil.
@property (nonatomic, strong, nullable) NSPredicate *filterPredicate;

/// Set to the data batch size that should be retrieved. Setting this property to zero retrieves all avialable data. Default: 0.
@property (nonatomic, readwrite) NSUInteger batchSize;


#pragma mark - Internal Properties & Methods (should only used by the framework or when subclassing)

/// The offset of the batch that should be first retrieved. Default: 0.
@property (nonatomic, readwrite) NSUInteger batchStartingOffset;

/// The current offset of the last retrieved batch.
@property (nonatomic, readonly) NSUInteger batchCurrentOffset;

/// The starting index for the next batch to be retrieved.
@property (nonatomic, readonly) NSUInteger nextBatchStartIndex;


/// Sets the current batch offset.
/// - Warning: Reserved for internal framework use only.
/// - Parameter offset: The offset.
- (void)setBatchOffset:(NSUInteger)offset;

/// Increments the current batch offset.
/// - Warning: Reserved for internal framework use only.
- (void)incrementBatchOffset;

/// Resets the current batch offset.
/// - Warning: Reserved for internal framework use only.
- (void)resetBatchOffset;

/// Returns an array of sort-descriptors based on the current sorting configuration.
- (NSArray<NSSortDescriptor *> *)sortDescriptors;

/// Sorts the given array based on the current sorting configuration.
/// - Parameter array: The mutable array to process.
- (void)sortMutableArray:(NSMutableArray<id> *)array;

/// Filters the given array based on the current filtering configuration.
/// - Parameter array: The mutable array to process.
- (void)filterMutableArray:(NSMutableArray<id> *)array;


@end

NS_ASSUME_NONNULL_END
