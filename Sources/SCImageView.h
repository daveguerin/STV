/*
 *  SCImageView.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

#pragma mark - SCBadgeView
/// An image view that supplies a property name for `SCCustomCell` binding.
///
/// The containing custom cell discovers this view among its direct
/// `contentView` subviews and loads the bound value. This class stores and
/// archives the binding name; it does not observe or load values by itself.
@interface SCImageView : UIImageView

/// The bound object's property name or key path, not the image value itself.
/// Defaults to `nil`, which supplies no automatic binding name.
///
/// The custom cell accepts these property values:
/// - A `UIImage`, assigned directly.
/// - An `NSString` containing a URL with a scheme and host, loaded on a
///   background queue and assigned on the main queue.
/// - Any other `NSString`, passed to `UIImage imageNamed:`.
///
/// `NSURL`, `nil`, and other value types leave the existing image unchanged.
/// A failed string-based load assigns `nil`. URL loads have no cancellation
/// or reuse check, so an earlier request can overwrite a newer image.
@property (nonatomic, copy, nullable) IBInspectable NSString *boundPropertyName;

@end

NS_ASSUME_NONNULL_END
