/*
 *  SCUserDefaultsDefinition.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCUserDefaultsDefinition.h"

#import "SCUserDefaultsStore.h"


@implementation SCUserDefaultsDefinition


+ (instancetype)definitionWithUserDefaultsKeyNamesString:(NSString *)keyNamesString
{
    return [[[self class] alloc] initWithUserDefaultsKeyNamesString:keyNamesString];
}

+ (instancetype)definitionWithUserDefaultsKeyNames:(NSArray *)keyNames
{
    return [[[self class] alloc] initWithUserDefaultsKeyNames:keyNames];
}

+ (instancetype)definitionWithUserDefaultsKeyNames:(NSArray *)keyNames keyTitles:(NSArray *)keyTitles
{
    return [[[self class] alloc] initWithUserDefaultsKeyNames:keyNames keyTitles:keyTitles];
}


- (instancetype)initWithUserDefaultsKeyNamesString:(NSString *)keyNamesString
{
    return [self initWithDictionaryKeyNamesString:keyNamesString];
}

- (instancetype)initWithUserDefaultsKeyNames:(NSArray *)keyNames
{
    return [self initWithDictionaryKeyNames:keyNames];
}

- (instancetype)initWithUserDefaultsKeyNames:(NSArray *)keyNames keyTitles:(NSArray *)keyTitles
{
    return [self initWithDictionaryKeyNames:keyNames keyTitles:keyTitles];
}


// overrides superclass
- (SCDataStore *)generateCompatibleDataStore
{
    return [SCUserDefaultsStore storeWithDefaultDataDefinition:self];
}

@end
