#import "SCCellActions.h"
#import "SCSectionActions.h"
#import "SCModelActions.h"
#import "SCViewControllerActions.h"
#import "SCTableViewControllerActions.h"

// Compile-only contracts: expected diagnostics prove optional hooks/results and
// typed collections; the remaining calls must accept nil without diagnostics.
void STVActionContracts(SCCellActions *_Nonnull cellActions, SCSectionActions *_Nonnull sectionActions, SCModelActions *_Nonnull modelActions, SCViewControllerActions *_Nonnull viewActions, SCTableViewControllerActions *_Nonnull tableActions, SCTableViewCell *_Nonnull cell, SCArrayOfItemsSection *_Nonnull section, SCArrayOfItemsModel *_Nonnull itemsModel, SCTableViewModel *_Nonnull model, NSIndexPath *_Nonnull path, NSObject *_Nonnull item) {
    SCCellAction_Block _Nonnull cellHook = cellActions.willConfigure;               // expected-warning {{implicit conversion from nullable pointer}}
    SCSectionAction_Block _Nonnull sectionHook = sectionActions.valueChanged;       // expected-warning {{implicit conversion from nullable pointer}}
    SCModelAction_Block _Nonnull modelHook = modelActions.didScroll;                // expected-warning {{implicit conversion from nullable pointer}}
    SCViewControllerAction_Block _Nonnull viewHook = viewActions.willAppear;        // expected-warning {{implicit conversion from nullable pointer}}
    SCTableViewControllerAction_Block _Nonnull tableHook = tableActions.willAppear; // expected-warning {{implicit conversion from nullable pointer}}
    SCTableViewModel *_Nonnull owner = modelActions.ownerTableViewModel;            // expected-warning {{implicit conversion from nullable pointer}}
    cellActions.willConfigure = nil;
    sectionActions.valueChanged = nil;
    modelActions.didScroll = nil;
    viewActions.willAppear = nil;
    tableActions.willAppear = nil;
    [cellActions setActionsTo:nil overrideExisting:YES];
    [sectionActions setActionsTo:nil overrideExisting:YES];
    [modelActions setActionsTo:nil overrideExisting:YES];
    cellActions.willConfigure(cell, nil);
    cellActions.didLoadBoundValue(cell, nil, nil);
    cellActions.canPerformAction(cell, nil, @selector(copy), nil);
    cellActions.detailModelCreated(cell, nil, nil);
    sectionActions.detailModelCreated((SCTableViewSection *)section, nil, nil);
    sectionActions.willInsertItem(section, item, nil);
    sectionActions.willUpdateItem(section, item, path, nil);
    sectionActions.fetchItemsFromStoreFailed(section, nil);
    sectionActions.insertItemFailed(section, item, nil);
    sectionActions.updateItemFailed(section, item, nil);
    sectionActions.deleteItemFailed(section, item, nil);
    NSObject *_Nonnull value = cellActions.calculatedValue(cell, nil);                         // expected-warning {{implicit conversion from nullable pointer}}
    UIViewController *_Nonnull detail = cellActions.detailViewController(cell, nil);           // expected-warning {{implicit conversion from nullable pointer}}
    SCTableViewModel *_Nonnull detailModel = cellActions.detailTableViewModel(cell, nil);      // expected-warning {{implicit conversion from nullable pointer}}
    SCCustomCell *_Nonnull customCell = sectionActions.cellForRowAtIndexPath(section, path);   // expected-warning {{implicit conversion from nullable pointer}}
    NSString *_Nonnull reuse = sectionActions.reuseIdentifierForRowAtIndexPath(section, path); // expected-warning {{implicit conversion from nullable pointer}}
    NSString *_Nonnull title = modelActions.sectionHeaderTitleForItem(itemsModel, item, 0);    // expected-warning {{implicit conversion from nullable pointer}}
    NSArray *_Nonnull titles = modelActions.sectionHeaderTitles(itemsModel);                   // expected-warning {{implicit conversion from nullable pointer}}
    NSArray *_Nonnull results = modelActions.didComputeSearchResults(itemsModel, @"", nil);    // expected-warning {{implicit conversion from nullable pointer}}
    NSMutableArray<NSString *> *wrongSections = [NSMutableArray array];
    modelActions.sortSections(model, wrongSections); // expected-warning {{incompatible pointer types}}
    modelActions.sectionHeaderTitles = ^NSArray<NSString *> *(SCArrayOfItemsModel *m) {
        return @[@"Title"];
    };
    // Fetched and special-cell lists can contain ordinary data objects too.
    sectionActions.didAddSpecialCells(section, [NSMutableArray arrayWithObject:item]);
    cellActions.leadingSwipeActions = ^NSArray<UIContextualAction *> *(SCTableViewCell *c, NSIndexPath *p) {
        return nil;
    };
    cellActions.trailingSwipeActions = ^NSArray<UIContextualAction *> *(SCTableViewCell *c, NSIndexPath *p) {
        return nil;
    };
}
