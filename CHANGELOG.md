# STV CHANGELOG
## STV 6.0.1
* All SCSelectionCell are now using linkColor for their textLabel when on iOS 13 up.

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
* STV 6.0.0 Nov 12 2019 - First [open source Swift Package release](https://github.com/daveguerin/STV) by [dgApps](http://dgapps.ie/).  
* STV 5.4.0 Apr 12 2018 - First [open source release](https://github.com/wizgod/STV) by [wizgod](https://github.com/wizgod) with [permission of Sensible Cocoa/Tarek Sakr](http://sensiblecocoa.com/community/topic/2534-stv-on-github/#entry13576).  
* STV 5.4.0 Oct 15 2016 - Last closed source release from Sensible Cocoa.  
* STV 4.2.5 Jun 30 2015  
* STV 3.4.2 Jul 02 2014  
* STV 2.2.0 Sep 29 2012
