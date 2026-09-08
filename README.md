# STV
A Swift Package of the amazing **Sensible TableView** framework by [Sensible Cocoa](http://sensiblecocoa.com/).

iOS compatibility and Swift Packaging by [dgApps](http://dgapps.ie/). Although it's now a Swift Package STV is still all glorious Objective-C! This version is not so much forked from the [open source version](https://github.com/wizgod/STV) from [wizgod](https://github.com/wizgod), as ripped apart and then rebuilt into an SPM-compatible format.

### What is Sensible TableView?
**Sensible TableView** (STV) is a Swift Package that drastically simplifies iOS table view development, while focusing on delivering a simple and enjoyable developer experience. Every single thing that can be done with normal table views can still be done with STV, only a lot easier. Have a custom UITableView subclass? No problem, STV flawlessly integrates with that. A custom cell? Even better! Custom view controllers? STV’s bread and butter! STV elegantly reduces the amount of work you have to do to an absolute bare minimum!

Based on countless user testimonials and feedback, [Sensible Cocoa](http://sensiblecocoa.com/) found that developers were saving on average about 70% of their development time when they started using STV in their apps. What this means is that an app that would normally take weeks or even months to develop alone, can now be conceived in just a matter of days using STV. Since STV keeps your code very short, simple and straight forward, maintenance suddenly becomes much easier.

Unfortunately the phenomenal visual interface that [Sensible Cocoa](http://sensiblecocoa.com/) developed that worked in Interface Builder was [kiboshed by Apple with Xcode 8](http://sensiblecocoa.com/community/topic/2462-sad-news-for-stv-from-wwdc/). However all the code side of STV still works, and that's what's packaged here.

## NOTE
Version 7.1.0 targets iOS 16 and later. `SCImagePickerCell` is intentionally excluded until it can be redesigned around PhotosUI without deprecated APIs.

### 7.1.0: Objective-C API modernization

- Properties use automatic synthesis by default, with underscored backing ivars.
  Six explicit syntheses remain for custom-accessor storage or runtime property
  metadata compatibility; each is documented at its declaration.
- Implementation-only ivars live in `.m` extensions. Protected storage used by
  subclasses remains available, with the migrated underscored names. Prefer
  properties rather than depending on an implementation's ivar layout.
- Public headers have audited nullability and lightweight collection generics.
  Swift callers must handle optional lookups, factories, callbacks, and results.
  Heterogeneous item collections and the legacy `editActions` hook remain untyped;
  use `leadingSwipeActions`/`trailingSwipeActions` for modern contextual actions.
- `SCViewController.popoverController` and
  `SCTableViewController.popoverController` are now **weak, nullable
  `UIPopoverPresentationController` references**, not `UIPopoverController`.
  Configure a presented controller with `UIModalPresentationPopover`, obtain its
  `popoverPresentationController`, and supply a source view/rectangle or bar item.
  UIKit owns presentation lifetime and compact-size adaptation. STV presents the
  actual detail navigation controller and preserves dismissal validation/actions.
- STV-managed navigation search uses stacked placement so moving it into a table
  header does not leave its text field behind. Detaching restores the host's
  placement preference. While header search is active, UIKit owns the bar's
  layout; STV reinstalls its header constraints after dismissal.
- Deprecation-warning suppressions have been removed, including disabled source.

Rebuild consumers after migrating direct ivar access, popover types, and Swift
optionals. Public property selectors remain available; private ivar layout is
not an ABI compatibility guarantee. Version 7.1.0 includes source and behavior
changes; review these migration notes before upgrading.

Verify all standalone headers, required Clang misuse diagnostics, and a real
Swift imported-API consumer against both SDKs:

```sh
python3 Scripts/check-api-contracts.py
python3 Scripts/check-api-contracts.py --sdk iphoneos
```

Actual presentation and search-editing regressions use an app-hosted test runner
separate from SwiftPM's hostless unit suite. See
[Popover/presentation verification](Scripts/PopoverVerification/README.md).

### 7.1.0: constraint-based layout

Version 7.1.0 uses Auto Layout and UIKit self-sizing throughout STV-owned
cell content and supporting views. Models default to automatic sizing. Generated
controls fit their cell's `contentView`, not a screen or table-frame width.
XIB/storyboard cells must provide complete constraints; their canvas heights are
only estimates. STV no longer clones cells to calculate a row height.

`SCTableViewCell.height` now reads `UITableViewAutomaticDimension`. Assigning a
positive height remains a compatibility convenience that requests a content
height **constraint**, rather than returning a numeric height to UITableView.
Model-wide fixed-height policy and custom-height callbacks are also translated
into constraints. Prefer automatic sizing for text that must remain fully
visible. The legacy sizing-mode name no longer selects a manual measurement
path.

STV no longer discovers split-view columns, rewrites their controllers, acts as
a split delegate, or makes collapse/separation decisions. The app owns its
container hierarchy. Explicit detail models, generated editors, validation,
and ordinary push/modal presentation remain supported.

#### Consumer migration

- Do not read `cell.height` as the final on-screen measurement. Read UIKit's
  laid-out geometry when necessary; configure appearance through constraints.
- Install custom-content constraints once, including a complete vertical chain.
  Do not set frames in `didLayoutSubviews` or expect a custom header/footer's
  initial frame height to determine its runtime height.
- Preserve UIKit's ownership of controller roots and native accessory roots.
  Disable autoresizing-mask translation on subviews **you constrain**, not on
  UIKit-managed roots whose placement is supplied by the container.
- Generated text editors expose UIKit's already-calculated text `contentSize`
  as an intrinsic height; STV does not measure text with its own TextKit code.
  Minimum/maximum constraints bound the viewport, while scrolling remains
  available so a capped editor still accepts and reveals more input. For a
  custom plain `UITextView` that should grow without a cap, disabling internal
  scrolling is the usual native intrinsic-sizing option.
- Assign any shared detail model explicitly; STV does not infer one from an
  app's split view. Keep a single app-owned split coordinator.

#### Verification

Run the source-boundary guard and simulator tests from the package root:

```sh
python3 Scripts/check-autolayout.py
xcodebuild -scheme STV -destination 'platform=iOS Simulator,name=iPhone 17 Pro' test
```

The source guard rejects STV-owned frame assignments, manual text-measurement
layout, content autoresizing-mask assignments, and split integration. Its one
explicit native-container exception is the keyboard accessory root's
`UIViewAutoresizingFlexibleHeight` hint: UIKit needs that hint to honor the
intrinsic height. The toolbar's content still uses constraints exclusively.
The guard complements, rather than replaces, runtime fitting, editing,
resizing, and presentation tests.

### Real UIKit rendering fixture

SwiftPM's hostless XCTest runner cannot always attach a real keyboard accessory
window. Use the isolated consumer app for that boundary and visual checks:

```sh
xcrun simctl list devices available
python3 Scripts/AutoLayoutVerification/run.py --device <SIMULATOR-UDID>
python3 Scripts/AutoLayoutVerification/run.py --device <SIMULATOR-UDID> --reuse-build --mode keyboard
python3 Scripts/AutoLayoutVerification/run.py --device <SIMULATOR-UDID> --reuse-build --mode date
python3 Scripts/AutoLayoutVerification/run.py --device <SIMULATOR-UDID> --reuse-build --mode embedded-date
python3 Scripts/AutoLayoutVerification/run.py --device <SIMULATOR-UDID> --reuse-build --no-sidebar
```

The fixture builds this local package, creates an ad-hoc-signed simulator app
under the system temporary directory, and installs/launches only that app.
Omit `--reuse-build` after source edits. It uses clearly labelled test data,
not SetList data, and does not modify an app project. Its split view belongs
to the fixture app, not STV.

Inspect both sidebar states, text wrapping, native control placement, expanded
picker containment, and the complete keyboard toolbar. The script reports a
launch, not an automated visual-test verdict; capture evidence separately:

```sh
xcrun simctl io <SIMULATOR-UDID> screenshot /tmp/STV-layout.png
```

## LICENSE
**SensibleTableView** is licensed under the MIT License. See [STV.txt](License/STV.txt)
and [STV.plist](License/STV.plist) in the [License](License/) folder.

Copyright © 2019-2026 dgApps. All rights reserved.

## REQUIREMENTS
* Xcode 15 and later.
* iOS 16 and later.

## USAGE
Add STV to an Xcode project as simply as:

* `File > Add Package Dependencies...`
* Use the URL: [https://github.com/daveguerin/STV/](https://github.com/daveguerin/STV/)
* Select 'Up to Next Major Version' with a minimum version of `7.1.0`.
* Choose the target where you want to use it.
* Add an `@import STV;` in your code.
* Marvel and enjoy! 
* Maybe RTFM a bit :-)
 
## DOCUMENTATION
5.x Manual: [http://sensiblecocoa.com/usermanual/latest](http://sensiblecocoa.com/usermanual/latest)

5.x API Docs: [http://sensiblecocoa.com/documentation/latest](http://sensiblecocoa.com/documentation/latest)

5.x Docset Feed: [http://sensiblecocoa.com/documentation/latest/com.sensiblecocoa.STV-5.0.atom](http://sensiblecocoa.com/documentation/latest/com.sensiblecocoa.STV-5.0.atom)  

6.x API Docs: [https://daveguerin.github.io/STVDocumentation/](https://daveguerin.github.io/STVDocumentation/)

6.x Docset Feed: [https://daveguerin.github.io/STVDocumentation/docsets/STV.xml](https://daveguerin.github.io/STVDocumentation/docsets/STV.xml)  

7.1.0 API documentation is maintained as DocC-compatible comments in the
[public headers](Sources/). The older 5.x and 6.x references above are historical
and do not describe the 7.x API removals or 7.1.0 migrations. A generated 7.x
documentation archive and build script are not currently shipped in this
repository.

The Docset Feeds are for [Dash](https://kapeli.com/dash) and other documentation browsers.

## ISSUES
Please report any issues and bugs with this Swift Package of STV via [https://github.com/daveguerin/STV/issues](https://github.com/daveguerin/STV/issues). 

[Pull requests](https://github.com/daveguerin/STV/pulls) are very much appreciated!

## CHANGELOG
See [CHANGELOG.md](CHANGELOG.md) for release history.
