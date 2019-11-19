/*
 *  SCUserDefaultsStore.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCUserDefaultsStore.h"

@implementation SCUserDefaultsStore

- (NSUserDefaults *)standardUserDefaultsObject
{
    return [NSUserDefaults standardUserDefaults];
}

// overrides superclass
- (SCDataDefinition *)definitionForObject:(NSObject *)object
{
    return self.defaultDataDefinition;
}

// overrides superclass
- (void)setDefaultsDictionary:(NSDictionary *)defaultsDictionary
{
    [super setDefaultsDictionary:defaultsDictionary];
    
    [self.standardUserDefaultsObject registerDefaults:defaultsDictionary];
}

// overrides superclass
- (NSArray *)fetchObjectsWithOptions:(SCDataFetchOptions *)fetchOptions
{
    return [NSArray arrayWithObject:self.standardUserDefaultsObject];
}

// overrides superclass
- (void)commitData
{
    [self.standardUserDefaultsObject synchronize];
}

@end


