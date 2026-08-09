# STV Agent Guide

This file applies to the entire repository. It records durable project context and
working conventions for agents modifying STV. Prefer the live package manifest,
source, tests, and build output whenever this guide and the repository diverge.

## Project Summary

STV is the Objective-C/UIKit Sensible TableView framework distributed as a Swift
Package. It provides data-driven table-view models, sections, cells, data
stores, definitions, generated detail screens, and action-block customization.

- Package product and target: `STV`
- Language: Objective-C
- UI framework: UIKit
- Minimum deployment target: iOS 16
- Swift tools version: 5.9
- Public umbrella header: `Sources/SensibleTableView.h`
- Current package version macro: `kSTVCurrentVersion`
- Unit-test target: `STVTests`

STV is an iOS package. A host-platform `swift build` is not a meaningful build
check for this UIKit code; use `xcodebuild` with iOS and iOS Simulator
destinations.

## Repository Layout

- `Package.swift` — package products, targets, exclusions, and deployment target.
- `Sources/` — flat Objective-C public headers and implementations.
- `Sources/SensibleTableView.h` — umbrella header and runtime version macro.
- `Tests/STVTests/` — Objective-C XCTest coverage and test resources.
- `Documentation/STV.docc/STV.md` — authored DocC module main page.
- `Documentation/STV.doccarchive/` — generated DocC archive.
- `Scripts/Build STV Documentation.command` — reproducible ExtractAPI and DocC build.
- `README.md` — consumer requirements, installation, and usage.
- `CHANGELOG.md` — release history and behavior changes.
- `License/` — package license resources.

The main target is rooted at `.` with `sources: ["Sources"]` and
`publicHeadersPath: "Sources"`. `Documentation` and `Scripts` must remain
excluded from the target so generated documentation and scripts are not treated
as Clang inputs.

## Architecture

### Table model

`SCTableViewModel` is the central coordinator. It owns table sections, handles
UITableView data-source/delegate behavior, active-cell state, keyboard and input
accessory behavior, generated detail screens, searching, refresh controls, and
data-driven section generation.

### Sections and cells

`SCTableViewSection` and its subclasses manage section-level data, generated
cells, selection, insertion, deletion, moving, placeholder/add-item cells, and
detail models.

`SCTableViewCell` and its subclasses manage binding, validation, editing,
selection, generated controls, detail presentation, sizing, and value-change
notifications. Changes to model, section, or cell lifecycle behavior usually
need review across all three layers.

### Definitions and stores

Data definitions describe how values become UI:

- `SCDataDefinition`
- `SCClassDefinition`
- `SCEntityDefinition`
- `SCDictionaryDefinition`
- `SCUserDefaultsDefinition`
- property definitions, groups, types, and attributes

Data stores provide persistence and collection behavior:

- `SCDataStore`
- `SCArrayStore`
- `SCCoreDataStore`
- `SCUserDefaultsStore`

Core Data contexts are supplied by consuming applications. Preserve queue
confinement and report save failures through the existing error contract; a
framework persistence error must not terminate the host application.

### Controllers and actions

`SCViewController` and `SCTableViewController` host models and generated detail
flows. `SCCellActions`, `SCSectionActions`, `SCModelActions`, and the controller
action classes expose callback-based customization.

Action-block behavior is public API. Verify which object owns the checked block
and which cell, section, model, or controller represents the event. In
particular, deselection callbacks for an old active cell must receive that old
cell and its saved index path.

## Public API and Package Conventions

- Keep Objective-C `.h` and `.m` files flat in `Sources/`.
- Import each implementation's own header first, followed by required local or
  system headers, matching neighboring files.
- Do not move headers into a separate `Public` directory without deliberately
  changing the package architecture.
- Headers under `publicHeadersPath` are exported by SwiftPM even when they are
  not directly imported by the umbrella header. Treat changes to declarations,
  typedefs, enums, nullability, ownership, and macros as API changes.
- Add supported top-level APIs to `Sources/SensibleTableView.h` when consumers
  should receive them through `@import STV;`.
- Keep `kSTVCurrentVersion`, `README.md`, `CHANGELOG.md`, tests, and release
  documentation aligned when changing the package version.
- Preserve source compatibility unless a breaking change is explicitly in
  scope.

Dynamic Objective-C behavior is part of STV's architecture. KVC/key paths,
runtime class lookup, selector dispatch, and generated UI are not automatically
modernization defects. Validate their boundaries and behavior rather than
replacing them wholesale.

## Behavioral Invariants

### Cell sizing

STV 7 uses UIKit automatic row sizing by default.

- Every numeric `SCTableViewCell.height`, including `44`, is an explicit fixed
  height.
- Programmatically generated controls that still depend on manual frames use
  legacy sizing behavior.
- XIB/storyboard canvas heights are estimates, not fixed row heights.
- Cell binding/configuration must not be moved back into height-query callbacks.
- Height invalidation must remain safe while table-view updates are active.
- Object-selection summary rows must update after their selected values change.

Review `SCTableViewCellSizingTests.m` for the executable form of these rules.

### Removed legacy APIs

STV 7 intentionally excludes obsolete components and compatibility paths,
including:

- `SCImagePickerCell`
- `SCTheme`
- `SCPullToRefreshView`
- legacy runtime-availability helpers
- the `UISearchDisplayController` search path

