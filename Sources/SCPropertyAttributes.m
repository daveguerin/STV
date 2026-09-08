/*
 *  SCPropertyAttributes.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import "SCPropertyAttributes.h"

#import "SCClassDefinition.h"
#import "SCStringDefinition.h"
#import "SCArrayStore.h"
#import "SCEntityDefinition.h"
#import "SCCoreDataStore.h"
#import "SCSectionActions.h"


@implementation SCPropertyAttributes

- (instancetype)init {
    if ((self = [super init])) {
        _imageView = nil;
        _imageViewArray = nil;

        _expandContentInCurrentView = FALSE;
        _expandedContentSectionActions = [[SCSectionActions alloc] init];
    }

    return self;
}


@end


@implementation SCTextViewAttributes

+ (instancetype)attributesWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight autoResize:(BOOL)_autoResize editable:(BOOL)_editable {
    return [[[self class] alloc] initWithMinimumHeight:minHeight maximumHeight:maxHeight autoResize:_autoResize editable:_editable];
}

- (instancetype)init {
    if ((self = [super init])) {
        _minimumHeight = FLT_MIN; // will be ignored
        _maximumHeight = FLT_MIN; // will be ignored
        _autoResize = TRUE;
        _editable = TRUE;
    }

    return self;
}

- (instancetype)initWithMinimumHeight:(CGFloat)minHeight maximumHeight:(CGFloat)maxHeight autoResize:(BOOL)_autoResize editable:(BOOL)_editable {
    if ((self = [self init])) {
        self.minimumHeight = minHeight;
        self.maximumHeight = maxHeight;
        self.autoResize = _autoResize;
        self.editable = _editable;
    }

    return self;
}


@end


@implementation SCTextFieldAttributes

+ (instancetype)attributesWithPlaceholder:(NSString *)_placeholder {
    return [[[self class] alloc] initWithPlaceholder:_placeholder];
}

+ (instancetype)attributesWithPlaceholder:(NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization {
    return [[[self class] alloc] initWithPlaceholder:_placeholder secureTextEntry:secure autocorrectionType:autocorrection autocapitalizationType:autocapitalization];
}

- (instancetype)init {
    if ((self = [super init])) {
        _placeholder = nil; // will be ignored
        _secureTextEntry = FALSE;
        _autocorrectionType = UITextAutocorrectionTypeDefault;
        _autocapitalizationType = UITextAutocapitalizationTypeSentences;
    }

    return self;
}

- (instancetype)initWithPlaceholder:(NSString *)_placeholder {
    if ((self = [self init])) {
        self.placeholder = _placeholder;
    }

    return self;
}

- (instancetype)initWithPlaceholder:(NSString *)_placeholder secureTextEntry:(BOOL)secure autocorrectionType:(UITextAutocorrectionType)autocorrection autocapitalizationType:(UITextAutocapitalizationType)autocapitalization {
    if ((self = [self init])) {
        self.placeholder = _placeholder;
        self.secureTextEntry = secure;
        self.autocorrectionType = autocorrection;
        self.autocapitalizationType = autocapitalization;
    }

    return self;
}


@end


@implementation SCNumericTextFieldAttributes

+ (instancetype)attributesWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat {
    return [[[self class] alloc] initWithMinimumValue:minValue maximumValue:maxValue allowFloatValue:allowFloat];
}

+ (instancetype)attributesWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat placeholder:(NSString *)_placeholder {
    return [[[self class] alloc] initWithMinimumValue:minValue maximumValue:maxValue allowFloatValue:allowFloat placeholder:_placeholder];
}

- (instancetype)init {
    if ((self = [super init])) {
        _minimumValue = nil; // will be ignored
        _maximumValue = nil; // will be ignored
        _allowFloatValue = TRUE;

        _numberFormatter = [[NSNumberFormatter alloc] init];
        [_numberFormatter setNumberStyle:NSNumberFormatterDecimalStyle];
    }

    return self;
}

- (instancetype)initWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat {
    if ((self = [self init])) {
        self.maximumValue = maxValue;
        self.minimumValue = minValue;
        self.allowFloatValue = allowFloat;
    }

    return self;
}

- (instancetype)initWithMinimumValue:(NSNumber *)minValue maximumValue:(NSNumber *)maxValue allowFloatValue:(BOOL)allowFloat placeholder:(NSString *)_placeholder {
    if ((self = [self initWithPlaceholder:_placeholder])) {
        self.maximumValue = maxValue;
        self.minimumValue = minValue;
        self.allowFloatValue = allowFloat;
    }

    return self;
}


@end


@implementation SCSliderAttributes

+ (instancetype)attributesWithMinimumValue:(float)minValue maximumValue:(float)maxValue {
    return [[[self class] alloc] initWithMinimumValue:minValue maximumValue:maxValue];
}

- (instancetype)init {
    if ((self = [super init])) {
        _minimumValue = FLT_MIN; // will be ignored
        _maximumValue = FLT_MIN; // will be ignored
    }

    return self;
}

- (instancetype)initWithMinimumValue:(float)minValue maximumValue:(float)maxValue {
    if ((self = [self init])) {
        self.minimumValue = minValue;
        self.maximumValue = maxValue;
    }

    return self;
}


@end


@implementation SCSegmentedAttributes

+ (instancetype)attributesWithSegmentTitlesArray:(NSArray *)titles {
    return [[[self class] alloc] initWithSegmentTitlesArray:titles];
}

- (instancetype)init {
    if ((self = [super init])) {
        _segmentTitlesArray = nil; // will be ignored
    }

    return self;
}

- (instancetype)initWithSegmentTitlesArray:(NSArray *)titles {
    if ((self = [self init])) {
        self.segmentTitlesArray = titles;
    }

    return self;
}

@end


@implementation SCDateAttributes

+ (instancetype)attributesWithDateFormatter:(NSDateFormatter *)formatter {
    return [[[self class] alloc] initWithDateFormatter:formatter];
}

+ (instancetype)attributesWithDateFormatter:(NSDateFormatter *)formatter datePickerMode:(UIDatePickerMode)mode displayDatePickerAsInputAccessoryView:(BOOL)displayDatePickerAsInputAccessoryView {
    return [[[self class] alloc] initWithDateFormatter:formatter datePickerMode:mode displayDatePickerAsInputAccessoryView:displayDatePickerAsInputAccessoryView];
}

- (instancetype)init {
    if ((self = [super init])) {
        _dateFormatter = nil; // will be ignored
        _datePickerMode = UIDatePickerModeDateAndTime;
        _displayDatePickerAsInputAccessoryView = FALSE;
    }

    return self;
}

- (instancetype)initWithDateFormatter:(NSDateFormatter *)formatter {
    if ((self = [self init])) {
        self.dateFormatter = formatter;
    }

    return self;
}

- (instancetype)initWithDateFormatter:(NSDateFormatter *)formatter datePickerMode:(UIDatePickerMode)mode displayDatePickerAsInputAccessoryView:(BOOL)displayAsInputAccessoryView {
    if ((self = [self init])) {
        self.dateFormatter = formatter;
        self.datePickerMode = mode;
        self.displayDatePickerAsInputAccessoryView = displayAsInputAccessoryView;
    }

    return self;
}

@end


@implementation SCSelectionAttributes

+ (instancetype)attributesWithItems:(NSArray *)items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    return [[[self class] alloc] initWithItems:items allowMultipleSelection:allowMultipleSel allowNoSelection:allowNoSel];
}

+ (instancetype)attributesWithItems:(NSArray *)items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss hideDetailViewNavigationBar:(BOOL)hideNavBar {
    return [[[self class] alloc] initWithItems:items allowMultipleSelection:allowMultipleSel allowNoSelection:allowNoSel autoDismissDetailView:autoDismiss hideDetailViewNavigationBar:hideNavBar];
}

- (instancetype)init {
    if ((self = [super init])) {
        _selectionItemsStore = nil;
        _selectionItemsFetchOptions = nil; // will be re-initialized when selectionItemsStore is set

        _allowMultipleSelection = FALSE;
        _allowNoSelection = FALSE;
        _maximumSelections = 0;
        _autoDismissDetailView = FALSE;
        _hideDetailViewNavigationBar = FALSE;

        _allowAddingItems = FALSE;
        _allowDeletingItems = FALSE;
        _allowMovingItems = FALSE;
        _allowEditingItems = FALSE;

        _addNewObjectuiElement = nil;
        _placeholderuiElement = nil;
    }

    return self;
}

- (instancetype)initWithItems:(NSArray *)items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    if ((self = [self init])) {
        self.selectionItemsStore = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:items] defaultDefiniton:[SCStringDefinition definition]];

        self.allowMultipleSelection = allowMultipleSel;
        self.allowNoSelection = allowNoSel;
    }

    return self;
}

- (instancetype)initWithItems:(NSArray *)items allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel autoDismissDetailView:(BOOL)autoDismiss hideDetailViewNavigationBar:(BOOL)hideNavBar {
    if ((self = [self initWithItems:items allowMultipleSelection:allowMultipleSel allowNoSelection:allowNoSel])) {
        self.autoDismissDetailView = autoDismiss;
        self.hideDetailViewNavigationBar = hideNavBar;
    }

    return self;
}


- (void)setSelectionItemsStore:(SCDataStore *)store {
    _selectionItemsStore = store;

    _selectionItemsFetchOptions = [store.defaultDataDefinition generateCompatibleDataFetchOptions];
}

- (NSArray *)items {
    return [self.selectionItemsStore fetchObjectsWithOptions:self.selectionItemsFetchOptions];
}

@end


@implementation SCObjectSelectionAttributes

+ (instancetype)attributesWithSelectionObjects:(NSArray *)objects objectsDefinition:(SCDataDefinition *)definition allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    return [[[self class] alloc] initWithSelectionObjects:objects objectsDefinition:definition allowMultipleSelection:allowMultipleSel allowNoSelection:allowNoSel];
}

+ (instancetype)attributesWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition usingPredicate:(NSPredicate *)predicate allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    return [[[self class] alloc] initWithObjectsEntityDefinition:entityDefinition usingPredicate:predicate allowMultipleSelection:allowMultipleSel allowNoSelection:allowNoSel];
}

- (instancetype)init {
    if ((self = [super init])) {
        _intermediateEntityDefinition = nil;
    }

    return self;
}

- (instancetype)initWithSelectionObjects:(NSArray *)objects objectsDefinition:(SCDataDefinition *)definition allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    if ((self = [self init])) {
        self.selectionItemsStore = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:objects] defaultDefiniton:definition];

        self.allowMultipleSelection = allowMultipleSel;
        self.allowNoSelection = allowNoSel;
    }

    return self;
}

- (instancetype)initWithObjectsEntityDefinition:(SCEntityDefinition *)entityDefinition usingPredicate:(NSPredicate *)predicate allowMultipleSelection:(BOOL)allowMultipleSel allowNoSelection:(BOOL)allowNoSel {
    if ((self = [self init])) {
        self.selectionItemsStore = [SCCoreDataStore storeWithManagedObjectContext:entityDefinition.managedObjectContext defaultEntityDefinition:entityDefinition];

        if (predicate) {
            self.selectionItemsFetchOptions.filter = YES;
            self.selectionItemsFetchOptions.filterPredicate = predicate;
        }

        self.allowMultipleSelection = allowMultipleSel;
        self.allowNoSelection = allowNoSel;
    }

    return self;
}

@end


@implementation SCObjectAttributes

+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition {
    return [[[self class] alloc] initWithObjectDefinition:definition];
}

+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent {
    return [[[self class] alloc] initWithObjectDefinition:definition expandContentInCurrentView:expandContent];
}

- (instancetype)init {
    if ((self = [super init])) {
        _objectDefinition = nil;
    }

    return self;
}

- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition {
    if ((self = [self init])) {
        self.objectDefinition = definition;
    }

    return self;
}

- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition expandContentInCurrentView:(BOOL)expandContent {
    if ((self = [self initWithObjectDefinition:definition])) {
        self.expandContentInCurrentView = expandContent;
    }

    return self;
}

@end


@interface SCArrayOfObjectsAttributes () {
    BOOL addNewObjectuiElementExistsInEdtingMode;
}
@end

@implementation SCArrayOfObjectsAttributes

+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving {
    return [[[self class] alloc] initWithObjectDefinition:definition allowAddingItems:allowAdding allowDeletingItems:allowDeleting allowMovingItems:allowMoving];
}

+ (instancetype)attributesWithObjectDefinition:(SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving expandContentInCurrentView:(BOOL)expandContent placeholderuiElement:(NSObject *)placeholderUI addNewObjectuiElement:(NSObject *)newObjectUI addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode {
    return [[[self class] alloc] initWithObjectDefinition:definition allowAddingItems:allowAdding allowDeletingItems:allowDeleting allowMovingItems:allowMoving expandContentInCurrentView:expandContent placeholderuiElement:placeholderUI addNewObjectuiElement:newObjectUI addNewObjectuiElementExistsInNormalMode:existsInNormalMode addNewObjectuiElementExistsInEditingMode:existsInEditingMode];
}

- (instancetype)init {
    if ((self = [super init])) {
        _defaultObjectsDefinition = nil;
        _objectsFetchOptions = nil; // will be re-initialized when defaultObjectsDefinition is set

        _allowAddingItems = TRUE;
        _allowDeletingItems = TRUE;
        _allowMovingItems = TRUE;
        _allowEditingItems = TRUE;
        _placeholderuiElement = nil;
        _addNewObjectuiElement = nil;
        _addNewObjectuiElementExistsInNormalMode = TRUE;
        _addNewObjectuiElementExistsInEditingMode = TRUE;

        _sectionActions = [[SCSectionActions alloc] init];
    }

    return self;
}

- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving {
    if ((self = [self init])) {
        self.defaultObjectsDefinition = definition;

        _allowAddingItems = allowAdding;
        _allowDeletingItems = allowDeleting;
        _allowMovingItems = allowMoving;
    }

    return self;
}

- (instancetype)initWithObjectDefinition:(SCDataDefinition *)definition allowAddingItems:(BOOL)allowAdding allowDeletingItems:(BOOL)allowDeleting allowMovingItems:(BOOL)allowMoving expandContentInCurrentView:(BOOL)expandContent placeholderuiElement:(NSObject *)placeholderUI addNewObjectuiElement:(NSObject *)newObjectUI addNewObjectuiElementExistsInNormalMode:(BOOL)existsInNormalMode addNewObjectuiElementExistsInEditingMode:(BOOL)existsInEditingMode {
    if ((self = [self initWithObjectDefinition:definition allowAddingItems:allowAdding allowDeletingItems:allowDeleting allowMovingItems:allowMoving])) {
        self.expandContentInCurrentView = expandContent;
        self.placeholderuiElement = placeholderUI;
        self.addNewObjectuiElement = newObjectUI;
        self.addNewObjectuiElementExistsInNormalMode = existsInNormalMode;
        self.addNewObjectuiElementExistsInEditingMode = existsInEditingMode;
    }

    return self;
}


- (void)setDefaultObjectsDefinition:(SCDataDefinition *)definition {
    _defaultObjectsDefinition = definition;

    _objectsFetchOptions = [definition generateCompatibleDataFetchOptions];
}

@end
