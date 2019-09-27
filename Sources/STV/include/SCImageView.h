/*
 *  SCImageView.h
 *  Sensible TableView
 *  Version: 13.0.0
 *

 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>

/****************************************************************************************/
/*	class SCBadgeView	*/
/****************************************************************************************/
/**
 This class provides a way for UIImageViews to bind themselves to a specific object property.
 */
@interface SCImageView : UIImageView

/** The property the image view is bound to. The runtime value of this property must be one of the following: 1. A UIImage object. 2. An NSURL object containing a URL to the image. 3. The literal name of an image in the app's Resource folder (e.g. "MyImage.png"). */
@property (nonatomic, copy) IBInspectable NSString *boundPropertyName;

@end
