/*
 *  SCExpandCollapseCell.h
 *  Sensible TableView
 *  Version: 13.0.0
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCTableViewCell.h"




/**
 SCExpandCollapseCell is a special cell class that, once tapped, collapses its owner section's contents into a single cell. When tapped again, it will expand the section's contents to their original state.
 
 Sample use:
    mySection.expandCollapseCell = [SCExpandCollapseCell cellWithExpandText:@"Expand Section" 
        collapseText:@"Collapse Section" ownerSectionExpanded:FALSE];
 
 See also: SCTableViewSection
 
 @note SCExpandCollapseCell is always displayed at the top of the section.
 */
@interface SCExpandCollapseCell : SCCustomCell


//////////////////////////////////////////////////////////////////////////////////////////
/// @name Creation and Initialization
//////////////////////////////////////////////////////////////////////////////////////////

/** Allocates and returns an initialized SCExpandCollapseCell given its expandText, collapseText, an initial owner section expand state.
 @param expandText The text that will be displayed in the cell when the section is collapsed.
 @param collapseText The text that will be displayed in the cell when the section is expanded.
 @param expanded The initial expanded state of the cell's owner section.
 */
+ (instancetype)cellWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded;

/** Returns an initialized SCExpandCollapseCell given its expandText, collapseText, an initial owner section expand state.
 @param expandText The text that will be displayed in the cell when the section is collapsed.
 @param collapseText The text that will be displayed in the cell when the section is expanded.
 @param expanded The initial expanded state of the cell's owner section.
 */
- (instancetype)initWithExpandText:(NSString *)expandText collapseText:(NSString *)collapseText ownerSectionExpanded:(BOOL)expanded;


//////////////////////////////////////////////////////////////////////////////////////////
/// @name Configuration
//////////////////////////////////////////////////////////////////////////////////////////

/** The text that will be displayed in the cell when the section is collapsed. */
@property (nonatomic, copy) NSString *expandText;

/** The text that will be displayed in the cell when the section is expanded. */
@property (nonatomic, copy) NSString *collapseText;

/** The expanded state of the ownerSection. Set to TRUE to expand the ownerSection's content. Default:FALSE. */
@property (nonatomic, readwrite) BOOL ownerSectionExpanded;

@end
