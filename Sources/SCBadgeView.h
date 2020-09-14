/*
 *  SCBadgeView.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */


#import "SCGlobals.h"

/****************************************************************************************/
/*	class SCBadgeView	*/
/****************************************************************************************/ 
/**	
 This class provides as a badge view similar to the one used by iPhone's mail application to
 display the number of messages in an inbox. SCBadgeView is typically used by SCTableViewCell to display these kind of badges.
 
 Sample use:
    SCTableViewCell *cell = [SCTableViewCell cellWithText:@"Number of pending tasks"];
    cell.badgeView.text = @"50";
 
 See also: SCTableViewCell
 */
@interface SCBadgeView : UILabel

//////////////////////////////////////////////////////////////////////////////////////////
/// @name Configuration
//////////////////////////////////////////////////////////////////////////////////////////

@end
