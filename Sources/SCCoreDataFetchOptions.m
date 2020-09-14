/*
 *  SCCoreDataFetchOptions.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCCoreDataFetchOptions.h"

@implementation SCCoreDataFetchOptions

@synthesize orderAttributeName = _orderAttributeName;


- (instancetype)init
{
	if( (self = [super init]) )
	{
        _orderAttributeName = nil;
	}
	return self;
}



- (NSArray *)sortDescriptors
{
    NSArray *descriptors = nil;
    if(self.orderAttributeName)
    {
        NSSortDescriptor *descriptor = [NSSortDescriptor sortDescriptorWithKey:self.orderAttributeName ascending:self.sortAscending];
        descriptors = [NSArray arrayWithObject:descriptor];
    }
    else 
    {
        descriptors = [super sortDescriptors];
    }
    
    return descriptors;
}

@end
