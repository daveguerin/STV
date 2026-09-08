/*
 *  SCUserDefaultsStore.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCDataStore.h"


NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCUserDefaultsStore
/// A synchronous binding store for standardUserDefaults.
///
/// Fetches always return an array containing the standard NSUserDefaults object,
/// ignoring fetch options. Property access uses the inherited KVC binding methods;
/// collection insertion, deletion, and update methods remain unsupported base
/// hooks rather than operations on individual preference keys.
///
/// SCUserDefaultsDefinition generates this store for
/// SCTableViewModel's generateSectionsForUserDefaultsDefinition: method.
/// Assigning defaultsDictionary also calls registerDefaults:; it does not overwrite
/// stored preferences. commitData calls synchronize and does not expose its result.
///
/// - Note: Use property-list values. Standard user defaults are not secure storage
///   and are not a substitute for Keychain storage of passwords or tokens.
/// - SeeAlso: ``SCDataStore``, ``SCUserDefaultsDefinition``
@interface SCUserDefaultsStore : SCDataStore

/// The NSUserDefaults standardUserDefaults singleton used by this store.
///
/// It is returned on every access; this property does not configure an app-group
/// or custom-suite defaults object.
@property (nonatomic, readonly) NSUserDefaults *standardUserDefaultsObject;

@end

NS_ASSUME_NONNULL_END
