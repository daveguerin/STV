# STV CHANGELOG

## STV 6.0.5
swift-tools-version:5.3 and .iOS(.v13).
SCImagePickerCell remarked out for now, it needs a full rewrite for iOS 13 and up.
As we're now iOS 13 and up UIActivityIndicatorViewStyleMedium is always available.
beginAnimations:/commitAnimations replaced with the block based animateWithDuration:animations: completion: in SCPullToRefreshView.
cellWithCell: now forced to be requiringSecureCoding:NO so STVCell is actually a copy once more of cell and not nil in SCTableViewCell.
When laying out subviews of a SCCustomCell 0.95 times the calculated width is now used as the preferredMaxLayoutWidth to avoid the last word or two of labels not being displayed occasionally.

## STV 6.0.4
SCDebugLog now logs more information.
The default textColor of the label in a SCLabelCell is now secondaryLabelColor on iOS 13 and up.
The default backgroundColor of the label in a SCLabelCell is now clearColor.
The default font of the label in a SCLabelCell is now UIFontTextStyleCallout.

## STV 6.0.3
* Reconfigured the SwiftPM somewhat so that all the H and M files are now back together where they should be, and together in Sources not Sources/STV.
* The main Git branch is now named `main`.

## STV 6.0.2
* uiElementNibName can now use NIBs from a SwiftPM using the naming format of `PACKAGE_TARGET_NIBName`, and still not using including the `xib` extension.
* In iOS 14 a nil background colour was displaying as black, so all UILabel backgroundColor are now clearColor.

## STV 6.0.1
* Documentation is now in a seperate git repository.
* All SCSelectionCell are now using linkColor for their textLabel when on iOS 13 up.
* The pullToRefreshView property of SCTableViewModel that was marked deprecated is now remarked out.
* The modeledTableView  property of SCTableViewModel that was marked deprecated is now remarked out.
* The setModeledTableViewEditing:animated: method  of SCTableViewModel that was marked deprecated is now remarked out.

## STV 6.0.0
* No longer a framework, now a Swift Package.
* Added Ozies fix for disappearing cells in iOS 11 [http://sensiblecocoa.com/community/topic/2535-stv-ios-11-issues/#entry13604](http://sensiblecocoa.com/community/topic/2535-stv-ios-11-issues/#entry13604). Thanks Ozie!
* Added lots of `self->` to avoid an Xcode warning: "_Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behaviour_".
* Remarked out the `-splitViewController:shouldHideViewController:inOrientation:` method as it's now deprecated.
* The `-live` method of `SCTableViewModel` was causing an assert in iOS 13: "_Attempted to access the table view's visibleCells while they were in the process of being updated, which is not allowed_". So STV is no longer trying to count the visibleCells there, which might make it slightly less efficient, but there's no assert!
* The `-cellAtIndexPath:` method of `SCTableViewModel` was also causing an assert in iOS 13: "_Attempted to call -cellForRowAtIndexPath: on the table view while it was in the process of updating its visible cells, which is not allowed_". This was introduced by the change in `-live`. So STV is no longer calling `-cellForRowAtIndexPath` here, which again is going to make it less efficient, but now there's no assert.
* The `-tableView:shouldIndentWhileEditingRowAtIndexPath:` method of  `SCTableViewModel` was also causing an assert in iOS 13: "_Attempted to call -cellForRowAtIndexPath: on the table view while it was in the process of updating its visible cells, which is not allowed._" A bit of a kludge, but the method now just returns the default value, YES, here.
* Remarked out a deprecated method `-splitViewController:shouldHideViewController:inOrientation:` in both `SCViewController` and `SCTableViewController`.
* Fixed lots of "_This block declaration is not a prototype_" warnings from Xcode 11 with the suggested fix of adding `void`.
* Removed all the Interface Builder/Storyboard stuff, sorry Tarek.
* Removed the examples.
* Removed the framework build scripts as they are no longer required.
* Removed the install and uninstall scripts as they are no longer required.
* Removed the Xcode templates.
* Edited the inline documentation to fix all Xcode warnings.
* Added STVCoreData classes to the main package. STViCloud, STVParse, and STVWebServices are not part of this package.
* Added license files in TXT and PLIST format. The PLIST can be used in a Settings bundle.
* Bumped the version to 6.0.0.
* Now using iOS 13 labelColor with a fallback for earlier iOS.
* Now using UIFontTextStyleBody as the main font.
* Added two new cellActions: leadingSwipeActions and trailingSwipeActions. iOS 11 and up only. 
* All UIPopoverController are now clang diagnostic ignored "-Wdeprecated-declarations"


## VERSION HISTORY
* STV 6.0.5 Sep 30 2023
* STV 6.0.3 Sep 14 2020
* STV 6.0.2 Sep 14 2020
* STV 6.0.1 Nov 19 2019 
* STV 6.0.0 Nov 12 2019 - First [open source Swift Package release](https://github.com/daveguerin/STV) by [dgApps](http://dgapps.ie/).  
* STV 5.4.0 Apr 12 2018 - First [open source release](https://github.com/wizgod/STV) by [wizgod](https://github.com/wizgod) with [permission of Sensible Cocoa/Tarek Sakr](http://sensiblecocoa.com/community/topic/2534-stv-on-github/#entry13576).  
* STV 5.4.0 Oct 15 2016 - Last closed source release from Sensible Cocoa.  
* STV 4.2.5 Jun 30 2015  
* STV 3.4.2 Jul 02 2014  
* STV 2.2.0 Sep 29 2012
