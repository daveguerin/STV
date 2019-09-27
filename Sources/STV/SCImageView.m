/*
 *  SCImageView.m
 *  Sensible TableView
 *  Version: 13.0.0
 *

 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCImageView.h"


#define kBoundPropertyNameKey  @"boundPropertyName"


@implementation SCImageView

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    
    if(self)
    {
        self.boundPropertyName = [aDecoder decodeObjectForKey:kBoundPropertyNameKey];
    }
    
    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    [aCoder encodeObject:self.boundPropertyName forKey:kBoundPropertyNameKey];
}

@end
