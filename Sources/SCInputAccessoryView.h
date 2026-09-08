/*
 *  SCInputAccessoryView.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


@protocol SCInputAccessoryViewDelegate;

#pragma mark - SCInputAccessoryView
/// A self-sizing keyboard accessory with Previous, Next, Done, and optional
/// Clear controls.
///
/// STV assigns this view to supported input controls. Button actions notify
/// its delegate; `SCTableViewModel` implements navigation, clearing, and
/// keyboard dismissal. A standalone instance does not perform those actions
/// without a delegate. Programmatic initialization uses a constrained toolbar
/// with a minimum height of 44 points.
///
/// Sample use:
/// ```objective-c
/// self.tableViewModel.inputAccessoryView.rewind = FALSE;
/// ```
///
/// - SeeAlso: ``SCTableViewModel``
@interface SCInputAccessoryView : UIInputView


#pragma mark - Configuration

/// The toolbar holding the accessory's different buttons and controls.
@property (nonatomic, readonly) UIToolbar *toolbar;

/// The segmented control displaying the `Next` and `Previous` buttons.
@property (nonatomic, readonly) UISegmentedControl *previousNextSegmentedControl;

/// The `Done` button control.
@property (nonatomic, readonly) UIBarButtonItem *doneButton;

/// Whether the toolbar includes the Previous/Next control. Defaults to `YES`.
/// - Warning: The current setter removes the initial toolbar item when hidden,
///   but does not rebuild its bar-button wrapper when shown again. Avoid
///   toggling this back to `YES` after hiding the control.
@property (nonatomic, readwrite) BOOL showPrevNextButtons;

/// Whether the toolbar includes the Clear button. Defaults to `NO`.
/// STV updates this as controls begin editing, using the cell's
/// `showClearButtonInInputAccessoryView` setting.
@property (nonatomic, readwrite) BOOL showClearButton;

/// The `Clear` button control.
@property (nonatomic, readonly) UIBarButtonItem *clearButton;

/// Whether the model wraps Previous/Next navigation at the first or last
/// eligible input control. Defaults to `YES`. This is a navigation preference
/// read by the delegate; the accessory does not move focus itself.
@property (nonatomic, readwrite) BOOL rewind;


#pragma mark - Internal Properties & Methods (should only be used by the framework or when subclassing)

/// The weak event recipient, normally the owning `SCTableViewModel`.
/// Defaults to `nil`; actions with no implementing delegate are no-ops.
@property (nonatomic, weak, nullable) id<SCInputAccessoryViewDelegate> delegate;

/// Sends the optional Previous callback to the delegate.
- (void)previousTapped;

/// Sends the optional Next callback to the delegate.
- (void)nextTapped;

/// Sends the optional Clear callback to the delegate.
- (void)clearTapped;

/// Sends the optional Done callback to the delegate.
- (void)doneTapped;

@end


/// Receives accessory button requests. Used internally by `SCTableViewModel`
/// to perform the corresponding operation; all callbacks are optional.
@protocol SCInputAccessoryViewDelegate <NSObject>

@optional
/// Notifies the delegate that the previous control was requested.
/// - Parameter inputAccessoryView: The input accessory view sending the event.
- (void)inputAccessoryViewPreviousTapped:(SCInputAccessoryView *)inputAccessoryView;
/// Notifies the delegate that the next control was requested.
/// - Parameter inputAccessoryView: The input accessory view sending the event.
- (void)inputAccessoryViewNextTapped:(SCInputAccessoryView *)inputAccessoryView;
/// Notifies the delegate that the current value should be cleared.
/// - Parameter inputAccessoryView: The input accessory view sending the event.
- (void)inputAccessoryViewClearTapped:(SCInputAccessoryView *)inputAccessoryView;
/// Notifies the delegate that input is complete.
/// - Parameter inputAccessoryView: The input accessory view sending the event.
- (void)inputAccessoryViewDoneTapped:(SCInputAccessoryView *)inputAccessoryView;

@end

NS_ASSUME_NONNULL_END
