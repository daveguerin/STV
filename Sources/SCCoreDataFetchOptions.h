/*
 *  SCCoreDataFetchOptions.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDataFetchOptions.h"

NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCCoreDataFetchOptions
/// Fetch options with an optional ordering attribute for SCCoreDataStore.
///
/// - SeeAlso: ``SCDataFetchOptions``
@interface SCCoreDataFetchOptions : SCDataFetchOptions


/// The ordering attribute used instead of sortKey to build sort descriptors.
///
/// Defaults to nil. When set, sortDescriptors returns one descriptor using this
/// attribute and sortAscending, regardless of sort. Otherwise it uses the base
/// implementation. SCEntityDefinition copies its ordering attribute into new
/// compatible options; later definition changes are not synchronized.
@property (nonatomic, copy, nullable) NSString *orderAttributeName;

@end

NS_ASSUME_NONNULL_END
