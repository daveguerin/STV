/*
 *  SCBadgeView.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCGlobals.h"

#pragma mark - SCBadgeView
/// A label used by `SCTableViewCell` to display badge text, such as a count.
///
/// Programmatic frame initialization uses a 15-point system font, gray text,
/// and a clear background. The view does not draw a badge shape; customize it
/// using the inherited `UILabel` properties.
///
/// Sample use:
/// ```objective-c
/// SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Number of pending tasks"];
/// cell.badgeView.text = @"50";
/// ```
///
/// - SeeAlso: ``SCTableViewCell``
@interface SCBadgeView : UILabel

#pragma mark - Configuration

@end
