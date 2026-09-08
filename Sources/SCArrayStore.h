/*
 *  SCArrayStore.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCDataStore.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCArrayStore
/// An in-memory store backed by a retained mutable array.
///
/// Class, dictionary, and basic-value definitions create this store through
/// `generateCompatibleDataStore`. Object sections supply their items array to
/// that generated store.
///
/// Fetches copy the array, filter and sort the copy, then optionally select a
/// batch. Collection writes modify the original array. Reordering uses backing
/// array indexes and ignores the supplied subset. `commitData` does nothing.
///
/// - Note: Configure `objectsArray` before collection operations. Generated stores
///   have no backing array; the store does not allocate one automatically.
/// - SeeAlso: ``SCDataStore``
@interface SCArrayStore : SCDataStore


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCArrayStore given an array of objects and their default data definition.
/// - Parameter array: The retained backing array, or nil to leave storage unset.
/// - Parameter definition: The default definition, or nil to configure it later.
+ (instancetype)storeWithObjectsArray:(nullable NSMutableArray<id> *)array defaultDefiniton:(nullable SCDataDefinition *)definition;

/// Returns an initialized SCArrayStore given an array of objects and their default data definition.
/// - Parameter array: The retained backing array, or nil to leave storage unset.
/// - Parameter definition: The default definition, or nil to configure it later.
- (id)initWithObjectsArray:(nullable NSMutableArray<id> *)array defaultDefiniton:(nullable SCDataDefinition *)definition;

/// The retained mutable backing array, or nil when unconfigured.
///
/// Assignments are not copied. Nil clears storage. An assigned object that is not
/// an NSMutableArray also clears storage and logs a warning.
@property (nonatomic, strong, nullable) NSMutableArray<id> *objectsArray;

@end


// Create an SCArrayStore alias called SCMemoryStore (for backward compatibility with STV 3.0)
/// A convenience alias for `SCArrayStore`.
#define SCMemoryStore SCArrayStore

NS_ASSUME_NONNULL_END
