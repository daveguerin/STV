# App-hosted presentation verification

This isolated Xcode fixture consumes the local STV package without changing an
application project or the package's hostless unit-test target. UIKit needs a
foreground application to complete modal transitions and search editing reliably.

From the STV package root:

```sh
xcrun simctl list devices available
xcodebuild -project Scripts/PopoverVerification/STVPopoverHost.xcodeproj \
  -scheme STVPopoverHost \
  -destination 'platform=iOS Simulator,id=<SIMULATOR-UDID>' \
  -derivedDataPath /tmp/STVPresentationVerification \
  -parallel-testing-enabled NO test
```

Run on both iPhone and iPad. Do not run another XCTest invocation against the same
simulator concurrently. The fixture installs only `ie.dgapps.STVPopoverHost`.

Coverage includes real detail navigation-controller presentation, cell/bar-item/
fallback anchors, weak ownership, explicit Done/Cancel veto and actions, compact/
regular adaptation, popover detection, and repeated header-search activation after
navigation/header reattachment. Interactive dismissal delegate methods are
invoked explicitly to test cancellation, completion, unrelated-controller guards,
and duplicate callbacks; these are not automated outside-tap or drag gestures.
The search test activates the real keyboard and checks field geometry, filtering,
dismissal, and reactivation.

Keep the ordinary package suite as well:

```sh
xcodebuild -scheme STV \
  -destination 'platform=iOS Simulator,id=<SIMULATOR-UDID>' test
```
