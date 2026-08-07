# ``STV``

Build data-driven UIKit table interfaces with Sensible TableView.

## Overview

STV is an Objective-C framework distributed as a Swift Package for iOS 16 and
later. It coordinates table-view controllers, models, sections, cells, data
definitions, and data stores so applications can construct rich UIKit table
interfaces with substantially less data-source and delegate plumbing.

A typical STV interface starts with an ``SCTableViewController`` or
``SCViewController``. Its ``SCTableViewModel`` owns one or more
``SCTableViewSection`` instances, and each section either contains explicitly
created ``SCTableViewCell`` objects or generates cells from a data definition.
Action objects provide callback-based customization without requiring a
subclass for every lifecycle event.

For example, an `SCTableViewController` subclass can add a simple section to its
existing model:

```objective-c
SCTableViewSection *section =
    [SCTableViewSection sectionWithHeaderTitle:@"Status"];
[section addCell:[SCTableViewCell cellWithText:@"Connected"]];
[self.tableViewModel addSection:section];
```

For object-driven interfaces, use a definition such as
``SCClassDefinition``, ``SCEntityDefinition``, or ``SCDictionaryDefinition``
together with a compatible ``SCDataStore``. STV then derives the appropriate
sections and controls from the object's properties while retaining access to
normal UIKit customization.

### Core Concepts

- **Models** coordinate a table view and own its sections.
- **Sections** organize rows and can generate cells from objects or collections.
- **Cells** bind UIKit controls to values, validate edits, and manage detail
  presentation.
- **Definitions** describe the properties and controls used to represent data.
- **Stores** provide collection, Core Data, or user-defaults persistence.
- **Actions** customize lifecycle events with Objective-C blocks.

## Topics

### Controllers and Models

- ``SCTableViewController``
- ``SCViewController``
- ``SCTableViewModel``
- ``SCArrayOfItemsModel``
- ``SCArrayOfObjectsModel``
- ``SCSelectionModel``
- ``SCObjectSelectionModel``
- ``SCDetailViewControllerOptions``

### Sections

- ``SCTableViewSection``
- ``SCObjectSection``
- ``SCArrayOfItemsSection``
- ``SCArrayOfObjectsSection``
- ``SCSelectionSection``
- ``SCObjectSelectionSection``

### Cells

- ``SCTableViewCell``
- ``SCCustomCell``
- ``SCControlCell``
- ``SCTextFieldCell``
- ``SCNumericTextFieldCell``
- ``SCTextViewCell``
- ``SCSelectionCell``
- ``SCObjectSelectionCell``
- ``SCObjectCell``

### Data Definitions

- ``SCDataDefinition``
- ``SCClassDefinition``
- ``SCEntityDefinition``
- ``SCDictionaryDefinition``
- ``SCUserDefaultsDefinition``
- ``SCPropertyDefinition``
- ``SCPropertyGroup``
- ``SCPropertyAttributes``
- ``SCPropertyType``
- ``SCDataType``

### Data Stores

- ``SCDataStore``
- ``SCArrayStore``
- ``SCCoreDataStore``
- ``SCUserDefaultsStore``

### Actions and Delegates

- ``SCCellActions``
- ``SCSectionActions``
- ``SCModelActions``
- ``SCTableViewControllerActions``
- ``SCViewControllerActions``
- ``SCTableViewControllerDelegate``
- ``SCViewControllerDelegate``

### Navigation and Presentation

- ``SCNavigationBarType``