Do not reintroduce them accidentally through headers, runtime class exposure, or
fallback code. `STVAvailabilityTests.m` verifies their absence and the supported
UIKit replacements.

### Existing deprecated UIKit

Some retained controller declarations still reference `UIPopoverController`
inside targeted `-Wdeprecated-declarations` suppression. Do not broaden warning
suppression or rewrite these paths as a drive-by change. If replacing the API,
audit presentation, delegate ownership, source view/bar button attachment, and
compact/regular-width behavior together.

### Known warning

The package currently emits `#warning TODO CHECK THIS` near the custom-label
`preferredMaxLayoutWidth` handling in `SCTableViewCell.m`. Do not remove the
warning without resolving and testing the underlying multiline-label behavior.

## Objective-C and UIKit Style

- Match the existing brace placement, naming, property attributes, and manual
  formatting in neighboring code.
- Use modern Objective-C literals and APIs when making a scoped change, but avoid
  broad formatting or syntax churn in these large legacy files.
- Keep UIKit work on the main thread.
- Use weak/strong captures where retained action blocks can capture owners.
- Do not invoke a block through a different object from the one whose block was
  checked.
- Preserve lifecycle ordering around model ownership, cell binding, selection,
  detail presentation, and table updates.
- Prefer explicit error propagation over exceptions, assertions, or process
  termination in reusable framework code.
- Do not replace dynamic STV model/section/cell behavior with a parallel table
  architecture for a narrow fix.

## Tests

The Objective-C XCTest target covers:

- automatic, fixed, legacy, and XIB-derived sizing;
- update-safe height invalidation;
- active-cell callback correctness;
- instance-scoped unarchiver mappings;
- Core Data save-error propagation;
- absence of removed STV 7 APIs;
- native search and refresh-control behavior;
- runtime package version consistency.

Add focused regression coverage when changing these areas. Test resources belong
under `Tests/STVTests/TestResources/` and must remain declared in `Package.swift`.

Run tests on an available iOS Simulator. For example:

```sh
xcodebuild \
  -scheme STV \
  -destination 'platform=iOS Simulator,name=iPhone 17 Pro' \
  -derivedDataPath /tmp/STVTests \
  test
```

Choose another available simulator when that device is not installed. Clean
temporary build products with `trash`, not `rm`, and do not clear global
DerivedData.

## DocC Documentation

Public Objective-C API comments use DocC-compatible `///` Markdown.

- Document public types, categories, protocols, methods, properties, typedefs,
  enums, enum cases, constants, and supported macros.
- Use `- Parameter name:`, `- Returns:`, `- Note:`, `- Warning:`, and
  `- SeeAlso:` fields.
- For block-valued properties, describe callback inputs and results as prose
  (for example, “The block receives…” and “The block returns…”); parameter and
  return fields can otherwise be misattached to the property itself.
- Use fenced `objective-c` blocks for examples.
- Use backticks or validated DocC links for symbol names; do not restore
  appledoc-style single-quoted references.
- Keep comments immediately adjacent to their declaration. Pragmas and
  preprocessor directives between a comment and declaration can detach the
  documentation.
- Clang may omit macro `docComment` fields from its symbol graph even when the
  macro has adjacent source documentation. Check source attachment before
  calling a macro undocumented.

Build the documentation with:

```sh
Scripts/Build STV Documentation.command
```

The module main page is authored in `Documentation/STV.docc/STV.md`. Keep its
overview and curated topic links aligned with the public API. The build script
passes this catalog to DocC together with the extracted Objective-C symbol
graph.

The script extracts the full Objective-C API, treats DocC warnings as errors,
emits coverage and navigator indexes, and writes the final archive to:

```text
Documentation/STV.doccarchive
```

Intermediate symbol graphs belong under `.build/documentation`. Do not place a
`.docc` catalog under `Sources`; because the Clang target is rooted at `.`, it
can be treated as a compiler input unless package exclusions are changed.

For documentation-only migrations, a successful compile is not enough to prove
that declarations were untouched. Preprocess the baseline and current umbrella
headers with the same SDK/target flags and compare their whitespace-separated
tokens when the diff is large.

## Build and Verification

Minimum package validation:

```sh
swift package dump-package

xcodebuild \
  -scheme STV \
  -destination 'generic/platform=iOS Simulator' \
  -derivedDataPath /tmp/STVBuild \
  build

git diff --check
```

For public API or portability changes, also build a device destination:

```sh
xcodebuild \
  -scheme STV \
  -destination 'generic/platform=iOS' \
  -derivedDataPath /tmp/STVDeviceBuild \
  CODE_SIGNING_ALLOWED=NO \
  build
```

For documentation changes, run `Scripts/Build STV Documentation.command` and verify
that `Documentation/STV.doccarchive/metadata.json` and the archive indexes exist.

## Repository Hygiene

- Check `git status --short --branch` before editing and preserve unrelated user
  changes in this frequently modified checkout.
- Do not reset, stash, commit, push, tag, publish, or rewrite history unless
  explicitly requested.
- Inspect scripts before running them; package scripts may have release or
  publishing side effects. The documentation script is build-only.
- Do not expose credentials, signing material, or private configuration.
- Keep edits narrow. Avoid drive-by rewrites of the large model, section, and
  cell implementations.
- Use `trash` for cleanup and removal.
- Do not silently revert generated documentation or package-manifest changes.
