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
/// SCArrayStore is an SCDataStore subclass that encapsulates NSArray memory storage, providing means for the SC framework to communicate with this array storage to fetch, add, update and remove data objects.
///
/// - Note: It is very rare when you'll need to create an SCArrayStore instance yourself, as it's typically automatically created for you when you use data definitions such as SCClassDefinition and SCDictionaryDefinition. For example, when you use the SCArrayOfObjectsSection initializer method called [SCArrayOfObjectsSection sectionWithHeaderTitle:items:itemsDefinition:], SCArrayOfObjectsSection automatically sets its dataStore property by calling your itemsDefinition's [SCDataDefinition generateCompatibleDataStore:] method.
///
/// - Note: For more information on data stores, check out the SCDataStore base class documentation.
@interface SCArrayStore : SCDataStore


#pragma mark - Creation and Initialization

/// Allocates and returns an initialized SCArrayStore given an array of objects and their default data definition.
/// - Parameter array: The mutable array to process.
/// - Parameter definition: The data definition involved in the operation.
+ (instancetype)storeWithObjectsArray:(nullable NSMutableArray<id> *)array defaultDefiniton:(nullable SCDataDefinition *)definition;

/// Returns an initialized SCArrayStore given an array of objects and their default data definition.
/// - Parameter array: The mutable array to process.
/// - Parameter definition: The data definition involved in the operation.
- (id)initWithObjectsArray:(nullable NSMutableArray<id> *)array defaultDefiniton:(nullable SCDataDefinition *)definition;

/// The objects array storage managed by the memory store.
@property (nonatomic, strong, nullable) NSMutableArray<id> *objectsArray;

@end


// Create an SCArrayStore alias called SCMemoryStore (for backward compatibility with STV 3.0)
/// A convenience alias for `SCArrayStore`.
#define SCMemoryStore SCArrayStore

NS_ASSUME_NONNULL_END
