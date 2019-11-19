/*
 *  SCBadgeView.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCBadgeView.h"


@implementation SCBadgeView

- (instancetype)initWithFrame:(CGRect)aRect
{
	if( (self = [super initWithFrame:aRect]) )
	{
        self.font = [UIFont systemFontOfSize:15];
        self.textColor = [UIColor grayColor];
        
		self.backgroundColor = [UIColor clearColor];//self.backgroundColor = [UIColor redColor];
	}
	
	return self;
}

@end
