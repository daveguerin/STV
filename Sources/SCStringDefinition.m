/*
 *  SCStringDefinition.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCStringDefinition.h"

#import "SCArrayStore.h"
#import <objc/runtime.h>


@implementation SCStringDefinition

@synthesize stringPropertyDefinition = _stringPropertyDefinition;


+ (instancetype)definition
{
    return [[[self class] alloc] init];
}

- (instancetype)init
{
	if( (self = [super init]) )
	{
        _stringPropertyDefinition = [[SCPropertyDefinition alloc] initWithName:@"string"];
        _stringPropertyDefinition.dataType = SCDataTypeNSString;
        _stringPropertyDefinition.type = SCPropertyTypeTextField;
        
        [propertyDefinitions addObject:_stringPropertyDefinition];
	}
    
	return self;
}



// overrides superclass
- (SCDataType)propertyDataTypeForPropertyWithName:(NSString *)propertyName
{
    return SCDataTypeNSString;
}

// overrides superclass
- (NSString *)titleValueForObject:(NSObject *)object
{
    return (NSString *)object;  // The object itself is an NSString
}

// overrides superclass
- (NSString *)descriptionValueForObject:(NSObject *)object
{
    // Not applicable for NSString objects
    return nil;
}

// overrides superclass
- (NSString *)dataStructureName
{
	return [SCUtilities dataStructureNameForClass:[NSString class]];
}

// overrides superclass
- (SCDataStore *)generateCompatibleDataStore
{
    return [SCArrayStore storeWithObjectsArray:nil defaultDefiniton:self];
}


@end
