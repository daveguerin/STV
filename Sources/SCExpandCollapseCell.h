/*
 *  SCExpandCollapseCell.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCTableViewCell.h"

NS_ASSUME_NONNULL_BEGIN


/// A special row that toggles the visibility of its owner section's other rows.
///
/// Assign the cell to the section's `expandCollapseCell` property rather than
/// adding it as an ordinary cell. A collapsed section shows only this row.
///
/// Sample use:
/// ```objective-c
/// mySection.expandCollapseCell = [SCExpandCollapseCell cellWithExpandText:@"Expand Section"
///     collapseText:@"Collapse Section" ownerSectionExpanded:FALSE];
/// ```
///
/// - SeeAlso: ``SCTableViewSection``
///
/// - Note: SCExpandCollapseCell is always displayed at the top of the section.
@interface SCExpandCollapseCell : SCCustomCell


#pragma mark - Creation and Initialization

/// Creates a cell with labels for each state and an initial expanded state.
/// - Parameter expandText: The collapsed-state label, or `nil` for no text.
/// - Parameter collapseText: The expanded-state label, or `nil` for no text.
/// - Parameter expanded: The initial expanded state of the cell's owner section.
+ (instancetype)cellWithExpandText:(nullable NSString *)expandText collapseText:(nullable NSString *)collapseText ownerSectionExpanded:(BOOL)expanded;

/// Initializes a cell with labels for each state and an initial expanded state.
/// - Parameter expandText: The collapsed-state label, or `nil` for no text.
/// - Parameter collapseText: The expanded-state label, or `nil` for no text.
/// - Parameter expanded: The initial expanded state of the cell's owner section.
- (instancetype)initWithExpandText:(nullable NSString *)expandText collapseText:(nullable NSString *)collapseText ownerSectionExpanded:(BOOL)expanded;


#pragma mark - Configuration

/// The label used when `ownerSectionExpanded` is set to `NO`. Defaults to `nil`.
/// Changing this property alone does not refresh the currently displayed text;
/// assign `ownerSectionExpanded` again to apply it.
@property (nonatomic, nullable, copy) NSString *expandText;

/// The label used when `ownerSectionExpanded` is set to `YES`. Defaults to `nil`.
/// Changing this property alone does not refresh the currently displayed text;
/// assign `ownerSectionExpanded` again to apply it.
@property (nonatomic, nullable, copy) NSString *collapseText;

/// Whether the owner section's other rows are visible. Defaults to `NO` unless
/// an expanded state is supplied to the initializer.
///
/// Assigning this property updates the label and disclosure accessory, and
/// asks the attached owner section to expand or collapse. Before attachment,
/// the cell stores the state for the section to use.
@property (nonatomic, readwrite) BOOL ownerSectionExpanded;

@end

NS_ASSUME_NONNULL_END
