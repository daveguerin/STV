/*
 *  SCTableViewCell.m
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *  Copyright 2019-2020 dgApps. All rights reserved.
 *
 */

#import "SCTableViewCell.h"

#import <objc/runtime.h>
#import <QuartzCore/QuartzCore.h>
#import "SCGlobals.h"
#import "SCTableViewModel.h"
#import "SCStringDefinition.h"
#import "SCArrayStore.h"
#import "SCEntityDefinition.h"
#import "SCCoreDataStore.h"
#import "SCImageView.h"



// Encoding constants

#define kBoundPropertyNameKey @"boundPropertyName"
#define KibDetailViewControllerIdentifier    @"ibDetailViewControllerIdentifier"


@interface SCTableViewCell()

@property (nonatomic, strong) NSObject *initialBoundValue;

@property (nonatomic, strong) NSLayoutConstraint *sc_heightConstraint;
@property (nonatomic) CGFloat sc_resolvedHeight;
@property (nonatomic, strong) NSArray<NSLayoutConstraint *> *sc_labelConstraints;
@property (nonatomic, strong) NSArray<NSLayoutConstraint *> *sc_authoredContentBadgeConstraints;
@property (nonatomic) BOOL sc_authoredContentBadgeVisible;
- (void)sc_installBadgeConstraintsForAuthoredContent;
@property (nonatomic, copy) NSString *sc_layoutKey;
@property (nonatomic, strong) UILayoutGuide *sc_textGroup;
@property (nonatomic, strong) UILayoutGuide *sc_controlGroup;
- (UILayoutGuide *)sc_controlGuideAddingConstraints:(NSMutableArray *)constraints;
@property (nonatomic) UITableViewCellStyle sc_nativeStyle;
- (void)sc_updateHeightConstraint;
- (void)sc_installContentConstraints;

- (void)setActiveDetailModel:(SCTableViewModel *)model;

- (void)didLayoutSubviews;

- (SCTableViewModel *)modelForViewController:(UIViewController *)viewController;
- (BOOL)isViewControllerActive:(UIViewController *)viewController;
- (SCTableViewModel *)getCustomDetailModelForRowAtIndexPath:(NSIndexPath *)indexPath;
- (void)presentDetailViewController:(UIViewController *)detailViewController forCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath withPresentationMode:(SCPresentationMode)mode;

- (void)handleDetailViewControllerWillPresent:(UIViewController *)detailViewController;
- (void)handleDetailViewControllerDidPresent:(UIViewController *)detailViewController;
- (BOOL)handleDetailViewControllerShouldDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;
- (void)handleDetailViewControllerWillDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;
- (void)handleDetailViewControllerDidDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;

- (void)handleDetailViewControllerWillGainFocus:(UIViewController *)detailViewController;
- (void)handleDetailViewControllerDidGainFocus:(UIViewController *)detailViewController;
- (void)handleDetailViewControllerWillLoseFocus:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;
- (void)handleDetailViewControllerDidLoseFocus:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped;


@end

#pragma mark - SCTableViewCell

@interface SCTableViewCell ()
{
    __weak SCTableViewModel *activeDetailModel;
    __weak id ownerViewControllerDelegate;
    BOOL _isCustomBoundProperty;
    SCDetailViewControllerOptions *_detailViewControllerOptions;
}
@end

@implementation SCTableViewCell

// Custom accessors disable autosynthesis; create backing storage and its V attribute.
@synthesize height = _height;

+ (instancetype)cell
{
	return [[[self class] alloc] initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]; 
}

+ (instancetype)cellWithStyle:(UITableViewCellStyle)style
{
    return [[[self class] alloc] initWithStyle:style reuseIdentifier:nil]; 
}

+ (instancetype)cellWithText:(NSString *)cellText
{
	return [[[self class] alloc] initWithText:cellText];
}

+ (instancetype)cellWithText:(NSString *)cellText textAlignment:(NSTextAlignment)textAlignment
{
    return [[[self class] alloc] initWithText:cellText textAlignment:textAlignment];
}

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object boundPropertyName:(NSString *)propertyName;
{
	return [[[self class] alloc] initWithText:cellText boundObject:object boundPropertyName:propertyName];
}

+ (instancetype)cellWithCell:(UITableViewCell *)cell {
	// Game plan: since UITableViewCell doesn't support NSCopying, simply archive it then unarchive it back into our object
    id STVCell = nil;
    @try
    {
        NSString *sourceClassName = NSStringFromClass(cell.class);
        NSError *error = nil;
        NSData *cellData = [NSKeyedArchiver archivedDataWithRootObject:cell requiringSecureCoding:NO error:&error];
        if (!cellData || error) {
            SCDebugLog(@"Unable to archive table cell: %@", error);
            return nil;
        }

        NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingFromData:cellData error:&error];
        unarchiver.requiresSecureCoding = NO;
        if (!unarchiver || error) {
            SCDebugLog(@"Unable to create table-cell unarchiver: %@", error);
            return nil;
        }

        /*
        Scope the source-to-STV class substitution to this decoder instance so
        unrelated process-wide unarchiving can never observe it.
        */
        [unarchiver setClass:self forClassName:sourceClassName];
        STVCell = [unarchiver decodeObjectForKey:NSKeyedArchiveRootObjectKey];
        [unarchiver finishDecoding];
        if (![STVCell isKindOfClass:self]) {
            SCDebugLog(@"Decoded table cell has unexpected class %@ instead of %@", [STVCell class], self);
            STVCell = nil;
        }

        else {
            // Required to fix a suspected iOS bug where the cell's image is not correctly archived/unarchived.
            [(UITableViewCell *)STVCell imageView].image = cell.imageView.image;
        }

    }
    @catch(NSException *exception)
    {
        SCDebugLog(@"Unable to clone table cell: %@", exception);
        STVCell = nil;
    }

    return STVCell;
}


- (void)performInitialization
{
	self.shouldIndentWhileEditing = TRUE;
	
	_cellActions = [[SCCellActions alloc] init];
	_boundPropertyDataType = SCDataTypeUnknown;
    _isCustomBoundProperty = FALSE;
    if (!_height) {
        _height = UITableViewAutomaticDimension;
    }

    /*
    STV 7 models choose the inherited policy; keeping this on the cell allows
    fixed and legacy exceptions without subclass tests in the model.
    */
    _sizingMode = SCTableViewCellSizingModeInherit;
    _estimatedHeight = 44.0;
	_selectable = TRUE;
    _enabled = TRUE;
    _disabledTextColor = [UIColor labelColor];
    _autoResignFirstResponder = TRUE;
	_cellEditingStyle = UITableViewCellEditingStyleDelete;
	_autoValidateValue = TRUE;
	_commitChangesLive = TRUE;
	_needsCommit = FALSE;
	_beingReused = FALSE;
	_customCell = FALSE;
    _isSpecialCell = FALSE;
    _configured = FALSE;
    
    // Setup the badgeView
	_badgeView = [[SCBadgeView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
	[self.contentView addSubview:_badgeView];

    _leadingSwipePerformsFirstActionWithFullSwipe = YES;
    _trailingSwipePerformsFirstActionWithFullSwipe = YES;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];

    if (self) {
        [self performInitialization];
        
        // The following decodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only properties used in IB need to be here)
        
        self.cellCreatedInIB = YES;
        self.boundPropertyName = [aDecoder decodeObjectForKey:kBoundPropertyNameKey];
        self.ibDetailViewControllerIdentifier = [aDecoder decodeObjectForKey:KibDetailViewControllerIdentifier];
    }

    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    // The following encodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only properties used in IB need to be here)
    
    [aCoder encodeObject:self.boundPropertyName forKey:kBoundPropertyNameKey];
    [aCoder encodeObject:self.ibDetailViewControllerIdentifier forKey:KibDetailViewControllerIdentifier];
}

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if ( (self=[super initWithStyle:style reuseIdentifier:reuseIdentifier]) ) {
        [self performInitialization];
        self.sc_nativeStyle = style;
        /*
        hermes:
        Keep spare row width in the text/control gap, not in the image. Native
        fixed-width controls must not stretch an icon and thereby grow the row.
        Image constraints use UIImageView's intrinsic alignment size, including
        SF Symbol insets, rather than a bitmap ratio or a fixed maximum size.
        Set the default here so callers can still choose another priority.
        */
        [self.imageView setContentHuggingPriority:751 forAxis:UILayoutConstraintAxisHorizontal];
        self.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        self.textLabel.adjustsFontForContentSizeCategory = YES;
        self.detailTextLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleCallout];
        self.detailTextLabel.adjustsFontForContentSizeCategory = YES;

        if (reuseIdentifier) {
            _reuseId = [reuseIdentifier copy];
        }

    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText
{
    if ( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) ) {
        self.textLabel.text = cellText;
    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText textAlignment:(NSTextAlignment)textAlignment
{
    if ( (self=[self initWithText:cellText]) ) {
        self.textLabel.textAlignment = textAlignment;
    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object boundPropertyName:(NSString *)propertyName
{
    if ( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) ) {
        self.textLabel.text = cellText;
		
        self.boundObject = object;
        self.boundPropertyName = propertyName;
    }

    return self;
}


- (void)setBoundObject:(NSObject *)boundObject
{
    _boundObject = boundObject;
    
    [self resetInitialBoundValue];
}

- (void)setBoundPropertyName:(NSString *)boundPropertyName
{
    _boundPropertyName = boundPropertyName;
    
    [self resetInitialBoundValue];
}

- (void)resetInitialBoundValue
{
    self.initialBoundValue = nil;
}

- (void)rollbackToInitialBoundValue
{
    if (!self.initialBoundValue) {
        return;
    }

    NSObject *initialValue = self.initialBoundValue;
    if ([initialValue isKindOfClass:[NSNull class]]) {
        initialValue = nil;
    }

    self.boundValue = initialValue;
}


- (CGFloat)height
{
    return UITableViewAutomaticDimension;
}

- (void)setHeight:(CGFloat)value
{
    _height = value;
    [self setNeedsUpdateConstraints];
    [self sc_updateHeightConstraint];
}

- (void)setSizingMode:(SCTableViewCellSizingMode)value
{
    _sizingMode = value;
    [self setNeedsUpdateConstraints];
    [self sc_updateHeightConstraint];
}

- (void)sc_setResolvedHeightConstraint:(CGFloat)value
{
    self.sc_resolvedHeight = value;
    [self sc_updateHeightConstraint];
}

- (void)sc_updateHeightConstraint
{
    CGFloat value = self.sc_resolvedHeight;
    if (value <= 0) {
        value = _height;
    }

    SCTableViewCellSizingMode mode = self.sizingMode;
    if (mode == SCTableViewCellSizingModeInherit) {
        mode = self.ownerTableViewModel.defaultCellSizingMode;
    }

    if (value <= 0 && mode == SCTableViewCellSizingModeFixed) {
        value = self.ownerTableViewModel ? self.ownerTableViewModel.defaultCellHeight : 44;
    }

    if (!self.sc_heightConstraint) {
        self.sc_heightConstraint = [self.contentView.heightAnchor constraintEqualToConstant:44];
        self.sc_heightConstraint.priority = 999;
        self.sc_heightConstraint.identifier = @"STV.explicitContentHeight";
    }

    self.sc_heightConstraint.active = value > 0;
    if (value > 0) {
        self.sc_heightConstraint.constant = value;
    }

}

- (void)setDisabledTextColor:(UIColor *)disabledTextColor
{
    _disabledTextColor = disabledTextColor;

    if (!self.enabled) {
        // Make sure to apply new color
        self.enabled = FALSE;
    }

}

- (void)setIsCustomBoundProperty:(BOOL)custom
{
    _isCustomBoundProperty = custom;
}

- (void)setNeedsCommit:(BOOL)needs
{
    _needsCommit = needs;
}

- (void)setActiveDetailModel:(SCTableViewModel *)model
{
    activeDetailModel = model;
    self.ownerTableViewModel.activeDetailModel = model;
}

- (SCDetailViewControllerOptions *)detailViewControllerOptions
{
    // Conserve resources by lazy loading for only cells that need it
    if (!_detailViewControllerOptions) {
        _detailViewControllerOptions = [[SCDetailViewControllerOptions alloc] init];
    }

    return _detailViewControllerOptions;
}

- (void)setDetailViewControllerOptions:(SCDetailViewControllerOptions *)options
{
    _detailViewControllerOptions = options;
}

- (void)didLayoutSubviews
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];

    if (self.cellActions.didLayoutSubviews) {
        self.cellActions.didLayoutSubviews(self, indexPath);
    }

    else if (self.ownerSection.cellActions.didLayoutSubviews) {
        self.ownerSection.cellActions.didLayoutSubviews(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.didLayoutSubviews) {
        self.ownerTableViewModel.cellActions.didLayoutSubviews(self, indexPath);
    }

}

//overrides superclass
- (NSString *)reuseIdentifier
{
	return self.reuseId;
}

// overrides superclass
- (UITableViewCellSelectionStyle)selectionStyle
{
    if (self.enabled) {
        return [super selectionStyle];
    }

    //else
    return UITableViewCellSelectionStyleNone;
}
 
//overrides superclass
- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated
{
	[super setHighlighted:highlighted animated:animated];
	[self.badgeView setNeedsDisplay];
}

//overrides superclass
- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
	[super setSelected:selected animated:animated];
	[self.badgeView setNeedsDisplay];
}

- (UITableViewCellStyle)cellStyle
{
    // The only available way to determine the cell style is the availability and font sizes of textLabel & detailTextLabel (can't use label locations as they're not guaranteed to be set when this method gets called)
    
    BOOL textLabelPresent = (self.textLabel.text!=nil);
    BOOL detailTextLabelPresent = (self.detailTextLabel!=nil);

    if (!textLabelPresent && !detailTextLabelPresent) {
        return -1;  //  -1 is 'Custom' cell style
    }

    if (!detailTextLabelPresent) {
        return UITableViewCellStyleDefault;
    }

    if (self.textLabel.font.pointSize != self.detailTextLabel.font.pointSize) {
        return UITableViewCellStyleSubtitle;
    }

    if ([self.detailTextLabel.textColor isEqual:[UIColor blackColor]]) { // obviously not the best but there are no other clues
        return UITableViewCellStyleValue2;
    }

    return UITableViewCellStyleValue1;  // the only remaining option
}

//overrides superclass
- (void)updateConstraints
{
    [self sc_updateHeightConstraint];
    [self sc_installBadgeConstraintsForAuthoredContent];
    [self sc_installContentConstraints];
    [super updateConstraints];
}

- (void)sc_installBadgeConstraintsForAuthoredContent
{
    BOOL authoredContent = self.cellCreatedInIB ||
        ([self isKindOfClass:[SCControlCell class]] && [(SCControlCell *)self controlCreatedInIB]);
    if (!authoredContent) {
        return;
    }

    BOOL visible = self.badgeView.text.length && !self.badgeView.hidden;
    if (self.sc_authoredContentBadgeConstraints && self.sc_authoredContentBadgeVisible == visible) {
        return;
    }

    [NSLayoutConstraint deactivateConstraints:self.sc_authoredContentBadgeConstraints];
    self.sc_authoredContentBadgeVisible = visible;
    /*
    hermes:
    STV creates this badge after decoding; only the caller's authored views
    retain Interface Builder layout ownership.
    */
    SCBadgeView *badge = self.badgeView;
    badge.translatesAutoresizingMaskIntoConstraints = NO;
    UILayoutGuide *margins = self.contentView.layoutMarginsGuide;
    NSMutableArray *constraints = [NSMutableArray arrayWithArray:@[
        [badge.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor],
        [badge.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]
    ]];
    if (visible) {
        [constraints addObject:[badge.leadingAnchor constraintGreaterThanOrEqualToAnchor:margins.leadingAnchor]];
        NSLayoutConstraint *top = [badge.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
        NSLayoutConstraint *bottom = [badge.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
        top.priority = bottom.priority = 750;
        [constraints addObjectsFromArray:@[top, bottom]];
    }

    else {
        [constraints addObjectsFromArray:@[
            [badge.widthAnchor constraintEqualToConstant:0],
            [badge.heightAnchor constraintEqualToConstant:0]
        ]];
    }

    self.sc_authoredContentBadgeConstraints = constraints;
    [NSLayoutConstraint activateConstraints:constraints];
}

- (void)sc_installContentConstraints
{
    if (self.cellCreatedInIB || [self isKindOfClass:[SCControlCell class]]) {
        return;
    }

    BOOL titleVisible = self.textLabel.text.length && !self.textLabel.hidden;
    BOOL detailVisible = self.detailTextLabel.text.length && !self.detailTextLabel.hidden;
    BOOL imageVisible = self.imageView.image && !self.imageView.hidden;
    BOOL badgeVisible = self.badgeView.text.length && !self.badgeView.hidden;
    NSString *key = [NSString stringWithFormat:@"%d-%d-%p-%d-%ld",
        titleVisible, detailVisible, imageVisible ? self.imageView.image : nil,
        badgeVisible, (long)self.sc_nativeStyle];
    if ([self.sc_layoutKey isEqualToString:key]) {
        return;
    }

    self.sc_layoutKey = key;
    [NSLayoutConstraint deactivateConstraints:self.sc_labelConstraints];
    if (!self.sc_textGroup) {
        self.sc_textGroup = [[UILayoutGuide alloc] init];
        [self.contentView addLayoutGuide:self.sc_textGroup];
    }

    UILayoutGuide *group = self.sc_textGroup;
    UILayoutGuide *margins = self.contentView.layoutMarginsGuide;
    NSMutableArray *constraints = [NSMutableArray array];
    NSLayoutConstraint *minimum = [self.contentView.heightAnchor constraintGreaterThanOrEqualToConstant:44];
    minimum.priority = 750;
    [constraints addObject:minimum];
    NSArray *views = @[self.textLabel ?: [NSNull null], self.detailTextLabel ?: [NSNull null], self.imageView, self.badgeView];
    NSArray *visibility = @[@(titleVisible), @(detailVisible), @(imageVisible), @(badgeVisible)];
    for(NSUInteger index = 0; index < views.count; index++)
    {
        if (views[index] == [NSNull null]) {
            continue;
        }

        UIView *view = views[index];
        if (!view.superview) {
            if (![visibility[index] boolValue]) {
                continue;
            }

            [self.contentView addSubview:view];
        }

        view.translatesAutoresizingMaskIntoConstraints = NO;
        if (![visibility[index] boolValue]) {
            [constraints addObjectsFromArray:@[
                [view.widthAnchor constraintEqualToConstant:0],
                [view.heightAnchor constraintEqualToConstant:0],
                [view.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
                [view.topAnchor constraintEqualToAnchor:self.contentView.topAnchor]
            ]];
        }

    }
    [constraints addObject:[group.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]];
    NSLayoutConstraint *top = [group.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
    NSLayoutConstraint *bottom = [group.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
    top.priority = bottom.priority = 750;
    [constraints addObjectsFromArray:@[top, bottom]];
    NSLayoutXAxisAnchor *leading = margins.leadingAnchor;
    if (imageVisible) {
        UIImageView *image = self.imageView;
        image.contentMode = UIViewContentModeScaleAspectFit;
        [constraints addObjectsFromArray:@[
            [image.leadingAnchor constraintEqualToAnchor:leading],
            [image.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]
        ]];
        NSLayoutConstraint *imageTop = [image.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
        NSLayoutConstraint *imageBottom = [image.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
        imageTop.priority = imageBottom.priority = 750;
        [constraints addObjectsFromArray:@[imageTop, imageBottom]];
        leading = image.trailingAnchor;
    }

    [constraints addObject:[group.leadingAnchor constraintEqualToAnchor:leading constant:imageVisible ? 8 : 0]];
    if (badgeVisible) {
        [self.badgeView setContentHuggingPriority:252 forAxis:UILayoutConstraintAxisHorizontal];
        [self.badgeView setContentCompressionResistancePriority:999 forAxis:UILayoutConstraintAxisHorizontal];
        [constraints addObjectsFromArray:@[
            [group.trailingAnchor constraintEqualToAnchor:self.badgeView.leadingAnchor constant:-8],
            [self.badgeView.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor],
            [self.badgeView.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]
        ]];
        NSLayoutConstraint *badgeTop = [self.badgeView.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
        NSLayoutConstraint *badgeBottom = [self.badgeView.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
        badgeTop.priority = badgeBottom.priority = 750;
        [constraints addObjectsFromArray:@[badgeTop, badgeBottom]];
    }

    else {
        [constraints addObject:[group.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor]];
    }

    if (titleVisible && detailVisible) {
        UILabel *title = self.textLabel;
        UILabel *detail = self.detailTextLabel;
        [title setContentHuggingPriority:251 forAxis:UILayoutConstraintAxisHorizontal];
        [constraints addObjectsFromArray:@[
            [title.leadingAnchor constraintEqualToAnchor:group.leadingAnchor],
            [detail.trailingAnchor constraintEqualToAnchor:group.trailingAnchor]
        ]];
        if (self.sc_nativeStyle == UITableViewCellStyleSubtitle) {
            [constraints addObjectsFromArray:@[
                [title.trailingAnchor constraintEqualToAnchor:group.trailingAnchor],
                [detail.leadingAnchor constraintEqualToAnchor:group.leadingAnchor],
                [title.topAnchor constraintEqualToAnchor:group.topAnchor],
                [detail.topAnchor constraintEqualToAnchor:title.bottomAnchor constant:2],
                [detail.bottomAnchor constraintEqualToAnchor:group.bottomAnchor]
            ]];
        }

        else {
            [constraints addObjectsFromArray:@[
                [detail.leadingAnchor constraintEqualToAnchor:title.trailingAnchor constant:8],
                [title.topAnchor constraintEqualToAnchor:group.topAnchor],
                [title.bottomAnchor constraintEqualToAnchor:group.bottomAnchor],
                [detail.topAnchor constraintEqualToAnchor:group.topAnchor],
                [detail.bottomAnchor constraintEqualToAnchor:group.bottomAnchor]
            ]];
        }

    }

    else if (titleVisible || detailVisible) {
        UILabel *label = titleVisible ? self.textLabel : self.detailTextLabel;
        [constraints addObjectsFromArray:@[
            [label.leadingAnchor constraintEqualToAnchor:group.leadingAnchor],
            [label.trailingAnchor constraintEqualToAnchor:group.trailingAnchor],
            [label.topAnchor constraintEqualToAnchor:group.topAnchor],
            [label.bottomAnchor constraintEqualToAnchor:group.bottomAnchor]
        ]];
    }

    else {
        [constraints addObject:[group.heightAnchor constraintEqualToConstant:0]];
    }

    self.sc_labelConstraints = constraints;
    [NSLayoutConstraint activateConstraints:constraints];
}

- (UILayoutGuide *)sc_controlGuideAddingConstraints:(NSMutableArray *)constraints
{
    if (!self.sc_controlGroup) {
        self.sc_controlGroup = [[UILayoutGuide alloc] init];
        [self.contentView addLayoutGuide:self.sc_controlGroup];
    }

    UILayoutGuide *group = self.sc_controlGroup;
    UILayoutGuide *margins = self.contentView.layoutMarginsGuide;
    [constraints addObjectsFromArray:@[
        [group.topAnchor constraintEqualToAnchor:self.contentView.topAnchor],
        [group.bottomAnchor constraintEqualToAnchor:self.contentView.bottomAnchor]
    ]];
    BOOL imageVisible = self.imageView.image && !self.imageView.hidden;
    BOOL badgeVisible = self.badgeView.text.length && !self.badgeView.hidden;
    NSArray *views = @[self.imageView, self.badgeView, self.textLabel];
    NSArray *visibility = @[@(imageVisible), @(badgeVisible),
        @(self.textLabel.text.length && !self.textLabel.hidden)];
    for(NSUInteger index = 0; index < views.count; index++)
    {
        UIView *view = views[index];
        if (!view.superview) {
            if (![visibility[index] boolValue]) {
                continue;
            }

            [self.contentView addSubview:view];
        }

        view.translatesAutoresizingMaskIntoConstraints = NO;
        if (![visibility[index] boolValue]) {
            [constraints addObjectsFromArray:@[
                [view.widthAnchor constraintEqualToConstant:0],
                [view.heightAnchor constraintEqualToConstant:0],
                [view.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
                [view.topAnchor constraintEqualToAnchor:self.contentView.topAnchor]
            ]];
        }

    }
    if (imageVisible) {
        UIImageView *image = self.imageView;
        image.contentMode = UIViewContentModeScaleAspectFit;
        [constraints addObjectsFromArray:@[
            [image.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
            [image.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor],
            [group.leadingAnchor constraintEqualToAnchor:image.trailingAnchor constant:8]
        ]];
    }

    else {
        [constraints addObject:[group.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor]];
    }

    if (badgeVisible) {
        [self.badgeView setContentHuggingPriority:252 forAxis:UILayoutConstraintAxisHorizontal];
        [self.badgeView setContentCompressionResistancePriority:999 forAxis:UILayoutConstraintAxisHorizontal];
        [constraints addObjectsFromArray:@[
            [group.trailingAnchor constraintEqualToAnchor:self.badgeView.leadingAnchor constant:-8],
            [self.badgeView.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor],
            [self.badgeView.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]
        ]];
    }

    else {
        [constraints addObject:[group.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor]];
    }

    for(UIView *view in @[self.imageView, self.badgeView])
    {
        if ((view == self.imageView && !imageVisible) || (view == self.badgeView && !badgeVisible)) {
            continue;
        }

        NSLayoutConstraint *top = [view.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
        NSLayoutConstraint *bottom = [view.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
        top.priority = bottom.priority = 750;
        [constraints addObjectsFromArray:@[top, bottom]];
    }
    return group;
}

- (void)layoutSubviews
{
    [self sc_installBadgeConstraintsForAuthoredContent];
    [self sc_installContentConstraints];
    [super layoutSubviews];
    [self.contentView setNeedsLayout];
    [self.contentView layoutIfNeeded];
    [self didLayoutSubviews];
}

//overrides superclass
- (void)setBackgroundColor:(UIColor *)color {
    [super setBackgroundColor:color];

    if (self.cellCreatedInIB) {
        return;
    }

    /*
     In iOS 14, if the color was nil, then the backgroundColor was being set to
     black, not what was required, so we set the label backgrounds to clear to
     make sure they will be correct for the cell backgroundColor. dgApps
     */

    //	if(self.selectionStyle==UITableViewCellSelectionStyleNone && !self.backgroundView)
    //	{
    //		// This is much more optimized than [UIColor clearColor]
    //		self.textLabel.backgroundColor = color;
    //		self.detailTextLabel.backgroundColor = color;
    //	}
    //	else
    //	{
    self.textLabel.backgroundColor = [UIColor clearColor];
    self.detailTextLabel.backgroundColor = [UIColor clearColor];
    //	}
}

//overrides superclass
- (void)setBackgroundView:(UIView *)backgroundView
{
    [super setBackgroundView:backgroundView];

    if (self.cellCreatedInIB) {
        return;
    }

    if (backgroundView) {
        self.textLabel.backgroundColor = [UIColor clearColor];
        self.detailTextLabel.backgroundColor = [UIColor clearColor];
    }

}

- (void)setBoundValue:(NSObject *)value
{
    if (self.boundObject && [self.boundPropertyName length]) {
        if ([self.boundPropertyName characterAtIndex:0] == '~') {
            return;  // boundPropertyName not an actual property but rather a placeholder for a custom cell
        }

        if (self.cellActions.willCommitBoundValue) {
            NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
            value = self.cellActions.willCommitBoundValue(self, indexPath, value);
        }

        else if (self.ownerSection.cellActions.willCommitBoundValue) {
            NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
            value = self.ownerSection.cellActions.willCommitBoundValue(self, indexPath, value);
        }

        else if (self.ownerTableViewModel.cellActions.willCommitBoundValue) {
            NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
            value = self.ownerTableViewModel.cellActions.willCommitBoundValue(self, indexPath, value);
        }

        if (!self.initialBoundValue) {
            NSObject *initialValue = self.boundValue;
            if (initialValue) {
                self.initialBoundValue = initialValue;
            }

            else {
                self.initialBoundValue = [NSNull null];
            }

        }

        if (self.boundObjectStore) {
            [self.boundObjectStore setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            [SCUtilities setValue:value forPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        if ([SCUtilities isBasicDataTypeClass:[self.boundObject class]]) {
            self.boundObject = value;
        }

    }

}

- (NSObject *)boundValue
{
    NSObject *value = nil;

    if (self.cellActions.calculatedValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.cellActions.calculatedValue(self, indexPath);
    }

    else if (self.ownerSection.cellActions.calculatedValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.ownerSection.cellActions.calculatedValue(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.calculatedValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.ownerTableViewModel.cellActions.calculatedValue(self, indexPath);
    }

    if (value) {
        return value;  // return the calculated value if present
    }


    if ([self.boundPropertyName length] && [self.boundPropertyName characterAtIndex:0] == '~') {
        return nil;  // boundPropertyName not an actual property but rather a placeholder for a custom cell
    }

    if ([SCUtilities isBasicDataTypeClass:[self.boundObject class]]) {
        value = self.boundObject;
    }

    else if (self.boundObject && self.boundPropertyName && !_isCustomBoundProperty) {
        if (self.boundObjectStore) {
            value = [self.boundObjectStore valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

        else {
            value = [SCUtilities valueForPropertyName:self.boundPropertyName inObject:self.boundObject];
        }

    }

    if (self.cellActions.didLoadBoundValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.cellActions.didLoadBoundValue(self, indexPath, value);
    }

    else if (self.ownerSection.cellActions.didLoadBoundValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.ownerSection.cellActions.didLoadBoundValue(self, indexPath, value);
    }

    else if (self.ownerTableViewModel.cellActions.didLoadBoundValue) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        value = self.ownerTableViewModel.cellActions.didLoadBoundValue(self, indexPath, value);
    }

    return value;
}

- (BOOL)valueIsValid
{
    if (!self.enabled) {
        return YES;
    }

    if (self.cellActions.valueIsValid) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        return self.cellActions.valueIsValid(self, indexPath);
    }

    //else
    if (self.ownerSection.cellActions.valueIsValid) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        return self.ownerSection.cellActions.valueIsValid(self, indexPath);
    }

    //else
    if (self.ownerTableViewModel.cellActions.valueIsValid) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        return self.ownerTableViewModel.cellActions.valueIsValid(self, indexPath);
    }

    if (self.autoValidateValue) {
        return [self getValueIsValid];
    }

    BOOL valid = TRUE;
	
	return valid;
}

- (BOOL)getValueIsValid
{
	// Should be overridden by subclasses
	return TRUE;
}

- (BOOL)generatesDetailView
{
    // Should be overridden by subclasses
    return NO;
}

- (void)cellValueChanged
{
	_needsCommit = TRUE;

    if (self.commitChangesLive) {
        [self commitChanges];
    }

    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
	[self.ownerTableViewModel valueChangedForRowAtIndexPath:indexPath];
    /*
    Run app callbacks first so any bound summary text is current, then
    invalidate automatic height even though the detail model has already been
    cleared.
    */
    if (self.height == UITableViewAutomaticDimension) {
        [self.ownerTableViewModel invalidateHeightForCell:self animated:NO];
    }

}

- (void)willDisplay
{
}

- (void)didSelectCell
{
    if (self.autoDeselect) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        [self.ownerTableViewModel.tableView deselectRowAtIndexPath:indexPath animated:YES];
    }

}

- (void)willDeselectCell
{
    if (activeDetailModel) {
        UITableView *detailTableView = activeDetailModel.tableView;
        [self setActiveDetailModel:nil];
        detailTableView.dataSource = nil;
        detailTableView.delegate = nil;
        [detailTableView reloadData];
    }

}

- (void)didDeselectCell
{
    // Does nothing
}

- (void)markCellAsSpecial
{
    _isSpecialCell = TRUE;
}

- (void)commitChanges
{
	_needsCommit = FALSE;
}

- (void)reloadBoundValue
{
	// Does nothing, should be overridden by subclasses
}

- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	self.imageView.image = attributes.imageView.image;
	self.detailCellsImageViews = attributes.imageViewArray;
}

- (void)prepareCellForDetailViewAppearing
{
	// disable ownerViewControllerDelegate
    if ([self.ownerTableViewModel.viewController isKindOfClass:[SCTableViewController class]]) {
        SCTableViewController *tViewController = (SCTableViewController *)self.ownerTableViewModel.viewController;
        ownerViewControllerDelegate = tViewController.delegate;
        tViewController.delegate = nil;
    }

    else if ([self.ownerTableViewModel.viewController isKindOfClass:[SCViewController class]]) {
        SCViewController *vController = (SCViewController *)self.ownerTableViewModel.viewController;
        ownerViewControllerDelegate = vController.delegate;
        vController.delegate = nil;
    }

    // lock master cell selection (in case a custom detail view is provided)
    if (self.ownerTableViewModel.masterModel) {
        self.ownerTableViewModel.masterModel.lockCellSelection = TRUE;
    }

}

- (void)prepareCellForDetailViewDisappearing
{
	// enable ownerViewControllerDelegate
    if ([self.ownerTableViewModel.viewController isKindOfClass:[SCTableViewController class]]) {
        SCTableViewController *tViewController = (SCTableViewController *)self.ownerTableViewModel.viewController;
        tViewController.delegate = ownerViewControllerDelegate;
    }

    else if ([self.ownerTableViewModel.viewController isKindOfClass:[SCViewController class]]) {
        SCViewController *vController = (SCViewController *)self.ownerTableViewModel.viewController;
        vController.delegate = ownerViewControllerDelegate;
    }

    // resume cell selection
    if (self.ownerTableViewModel.masterModel) {
        self.ownerTableViewModel.masterModel.lockCellSelection = FALSE;
    }

}


- (SCTableViewModel *)modelForViewController:(UIViewController *)viewController
{
    SCTableViewModel *detailModel = nil;

    if ([viewController isKindOfClass:[SCTableViewController class]]) {
        detailModel = [(SCTableViewController *)viewController tableViewModel];
    }

    else if ([viewController isKindOfClass:[SCViewController class]]) {
        detailModel = [(SCViewController *)viewController tableViewModel];
    }

    return detailModel;
}

- (BOOL)isViewControllerActive:(UIViewController *)viewController
{
    BOOL active = FALSE;

    if ([viewController isKindOfClass:[SCTableViewController class]]) {
        active = [(SCTableViewController *)viewController state] == SCViewControllerStateActive;
    }

    else if ([viewController isKindOfClass:[SCViewController class]]) {
        active = [(SCViewController *)viewController state] == SCViewControllerStateActive;
    }

    return active;
}

- (BOOL)isViewControllerFocused:(UIViewController *)viewController
{
    BOOL focused = FALSE;

    if ([viewController isKindOfClass:[SCTableViewController class]]) {
        focused = [(SCTableViewController *)viewController hasFocus];
    }

    else if ([viewController isKindOfClass:[SCViewController class]]) {
        focused = [(SCViewController *)viewController hasFocus];
    }

    return focused;
}

- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType
{
    return self.detailViewControllerOptions.navigationBarType;
}

- (void)buildDetailModel:(SCTableViewModel *)detailModel
{
    // should be overridden by subclasses
}

- (void)configureDetailModel:(SCTableViewModel *)detailModel
{
    detailModel.masterBoundObject = self.boundObject;
    detailModel.masterBoundObjectStore = self.boundObjectStore;
}

- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
    // should be overridden by subclasses
}

- (UIViewController *)getDetailViewControllerForCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath allowUITableViewControllerSubclass:(BOOL)allowUITableViewController
{
    UIViewController *detailViewController = nil;

    if (self.cellActions.detailViewController) {
        detailViewController = self.cellActions.detailViewController(self, indexPath);
    }

    else if (self.ownerSection.cellActions.detailViewController) {
        detailViewController = self.ownerSection.cellActions.detailViewController(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.detailViewController) {
        detailViewController = self.ownerTableViewModel.cellActions.detailViewController(self, indexPath);
    }

    else if (self.ownerTableViewModel.detailViewController) {
        detailViewController = self.ownerTableViewModel.detailViewController;
    }

    if (!detailViewController) {
        if (allowUITableViewController) {
            detailViewController = [[SCTableViewController alloc] initWithStyle:self.detailViewControllerOptions.tableViewStyle];
        }

        else {
            detailViewController = [[SCViewController alloc] init];
        }

    }

    detailViewController.modalPresentationStyle = self.detailViewControllerOptions.modalPresentationStyle;
    if (self.detailViewControllerOptions.title) {
        detailViewController.title = self.detailViewControllerOptions.title;
    }

    else {
        detailViewController.title = cell.textLabel.text;
    }

    detailViewController.hidesBottomBarWhenPushed = self.detailViewControllerOptions.hidesBottomBarWhenPushed;
    detailViewController.preferredContentSize = self.ownerTableViewModel.viewController.preferredContentSize;
    if ([detailViewController isKindOfClass:[SCTableViewController class]] && [self.ownerTableViewModel.viewController isKindOfClass:[SCTableViewController class]]) {
        [(SCTableViewController *)detailViewController setAutoDisableNavigationButtonsUntilViewAppears:[(SCTableViewController *)self.ownerTableViewModel.viewController autoDisableNavigationButtonsUntilViewAppears]];
    }

    SCTableViewModel *detailModel = nil;
    detailModel = [self getCustomDetailModelForRowAtIndexPath:indexPath];

    if ([detailViewController isKindOfClass:[SCTableViewController class]]) {
        SCTableViewController *viewController = (SCTableViewController *)detailViewController;

        if (detailModel) {
            viewController.tableViewModel = detailModel;
        }

        else {
            detailModel = viewController.tableViewModel;
        }

    }

    else if ([detailViewController isKindOfClass:[SCViewController class]]) {
        SCViewController *viewController = (SCViewController *)detailViewController;

        if (detailModel) {
            viewController.tableViewModel = detailModel;
        }

        else {
            detailModel = viewController.tableViewModel;
        }

    }

    if (self.cellActions.detailModelCreated) {
        self.cellActions.detailModelCreated(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelCreated) {
        self.ownerSection.cellActions.detailModelCreated(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelCreated) {
        self.ownerTableViewModel.cellActions.detailModelCreated(self, indexPath, detailModel);
    }

    [self configureDetailViewController:detailViewController];
    [self.ownerTableViewModel configureDetailModel:detailModel];
	[self buildDetailModel:detailModel];
    [self configureDetailModel:detailModel];

    if (self.cellActions.detailModelConfigured) {
        self.cellActions.detailModelConfigured(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelConfigured) {
        self.ownerSection.cellActions.detailModelConfigured(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelConfigured) {
        self.ownerTableViewModel.cellActions.detailModelConfigured(self, indexPath, detailModel);
    }


    return detailViewController;
}

- (void)configureDetailViewController:(UIViewController *)detailViewController
{
    if ([detailViewController isKindOfClass:[SCTableViewController class]]) {
        SCTableViewController *viewController = (SCTableViewController *)detailViewController;
        
        viewController.delegate = self;
        SCNavigationBarType navBarType = [self defaultDetailViewControllerNavigationBarType];
        if (navBarType==SCNavigationBarTypeAuto && viewController.navigationBarType==SCNavigationBarTypeAuto) {
            viewController.navigationBarType = SCNavigationBarTypeDoneRightCancelLeft;
        }

        else if (viewController.navigationBarType==SCNavigationBarTypeAuto) {
            viewController.navigationBarType = navBarType;
        }

        viewController.allowEditingModeCancelButton = self.detailViewControllerOptions.allowEditingModeCancelButton;
    }

    else if ([detailViewController isKindOfClass:[SCViewController class]]) {
        SCViewController *viewController = (SCViewController *)detailViewController;
            
        viewController.delegate = self;
        SCNavigationBarType navBarType = [self defaultDetailViewControllerNavigationBarType];
        if (navBarType==SCNavigationBarTypeAuto && viewController.navigationBarType==SCNavigationBarTypeAuto) {
            viewController.navigationBarType = SCNavigationBarTypeDoneRightCancelLeft;
        }

        else if (viewController.navigationBarType==SCNavigationBarTypeAuto) {
            viewController.navigationBarType = navBarType;
        }

        viewController.allowEditingModeCancelButton = self.detailViewControllerOptions.allowEditingModeCancelButton;
    }

}

- (SCTableViewModel *)getCustomDetailModelForRowAtIndexPath:(NSIndexPath *)indexPath
{
	SCTableViewModel *detailModel = nil;

    if (self.cellActions.detailTableViewModel) {
        detailModel = self.cellActions.detailTableViewModel(self, indexPath);
    }

    else if (self.ownerSection.cellActions.detailTableViewModel) {
        detailModel = self.ownerSection.cellActions.detailTableViewModel(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.detailTableViewModel) {
        detailModel = self.ownerTableViewModel.cellActions.detailTableViewModel(self, indexPath);
    }

    return detailModel;
}

- (void)presentDetailViewController:(UIViewController *)detailViewController forCell:(SCTableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath withPresentationMode:(SCPresentationMode)mode
{
    if (!detailViewController) {
        return;
    }

    [self prepareCellForDetailViewAppearing];
    
    [self setActiveDetailModel:[self modelForViewController:detailViewController]];
    
    BOOL customPresentation = FALSE;
    if ([self isViewControllerActive:detailViewController] && mode==SCPresentationModeAuto) {
        customPresentation = TRUE;

        if ([detailViewController respondsToSelector:@selector(gainFocus)]) {
            [(id)detailViewController gainFocus];
        }

    }

    if (!customPresentation) {
        UINavigationController *navController = self.ownerTableViewModel.viewController.navigationController;
        if (mode == SCPresentationModeAuto) {
            if (navController) {
                mode = SCPresentationModePush;
            }

            else {
                mode = SCPresentationModeModal;
            }

        }

        if (mode==SCPresentationModePush && !navController) {
            mode = SCPresentationModeModal;
        }

        UINavigationController *detailNavController = nil;
        if (mode==SCPresentationModeModal || mode==SCPresentationModePopover) {
            detailNavController = [[UINavigationController alloc] initWithRootViewController:detailViewController];
            if (navController) {
                detailNavController.view.backgroundColor = navController.view.backgroundColor;
                UIBarStyle barStyle = navController.navigationBar.barStyle;
                if (![SCUtilities isViewInsidePopover:self.ownerTableViewModel.viewController.view]) {
                    detailNavController.navigationBar.barStyle = barStyle;
                }

                else {
                    detailNavController.navigationBar.barStyle = UIBarStyleBlack;
                }

                detailNavController.navigationBar.tintColor = navController.navigationBar.tintColor;
            }

            detailNavController.preferredContentSize = detailViewController.preferredContentSize;
            detailNavController.modalPresentationStyle = detailViewController.modalPresentationStyle;
        }

        switch (mode) {
            case SCPresentationModePush:
                [navController pushViewController:detailViewController animated:YES];
                break;
            case SCPresentationModeModal:
            {
                [self.ownerTableViewModel.viewController presentViewController:detailNavController animated:YES completion:nil];
            }
                break;
            case SCPresentationModePopover:
            {
                
                detailNavController.modalPresentationStyle = UIModalPresentationPopover;
                UIPopoverPresentationController *popoverController = detailNavController.popoverPresentationController;
                if ([detailViewController isKindOfClass:[SCTableViewController class]]) {
                    [(SCTableViewController *)detailViewController setPopoverController:popoverController];
                }

                else if ([detailViewController isKindOfClass:[SCViewController class]]) {
                    [(SCViewController *)detailViewController setPopoverController:popoverController];
                }

                detailNavController.modalInPresentation = YES;
                
                popoverController.sourceView = cell ?: self.ownerTableViewModel.tableView;
                popoverController.sourceRect = popoverController.sourceView.bounds;
                popoverController.permittedArrowDirections = UIPopoverArrowDirectionAny;
                [self.ownerTableViewModel.viewController presentViewController:detailNavController animated:YES completion:nil];
            }
                break;
                
            default:
                // Do nothing
                break;
        }
    }

}

- (void)handleDetailViewControllerWillPresent:(UIViewController *)detailViewController
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    SCTableViewModel *detailModel = [self modelForViewController:detailViewController];

    if (self.cellActions.detailModelWillPresent) {
        self.cellActions.detailModelWillPresent(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelWillPresent) {
        self.ownerSection.cellActions.detailModelWillPresent(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelWillPresent) {
        self.ownerTableViewModel.cellActions.detailModelWillPresent(self, indexPath, detailModel);
    }

}

- (void)handleDetailViewControllerDidPresent:(UIViewController *)detailViewController
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    SCTableViewModel *detailModel = [self modelForViewController:detailViewController];

    if (self.cellActions.detailModelDidPresent) {
        self.cellActions.detailModelDidPresent(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelDidPresent) {
        self.ownerSection.cellActions.detailModelDidPresent(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelDidPresent) {
        self.ownerTableViewModel.cellActions.detailModelDidPresent(self, indexPath, detailModel);
    }

}

- (BOOL)handleDetailViewControllerShouldDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    
    BOOL shouldDismiss = TRUE;
    if (self.cellActions.detailModelShouldDismiss) {
        shouldDismiss = self.cellActions.detailModelShouldDismiss(self, indexPath, [self modelForViewController:detailViewController]);
    }

    else if (self.ownerSection.cellActions.detailModelShouldDismiss) {
        self.ownerSection.cellActions.detailModelShouldDismiss(self, indexPath, [self modelForViewController:detailViewController]);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelShouldDismiss) {
        self.ownerTableViewModel.cellActions.detailModelShouldDismiss(self, indexPath, [self modelForViewController:detailViewController]);
    }

    return shouldDismiss;
}

- (void)handleDetailViewControllerWillDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self prepareCellForDetailViewDisappearing];
    
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    SCTableViewModel *detailModel = [self modelForViewController:detailViewController];

    if (self.cellActions.detailModelWillDismiss) {
        self.cellActions.detailModelWillDismiss(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelWillDismiss) {
        self.ownerSection.cellActions.detailModelWillDismiss(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelWillDismiss) {
        self.ownerTableViewModel.cellActions.detailModelWillDismiss(self, indexPath, detailModel);
    }

}

- (void)handleDetailViewControllerDidDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self setActiveDetailModel:nil];
	
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    SCTableViewModel *detailModel = [self modelForViewController:detailViewController];

    if (!cancelTapped) {
        [self commitDetailModelChanges:detailModel];
    }

    if (self.cellActions.detailModelDidDismiss) {
        self.cellActions.detailModelDidDismiss(self, indexPath, detailModel);
    }

    else if (self.ownerSection.cellActions.detailModelDidDismiss) {
        self.ownerSection.cellActions.detailModelDidDismiss(self, indexPath, detailModel);
    }

    else if (self.ownerTableViewModel.cellActions.detailModelDidDismiss) {
        self.ownerTableViewModel.cellActions.detailModelDidDismiss(self, indexPath, detailModel);
    }

}


- (void)handleDetailViewControllerWillGainFocus:(UIViewController *)detailViewController
{
    [self handleDetailViewControllerWillPresent:detailViewController];
}

- (void)handleDetailViewControllerDidGainFocus:(UIViewController *)detailViewController
{
    [self handleDetailViewControllerDidPresent:detailViewController];
}

- (void)handleDetailViewControllerWillLoseFocus:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerWillDismiss:detailViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)handleDetailViewControllerDidLoseFocus:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    [self.ownerTableViewModel.tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    [self handleDetailViewControllerDidDismiss:detailViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}


- (UIViewController *)generatedDetailViewController:(NSIndexPath *)indexPath
{
    return nil;  // should be implemented by subclasses
}



#pragma mark - SCTableViewControllerDelegate

- (void)tableViewControllerWillPresent:(SCTableViewController *)tableViewController
{
    [self handleDetailViewControllerWillPresent:tableViewController];
}

- (void)tableViewControllerDidPresent:(SCTableViewController *)tableViewController
{
    [self handleDetailViewControllerDidPresent:tableViewController];
}

- (BOOL)tableViewControllerShouldDismiss:(SCTableViewController *)tableViewController
					  cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    return [self handleDetailViewControllerShouldDismiss:tableViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)tableViewControllerWillDismiss:(SCTableViewController *)tableViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerWillDismiss:tableViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)tableViewControllerDidDismiss:(SCTableViewController *)tableViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
	[self handleDetailViewControllerDidDismiss:tableViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}


- (void)tableViewControllerWillGainFocus:(SCTableViewController *)tableViewController
{
    [self handleDetailViewControllerWillGainFocus:tableViewController];
}

- (void)tableViewControllerDidGainFocus:(SCTableViewController *)tableViewController
{
    [self handleDetailViewControllerDidGainFocus:tableViewController];
}

- (void)tableViewControllerWillLoseFocus:(SCTableViewController *)tableViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerWillLoseFocus:tableViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)tableViewControllerDidLoseFocus:(SCTableViewController *)tableViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerDidLoseFocus:tableViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

#pragma mark - SCViewControllerDelegate

- (void)viewControllerWillPresent:(SCViewController *)viewController
{
    [self handleDetailViewControllerWillPresent:viewController];
}

- (void)viewControllerDidPresent:(SCViewController *)viewController
{
    [self handleDetailViewControllerDidPresent:viewController];
}

- (BOOL)viewControllerShouldDismiss:(SCViewController *)viewController
                 cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    return [self handleDetailViewControllerShouldDismiss:viewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)viewControllerWillDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerWillDismiss:viewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)viewControllerDidDismiss:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
	[self handleDetailViewControllerDidDismiss:viewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}


- (void)viewControllerWillGainFocus:(SCViewController *)viewController
{
    [self handleDetailViewControllerWillGainFocus:viewController];
}

- (void)viewControllerDidGainFocus:(SCViewController *)viewController
{
    [self handleDetailViewControllerDidGainFocus:viewController];
}

- (void)viewControllerWillLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerWillLoseFocus:viewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

- (void)viewControllerDidLoseFocus:(SCViewController *)viewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
    [self handleDetailViewControllerDidLoseFocus:viewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];
}

@end




#pragma mark - SCCustomCell






@interface SCCustomCell ()
{
    NSMutableDictionary *_initialControlValues;  // used during rollback operations
    

}

// determines if the custom control is bound to either an object or a key
- (BOOL)controlWithTagIsBound:(NSUInteger)controlTag;

@end


#define kAutoResizeKey  @"autoResize"


@implementation SCCustomCell

+ (instancetype)cellWithText:(NSString *)cellText objectBindings:(NSDictionary *)bindings nibName:(NSString *)nibName
{
    return [self cellWithText:cellText boundObject:nil objectBindings:bindings nibName:nibName];
}

+ (instancetype)cellWithText:(NSString *)cellText objectBindingsString:(NSString *)bindingsString nibName:(NSString *)nibName
{
    NSDictionary *bindings = [SCUtilities bindingsDictionaryForBindingsString:bindingsString];
    
    return [self cellWithText:cellText boundObject:nil objectBindings:bindings nibName:nibName];
}

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object objectBindings:(NSDictionary *)bindings
	   nibName:(NSString *)nibName
{
	SCCustomCell *cell;
    if (nibName) {
        cell = (SCCustomCell *)[SCUtilities getFirstNodeInNibWithName:nibName];
        if (![cell isKindOfClass:[SCCustomCell class]] && [cell isKindOfClass:[UITableViewCell class]]) {
            cell = [SCCustomCell cellWithCell:cell];
        }

        if ([cell isKindOfClass:[SCCustomCell class]]) {
            cell.reuseId = nibName;
            /*
            A nib's canvas height is an estimate in STV 7; its constraints, not
            the design-time frame, determine the runtime row height.
            */
            cell.estimatedHeight = MAX(cell.frame.size.height, 1.0);
            cell.sizingMode = SCTableViewCellSizingModeAutomatic;
        }

        else {
            SCDebugLog(@"Warning: Unexpected cell type! Expecting 'SCCustomCell' but got '%@' instead.", NSStringFromClass([cell class]));
            
            cell = nil;
        }

    }

    else {
        cell = [[[self class] alloc] initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil];
    }

    cell.textLabel.text = cellText;
	[cell setBoundObject:object];
	[cell.objectBindings addEntriesFromDictionary:bindings];
	[cell configureCustomControls];
	
	return cell;
}

//overrides superclass
- (void)performInitialization
{
    [super performInitialization];
    
    _pauseControlEvents = FALSE;
    _objectBindings = [[NSMutableDictionary alloc] init];
    _autoResize = TRUE;
    _showClearButtonInInputAccessoryView = FALSE;
    
    _initialControlValues = [NSMutableDictionary dictionary];
}

// overrides superclass
- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];

    if (self) {
        // The following decodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only properties used in IB need to be here)
        if ([aDecoder containsValueForKey:kAutoResizeKey]) {
            self.autoResize = [aDecoder decodeBoolForKey:kAutoResizeKey];
        }

        [self configureCustomControls];
    }

    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    // The following encodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only IB published properties need to be here)
    [aCoder encodeBool:self.autoResize forKey:kAutoResizeKey];
}


// overrides superclass
- (void)setBoundObject:(NSObject *)boundObject
{
    [super setBoundObject:boundObject];
    
    [_initialControlValues removeAllObjects];
}

// overrides superclass
- (void)setBoundPropertyName:(NSString *)boundPropertyName
{
    [super setBoundPropertyName:boundPropertyName];
    
    [_initialControlValues removeAllObjects];
}



- (NSString *)objectBindingsString
{
    return [SCUtilities bindingsStringForBindingsDictionary:self.objectBindings];
}

- (void)setObjectBindingsString:(NSString *)objectBindingsString
{
    [self.objectBindings addEntriesFromDictionary:[SCUtilities bindingsDictionaryForBindingsString:objectBindingsString]];
}

- (void)setObjectBindingsPropertyName:(NSString *)propertyName forControlWithTag:(NSInteger)controlTag
{
    [self.objectBindings setValue:propertyName forKey:[NSString stringWithFormat:@"%i", (int)controlTag]];
}

//overrides superclass
- (BOOL)canBecomeFirstResponder
{
    return (self.inputControlsSortedByTag.count > 0);
}

//overrides superclass
- (BOOL)becomeFirstResponder
{
    NSArray *inputControls = self.inputControlsSortedByTag; // optimization
    if (inputControls.count) {
        [[inputControls objectAtIndex:0] becomeFirstResponder];
        
        return TRUE;
    }

    //else 
    return FALSE;
}

//overrides superclass
- (BOOL)resignFirstResponder
{
    [super resignFirstResponder];
    
    BOOL resign = FALSE;
    if (self == self.ownerTableViewModel.activeCell) {
        [self.ownerTableViewModel.activeCellControl resignFirstResponder];
        
        resign = TRUE;
    }

    return resign;
}

- (void)callDidBecomeFirstResponderActions
{
    if (self.cellActions.didBecomeFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.cellActions.didBecomeFirstResponder(self, indexPath);
    }

    else if (self.ownerSection.cellActions.didBecomeFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerSection.cellActions.didBecomeFirstResponder(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.didBecomeFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerTableViewModel.cellActions.didBecomeFirstResponder(self, indexPath);
    }

}

- (void)callDidResignFirstResponderActions
{
    if (self.cellActions.didResignFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.cellActions.didResignFirstResponder(self, indexPath);
    }

    else if (self.ownerSection.cellActions.didResignFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerSection.cellActions.didResignFirstResponder(self, indexPath);
    }

    else if (self.ownerTableViewModel.cellActions.didResignFirstResponder) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerTableViewModel.cellActions.didResignFirstResponder(self, indexPath);
    }

}


- (NSArray *)inputControlsSortedByTag
{
    NSMutableArray *inputControls = [NSMutableArray array];
    
    for(UIView *customControl in self.contentView.subviews)
    {
        if ([customControl isKindOfClass:[UITextField class]]
           || [customControl isKindOfClass:[UITextView class]]) {
            [inputControls addObject:customControl];
        }

    }
    
    // sort based on the controls' tag
    [inputControls sortedArrayUsingComparator:(NSComparator)^(id obj1, id obj2)
     {
        if ([(UIView *)obj1 tag] > [(UIView *)obj2 tag]) {
            return (NSComparisonResult)NSOrderedDescending;
        }

        if ([(UIView *)obj1 tag] < [(UIView *)obj2 tag]) {
            return (NSComparisonResult)NSOrderedAscending;
        }

        return (NSComparisonResult)NSOrderedSame;
     }];
    
    return inputControls;
}

- (UIView *)controlWithTag:(NSInteger)controlTag
{
    if (controlTag < 1) {
        return nil;
    }

    for(UIView *customControl in self.contentView.subviews) {
        if (customControl.tag == controlTag) {
            return customControl;
        }

    }


    return nil;
}

//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];
    
    for(UIControl *customControl in self.contentView.subviews)
    {
        if ([customControl isKindOfClass:[UITextView class]]) {
            [(UITextView *)customControl setEditable:_enabled];
        }

        else if (![customControl isKindOfClass:[UILabel class]]) {
            customControl.enabled = _enabled;
        }

    }
}

//overrides superclass


//overrides superclass


//override superclass
- (void)willDisplay
{
	[super willDisplay];

    if (!self.needsCommit) {
        [self loadBindingsIntoCustomControls];
    }

}

//override superclass
- (void)reloadBoundValue
{
	[super reloadBoundValue];
    
    [self loadBindingsIntoCustomControls];
}

//override superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    for(UIView *customControl in self.contentView.subviews)
	{
        if (customControl.tag < 1) {
            continue;
        }

        if ([customControl isKindOfClass:[UITextView class]]) {
            UITextView *textView = (UITextView *)customControl;
            [self commitValueForControlWithTag:textView.tag value:textView.text];
        }

        else if ([customControl isKindOfClass:[UITextField class]]) {
            UITextField *textField = (UITextField *)customControl;
            [self commitValueForControlWithTag:textField.tag value:textField.text];
        }

        else if ([customControl isKindOfClass:[UISlider class]]) {
            UISlider *slider = (UISlider *)customControl;
            [self commitValueForControlWithTag:slider.tag
												 value:[NSNumber numberWithFloat:slider.value]];
        }

        else if ([customControl isKindOfClass:[UISegmentedControl class]]) {
            UISegmentedControl *segmented = (UISegmentedControl *)customControl;
            [self commitValueForControlWithTag:segmented.tag
													 value:[NSNumber numberWithInteger:segmented.selectedSegmentIndex]];
        }

        else if ([customControl isKindOfClass:[UISwitch class]]) {
            UISwitch *switchControl = (UISwitch *)customControl;
            [self commitValueForControlWithTag:switchControl.tag
														 value:[NSNumber numberWithBool:switchControl.on]];
        }

    }
	
	[super commitChanges];
    
    
    [self.ownerTableViewModel reloadCellsIfNeeded];  // needed to make sure calculated cells are in sync
}

- (void)reloadControlValuesIfNeeded
{
    [self loadBindingsIntoCustomControls];
}

// overrides superclass
- (void)rollbackToInitialBoundValue
{
    [super rollbackToInitialBoundValue];
    
    for(NSString *propertyName in _initialControlValues)
    {
        NSObject *initialValue = [_initialControlValues valueForKey:propertyName];

        if (!initialValue) {
            continue;
        }

        if ([initialValue isKindOfClass:[NSNull class]]) {
            initialValue = nil;
        }

        // rollback initalValue
        if (self.boundObjectStore) {
            [self.boundObjectStore setValue:initialValue forPropertyName:propertyName inObject:self.boundObject];
        }

        else {
            [SCUtilities setValue:initialValue forPropertyName:propertyName inObject:self.boundObject];
        }

    }
}

- (BOOL)controlWithTagIsBound:(NSUInteger)controlTag
{
    BOOL isBound = FALSE;

    if (self.boundObject) {
        if ([self.objectBindings valueForKey:[NSString stringWithFormat:@"%i", (int)controlTag]]) {
            isBound = TRUE;
        }

    }

    return isBound;
}

- (NSObject *)boundValueForControlWithTag:(NSInteger)controlTag
{
	NSObject *controlValue = nil;

    if (self.boundObject) {
        NSString *propertyName = [self.objectBindings valueForKey:[NSString stringWithFormat:@"%i", (int)controlTag]];
        if (!propertyName) {
            return nil;
        }

        if ([SCUtilities propertyName:propertyName existsInObject:self.boundObject]) {
            if (self.boundObjectStore) {
                controlValue = [self.boundObjectStore valueForPropertyName:propertyName inObject:self.boundObject];
            }

            else {
                controlValue = [SCUtilities valueForPropertyName:propertyName inObject:self.boundObject];
            }

        }

    }

    return controlValue;
}

- (void)commitValueForControlWithTag:(NSInteger)controlTag value:(NSObject *)controlValue
{
    if (self.boundObject) {
        NSString *stringControlTag = [NSString stringWithFormat:@"%i", (int)controlTag];
        
        NSString *propertyName = [self.objectBindings valueForKey: stringControlTag];
        if (!propertyName) {
            return;
        }

        // set the initial value (if not exists) to provide rollback
        [self setInitialControlValueIfNeeded:[self boundValueForControlWithTag:controlTag] propertyName:propertyName];
        
        SCDataType propertyDataType = SCDataTypeUnknown;
        if (self.boundObjectStore) {
            SCDataDefinition *boundObjectDef = [self.boundObjectStore definitionForObject:self.boundObject];
            if (boundObjectDef) {
                propertyDataType = [boundObjectDef propertyDataTypeForPropertyWithName:propertyName];
            }

        }

        controlValue = [SCUtilities getValueCompatibleWithDataType:propertyDataType fromValue:controlValue];

        if (self.boundObjectStore) {
            [self.boundObjectStore setValue:controlValue forPropertyName:propertyName inObject:self.boundObject];
        }

        else {
            [SCUtilities setValue:controlValue forPropertyName:propertyName inObject:self.boundObject];
        }

    }

}

- (NSString *)curlyBraceBoundPropertyNameForControl:(UIView *)customControl
{
    if ([customControl isKindOfClass:[SCImageView class]]) {
        return [(SCImageView *)customControl boundPropertyName];
    }


    NSString *propertyName = nil;
    
    NSString *curlyBraceText = nil;
    if ([customControl isKindOfClass:[UITextView class]]) {
        UITextView *textView = (UITextView *)customControl;
        curlyBraceText = textView.text;
    }

    else if ([customControl isKindOfClass:[UITextField class]]) {
        UITextField *textField = (UITextField *)customControl;
        curlyBraceText = textField.text;
    }

    else if ([customControl isKindOfClass:[UILabel class]]) {
        UILabel *label = (UILabel *)customControl;
        curlyBraceText = label.text;
    }

    if (!curlyBraceText) {
        return nil;
    }

    NSRange leftBraceRange = [curlyBraceText rangeOfString:@"{"];
    if (leftBraceRange.location == NSNotFound) {
        return nil;
    }

    NSRange rightBraceRange = [curlyBraceText rangeOfString:@"}"];
    if (rightBraceRange.location == NSNotFound) {
        return nil;
    }

    NSRange propertyNameRange = (NSRange){.location = leftBraceRange.location+1, .length = rightBraceRange.location-leftBraceRange.location-1};
    propertyName = [curlyBraceText substringWithRange:propertyNameRange];
    propertyName = [propertyName stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];

    if ([customControl isKindOfClass:[UILabel class]]) {
        // Configure label's prefix and suffix
        
        UILabel *label = (UILabel *)customControl;
        if (leftBraceRange.location > 0) {
            label.prefix = [curlyBraceText substringToIndex:leftBraceRange.location];
        }

        if (rightBraceRange.location < [curlyBraceText length]-1) {
            label.suffix = [curlyBraceText substringFromIndex:rightBraceRange.location+1];
        }

    }

    if (![propertyName length]) {
        return nil;
    }

    //else
    return propertyName;
}

- (void)configureCustomControls
{
    // Parse controls for bindings with curly braces and auto generate bindings
    NSInteger maxTag = 0;
    for(UIView *customControl in self.contentView.subviews) {
        maxTag = MAX(customControl.tag, maxTag);
    }

    if (maxTag < kSTVTagStartRange) {
        maxTag = kSTVTagStartRange;
    }

    for(UIView *customControl in self.contentView.subviews)
    {
        maxTag++;
        
        NSString *propertyName = [self curlyBraceBoundPropertyNameForControl:customControl];
        if (propertyName) {
            if (!customControl.tag) {
                customControl.tag = maxTag;
            }

            [self.objectBindings setValue:propertyName forKey:[NSString stringWithFormat: @"%li", (long)customControl.tag]];
        }

    }
    
    
    // Connect controls' targets and actions
	for(UIView *customControl in self.contentView.subviews)
	{
        if (customControl.tag < 1) {
            continue;
        }

        if ([customControl isKindOfClass:[UITextView class]]) {
            UITextView *textView = (UITextView *)customControl;
            textView.delegate = self;
        }

        else if ([customControl isKindOfClass:[UITextField class]]) {
            UITextField *textField = (UITextField *)customControl;
            textField.delegate = self;
            [textField addTarget:self action:@selector(textFieldEditingChanged:)
					forControlEvents:UIControlEventEditingChanged];
        }

        else if ([customControl isKindOfClass:[UISlider class]]) {
            UISlider *slider = (UISlider *)customControl;
            [slider addTarget:self action:@selector(sliderValueChanged:)
                     forControlEvents:UIControlEventValueChanged];
        }

        else if ([customControl isKindOfClass:[UISegmentedControl class]]) {
            UISegmentedControl *segmented = (UISegmentedControl *)customControl;
            [segmented addTarget:self action:@selector(segmentedControlValueChanged:)
                            forControlEvents:UIControlEventValueChanged];
        }

        else if ([customControl isKindOfClass:[UISwitch class]]) {
            UISwitch *switchControl = (UISwitch *)customControl;
            [switchControl addTarget:self action:@selector(switchControlChanged:)
                                    forControlEvents:UIControlEventValueChanged];
        }

        else if ([customControl isKindOfClass:[UIButton class]]) {
            UIButton *customButton = (UIButton *)customControl;
            [customButton addTarget:self action:@selector(customButtonTapped:)
                                       forControlEvents:UIControlEventTouchUpInside];
        }

    }
}

- (void)loadBindingsIntoCustomControls
{
	_pauseControlEvents = TRUE;
	
	for(UIView *customControl in self.contentView.subviews)
	{
        if (customControl.tag<1 || ![self controlWithTagIsBound:customControl.tag]) {
            continue;
        }

        NSObject *controlValue = [self boundValueForControlWithTag:customControl.tag];

        if ([customControl isKindOfClass:[UILabel class]]) {
            if (!controlValue) {
                controlValue = @"";
            }

            if ([controlValue isKindOfClass:[NSDate class]]) {
                // Convert date into string using formatter from data definition (if possible)
                NSString *propertyName = [self.objectBindings valueForKey:[NSString stringWithFormat:@"%i", (int)customControl.tag]];
                SCPropertyDefinition *propertyDefinition = [[self.boundObjectStore definitionForObject:self.boundObject] propertyDefinitionWithName:propertyName];
                if ([propertyDefinition.attributes isKindOfClass:[SCDateAttributes class]]) {
                    SCDateAttributes *dateAttributes = (SCDateAttributes *)propertyDefinition.attributes;
                    controlValue = [dateAttributes.dateFormatter stringFromDate:(NSDate *)controlValue];
                }

            }

            UILabel *label = (UILabel *)customControl;
            
            NSString *prefixString = label.prefix;
            NSString *suffixString = label.suffix;
            NSString *labelString = [NSString stringWithFormat:@"%@%@%@", prefixString, controlValue, suffixString];
            if ([controlValue isKindOfClass:[NSNumber class]]) {
                // determine if suffix has any float formatters (float formatting syntax is ".00", where the number of zeros is the number of decimal places)
                NSUInteger numberOfDecimalPlaces = 0;
                NSRange zeroMarkRange = [suffixString rangeOfString:@"."];
                if (zeroMarkRange.location==0) // has to be the very first character in suffix
                {
                    numberOfDecimalPlaces = 0;
                    for(NSInteger i=1; i<[suffixString length]; i++) {
                        if ([suffixString characterAtIndex:i] == '0') {
                            numberOfDecimalPlaces++;
                        }

                    }


                    // Create formatString
                    NSString *floatSpecifier = [NSString stringWithFormat:@"%@%luf", @"%.", (unsigned long)numberOfDecimalPlaces];
                    NSString *formatString = [NSString stringWithFormat:@"%@%@%@", @"%@", floatSpecifier, @"%@"];
                    
                    // Remove floatFormatter from suffixString
                    suffixString = [suffixString substringFromIndex:numberOfDecimalPlaces+1];
                    
                    // Update labelString
                    float floatValue = [(NSNumber *)controlValue floatValue];
                    labelString = [NSString stringWithFormat:formatString, prefixString, floatValue, suffixString];
                }

            }

            label.text = labelString;
        }

        else if ([customControl isKindOfClass:[UITextView class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSString fromValue:controlValue];

            if (!controlValue) {
                controlValue = @"";
            }

            UITextView *textView = (UITextView *)customControl;
            textView.text = (NSString *)controlValue;
        }

        else if ([customControl isKindOfClass:[UITextField class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSString fromValue:controlValue];

            if (!controlValue) {
                controlValue = @"";
            }

            UITextField *textField = (UITextField *)customControl;
            textField.text = (NSString *)controlValue;
        }

        else if ([customControl isKindOfClass:[UISlider class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSNumber fromValue:controlValue];

            if (!controlValue) {
                controlValue = [NSNumber numberWithInt:0];
            }

            UISlider *slider = (UISlider *)customControl;
            slider.value = [(NSNumber *)controlValue floatValue];
        }

        else if ([customControl isKindOfClass:[UISegmentedControl class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSNumber fromValue:controlValue];

            if (!controlValue) {
                controlValue = [NSNumber numberWithInt:-1];
            }

            UISegmentedControl *segmented = (UISegmentedControl *)customControl;
            segmented.selectedSegmentIndex = [(NSNumber *)controlValue intValue];
        }

        else if ([customControl isKindOfClass:[UISwitch class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSNumber fromValue:controlValue];

            if (!controlValue) {
                controlValue = [NSNumber numberWithBool:FALSE];
            }

            UISwitch *switchControl = (UISwitch *)customControl;
            switchControl.on = [(NSNumber *)controlValue boolValue];
        }

        else if ([customControl isKindOfClass:[UIButton class]]) {
            controlValue = [SCUtilities getValueCompatibleWithDataType:SCDataTypeNSString fromValue:controlValue];

            if (controlValue) {
                UIButton *customButton = (UIButton *)customControl;
                NSString *buttonTitle = (NSString *)controlValue;
                [customButton setTitle:buttonTitle forState:UIControlStateNormal];
            }

        }

        else if ([customControl isKindOfClass:[UIImageView class]]) {
            UIImageView *imageView = (UIImageView *)customControl;

            if ([controlValue isKindOfClass:[UIImage class]]) {
                imageView.image = (UIImage *)controlValue;
            }

            else if ([controlValue isKindOfClass:[NSString class]]) {
                if ([SCUtilities isURLValid:(NSString *)controlValue]) {
                    NSURL *imageURL = [NSURL URLWithString:(NSString *)controlValue];
                                                
                    // Load image asynchronously
                    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul);
                    dispatch_async(queue, ^{
                        NSData * imageData = [NSData dataWithContentsOfURL:imageURL];
                        dispatch_async(dispatch_get_main_queue(), ^{
                            UIImage *image = [UIImage imageWithData:imageData];
                            [imageView setImage:image];
                        });
                    });
                }

                else {
                    // assume string contains simple image name
                    imageView.image = [UIImage imageNamed:(NSString *)controlValue];
                }

            }

        }

    }
	
	_pauseControlEvents = FALSE;
}

- (void)setInitialControlValueIfNeeded:(id)value propertyName:(NSString *)propertyName
{
    if (![_initialControlValues valueForKey:propertyName]) {
        if (!value) {
            value = [NSNull null];
        }

        [_initialControlValues setValue:value forKey:propertyName];
    }

}

#pragma mark - UITextView methods

- (BOOL)textViewShouldBeginEditing:(UITextView *)_textView
{
    if (!_textView.inputAccessoryView) {
        _textView.inputAccessoryView = self.ownerTableViewModel.inputAccessoryView;
    }

    if ([_textView.inputAccessoryView isKindOfClass:[SCInputAccessoryView class]]) {
        [(SCInputAccessoryView *)_textView.inputAccessoryView setShowClearButton:self.showClearButtonInInputAccessoryView];
    }

    BOOL shouldBegin = TRUE;

    if (shouldBegin) {
        [SCModelCenter sharedModelCenter].keyboardIssuer = self.ownerTableViewModel.viewController;
        
        self.ownerTableViewModel.activeCell = self;
        self.ownerTableViewModel.activeCellControl = _textView;
    }

    return shouldBegin;
}

- (void)textViewDidBeginEditing:(UITextView *)textView
{
    [self callDidBecomeFirstResponderActions];
    
    [self scrollToFocusCaretForTextView:textView];
}

- (BOOL)textViewShouldEndEditing:(UITextView *)_textView
{
	[SCModelCenter sharedModelCenter].keyboardIssuer = self.ownerTableViewModel.viewController;
	return TRUE;
}

- (void)textViewDidEndEditing:(UITextView *)textView
{
    [self callDidResignFirstResponderActions];
}

- (void)textViewDidChange:(UITextView *)_textView
{
    if (_pauseControlEvents) {
        return;
    }

    [self cellValueChanged];
}

- (void)scrollToFocusCaretForTextView:(UITextView *)textView
{
    CGRect caretRect = [textView caretRectForPosition:textView.selectedTextRange.start];
    
    CGRect convertedCaretRect = [self.ownerTableViewModel.tableView convertRect:caretRect fromView:textView];
    
    // Determine if convertedCaretRect is visible
    CGRect visibleRect;
    visibleRect.origin = self.ownerTableViewModel.tableView.contentOffset;
    visibleRect.origin.y += self.ownerTableViewModel.tableView.contentInset.top;
    visibleRect.size = self.ownerTableViewModel.tableView.bounds.size;
    visibleRect.size.height -= self.ownerTableViewModel.tableView.contentInset.top + self.ownerTableViewModel.tableView.contentInset.bottom;

    if (!CGRectContainsRect(visibleRect, convertedCaretRect)) {
        convertedCaretRect.size.height += 8; // add some space underneath the caret
        [self.ownerTableViewModel.tableView scrollRectToVisible:convertedCaretRect animated:YES];
    }

}

#pragma mark - UITextField methods

- (BOOL)textFieldShouldBeginEditing:(UITextField *)_textField
{
    if (!_textField.inputAccessoryView) {
        _textField.inputAccessoryView = self.ownerTableViewModel.inputAccessoryView;
    }

    if ([_textField.inputAccessoryView isKindOfClass:[SCInputAccessoryView class]]) {
        [(SCInputAccessoryView *)_textField.inputAccessoryView setShowClearButton:self.showClearButtonInInputAccessoryView];
    }

    BOOL shouldBegin = TRUE;

    if (shouldBegin) {
        [SCModelCenter sharedModelCenter].keyboardIssuer = self.ownerTableViewModel.viewController;
        
        self.ownerTableViewModel.activeCell = self;
        self.ownerTableViewModel.activeCellControl = _textField;
    }

    return shouldBegin;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    [self callDidBecomeFirstResponderActions];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    BOOL shouldChange = YES;

    if (self.cellActions.shouldChangeCharactersInRange) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        shouldChange = self.cellActions.shouldChangeCharactersInRange(self, indexPath, textField, range, string);
    }

    else if (self.ownerSection.cellActions.shouldChangeCharactersInRange) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        shouldChange = self.ownerSection.cellActions.shouldChangeCharactersInRange(self, indexPath, textField, range, string);
    }

    else if (self.ownerTableViewModel.cellActions.shouldChangeCharactersInRange) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        shouldChange = self.ownerTableViewModel.cellActions.shouldChangeCharactersInRange(self, indexPath, textField, range, string);
    }

    return shouldChange;
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)_textField
{
	[SCModelCenter sharedModelCenter].keyboardIssuer = self.ownerTableViewModel.viewController;
	return TRUE;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    [self callDidResignFirstResponderActions];
}

- (void)textFieldEditingChanged:(id)sender
{
    if (_pauseControlEvents) {
        return;
    }

    [self cellValueChanged];
}

- (BOOL)textFieldShouldReturn:(UITextField *)_textField
{
    if (self.cellActions.returnButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.cellActions.returnButtonTapped(self, indexPath);
        
        return TRUE;
    }

    // else
    if (self.ownerSection.cellActions.returnButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerSection.cellActions.returnButtonTapped(self, indexPath);
        
        return TRUE;
    }

    // else
    if (self.ownerTableViewModel.cellActions.returnButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerTableViewModel.cellActions.returnButtonTapped(self, indexPath);
        
        return TRUE;
    }

    BOOL handeledReturn;
	switch (_textField.returnKeyType)
	{
		case UIReturnKeyDefault:
		case UIReturnKeyNext:
			[self.ownerTableViewModel moveToNextCellControl:TRUE];
			handeledReturn = TRUE;
			break;
			
		case UIReturnKeyDone: 
			[_textField resignFirstResponder];
			handeledReturn = TRUE;
			break;
			
		default:
			handeledReturn = FALSE;
			break;
	}
	
	return handeledReturn;
}

#pragma mark - UISlider methods

- (void)sliderValueChanged:(id)sender
{
    if (_pauseControlEvents) {
        return;
    }

    self.ownerTableViewModel.activeCellControl = sender;
	
	[self cellValueChanged];
}

#pragma mark - UISegmentedControl methods

- (void)segmentedControlValueChanged:(id)sender
{
    if (_pauseControlEvents) {
        return;
    }

    self.ownerTableViewModel.activeCellControl = sender;
	
	[self cellValueChanged];
}

#pragma mark - UISwitch methods

- (void)switchControlChanged:(id)sender
{
    if (_pauseControlEvents) {
        return;
    }

    self.ownerTableViewModel.activeCellControl = sender;
	
	[self cellValueChanged];
}

#pragma mark - UIButton methods

- (void)customButtonTapped:(id)sender
{
    if (self.cellActions.customButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.cellActions.customButtonTapped(self, indexPath, sender);
    }

    else if (self.ownerSection.cellActions.customButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerSection.cellActions.customButtonTapped(self, indexPath, sender);
    }

    else if (self.ownerTableViewModel.cellActions.customButtonTapped) {
        NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
        self.ownerTableViewModel.cellActions.customButtonTapped(self, indexPath, sender);
    }

}


@end









#define kControlIndexInContentViewKey     @"controlIndexInContentView"
#define kControlCreatedInIBKey  @"controlCreatedInIB"

#pragma mark - SCControlCell


@interface SCControlCell ()

@property (nonatomic, strong) NSObject *lastLoadedControlValue;

@end



@interface SCControlCell ()
{
    BOOL showClearButtonInInputAccessoryView;
}
@end

@implementation SCControlCell

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];

    if (self) {
        // The following decodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only IB published properties need to be here)

        if ([aDecoder containsValueForKey:kControlCreatedInIBKey]) {
            _controlCreatedInIB = [aDecoder decodeBoolForKey:kControlCreatedInIBKey];
        }

        if ([aDecoder containsValueForKey:kControlIndexInContentViewKey]) {
            NSInteger controlIndexInContentView = [aDecoder decodeIntegerForKey:kControlIndexInContentViewKey];
            if (controlIndexInContentView!=-1 && controlIndexInContentView<self.contentView.subviews.count) {
                self.control = [self.contentView.subviews objectAtIndex:controlIndexInContentView];
            }

        }

    }

    return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    [super encodeWithCoder:aCoder];
    
    // The following encodings are required to be able to utilize the method [self cellWithCell:] with cells created in IB (only IB published properties need to be here)
    
    [aCoder encodeBool:self.controlCreatedInIB forKey:kControlCreatedInIBKey];
    
    NSInteger controlIndexInContentView = -1;
    if (self.control) {
        controlIndexInContentView = [self.contentView.subviews indexOfObject:self.control];
    }

    [aCoder encodeInteger:controlIndexInContentView forKey:kControlIndexInContentViewKey];
}

//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
    
    _maxTextLabelWidth = SC_DefaultMaxTextLabelWidth;
	_controlIndentation = SC_DefaultControlIndentation;
	_controlMargin = kDefaultControlMargin;

    if (!self.controlCreatedInIB) {
        self.textLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody]; //dgApps [UIFont fontWithName:self.textLabel.font.fontName size:kDefaultTitleLabelFontSize];
    }

}



//overrides superclass
- (void)awakeFromNib
{
    [super awakeFromNib];
    /*
    Interface Builder control cells in STV 7 use their contentView constraints
    instead of treating the design-time frame as a fixed runtime height.
    */
    self.sizingMode = SCTableViewCellSizingModeAutomatic;

    if (self.control) {
        _controlCreatedInIB = YES;
    }

}


//overrides superclass
- (void)commitChanges
{
    self.lastLoadedControlValue = self.boundValue;   // make sure self doesn't get reloaded when reloading calculated cells
    
    [super commitChanges];
}

//overrides superclass
- (void)reloadControlValuesIfNeeded
{
    [super reloadControlValuesIfNeeded];

    if (self.lastLoadedControlValue != self.boundValue) {
        _boundValueLoaded = NO;
        [self loadBoundValueIntoControl];
    }

}


- (UILabel *)ibControlLabel
{
    UILabel *controlLabel = nil;
    UIView *view = [self.contentView viewWithTag:kSTVTagStartRange];
    if ([view isKindOfClass:[UILabel class]]) {
        controlLabel = (UILabel *)view;
    }

    return controlLabel;
}


//overrides superclass
- (void)setBackgroundColor:(UIColor *)backgroundColor {
    [super setBackgroundColor:backgroundColor];

    if (self.cellCreatedInIB) {
        return;
    }

    for (UIView *subview in self.contentView.subviews) {
        if ([subview isKindOfClass:[UIControl class]] || [subview isKindOfClass:[UITextView class]] || [subview isKindOfClass:[UILabel class]]) {
            if (!self.backgroundView && backgroundColor) {
                [(UIControl *)subview setBackgroundColor:backgroundColor];
            }

        }

    }
}

//overrides superclass
- (void)setBackgroundView:(UIView *)backgroundView
{
    [super setBackgroundView:backgroundView];

    if (self.cellCreatedInIB) {
        return;
    }

    if (backgroundView) {
        for(UIView *subview in self.contentView.subviews)
        {
            if ([subview isKindOfClass:[UIControl class]] || [subview isKindOfClass:[UITextView class]] || [subview isKindOfClass:[UILabel class]]) {
                [(UIControl *)subview setBackgroundColor:[UIColor clearColor]];
            }

        }
    }

}


//overrides superclass


//overrides superclass
- (void)sc_installContentConstraints
{
    if (self.controlCreatedInIB || self.cellCreatedInIB || !self.control) {
        return;
    }

    NSString *key = [NSString stringWithFormat:@"%p-%d-%g-%g-%g",
        self.control, (self.textLabel.text.length && !self.textLabel.hidden) > 0, self.maxTextLabelWidth,
        self.controlMargin, self.controlIndentation];
    key = [key stringByAppendingFormat:@"-%p-%d-%d-%d",
        self.imageView.image, self.imageView.hidden, self.badgeView.text.length > 0,
        self.badgeView.hidden];
    if ([self.sc_layoutKey isEqualToString:key]) {
        return;
    }

    self.sc_layoutKey = key;
    [NSLayoutConstraint deactivateConstraints:self.sc_labelConstraints];
    NSMutableArray *constraints = [NSMutableArray array];
    UILayoutGuide *margins = [self sc_controlGuideAddingConstraints:constraints];
    UIView *control = self.control;
    control.translatesAutoresizingMaskIntoConstraints = NO;
    NSLayoutConstraint *minimum = [self.contentView.heightAnchor constraintGreaterThanOrEqualToConstant:44];
    minimum.priority = 750;
    [constraints addObject:minimum];
    [constraints addObject:[control.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor]];
    [constraints addObject:[control.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]];
    /*
    hermes:
    Native controls include their own internal vertical padding. A compact
    inset lets switches, sliders and segments share the standard row minimum
    without compressing their intrinsic height. Labels retain the text inset.
    */
    CGFloat controlInset = [control isKindOfClass:[UIControl class]] ? 4 : 8;
    NSLayoutConstraint *top = [control.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:controlInset];
    NSLayoutConstraint *bottom = [control.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-controlInset];
    NSLayoutConstraint *controlMinimum = [control.heightAnchor constraintGreaterThanOrEqualToConstant:20];
    top.priority = bottom.priority = controlMinimum.priority = 750;
    [constraints addObjectsFromArray:@[top, bottom, controlMinimum]];
    if ((self.textLabel.text.length && !self.textLabel.hidden)) {
        UILabel *title = self.textLabel;
        title.translatesAutoresizingMaskIntoConstraints = NO;
        title.adjustsFontForContentSizeCategory = YES;
        [title setContentHuggingPriority:251 forAxis:UILayoutConstraintAxisHorizontal];
        [constraints addObjectsFromArray:@[
            [title.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
            [title.centerYAnchor constraintEqualToAnchor:control.centerYAnchor],
            [title.widthAnchor constraintLessThanOrEqualToConstant:self.maxTextLabelWidth],
            [title.widthAnchor constraintLessThanOrEqualToAnchor:margins.widthAnchor multiplier:0.45],
            [control.leadingAnchor constraintGreaterThanOrEqualToAnchor:title.trailingAnchor constant:self.controlMargin]
        ]];
        NSLayoutConstraint *spacing = [control.leadingAnchor constraintEqualToAnchor:title.trailingAnchor constant:self.controlMargin];
        spacing.priority = 749;
        NSLayoutConstraint *indent = [control.leadingAnchor constraintGreaterThanOrEqualToAnchor:margins.leadingAnchor constant:self.controlIndentation];
        indent.priority = 750;
        NSLayoutConstraint *titleTop = [title.topAnchor constraintGreaterThanOrEqualToAnchor:self.contentView.topAnchor constant:8];
        NSLayoutConstraint *titleBottom = [title.bottomAnchor constraintLessThanOrEqualToAnchor:self.contentView.bottomAnchor constant:-8];
        titleTop.priority = titleBottom.priority = 750;
        [constraints addObjectsFromArray:@[spacing, indent, titleTop, titleBottom]];
    }

    else if ([control isKindOfClass:[UISwitch class]]) {
        [constraints addObject:[control.leadingAnchor constraintGreaterThanOrEqualToAnchor:margins.leadingAnchor]];
    }

    else {
        [constraints addObject:[control.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor]];
    }

    if ([control isKindOfClass:[UISwitch class]]) {
        [control setContentHuggingPriority:UILayoutPriorityRequired forAxis:UILayoutConstraintAxisHorizontal];
        [control setContentCompressionResistancePriority:UILayoutPriorityRequired forAxis:UILayoutConstraintAxisHorizontal];
    }

    self.sc_labelConstraints = constraints;
    [NSLayoutConstraint activateConstraints:constraints];
}

//override superclass
- (void)willDisplay
{
	[super willDisplay];

    if (!self.needsCommit) {
        [self loadBoundValueIntoControl];
    }

}

//override superclass
- (void)reloadBoundValue
{
    [super reloadBoundValue];
    
    _boundValueLoaded = NO;
    
	[self loadBoundValueIntoControl];
}


- (void)setControl:(UIView *)control
{
    if (control && ![control isKindOfClass:self.expectedControlClass]) {
        SCDebugLog(@"Warning: Control not assigned in control cell:%@. Expecting a control of class '%@' but got '%@' instead.", self, self.expectedControlClass, [control class]);
        return;
    }

    if (_control != control) {
        [NSLayoutConstraint deactivateConstraints:self.sc_labelConstraints];
        self.sc_labelConstraints = nil;
        [_control removeFromSuperview];
    }

    _control = control;
    if (_control && !_control.superview) {
        [self.contentView addSubview:_control];
    }

    [self configureControl];
    self.sc_layoutKey = nil;
    [self setNeedsUpdateConstraints];
}

- (Class)expectedControlClass
{
    // should be overridden by subclasses
    return [UIView class];
}

- (void)configureControl
{
    // does nothig, must be overridden by subclasses
}

- (NSObject *)controlValue
{
	// must be overridden by subclasses
    return nil;
}

- (void)loadBoundValueIntoControl
{
    // does nothing, should be overridden by subclasses
}

- (void)clearControl
{
    // does nothing, should be overridden by subclasses
}

@end




#pragma mark - SCLabelCell



@implementation SCLabelCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object labelTextPropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText boundObject:object labelTextPropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];

	self.selectionStyle = UITableViewCellSelectionStyleNone;
	
    UILabel *label = [[UILabel alloc] init];
    label.textAlignment = NSTextAlignmentRight;
    label.textColor = [UIColor secondaryLabelColor];  //dgApps
    label.font = [UIFont preferredFontForTextStyle:UIFontTextStyleCallout]; // dgApps [UIFont fontWithName:label.font.fontName size:kDefaultControlFontSize];
    /*
     If self.backgroundColor was null then iOS 14 was causing
     label.backgroundColor to display as black. Making the defaut clearColor
     here makes the label disply as expected. dgApps
     */
    label.backgroundColor = [UIColor clearColor];
    label.adjustsFontForContentSizeCategory = YES;
    self.control = label;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object labelTextPropertyName:(NSString *)propertyName
{
	return [self initWithText:cellText boundObject:object boundPropertyName:propertyName];
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UILabel class];
}

// overrides superclass
- (void)setControl:(UIView *)control
{
    [super setControl:control];
    
    _initialControlColor = self.label.textColor;
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    // no additional configuration
}

//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];

    if (_enabled) {
        self.label.textColor = _initialControlColor;
    }

    else {
        self.label.textColor = self.disabledTextColor;
    }

}

//overrides superclass


//overrides superclass
- (NSObject *)controlValue
{
	return self.label.text;
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    [self loadBoundValueIntoLabel];
    
    self.lastLoadedControlValue = self.boundValue;
}

- (void)loadBoundValueIntoLabel
{
    if (self.boundPropertyName) {
        NSObject *val = self.boundValue;
        if (!val) {
            val = @"";
        }

        self.label.text = [NSString stringWithFormat:@"%@", val];
        
        _boundValueLoaded = YES;
    }

}

- (UILabel *)label
{
	return (UILabel *)self.control;
}

@end




#pragma mark - SCTextViewCell





/*
hermes:
A scrolling editor exposes UIKit's already-calculated content size as its
intrinsic height. Constraints cap the viewport without measuring text,
rejecting input, or replacing the user's selection. UIKit owns text layout.
*/
@interface SCIntrinsicTextView : UITextView
@property (nonatomic) CGFloat sc_previousWidth;
@end

@implementation SCIntrinsicTextView
- (void)setText:(NSString *)text
{
    [super setText:text];
    [self invalidateIntrinsicContentSize];
}

- (void)setAttributedText:(NSAttributedString *)text
{
    [super setAttributedText:text];
    [self invalidateIntrinsicContentSize];
}

- (CGSize)intrinsicContentSize
{
    return CGSizeMake(UIViewNoIntrinsicMetric, self.contentSize.height);
}

- (void)setContentSize:(CGSize)size
{
    BOOL changed = !CGSizeEqualToSize(size, self.contentSize);
    [super setContentSize:size];
    if (changed) {
        [self invalidateIntrinsicContentSize];
    }

}

- (void)layoutSubviews
{
    [super layoutSubviews];
    if (self.sc_previousWidth != CGRectGetWidth(self.bounds)) {
        self.sc_previousWidth = CGRectGetWidth(self.bounds);
        [self invalidateIntrinsicContentSize];
    }

}
@end

@implementation SCTextViewCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object textViewTextPropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText boundObject:object textViewTextPropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
    [super performInitialization];
	
    self.selectionStyle = UITableViewCellSelectionStyleNone;
    
    _minimumHeight = kMinTextViewHeight;
    _maximumHeight = kMaxTextViewHeight;
	
    UITextView *textView = [[SCIntrinsicTextView alloc] init];
    textView.adjustsFontForContentSizeCategory = YES;
    textView.scrollEnabled = YES;
    [textView setContentCompressionResistancePriority:249 forAxis:UILayoutConstraintAxisVertical];
    [textView setContentHuggingPriority:249 forAxis:UILayoutConstraintAxisVertical];
    textView.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody]; // dgApps [UIFont fontWithName:self.textView.font.fontName size:kDefaultControlFontSize];
    textView.textColor = [UIColor secondaryLabelColor]; // dgApps
    self.control = textView;
}	

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object textViewTextPropertyName:(NSString *)propertyName
{
	return [self initWithText:cellText boundObject:object boundPropertyName:propertyName];
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UITextView class];
}

// overrides superclass
- (void)setControl:(UIView *)control
{
    [super setControl:control];

    if ([self.textView isKindOfClass:[SCIntrinsicTextView class]]) {
        self.textView.scrollEnabled = YES;
    }

    _initialControlColor = self.textView.textColor;
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    self.textView.delegate = self;
}

- (void)setAutoResize:(BOOL)autoResize
{
    _autoResize = autoResize;
    if ([self.textView isKindOfClass:[SCIntrinsicTextView class]]) {
        self.textView.scrollEnabled = YES;
    }

    self.sc_layoutKey = nil;
    [self setNeedsUpdateConstraints];
}

- (void)setMinimumHeight:(CGFloat)value
{
    _minimumHeight = MAX(0, value);
    self.sc_layoutKey = nil;
    [self setNeedsUpdateConstraints];
}

- (void)setMaximumHeight:(CGFloat)value
{
    _maximumHeight = value;
    self.sc_layoutKey = nil;
    [self setNeedsUpdateConstraints];
}

//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];

    if (_enabled) {
        self.textView.textColor = _initialControlColor;
    }

    else {
        self.textView.textColor = self.disabledTextColor;
    }

}

//overrides superclass
- (BOOL)canBecomeFirstResponder
{
    return TRUE;
}

//overrides superclass
- (BOOL)becomeFirstResponder
{
	[self.textView becomeFirstResponder];
    
    return TRUE;
}

//overrides superclass
- (BOOL)resignFirstResponder
{
    [super resignFirstResponder];
    
	[self.textView resignFirstResponder];
    
    return TRUE;
}

//overrides superclass
- (void)sc_installContentConstraints
{
    if (self.controlCreatedInIB || self.cellCreatedInIB || !self.control) {
        return;
    }

    NSString *key = [NSString stringWithFormat:@"text-%p-%d-%g-%g-%d",
        self.control, (self.textLabel.text.length && !self.textLabel.hidden) > 0,
        self.minimumHeight, self.maximumHeight, self.autoResize];
    key = [key stringByAppendingFormat:@"-%p-%d-%d-%d",
        self.imageView.image, self.imageView.hidden, self.badgeView.text.length > 0,
        self.badgeView.hidden];
    if ([self.sc_layoutKey isEqualToString:key]) {
        return;
    }

    self.sc_layoutKey = key;
    [NSLayoutConstraint deactivateConstraints:self.sc_labelConstraints];
    NSMutableArray *constraints = [NSMutableArray array];
    UILayoutGuide *margins = [self sc_controlGuideAddingConstraints:constraints];
    UITextView *editor = self.textView;
    editor.translatesAutoresizingMaskIntoConstraints = NO;
    [constraints addObjectsFromArray:@[
        [editor.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
        [editor.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor]
    ]];
    NSLayoutConstraint *bottom = [editor.bottomAnchor constraintEqualToAnchor:self.contentView.bottomAnchor constant:-8];
    bottom.priority = 750;
    [constraints addObject:bottom];
    if ((self.textLabel.text.length && !self.textLabel.hidden)) {
        UILabel *title = self.textLabel;
        title.translatesAutoresizingMaskIntoConstraints = NO;
        title.adjustsFontForContentSizeCategory = YES;
        [constraints addObjectsFromArray:@[
            [title.topAnchor constraintEqualToAnchor:self.contentView.topAnchor constant:8],
            [title.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
            [title.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor],
            [editor.topAnchor constraintEqualToAnchor:title.bottomAnchor constant:4]
        ]];
    }

    else {
        [constraints addObject:[editor.topAnchor constraintEqualToAnchor:self.contentView.topAnchor constant:8]];
    }

    NSLayoutConstraint *minimum = [editor.heightAnchor constraintGreaterThanOrEqualToConstant:self.minimumHeight];
    minimum.priority = 750;
    [constraints addObject:minimum];
    if (!self.autoResize || self.maximumHeight > 0) {
        CGFloat limit = self.autoResize ? MAX(self.minimumHeight, self.maximumHeight) : self.minimumHeight;
        NSLayoutConstraint *maximum = [editor.heightAnchor constraintLessThanOrEqualToConstant:limit];
        maximum.priority = 999;
        [constraints addObject:maximum];
    }

    self.sc_labelConstraints = constraints;
    [NSLayoutConstraint activateConstraints:constraints];
}


//overrides superclass
- (NSObject *)controlValue
{
	return self.textView.text;
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if (self.boundPropertyName && (!self.boundValue || [SCUtilities isStringClass:[self.boundValue class]]) ) {
        _pauseControlEvents = TRUE;
        self.textView.text = (NSString *)self.boundValue;
        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    self.boundValue = self.textView.text;
	
    [super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCTextViewAttributes class]]) {
        return;
    }

    SCTextViewAttributes *textViewAttributes = (SCTextViewAttributes *)attributes;
	self.autoResize = textViewAttributes.autoResize;
    if (textViewAttributes.minimumHeight > 0) {
        self.minimumHeight = textViewAttributes.minimumHeight;
    }

    if (textViewAttributes.maximumHeight > 0) {
        self.maximumHeight = textViewAttributes.maximumHeight;
    }

    if (!self.controlCreatedInIB) {
        self.textView.editable = textViewAttributes.editable;
    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if (![self.textView.text length] && self.valueRequired) {
        return FALSE;
    }

    //else
	return TRUE;
}

- (UITextView *)textView
{
	return (UITextView *)self.control;
}

- (void)clearControl
{
    self.textView.text = nil;
}

#pragma mark - UITextViewDelegate methods

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    return YES;
}

- (void)textViewDidChange:(UITextView *)_textView
{
    if (_pauseControlEvents) {
        return;
    }

    if (_textView != self.textView) {
        [super textViewDidChange:_textView];
        return;
    }

    [self cellValueChanged];
    
    
    // Resize cell if needed
    if (self.autoResize) {
        [self.textView invalidateIntrinsicContentSize];
        [self setNeedsUpdateConstraints];
        [self scrollToFocusCaretForTextView:self.textView];
    }

}

@end


#pragma mark - SCTextFieldCell




@implementation SCTextFieldCell

+ (instancetype)cellWithText:(NSString *)cellText placeholder:(NSString *)placeholder boundObject:(NSObject *)object textFieldTextPropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText placeholder:placeholder boundObject:object textFieldTextPropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];

	self.selectionStyle = UITableViewCellSelectionStyleNone;
	
    UITextField *textField = [[UITextField alloc] init];
    textField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    textField.autocorrectionType = UITextAutocorrectionTypeNo;
    textField.textColor =  [UIColor secondaryLabelColor];  //dgApps
    textField.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody]; // dgApps [UIFont fontWithName:self.textField.font.fontName size:kDefaultControlFontSize];
    textField.adjustsFontForContentSizeCategory = YES;
    self.control = textField;
}

- (instancetype)initWithText:(NSString *)cellText placeholder:(NSString *)placeholder boundObject:(NSObject *)object textFieldTextPropertyName:(NSString *)propertyName
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        self.textField.placeholder = placeholder;
    }

    return self;
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UITextField class];
}

// overrides superclass
- (void)setControl:(UIView *)control
{
    [super setControl:control];
    
    _initialControlColor = self.textField.textColor;
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    self.textField.delegate = self;
    [self.textField addTarget:self action:@selector(textFieldEditingChanged:)
             forControlEvents:UIControlEventEditingChanged];
}


//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];

    if (_enabled) {
        self.textField.textColor = _initialControlColor;
    }

    else {
        self.textField.textColor = self.disabledTextColor;
    }

}

//overrides superclass
- (BOOL)canBecomeFirstResponder
{
    return TRUE;
}

//overrides superclass
- (BOOL)becomeFirstResponder
{
	[self.textField becomeFirstResponder];
    
    return TRUE;
}

//overrides superclass
- (BOOL)resignFirstResponder
{
    [super resignFirstResponder];
    
	[self.textField resignFirstResponder];
    
    return TRUE;
}

//override's superclass


//overrides superclass
- (NSObject *)controlValue
{
	return self.textField.text;
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if (self.boundPropertyName && (!self.boundValue || [SCUtilities isStringClass:[self.boundValue class]]) ) {
        _pauseControlEvents = TRUE;
        self.textField.text = (NSString *)self.boundValue;
        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    self.boundValue = self.controlValue;
	
	[super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCTextFieldAttributes class]]) {
        return;
    }

    SCTextFieldAttributes *textFieldAttributes = (SCTextFieldAttributes *)attributes;

    if (!self.controlCreatedInIB) {
        if (textFieldAttributes.placeholder) {
            self.textField.placeholder = textFieldAttributes.placeholder;
        }

        self.textField.secureTextEntry = textFieldAttributes.secureTextEntry;
        self.textField.autocorrectionType = textFieldAttributes.autocorrectionType;
        self.textField.autocapitalizationType = textFieldAttributes.autocapitalizationType;
    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if (![self.textField.text length] && self.valueRequired) {
        return FALSE;
    }

    //else
	return TRUE;
}

- (UITextField *)textField
{
	return (UITextField *)self.control;
}

- (void)clearControl
{
    self.textField.text = nil;
}

@end


#pragma mark - SCNumericTextFieldCell



@implementation SCNumericTextFieldCell

//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	self.textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
	
	_minimumValue = nil;
	_maximumValue = nil;
	_allowFloatValue = TRUE;
	_displayZeroAsBlank = FALSE;
    
    _numberFormatter = [[NSNumberFormatter alloc] init];
    [_numberFormatter setNumberStyle:NSNumberFormatterDecimalStyle];
}


//overrides superclass
- (NSObject *)controlValue
{
    NSObject *value = nil;
    if ([self.textField.text length]) {
        [_numberFormatter setMinimum:self.minimumValue];
        [_numberFormatter setMaximum:self.maximumValue];
        [_numberFormatter setAllowsFloats:self.allowFloatValue];
        
        value = [_numberFormatter numberFromString:self.textField.text];
    }

    return value;
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if ( self.boundPropertyName && (!self.boundValue || [self.boundValue isKindOfClass:[NSNumber class]])) {
        _pauseControlEvents = TRUE;
		
        NSNumber *numericValue = (NSNumber *)self.boundValue;
        if (numericValue) {
            if ([numericValue floatValue]==0.0f && self.displayZeroAsBlank) {
                self.textField.text = nil;
            }

            else {
                [_numberFormatter setMinimum:self.minimumValue];
                [_numberFormatter setMaximum:self.maximumValue];
                [_numberFormatter setAllowsFloats:self.allowFloatValue];
                
                self.textField.text = [_numberFormatter stringFromNumber:numericValue];
            }

        }

        else {
            self.textField.text = nil;
        }

        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCNumericTextFieldAttributes class]]) {
        return;
    }

    SCNumericTextFieldAttributes *numericTextFieldAttributes =
											(SCNumericTextFieldAttributes *)attributes;
    if (numericTextFieldAttributes.minimumValue) {
        self.minimumValue = numericTextFieldAttributes.minimumValue;
    }

    if (numericTextFieldAttributes.maximumValue) {
        self.maximumValue = numericTextFieldAttributes.maximumValue;
    }

    self.allowFloatValue = numericTextFieldAttributes.allowFloatValue;
    if (numericTextFieldAttributes.numberFormatter) {
        _numberFormatter = numericTextFieldAttributes.numberFormatter;
    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if (![self.textField.text length]) {
        if (self.valueRequired) {
            return FALSE;
        }

        //else
        return TRUE;
    }

    [_numberFormatter setMinimum:self.minimumValue];
	[_numberFormatter setMaximum:self.maximumValue];
	[_numberFormatter setAllowsFloats:self.allowFloatValue];
	BOOL valid;
    if ([_numberFormatter numberFromString:self.textField.text]) {
        valid = TRUE;
    }

    else {
        valid = FALSE;
    }

    return valid;
}


@end




#pragma mark - SCSliderCell



@implementation SCSliderCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object sliderValuePropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText boundObject:object sliderValuePropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	self.selectionStyle = UITableViewCellSelectionStyleNone;
    
    UISlider *slider = [[UISlider alloc] init];
    slider.continuous = FALSE;
    self.control = slider;
}

//overrides superclass
- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object sliderValuePropertyName:(NSString *)propertyName
{
	return [self initWithText:cellText boundObject:object boundPropertyName:propertyName];
}

//overrides superclass
- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object boundPropertyName:(NSString *)propertyName
{
	self = [super initWithText:cellText boundObject:object boundPropertyName:propertyName];

    if (self.boundObject && !self.boundValue && self.commitChangesLive) {
        self.boundValue = [NSNumber numberWithFloat:self.slider.value];
    }

    return self;
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UISlider class];
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    [self.slider addTarget:self action:@selector(sliderValueChanged:)
          forControlEvents:UIControlEventValueChanged];
}


//overrides superclass
- (NSObject *)controlValue
{
    return [NSNumber numberWithFloat:self.slider.value];
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if (self.boundPropertyName && [self.boundValue isKindOfClass:[NSNumber class]]) {
        _pauseControlEvents = TRUE;
        self.slider.value = [(NSNumber *)self.boundValue floatValue];
        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    self.boundValue = self.controlValue;
	
    [super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCSliderAttributes class]]) {
        return;
    }

    SCSliderAttributes *sliderAttributes = (SCSliderAttributes *)attributes;

    if (!self.controlCreatedInIB) {
        if (sliderAttributes.minimumValue >= 0) {
            self.slider.minimumValue = sliderAttributes.minimumValue;
        }

        if (sliderAttributes.maximumValue >= 0) {
            self.slider.maximumValue = sliderAttributes.maximumValue;
        }

    }

}

- (UISlider *)slider
{
	return (UISlider *)self.control;
}

@end



#pragma mark - SCSegmentedCell



@implementation SCSegmentedCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectedSegmentIndexPropertyName:(NSString *)propertyName segmentTitlesArray:(NSArray *)cellSegmentTitlesArray
{
	return [[[self class] alloc] initWithText:cellText boundObject:object selectedSegmentIndexPropertyName:propertyName
						segmentTitlesArray:cellSegmentTitlesArray];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	self.selectionStyle = UITableViewCellSelectionStyleNone;
	
    UISegmentedControl *segmentedControl = [[UISegmentedControl alloc] init];
    self.control = segmentedControl;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectedSegmentIndexPropertyName:(NSString *)propertyName segmentTitlesArray:(NSArray *)cellSegmentTitlesArray
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        [self createSegmentsUsingArray:cellSegmentTitlesArray];
    }

    return self;
}


//overrides superclass
- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object boundPropertyName:(NSString *)propertyName
{
	self = [super initWithText:cellText boundObject:object boundPropertyName:propertyName];

    if (self.boundObject && !self.boundValue) {
        self.boundValue = [NSNumber numberWithInt:-1];
    }

    return self;
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UISegmentedControl class];
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    [self.segmentedControl addTarget:self action:@selector(segmentedControlValueChanged:) forControlEvents:UIControlEventValueChanged];
}


//override's superclass


//overrides superclass
- (NSObject *)controlValue
{
    if (self.boundPropertyDataType == SCDataTypeNSString) {
        return [self.segmentedControl titleForSegmentAtIndex:self.segmentedControl.selectedSegmentIndex];
    }

    //else
    return [NSNumber numberWithUnsignedInteger:self.segmentedControl.selectedSegmentIndex];
}

//override's superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if (self.boundPropertyName) {
        _pauseControlEvents = TRUE;

        if ([self.boundValue isKindOfClass:[NSNumber class]]) {
            self.segmentedControl.selectedSegmentIndex = [(NSNumber *)self.boundValue intValue];
        }

        else if ([self.boundValue isKindOfClass:[NSString class]]) {
            NSMutableArray *segmentTitles = [NSMutableArray array];
            for(NSInteger i=0; i<self.segmentedControl.numberOfSegments; i++) {
                [segmentTitles addObject:[self.segmentedControl titleForSegmentAtIndex:i]];
            }

            NSInteger selectedIndex = [segmentTitles indexOfObject:self.boundValue];
            if (selectedIndex != NSNotFound) {
                self.segmentedControl.selectedSegmentIndex = selectedIndex;
            }

        }

        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//override's superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    self.boundValue = self.controlValue;
    
	[super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCSegmentedAttributes class]]) {
        return;
    }

    SCSegmentedAttributes *segmentedAttributes = (SCSegmentedAttributes *)attributes;

    if (!self.controlCreatedInIB || self.segmentedControl.numberOfSegments<2) {
        if (segmentedAttributes.segmentTitlesArray) {
            [self createSegmentsUsingArray:segmentedAttributes.segmentTitlesArray];
        }

    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if ( (self.segmentedControl.selectedSegmentIndex==-1) && self.valueRequired ) {
        return FALSE;
    }

    //else
	return TRUE;
}

- (UISegmentedControl *)segmentedControl
{
	return (UISegmentedControl *)self.control;
}

- (void)createSegmentsUsingArray:(NSArray *)segmentTitlesArray
{
	[self.segmentedControl removeAllSegments];
    if (segmentTitlesArray) {
        for(NSUInteger i=0; i<segmentTitlesArray.count; i++)
		{
            NSString *segmentTitle = (NSString *)[segmentTitlesArray objectAtIndex:i];
            [self.segmentedControl insertSegmentWithTitle:segmentTitle atIndex:i
												 animated:FALSE];
        }
    }

}


@end


#pragma mark - SCSwitchCell




@implementation SCSwitchCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object switchOnPropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText boundObject:object switchOnPropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	self.selectionStyle = UITableViewCellSelectionStyleNone;
    
    UISwitch *switchControl = [[UISwitch alloc] init];
    self.control = switchControl;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object switchOnPropertyName:(NSString *)propertyName
{
	return [self initWithText:cellText boundObject:object boundPropertyName:propertyName];
}

//overrides superclass
- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object boundPropertyName:(NSString *)propertyName
{
	self = [super initWithText:cellText boundObject:object boundPropertyName:propertyName];

    if (self.boundObject && !self.boundValue && self.commitChangesLive) {
        self.boundValue = [NSNumber numberWithBool:self.switchControl.on];
    }

    return self;
}


// overrides superclass
- (Class)expectedControlClass
{
    return [UISwitch class];
}

// overrides superclass
- (void)configureControl
{
    if (!self.control) {
        return;
    }

    [self.switchControl addTarget:self action:@selector(switchControlChanged:) forControlEvents:UIControlEventValueChanged];
}


//overrides superclass


//overrides superclass
- (NSObject *)controlValue
{
    return [NSNumber numberWithBool:self.switchControl.on];
}

//overrides superclass
- (void)loadBoundValueIntoControl
{
    if (_boundValueLoaded) {
        return;
    }

    if (self.boundPropertyName && [self.boundValue isKindOfClass:[NSNumber class]]) {
        _pauseControlEvents = TRUE;
        self.switchControl.on = [(NSNumber *)self.boundValue boolValue];
        _pauseControlEvents = FALSE;
        
        _boundValueLoaded = YES;
        
        self.lastLoadedControlValue = self.boundValue;
    }

}

//overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    self.boundValue = self.controlValue;
	
    [super commitChanges];
}

- (UISwitch *)switchControl
{
	return (UISwitch *)self.control;
}

@end






#pragma mark - SCDateCell



@interface SCDateCell ()
{
    BOOL _embeddedPickerVisible;
    BOOL _dateCleared;
}

@property (nonatomic, strong) NSArray<NSLayoutConstraint *> *sc_pickerConstraints;

- (void)pickerValueChanged;
- (void)deviceOrientationDidChange:(NSNotification *)notification;

@end



@interface SCDateCell ()
{
    UITextField *_pickerField;
    UIViewController *_activePickerDetailViewController;
}
@end

@implementation SCDateCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object datePropertyName:(NSString *)propertyName
{
	return [[[self class] alloc] initWithText:cellText boundObject:object datePropertyName:propertyName];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
    
    _dateCleared = FALSE;
    _activePickerDetailViewController = nil;
	
	_datePicker = [[UIDatePicker alloc] init];
	[_datePicker addTarget:self action:@selector(pickerValueChanged)
		 forControlEvents:UIControlEventValueChanged];
	
	_pickerField = [[UITextField alloc] initWithFrame:CGRectZero];
	_pickerField.delegate = self;
	_pickerField.inputView = _datePicker;
    [self.contentView addSubview:_pickerField];
    _pickerField.translatesAutoresizingMaskIntoConstraints = NO;
    [NSLayoutConstraint activateConstraints:@[
        [_pickerField.leadingAnchor constraintEqualToAnchor:self.contentView.leadingAnchor],
        [_pickerField.topAnchor constraintEqualToAnchor:self.contentView.topAnchor],
        [_pickerField.widthAnchor constraintEqualToConstant:0],
        [_pickerField.heightAnchor constraintEqualToConstant:0]
    ]];
	
	_dateFormatter = [[NSDateFormatter alloc] init];
	[_dateFormatter setDateFormat:@"MMM d  hh:mm a"];
	_displaySelectedDate = TRUE;
	_displayDatePickerAsInputAccessoryView = FALSE;
    self.showClearButtonInInputAccessoryView = TRUE;
    
    // Track device orientation changes to correctly show/hide the date picker
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self selector: @selector(deviceOrientationDidChange:) name: UIDeviceOrientationDidChangeNotification object:nil];
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object datePropertyName:(NSString *)propertyName
{
	return [self initWithText:cellText boundObject:object boundPropertyName:propertyName];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}


//overrides superclass
- (BOOL)canBecomeFirstResponder
{
    if (self.displayDatePickerAsInputAccessoryView) {
        return TRUE;
    }

    //else
    return FALSE;
}

//overrides superclass
- (BOOL)becomeFirstResponder
{
    if (self.displayDatePickerAsInputAccessoryView) {
        [_pickerField becomeFirstResponder];
        [self callDidBecomeFirstResponderActions];
        
        return TRUE;
    }

    return FALSE;
}

//overrides superclass
- (BOOL)resignFirstResponder
{
    [super resignFirstResponder];
    
    BOOL response = YES;

    if (self.displayDatePickerAsInputAccessoryView) {
        response = [_pickerField resignFirstResponder];
    }

    else {
        response = NO;
    }

    [self callDidResignFirstResponderActions];
    
	return response;
}

- (void)showEmbeddedDatePicker
{
    if (_embeddedPickerVisible) {
        return;
    }

    _embeddedPickerVisible = YES;
    if (!self.datePicker.superview) {
        [self.contentView addSubview:self.datePicker];
    }

    [self setNeedsUpdateConstraints];
    [self sc_installContentConstraints];
    [self.ownerTableViewModel invalidateHeightForCell:self animated:YES];
}

- (void)hideEmbeddedDatePicker
{
    if (!_embeddedPickerVisible) {
        return;
    }

    _embeddedPickerVisible = NO;
    [NSLayoutConstraint deactivateConstraints:self.sc_pickerConstraints];
    self.sc_pickerConstraints = nil;
    if (self.datePicker.superview == self.contentView) {
        [self.datePicker removeFromSuperview];
    }

    self.sc_layoutKey = nil;
    [self setNeedsUpdateConstraints];
    [self sc_installContentConstraints];
    [self.ownerTableViewModel invalidateHeightForCell:self animated:YES];
}

//overrides superclass
- (NSObject *)controlValue
{
    NSDate *value = nil;
    if (self.label.text && ![self.label.text isEqualToString:self.placeholder]) { // a date has been selected
        value = self.datePicker.date;
    }

    if (self.datePicker.datePickerMode == UIDatePickerModeDate) {
        value = [SCUtilities stripTimeFromDate:value];
    }

    return value;
}

//overrides superclass
- (void)loadBoundValueIntoLabel
{
	NSDate *date = nil;
    if (self.boundPropertyName && [self.boundValue isKindOfClass:[NSDate class]]) {
        date = (NSDate *)self.boundValue;
        self.datePicker.date = date;
    }

    if (date) {
        self.label.text = [_dateFormatter stringFromDate:date];
    }

    else {
        self.label.text = self.placeholder;
    }

    self.label.hidden = !self.displaySelectedDate;
    
    _boundValueLoaded = YES;
}

//override superclass


//override superclass
- (void)sc_installContentConstraints
{
    NSArray<NSLayoutConstraint *> *previousRowConstraints = self.sc_labelConstraints;
    [super sc_installContentConstraints];
    if (!_embeddedPickerVisible) {
        return;
    }

    if (previousRowConstraints != self.sc_labelConstraints) {
        /*
        hermes:
        The expanded row must follow the current control/title topology after
        the base installer rebuilds it, rather than retaining stale anchors.
        */
        [NSLayoutConstraint deactivateConstraints:self.sc_pickerConstraints];
        self.sc_pickerConstraints = nil;
    }

    if (self.sc_pickerConstraints) {
        return;
    }

    for(NSLayoutConstraint *constraint in self.sc_labelConstraints) {
        if (constraint.firstItem == self.control && constraint.firstAttribute == NSLayoutAttributeCenterY) {
            constraint.active = NO;
        }

    }


    UIDatePicker *picker = self.datePicker;
    picker.translatesAutoresizingMaskIntoConstraints = NO;
    UILayoutGuide *margins = self.contentView.layoutMarginsGuide;
    NSLayoutConstraint *rowTop = [self.control.topAnchor constraintEqualToAnchor:self.contentView.topAnchor constant:8];
    rowTop.priority = 749;
    NSLayoutConstraint *pickerTop = [picker.topAnchor constraintEqualToAnchor:self.control.bottomAnchor constant:8];
    pickerTop.priority = 749;
    NSLayoutConstraint *bottom = [picker.bottomAnchor constraintEqualToAnchor:self.contentView.bottomAnchor constant:-8];
    bottom.priority = 750;
    NSMutableArray *constraints = [NSMutableArray arrayWithArray:@[
        rowTop, pickerTop, bottom,
        [picker.topAnchor constraintGreaterThanOrEqualToAnchor:self.control.bottomAnchor constant:8],
        [picker.leadingAnchor constraintEqualToAnchor:margins.leadingAnchor],
        [picker.trailingAnchor constraintEqualToAnchor:margins.trailingAnchor]
    ]];
    if (self.textLabel.text.length && !self.controlCreatedInIB) {
        [constraints addObject:[picker.topAnchor constraintGreaterThanOrEqualToAnchor:self.textLabel.bottomAnchor constant:8]];
    }

    self.sc_pickerConstraints = constraints;
    [NSLayoutConstraint activateConstraints:constraints];
}

//override superclass
- (void)cellValueChanged
{
    if (!_dateCleared) {
        self.label.text = [_dateFormatter stringFromDate:self.datePicker.date];
    }

    else {
        _dateCleared = FALSE; // reset flag
    }

    [super cellValueChanged];
}

//overrides superclass
- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
	[self cellValueChanged];
}

//overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    if (self.label.text) { // if a date value have been selected
        self.boundValue = self.datePicker.date;
    }

    else {
        self.boundValue = nil;
    }

    [super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCDateAttributes class]]) {
        return;
    }

    SCDateAttributes *dateAttributes = (SCDateAttributes *)attributes;
    if (dateAttributes.dateFormatter) {
        self.dateFormatter = dateAttributes.dateFormatter;
    }

    self.datePicker.datePickerMode = dateAttributes.datePickerMode;
    if (!self.displayDatePickerAsInputAccessoryView) {
        self.displayDatePickerAsInputAccessoryView = dateAttributes.displayDatePickerAsInputAccessoryView;
    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if (!self.label.text && self.valueRequired) {
        return FALSE;
    }

    //else
	return TRUE;
}

// overrides superclass
- (void)setEnabled:(BOOL)isEnabled
{
    if (!isEnabled && !self.displayDatePickerAsInputAccessoryView) {
        [self hideEmbeddedDatePicker];
    }

    [super setEnabled:isEnabled];
}

//override parent's
- (void)didSelectCell
{
    if (self.displayDatePickerAsInputAccessoryView) {
        if (![_pickerField isFirstResponder]) {
            [_pickerField becomeFirstResponder];
            [self callDidBecomeFirstResponderActions];
        }

    }

    else {
        if (_embeddedPickerVisible) {
            [self hideEmbeddedDatePicker];
        }

        else {
            [self showEmbeddedDatePicker];
        }

    }

    self.ownerTableViewModel.activeCell = self;
    
    [super didSelectCell];
}

- (void)willDeselectCell
{
    if (_embeddedPickerVisible) {
        [self hideEmbeddedDatePicker];
    }

    [super willDeselectCell];
}

- (void)pickerValueChanged
{
	[self cellValueChanged];
}

- (void)clearControl
{
    self.label.text = nil;
    _dateCleared = TRUE;
    
    [self cellValueChanged];
}

- (void)deviceOrientationDidChange:(NSNotification *)notification
{
    [self resignFirstResponder];
}

@end



#pragma mark - SCSelectionCell



@interface SCSelectionCell ()
{
    @protected
    NSArray *_items;
    BOOL itemsInSync;
    BOOL _loadingContents;
    UIActivityIndicatorView *_activityIndicator;
    BOOL _commitingDetailModel;
}

- (void)buildSelectedItemsIndexesFromBoundValue;
- (void)buildSelectedItemsIndexesFromString:(NSString *)string;
- (NSString *)buildStringFromSelectedItemsIndexes;

- (NSString *)getTitleForItemAtIndex:(NSUInteger)index;

@end

@implementation SCSelectionCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectedIndexPropertyName:(NSString *)propertyName items:(NSArray *)cellItems
{
	return [[[self class] alloc] initWithText:cellText boundObject:object selectedIndexPropertyName:propertyName items:cellItems];
}

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectedIndexesPropertyName:(NSString *)propertyName items:(NSArray *)cellItems allowMultipleSelection:(BOOL)multipleSelection;
{
	return [[[self class] alloc] initWithText:cellText boundObject:object selectedIndexesPropertyName:propertyName items:cellItems allowMultipleSelection:multipleSelection];
}

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectionStringPropertyName:(NSString *)propertyName items:(NSArray *)cellItems
{
	return [[[self class] alloc] initWithText:cellText boundObject:object selectionStringPropertyName:propertyName items:cellItems];
}


//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	_selectionItemsStore = nil;
    _selectionItemsFetchOptions = nil;  // will be re-initialized when selectionItemsStore is set
    _items = nil;
    itemsInSync = FALSE;
    _loadingContents = FALSE;
    _activityIndicator = nil;
    _commitingDetailModel = FALSE;
    
	_allowMultipleSelection = FALSE;
	_allowNoSelection = FALSE;
	_maximumSelections = 0;
	_autoDismissDetailView = FALSE;
	_hideDetailViewNavigationBar = FALSE;
    _allowAddingItems = FALSE;
	_allowDeletingItems = FALSE;
	_allowMovingItems = FALSE;
	_allowEditDetailView = FALSE;
	_displaySelection = TRUE;
	_delimeter = @", ";
	_selectedItemsIndexes = [[NSMutableSet alloc] init];
    _placeholderCell = nil;
    
    _addNewItemCell = nil;
    _addNewItemCellExistsInNormalMode = FALSE;
    _addNewItemCellExistsInEditingMode = TRUE;
	
	self.detailViewControllerOptions.tableViewStyle = UITableViewStyleGrouped;
	self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	self.selectionStyle = UITableViewCellSelectionStyleBlue;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectedIndexPropertyName:(NSString *)propertyName items:(NSArray *)cellItems
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        self.boundPropertyDataType = SCDataTypeNSNumber;
        self.allowMultipleSelection = FALSE;
        
        self.selectionItemsStore = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:cellItems] defaultDefiniton:[SCStringDefinition definition]];
		
        [self buildSelectedItemsIndexesFromBoundValue];

        if (self.boundObject && !self.boundValue && self.commitChangesLive) {
            self.boundValue = [NSNumber numberWithInt:-1];
        }

    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectedIndexesPropertyName:(NSString *)propertyName items:(NSArray *)cellItems allowMultipleSelection:(BOOL)multipleSelection
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        self.selectionItemsStore = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:cellItems] defaultDefiniton:[SCStringDefinition definition]];
        
        self.allowMultipleSelection = multipleSelection;
		
        [self buildSelectedItemsIndexesFromBoundValue];

        if (self.boundObject && !self.boundValue && self.commitChangesLive) {
            self.boundValue = [NSMutableSet set];   //Empty set
        }

    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectionStringPropertyName:(NSString *)propertyName items:(NSArray *)cellItems
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        self.boundPropertyDataType = SCDataTypeNSString;
        self.allowMultipleSelection = FALSE;
        
        self.selectionItemsStore = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:cellItems] defaultDefiniton:[SCStringDefinition definition]];
		
        [self buildSelectedItemsIndexesFromBoundValue];
    }

    return self;
}


- (NSArray *)items
{
    if (!itemsInSync) {
        if (!_items) {
            _items = [NSArray array];
        }

        switch (self.selectionItemsStore.storeMode)
        {
            case SCStoreModeSynchronous:
                _items = [self.selectionItemsStore fetchObjectsWithOptions:self.selectionItemsFetchOptions];
                itemsInSync = TRUE;
                break;
                
            case SCStoreModeAsynchronous:
            {
                BOOL skipBuildingFromBoundValue = _commitingDetailModel;
                _loadingContents = TRUE;
                self.selectable = FALSE;
                if (!_activityIndicator) {
                    _activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleMedium]; // dgApps
                    _activityIndicator.translatesAutoresizingMaskIntoConstraints = NO;
                    [self.contentView addSubview:_activityIndicator];
                    [NSLayoutConstraint activateConstraints:@[
                        [_activityIndicator.centerXAnchor constraintEqualToAnchor:self.contentView.centerXAnchor],
                        [_activityIndicator.centerYAnchor constraintEqualToAnchor:self.contentView.centerYAnchor]
                    ]];
                }

                [_activityIndicator startAnimating];
                [self.selectionItemsStore asynchronousFetchObjectsWithOptions:self.selectionItemsFetchOptions
                success:^(NSArray *results)
                 {
                    self->_items = results;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"

                    if (!skipBuildingFromBoundValue) {
                        [self buildSelectedItemsIndexesFromBoundValue];
                    }

                    self->_boundValueLoaded = NO;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    [self loadBoundValueIntoControl];
                     
                    self->_loadingContents = FALSE;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    [self->_activityIndicator stopAnimating];  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    self.selectable = TRUE;
                }
                failure:^(NSError *error)
                 {
                    self->_loadingContents = FALSE;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    [self->_activityIndicator stopAnimating];  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    self->itemsInSync = FALSE;  // dgApps added the self-> to avoid a warning: "Block implicitly retains 'self'; explicitly mention 'self' to indicate this is intended behavior"
                    self.selectable = TRUE;
                }
                 noConnection:^BOOL()
                 {
                     return NO;
                 }];
                itemsInSync = TRUE;
            }
                break;
        }
    }

    return _items;
}

- (void)setItems:(NSArray *)customItems
{
    if ([self.selectionItemsStore isKindOfClass:[SCArrayStore class]]) {
        [(SCArrayStore *)self.selectionItemsStore setObjectsArray:[NSMutableArray arrayWithArray:customItems]];
    }

    _items = customItems;
    itemsInSync = TRUE;
}

- (void)setSelectionItemsStore:(SCDataStore *)store
{
    _selectionItemsStore = store;
    
    _selectionItemsFetchOptions = [store.defaultDataDefinition generateCompatibleDataFetchOptions];
    
    
    itemsInSync = FALSE;
    
    [self buildSelectedItemsIndexesFromBoundValue];
    
    _boundValueLoaded = NO;
    [self loadBoundValueIntoControl];
}

- (void)setSelectionItemsFetchOptions:(SCDataFetchOptions *)fetchOptions
{
    _selectionItemsFetchOptions = fetchOptions;
    
    itemsInSync = FALSE;
    
    [self buildSelectedItemsIndexesFromBoundValue];
    
    _boundValueLoaded = NO;
    [self loadBoundValueIntoControl];
}

//overrides superclass


//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];

    if (_enabled) {
        self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    else {
        self.accessoryType = UITableViewCellAccessoryNone;
    }

}

- (void)buildSelectedItemsIndexesFromBoundValue
{
	[self.selectedItemsIndexes removeAllObjects];

    if ([self.boundValue isKindOfClass:[NSNumber class]]) {
        [self.selectedItemsIndexes addObject:(NSNumber *)self.boundValue];
    }

    else if ([self.boundValue isKindOfClass:[NSMutableSet class]]) {
        NSMutableSet *boundSet = (NSMutableSet *)self.boundValue;
        for(NSNumber *index in boundSet) {
            [self.selectedItemsIndexes addObject:index];
        }

    }

    else if ([SCUtilities isStringClass:[self.boundValue class]] && self.items) {
        [self buildSelectedItemsIndexesFromString:(NSString *)self.boundValue];
    }

}

- (void)buildSelectedItemsIndexesFromString:(NSString *)string
{
	NSArray *selectionStrings = [string componentsSeparatedByString:@";"];
	
	[self.selectedItemsIndexes removeAllObjects];
	for(NSString *selectionString in selectionStrings)
	{
		NSUInteger index = [self.items indexOfObject:selectionString];
        if (index != NSNotFound) {
            [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
        }

    }
}

- (NSString *)buildStringFromSelectedItemsIndexes
{
	NSMutableArray *selectionStrings = [NSMutableArray arrayWithCapacity:[self.selectedItemsIndexes count]];
	for(NSNumber *index in self.selectedItemsIndexes)
	{
		[selectionStrings addObject:[self.items objectAtIndex:[index intValue]]];
	}
	
	return [selectionStrings componentsJoinedByString:@";"];
}

//override superclass
- (void)cellValueChanged
{
    _boundValueLoaded = NO;
	[self loadBoundValueIntoControl];
	
	[super cellValueChanged];
}

- (NSString *)getTitleForItemAtIndex:(NSUInteger)index
{
    return [self.items objectAtIndex:index];
}

//override superclass
- (void)loadBoundValueIntoLabel
{
    NSArray *indexesArray = [[self.selectedItemsIndexes allObjects]
                             sortedArrayUsingSelector:@selector(compare:)];
    if (self.items.count && self.displaySelection && indexesArray.count) {
        NSMutableString *selectionString = [[NSMutableString alloc] init];
        for(NSUInteger i=0; i<indexesArray.count; i++)
        {
            NSUInteger index = [(NSNumber *)[indexesArray objectAtIndex:i] intValue];
            if (index > (self.items.count-1)) {
                continue;
            }

            NSString *itemTitle = [self getTitleForItemAtIndex:index];
            if (!itemTitle) {
                continue;
            }

            if (i==0) {
                [selectionString appendString:itemTitle];
            }

            else {
                [selectionString appendFormat:@"%@%@", self.delimeter, itemTitle];
            }

        }
        self.label.text = selectionString;
    }

    else {
        self.label.text = nil;
    }

    _boundValueLoaded = YES;
}

- (void)reloadBoundValue
{
    itemsInSync = FALSE;
    _boundValueLoaded = NO;
    
	[self buildSelectedItemsIndexesFromBoundValue];
	[self loadBoundValueIntoControl];
}
			 
- (void)buildDetailModel:(SCTableViewModel *)detailModel
{
    [detailModel clear];

    if ([detailModel isKindOfClass:[SCSelectionModel class]]) {
        SCSelectionModel *selectionModel = (SCSelectionModel *)detailModel;
        
        selectionModel.dataStore = self.selectionItemsStore;
        selectionModel.dataFetchOptions = self.selectionItemsFetchOptions;
        
        selectionModel.boundObjectStore = self.boundObjectStore;
        selectionModel.allowNoSelection = self.allowNoSelection;
        selectionModel.maximumSelections = self.maximumSelections;
        selectionModel.allowMultipleSelection = self.allowMultipleSelection;
        selectionModel.autoDismissViewController = self.autoDismissDetailView;
        
        selectionModel.allowAddingItems = self.allowAddingItems;
        selectionModel.allowDeletingItems = self.allowDeletingItems;
        selectionModel.allowMovingItems = self.allowMovingItems;
        selectionModel.allowEditDetailView = self.allowEditDetailView;
        
        [selectionModel setDetailViewControllerOptions:self.detailViewControllerOptions];
    }

    else {
        SCSelectionSection *selectionSection = [SCSelectionSection sectionWithHeaderTitle:nil dataStore:self.selectionItemsStore];
        
        selectionSection.dataFetchOptions = self.selectionItemsFetchOptions;

        if (self.boundPropertyDataType == SCDataTypeNSNumber) {
            selectionSection.selectedItemIndex = self.selectedItemIndex;
        }

        else {
            for(NSNumber *index in self.selectedItemsIndexes) {
                [selectionSection.selectedItemsIndexes addObject:index];
            }

        }

        selectionSection.boundObjectStore = self.boundObjectStore;
        selectionSection.allowNoSelection = self.allowNoSelection;
        selectionSection.maximumSelections = self.maximumSelections;
        selectionSection.allowMultipleSelection = self.allowMultipleSelection;
        selectionSection.autoDismissViewController = self.autoDismissDetailView;
        selectionSection.cellsImageViews = self.detailCellsImageViews;
        
        selectionSection.allowAddingItems = self.allowAddingItems;
        selectionSection.allowDeletingItems = self.allowDeletingItems;
        selectionSection.allowMovingItems = self.allowMovingItems;
        selectionSection.allowEditDetailView = self.allowEditDetailView;
        
        selectionSection.placeholderCell = self.placeholderCell;
        selectionSection.addNewItemCell = self.addNewItemCell;
        selectionSection.addNewItemCellExistsInNormalMode = self.addNewItemCellExistsInNormalMode;
        selectionSection.addNewItemCellExistsInEditingMode = self.addNewItemCellExistsInEditingMode;
        
        [selectionSection setDetailViewControllerOptions:self.detailViewControllerOptions];
        
        [detailModel addSection:selectionSection];
    }

}

- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
    _commitingDetailModel = TRUE;
    
    // The detail model my have added/modified/removed items
    itemsInSync = FALSE;
    
    NSSet *selectedIndexes = nil;
    if ([detailModel isKindOfClass:[SCSelectionModel class]]) {
        selectedIndexes = [(SCSelectionModel *)detailModel selectedItemsIndexes];
    }

    else {
        for(NSUInteger i=0; i<detailModel.sectionCount; i++)
        {
            SCTableViewSection *section = [detailModel sectionAtIndex:i];
            if ([section isKindOfClass:[SCSelectionSection class]]) {
                selectedIndexes = [(SCSelectionSection *)section selectedItemsIndexes];
                break;
            }

        }
    }

    if (selectedIndexes) {
        [self.selectedItemsIndexes removeAllObjects];
        for(NSNumber *index in selectedIndexes) {
            [self.selectedItemsIndexes addObject:index];
        }

        
        [self cellValueChanged];
    }

    _commitingDetailModel = FALSE;
}

//override superclass
- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType
{
    if (self.detailViewControllerOptions.navigationBarType != SCNavigationBarTypeAuto) {
        return self.detailViewControllerOptions.navigationBarType;
    }


    SCNavigationBarType navBarType;
    if (self.allowAddingItems || self.allowDeletingItems || self.allowMovingItems || self.allowEditDetailView) {
        navBarType = SCNavigationBarTypeEditRight;
    }

    else {
        navBarType = SCNavigationBarTypeDoneRightCancelLeft;
    }

    return navBarType;
}

//override superclass
- (void)didSelectCell
{
    [super didSelectCell];
    
	self.ownerTableViewModel.activeCell = self;

    if (!self.items) {
        return;
    }

    if (_loadingContents) {
        return;
    }

    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    
    UIViewController *detailViewController = [self generatedDetailViewController:indexPath];
    
    
    [self presentDetailViewController:detailViewController forCell:self forRowAtIndexPath:indexPath withPresentationMode:self.detailViewControllerOptions.presentationMode];
}

// overrides superclass
- (UIViewController *)generatedDetailViewController:(NSIndexPath *)indexPath
{
    UIViewController *detailViewController = nil;
    if ([self.ibDetailViewControllerIdentifier length]) {
        detailViewController = [SCUtilities instantiateViewControllerWithIdentifier:self.ibDetailViewControllerIdentifier usingStoryboard:self.ownerTableViewModel.viewController.storyboard];

        if (detailViewController) {
            [self configureDetailViewController:detailViewController];
            SCTableViewModel *detailModel = [self modelForViewController:detailViewController];
            [self.ownerTableViewModel configureDetailModel:detailModel];
            [self configureDetailModel:detailModel];
        }

        else {
            SCDebugLog(@"Warning: Could not instantiate view controller with id '%@' from Storyboard.", self.ibDetailViewControllerIdentifier);
        }

    }

    else {
        detailViewController = [self getDetailViewControllerForCell:self forRowAtIndexPath:indexPath allowUITableViewControllerSubclass:YES];
    }

    return detailViewController;
}

// overrides superclass
- (BOOL)generatesDetailView
{
    return YES;
}

// overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    if (self.boundPropertyDataType == SCDataTypeNSNumber) {
        self.boundValue = self.selectedItemIndex;
    }

    else if (self.boundPropertyDataType==SCDataTypeNSString || self.boundPropertyDataType==SCDataTypeDictionaryItem) {
        self.boundValue = [self buildStringFromSelectedItemsIndexes];
    }

    else {
        if ([self.boundValue isKindOfClass:[NSMutableSet class]]) {
            NSMutableSet *boundValueSet = (NSMutableSet *)self.boundValue;
            [boundValueSet removeAllObjects];
            for(NSNumber *index in self.selectedItemsIndexes) {
                [boundValueSet addObject:index];
            }

        }

    }

    [super commitChanges];
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCSelectionAttributes class]]) {
        return;
    }

    SCSelectionAttributes *selectionAttributes = (SCSelectionAttributes *)attributes;

    if (selectionAttributes.selectionItemsStore) {
        self.selectionItemsStore = selectionAttributes.selectionItemsStore;	
        self.selectionItemsFetchOptions = selectionAttributes.selectionItemsFetchOptions;
    }

    self.allowMultipleSelection = selectionAttributes.allowMultipleSelection;
	self.allowNoSelection = selectionAttributes.allowNoSelection;
	self.maximumSelections = selectionAttributes.maximumSelections;
	self.autoDismissDetailView = selectionAttributes.autoDismissDetailView;
	self.hideDetailViewNavigationBar = selectionAttributes.hideDetailViewNavigationBar;
    self.allowAddingItems = selectionAttributes.allowAddingItems;
    self.allowDeletingItems = selectionAttributes.allowDeletingItems;
    self.allowMovingItems = selectionAttributes.allowMovingItems;
    self.allowEditDetailView = selectionAttributes.allowEditingItems;
    if ([selectionAttributes.placeholderuiElement isKindOfClass:[SCTableViewCell class]]) {
        self.placeholderCell = (SCTableViewCell *)selectionAttributes.placeholderuiElement;
    }

    if ([selectionAttributes.addNewObjectuiElement isKindOfClass:[SCTableViewCell class]]) {
        self.addNewItemCell = (SCTableViewCell *)selectionAttributes.addNewObjectuiElement;
    }

}

//overrides superclass
- (BOOL)getValueIsValid
{
    if (![self.selectedItemsIndexes count] && !self.allowNoSelection && self.valueRequired) {
        return FALSE;
    }

    //else
	return TRUE;
}

- (void)setSelectedItemIndex:(NSNumber *)number
{
	[self.selectedItemsIndexes removeAllObjects];
    if ([number intValue] >= 0) {
        NSNumber *num = [number copy];
        [self.selectedItemsIndexes addObject:num];
    }

}

- (NSNumber *)selectedItemIndex
{
	NSNumber *index = [self.selectedItemsIndexes anyObject];

    if (index) {
        return index;
    }

    //else
	return [NSNumber numberWithInt:-1];
}


- (void)handleDetailViewControllerWillPresent:(UIViewController *)detailViewController
{
    if (self.autoDismissDetailView && self.hideDetailViewNavigationBar) {
        [self.ownerTableViewModel.viewController.navigationController setNavigationBarHidden:YES animated:YES];
    }

    [super handleDetailViewControllerWillPresent:detailViewController];
}

- (void)handleDetailViewControllerWillDismiss:(UIViewController *)detailViewController cancelButtonTapped:(BOOL)cancelTapped doneButtonTapped:(BOOL)doneTapped
{
	[self.ownerTableViewModel.viewController.navigationController setNavigationBarHidden:FALSE animated:YES];
	
	[super handleDetailViewControllerWillDismiss:detailViewController cancelButtonTapped:cancelTapped doneButtonTapped:doneTapped];	
}

@end




#pragma mark - SCObjectSelectionCell




@interface SCObjectSelectionCell ()

- (NSMutableSet *)boundMutableSet;

@end


@implementation SCObjectSelectionCell

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName selectionItemsStore:(SCDataStore *)store
{
    return [[[self class] alloc] initWithText:cellText boundObject:object selectedObjectPropertyName:propertyName selectionItemsStore:store];
}

+ (instancetype)cellWithText:(NSString *)cellText boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName selectionItems:(NSArray *)items itemsDefintion:(SCDataDefinition *)definition
{
	return [[[self class] alloc] initWithText:cellText boundObject:object selectedObjectPropertyName:propertyName selectionItems:items itemsDefintion:definition];
}

//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
    
    _intermediateEntityDefinition = nil;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName selectionItemsStore:(SCDataStore *)store
{
    if ( (self=[self initWithText:cellText boundObject:object boundPropertyName:propertyName]) ) {
        self.selectionItemsStore = store;
        
        [self buildSelectedItemsIndexesFromBoundValue];
    }

    return self;
}

- (instancetype)initWithText:(NSString *)cellText boundObject:(NSObject *)object selectedObjectPropertyName:(NSString *)propertyName selectionItems:(NSArray *)selitems itemsDefintion:(SCDataDefinition *)definition
{
	SCArrayStore *store = [SCArrayStore storeWithObjectsArray:[NSMutableArray arrayWithArray:selitems] defaultDefiniton:definition];
    
    self = [self initWithText:cellText boundObject:object selectedObjectPropertyName:propertyName selectionItemsStore:store];
    
    return self;
}


- (NSMutableSet *)boundMutableSet
{
    if (self.boundPropertyDataType == SCDataTypeNSString) {
        return nil;
    }

    NSMutableSet *set = nil;
    if ([self.boundObject respondsToSelector:@selector(mutableSetValueForKey:)]) {
        SCPropertyDefinition *boundPropertyDef = [[self.boundObjectStore definitionForObject:self.boundObject] propertyDefinitionWithName:self.boundPropertyName];

        if (boundPropertyDef.dataType == SCDataTypeNSMutableSet) {
            set = [self.boundObject mutableSetValueForKey:self.boundPropertyName];
        }

        else if (boundPropertyDef.dataType == SCDataTypeNSMutableOrderedSet) {
            set = (NSMutableSet *)[self.boundObject mutableOrderedSetValueForKey:self.boundPropertyName];
        }

    }

    else {
        set = (NSMutableSet *)self.boundValue;
    }

    return set;
}

//overrides superclass
- (void)buildSelectedItemsIndexesFromBoundValue
{
    [self.selectedItemsIndexes removeAllObjects];

    if (self.boundPropertyDataType == SCDataTypeNSString) {
        NSString *stringBoundValue = (NSString *)self.boundValue;
        NSArray *objectTitles = [stringBoundValue componentsSeparatedByString:@";"];
        SCDataDefinition *definition = [self.selectionItemsStore defaultDataDefinition];
        for(NSString *title in objectTitles)
        {
            NSObject *obj = [definition objectWithTitle:title inObjectsArray:self.items];
            NSUInteger index = [self.items indexOfObjectIdenticalTo:obj];
            if (index != NSNotFound) {
                [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
            }

        }
        
        return;
    }


    if (self.allowMultipleSelection) {
        if (!self.intermediateEntityDefinition) {
            NSMutableSet *boundSet = [self boundMutableSet];  //optimize
            for(NSObject *obj in boundSet)
            {
                NSUInteger index = [self.items indexOfObjectIdenticalTo:obj];
                if (index != NSNotFound) {
                    [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
                }

            }
        }

        else {
            // TODO - Future implementation

            /*
            NSEntityDescription *boundObjEntity = [(NSManagedObject *)self.boundObject entity];
            NSEntityDescription *intermediateEntity = self.intermediateEntityClassDefinition.entity;
            NSEntityDescription *itemsEntity = self.itemsClassDefinition.entity;
            
            // Determine the boundObjEntity relationship name that connects to intermediateEntity
            NSString *intermediatesRel = nil;
            NSArray *relationships = [boundObjEntity relationshipsWithDestinationEntity:intermediateEntity];
            if(relationships.count)
                intermediatesRel = [(NSRelationshipDescription *)[relationships objectAtIndex:0] name];
            
            // Determine the intermediateEntity relationship name that connects to itemsEntity
            NSString *itemRel = nil;
            relationships = [intermediateEntity relationshipsWithDestinationEntity:itemsEntity];
            if(relationships.count)
                itemRel = [(NSRelationshipDescription *)[relationships objectAtIndex:0] name];
            
            if(intermediatesRel && itemRel)
            {
                NSMutableSet *intermediatesSet = [(NSManagedObject *)self.boundObject mutableSetValueForKey:intermediatesRel];
                for(NSManagedObject *intermediateObj in intermediatesSet)
                {
                    NSManagedObject *itemObj = [intermediateObj valueForKey:itemRel];
                    int index = [self.items indexOfObjectIdenticalTo:itemObj];
                    if(index != NSNotFound)
                        [self.selectedItemsIndexes addObject:[NSNumber numberWithInt:index]];
                }
            }
             */
        }

    }

    else {
        NSObject *selectedObject = [SCUtilities valueForPropertyName:self.boundPropertyName inObject:self.boundObject]; 
        NSUInteger index = [self.items indexOfObjectIdenticalTo:selectedObject];
        if (index != NSNotFound) {
            [self.selectedItemsIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
        }

    }

}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCObjectSelectionAttributes class]]) {
        return;
    }

    SCObjectSelectionAttributes *objectSelectionAttributes = (SCObjectSelectionAttributes *)attributes;

    if ([objectSelectionAttributes.ibPlaceholderText length] && !objectSelectionAttributes.placeholderuiElement) {
        self.placeholderCell = [SCTableViewCell cellWithText:objectSelectionAttributes.ibPlaceholderText textAlignment:objectSelectionAttributes.ibPlaceholderTextAlignment];
    }

    if ([objectSelectionAttributes.ibAddNewObjectText length] && !objectSelectionAttributes.addNewObjectuiElement) {
        self.addNewItemCell = [SCTableViewCell cellWithText:objectSelectionAttributes.ibAddNewObjectText textAlignment:NSTextAlignmentCenter];
    }

    self.intermediateEntityDefinition = objectSelectionAttributes.intermediateEntityDefinition;
    
    [self buildSelectedItemsIndexesFromBoundValue];
}

// override superclass
- (NSString *)getTitleForItemAtIndex:(NSUInteger)index
{
    NSObject *item = [self.items objectAtIndex:index];
    SCDataDefinition *itemDefinition = [self.selectionItemsStore definitionForObject:item];
    if (itemDefinition.titlePropertyName) {
        return [itemDefinition titleValueForObject:[self.items objectAtIndex:index]];
    }

    //else
	return nil;
}

// override superclass
- (void)buildDetailModel:(SCTableViewModel *)detailModel
{
    [detailModel clear];

    if ([detailModel isKindOfClass:[SCObjectSelectionModel class]]) {
        SCObjectSelectionModel *selectionModel = (SCObjectSelectionModel *)detailModel;
        selectionModel.autoFetchItems = FALSE;
        [selectionModel setMutableItems:[NSMutableArray arrayWithArray:self.items]];
        
        // Override object's bound value since it might not yet be committed
        [selectionModel.selectedItemsIndexes removeAllObjects];
        for(NSNumber *index in self.selectedItemsIndexes) {
            [selectionModel.selectedItemsIndexes addObject:index];
        }

        
        selectionModel.boundObject = self.boundObject;
        selectionModel.boundObjectStore = self.boundObjectStore;
        selectionModel.boundPropertyName = self.boundPropertyName;
        selectionModel.dataStore = self.selectionItemsStore;
        selectionModel.dataFetchOptions = self.selectionItemsFetchOptions;
        
        selectionModel.allowNoSelection = self.allowNoSelection;
        selectionModel.maximumSelections = self.maximumSelections;
        selectionModel.allowMultipleSelection = self.allowMultipleSelection;
        selectionModel.autoDismissViewController = self.autoDismissDetailView;
        
        selectionModel.allowAddingItems = self.allowAddingItems;
        selectionModel.allowDeletingItems = self.allowDeletingItems;
        selectionModel.allowMovingItems = self.allowMovingItems;
        selectionModel.allowEditDetailView = self.allowEditDetailView;
        
        [selectionModel setDetailViewControllerOptions:self.detailViewControllerOptions];
    }

    else {
        SCObjectSelectionSection *selectionSection = [SCObjectSelectionSection sectionWithHeaderTitle:nil boundObject:self.boundObject selectedObjectPropertyName:self.boundPropertyName selectionItemsStore:self.selectionItemsStore];
        selectionSection.dataFetchOptions = self.selectionItemsFetchOptions;
        selectionSection.autoFetchItems = FALSE;
        [selectionSection setMutableItems:[NSMutableArray arrayWithArray:self.items]];
        
        selectionSection.intermediateEntityDefinition = self.intermediateEntityDefinition;
        
        // Override object's bound value since it might not yet be committed
        [selectionSection.selectedItemsIndexes removeAllObjects];
        for(NSNumber *index in self.selectedItemsIndexes) {
            [selectionSection.selectedItemsIndexes addObject:index];
        }

        
        selectionSection.boundObjectStore = self.boundObjectStore;
        selectionSection.commitCellChangesLive = FALSE;
        selectionSection.allowNoSelection = self.allowNoSelection;
        selectionSection.maximumSelections = self.maximumSelections;
        selectionSection.allowMultipleSelection = self.allowMultipleSelection;
        selectionSection.autoDismissViewController = self.autoDismissDetailView;
        selectionSection.cellsImageViews = self.detailCellsImageViews;
        
        selectionSection.allowAddingItems = self.allowAddingItems;
        selectionSection.allowDeletingItems = self.allowDeletingItems;
        selectionSection.allowMovingItems = self.allowMovingItems;
        selectionSection.allowEditDetailView = self.allowEditDetailView;
        
        selectionSection.placeholderCell = self.placeholderCell;
        selectionSection.addNewItemCell = self.addNewItemCell;
        selectionSection.addNewItemCellExistsInNormalMode = self.addNewItemCellExistsInNormalMode;
        selectionSection.addNewItemCellExistsInEditingMode = self.addNewItemCellExistsInEditingMode;
        
        [selectionSection setDetailViewControllerOptions:self.detailViewControllerOptions];
        
        [detailModel addSection:selectionSection];
    }

}

// overrides superclass
- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
    _commitingDetailModel = TRUE;
    
    // The detail model my have added/modified/removed items
    itemsInSync = FALSE;
    
    NSSet *detailIndexes;
    if ([detailModel isKindOfClass:[SCObjectSelectionModel class]]) {
        detailIndexes = [(SCObjectSelectionModel *)detailModel selectedItemsIndexes];
    }

    else {
        SCObjectSelectionSection *selectionSection = (SCObjectSelectionSection *)[detailModel sectionAtIndex:0];
        detailIndexes = selectionSection.selectedItemsIndexes;
    }

    [self.selectedItemsIndexes removeAllObjects];
	for(NSNumber *index in detailIndexes) {
		[self.selectedItemsIndexes addObject:index];
	}

    
    [self cellValueChanged];
    
    _commitingDetailModel = FALSE;
}

// overrides superclass
- (void)commitChanges
{
    if (!self.needsCommit || !self.valueIsValid) {
        return;
    }

    if (self.boundPropertyDataType == SCDataTypeNSString) {
        SCDataDefinition *definition = [self.selectionItemsStore defaultDataDefinition];
        
        NSMutableString *objectTitles = [NSMutableString string];
        BOOL addSeparator = FALSE;
        for(NSNumber *index in self.selectedItemsIndexes)
        {
            NSObject *obj = [self.items objectAtIndex:(NSUInteger)[index intValue]];
            NSString *objTitle = [definition titleValueForObject:obj];
            if (!addSeparator) {
                [objectTitles appendString:objTitle];
                addSeparator = TRUE;
            }

            else {
                [objectTitles appendFormat:@";%@", objTitle];
            }

        }
        
        self.boundValue = objectTitles;
        
        return;
    }

    if (self.allowMultipleSelection) {
        if (!self.intermediateEntityDefinition) {
            NSMutableSet *boundValueSet = [self boundMutableSet];
            [boundValueSet removeAllObjects];
            for(NSNumber *index in self.selectedItemsIndexes)
            {
                NSObject *obj = [self.items objectAtIndex:(NSUInteger)[index intValue]];
                [boundValueSet addObject:obj];
            }
        }

        else {
            // TODO - Future implementation
#ifdef _COREDATADEFINES_H 
            /*
            NSEntityDescription *boundObjEntity = [(NSManagedObject *)self.boundObject entity];
            NSEntityDescription *intermediateEntity = self.intermediateEntityDefinition.entity;
            NSEntityDescription *itemsEntity = self.itemsClassDefinition.entity;
            
            // Determine the boundObjEntity relationship name that connects to intermediateEntity
            NSString *intermediatesRel = nil;
            NSArray *relationships = [boundObjEntity relationshipsWithDestinationEntity:intermediateEntity];
            if(relationships.count)
                intermediatesRel = [(NSRelationshipDescription *)[relationships objectAtIndex:0] name];
            
            // Determine the intermediateEntity relationship name that connects to itemsEntity
            NSString *itemRel = nil;
            NSString *invItemRel = nil;
            relationships = [intermediateEntity relationshipsWithDestinationEntity:itemsEntity];
            if(relationships.count)
            {
                itemRel = [(NSRelationshipDescription *)[relationships objectAtIndex:0] name];
                invItemRel = [[(NSRelationshipDescription *)[relationships objectAtIndex:0] inverseRelationship] name];
            }
                
            
            if(intermediatesRel && itemRel && invItemRel)
            {
                NSMutableSet *intermediatesSet = [(NSManagedObject *)self.boundObject mutableSetValueForKey:intermediatesRel];
                
                // remove all intermediate objects
                for(NSManagedObject *intermediateObj in intermediatesSet)
                {
                    [self.intermediateEntityClassDefinition.managedObjectContext deleteObject:intermediateObj];
                }
                
                // add new intermediate objects
                for(NSNumber *index in self.selectedItemsIndexes)
                {
                    NSManagedObject *itemObj = [self.items objectAtIndex:[index intValue]];
                    
                    NSManagedObject *intermediateObj = [NSEntityDescription insertNewObjectForEntityForName:[intermediateEntity name] inManagedObjectContext:self.intermediateEntityClassDefinition.managedObjectContext];
                    [intermediatesSet addObject:intermediateObj];
                    [[itemObj mutableSetValueForKey:invItemRel] addObject:intermediateObj];
                }
            }
             */
#endif
        }

    }

    else {
        NSObject *selectedObject = nil;
        NSInteger index = [self.selectedItemIndex intValue];
        if (index >= 0) {
            selectedObject = [self.items objectAtIndex:index];
        }

        self.boundValue = selectedObject;
    }

}

@end





#pragma mark - SCObjectCell





@interface SCObjectCell ()

- (void)setCellTextAndDetailText;

@end



@implementation SCObjectCell

+ (instancetype)cellWithBoundObject:(NSObject *)object
{
	return [[[self class] alloc] initWithBoundObject:object];
}

+ (instancetype)cellWithBoundObject:(NSObject *)object boundObjectDefinition:(SCDataDefinition *)definition
{
	return [[[self class] alloc] initWithBoundObject:object boundObjectDefinition:definition];
}

+ (instancetype)cellWithBoundObject:(NSObject *)object boundObjectStore:(SCDataStore *)store
{
    return [[[self class] alloc] initWithBoundObject:object boundObjectStore:store];
}

//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
	_boundObjectTitleText = nil;
	self.detailViewControllerOptions.tableViewStyle = UITableViewStyleGrouped;
	
	self.selectionStyle = UITableViewCellSelectionStyleBlue;
}

- (instancetype)initWithBoundObject:(NSObject *)object
{
	return [self initWithBoundObject:object boundObjectDefinition:nil];
}

- (instancetype)initWithBoundObject:(NSObject *)object boundObjectDefinition:(SCDataDefinition *)definition
{
	SCDataStore *store = [definition generateCompatibleDataStore];
    
    return [self initWithBoundObject:object boundObjectStore:store];
}

- (instancetype)initWithBoundObject:(NSObject *)object boundObjectStore:(SCDataStore *)store
{
    if ( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) ) {
        self.boundObject = object;
		
        self.boundObjectStore = store;
    }

    return self;
}


- (SCDataDefinition *)objectDefinition
{
    return [self.boundObjectStore definitionForObject:self.boundObject];
}

- (void)setObjectDefinition:(SCDataDefinition *)objectDefinition
{
    [self.boundObjectStore setDefaultDataDefinition:objectDefinition];
}

- (void)setBoundObjectTitleText: (NSString*)input 
{ 
    _boundObjectTitleText = [input copy];
    
    [self setCellTextAndDetailText];
}

//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];

    if (_enabled && self.boundObject) {
        self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    else {
        self.accessoryType = UITableViewCellAccessoryNone;
    }

}

//override superclass
- (void)willDisplay
{
	[super willDisplay];

    if (self.boundObject && self.enabled) {
        self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    else {
        self.accessoryType = UITableViewCellAccessoryNone;
    }

    [self setCellTextAndDetailText];
}

//override superclass
- (void)cellValueChanged
{
	[self setCellTextAndDetailText];
	
	[super cellValueChanged];
}

- (void)buildDetailModel:(SCTableViewModel *)detailModel
{
    [detailModel clear];

    if (self.boundObjectStore) {
        [detailModel generateSectionsForObject:self.boundObject withDataStore:self.boundObjectStore newObject:NO];
    }

}

- (void)configureDetailModel:(SCTableViewModel *)detailModel
{
    [super configureDetailModel:detailModel];
    
    for(NSUInteger i=0; i<detailModel.sectionCount; i++)
    {
        SCTableViewSection *section = [detailModel sectionAtIndex:i];
        
        [section setDetailViewControllerOptions:self.detailViewControllerOptions];
        section.commitCellChangesLive = FALSE;
        section.cellsImageViews = self.detailCellsImageViews;
        for(NSUInteger j=0; j<section.cellCount; j++)
        {
            [[section cellAtIndex:j] setDetailViewControllerOptions:self.detailViewControllerOptions];
        }
    }
}

- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
	// commitChanges & ignore self.commitChangesLive setting as it's not applicable here
	//looping to include any custom user added sections too
	for(NSUInteger i=0; i<detailModel.sectionCount; i++)
	{
		SCTableViewSection *section = [detailModel sectionAtIndex:i];
		[section commitCellChanges];
	}
	
	[self cellValueChanged];
}

//override superclass
- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType
{
    SCNavigationBarType navBarType;
    if (self.objectDefinition.requireEditingModeToEditPropertyValues) {
        navBarType = SCNavigationBarTypeEditRight;
    }

    else {
        navBarType = SCNavigationBarTypeDoneRightCancelLeft;
    }

    return navBarType;
}

//override superclass
- (void)didSelectCell
{
    [super didSelectCell];
    
	self.ownerTableViewModel.activeCell = self;

    if (!self.boundObject) {
        return;
    }

    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    
    UIViewController *detailViewController = [self generatedDetailViewController:indexPath];
    
    [self presentDetailViewController:detailViewController forCell:self forRowAtIndexPath:indexPath withPresentationMode:self.detailViewControllerOptions.presentationMode];
}

// overrides superclass
- (UIViewController *)generatedDetailViewController:(NSIndexPath *)indexPath
{
    UIViewController *detailViewController;
    if ([self.ibDetailViewControllerIdentifier length]) {
        detailViewController = [SCUtilities instantiateViewControllerWithIdentifier:self.ibDetailViewControllerIdentifier usingStoryboard:self.ownerTableViewModel.viewController.storyboard];

        if (detailViewController) {
            [self configureDetailViewController:detailViewController];
            SCTableViewModel *detailModel = [self modelForViewController:detailViewController];
            [self.ownerTableViewModel configureDetailModel:detailModel];
            [self configureDetailModel:detailModel];
        }

        else {
            SCDebugLog(@"Warning: Could not instantiate view controller with id '%@' from Storyboard.", self.ibDetailViewControllerIdentifier);
        }

    }

    else {
        detailViewController = [self getDetailViewControllerForCell:self forRowAtIndexPath:indexPath allowUITableViewControllerSubclass:YES];
    }

    return detailViewController;
}


// overrides superclass
- (BOOL)generatesDetailView
{
    return YES;
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCObjectAttributes class]]) {
        return;
    }

    __unused SCObjectAttributes *objectAttributes = (SCObjectAttributes *)attributes;
	// No assignments currently needed.
    // Placeholder for future assignments.
}

- (void)setCellTextAndDetailText
{
    if (self.boundObjectTitleText) {
        self.textLabel.text = self.boundObjectTitleText;
    }

    else {
        if (self.boundObject && self.objectDefinition.titlePropertyName) {
            self.textLabel.text = [self.objectDefinition titleValueForObject:self.boundObject];
        }

    }

    if (self.boundObject && self.objectDefinition.descriptionPropertyName) {
        self.detailTextLabel.text = [SCUtilities stringValueForPropertyName:self.objectDefinition.descriptionPropertyName
																inObject:self.boundObject
											separateValuesUsingDelimiter:@" "];
    }

}


@end





#pragma mark - SCArrayOfObjectsCell



@implementation SCArrayOfObjectsCell

+ (instancetype)cellWithDataStore:(SCDataStore *)store
{
    return [[[self class] alloc] initWithDataStore:store];
}

+ (instancetype)cellWithItems:(NSMutableArray *)cellItems itemsDefinition:(SCDataDefinition *)definition
{
	return [[[self class] alloc] initWithItems:cellItems itemsDefinition:definition];
}

+ (instancetype)cellWithEntityDefinition:(SCEntityDefinition *)definition
{
	return [[[self class] alloc] initWithEntityDefinition:definition];
}

+ (instancetype)cellWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects
{
	return [[[self class] alloc] initWithBoundItemsSet:cellItemsSet boundSetEntityDefinition:definition boundSetOwnsObjects:ownsObjects];
}

//overrides superclass
- (void)performInitialization
{
	[super performInitialization];
	
    _dataStore = nil;
    _dataFetchOptions = nil;  // will be re-initialized when dataStore is set
    
	_allowAddingItems = TRUE;
	_allowDeletingItems = TRUE;
	_allowMovingItems = TRUE;
	_allowEditDetailView = TRUE;
	_allowRowSelection = TRUE;
	_autoSelectNewItemCell = FALSE;
	_displayItemsCountInBadgeView = TRUE;
    
    _placeholderCell = nil;
    _addNewItemCell = nil;
    _addNewItemCellExistsInNormalMode = TRUE;
    _addNewItemCellExistsInEditingMode = TRUE;
    
    _detailSectionActions = [[SCSectionActions alloc] init];
}

- (instancetype)initWithDataStore:(SCDataStore *)store
{
    if ( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) ) {
        self.dataStore = store;
    }

    return self;
}

- (instancetype)initWithItems:(NSMutableArray *)cellItems itemsDefinition:(SCDataDefinition *)definition
{
    if ( (self=[self initWithStyle:SC_DefaultCellStyle reuseIdentifier:nil]) ) {
        self.dataStore = [SCArrayStore storeWithObjectsArray:cellItems defaultDefiniton:definition];
        [self.dataStore addDataDefinition:definition];
    }

    return self;
}

- (instancetype)initWithEntityDefinition:(SCEntityDefinition *)definition
{
	SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext defaultEntityDefinition:definition];

	self = [self initWithDataStore:store];

    return self;
}

- (instancetype)initWithBoundItemsSet:(NSMutableSet *)cellItemsSet boundSetEntityDefinition:(SCEntityDefinition *)definition boundSetOwnsObjects:(BOOL)ownsObjects
{
	SCCoreDataStore *store = [SCCoreDataStore storeWithManagedObjectContext:definition.managedObjectContext boundSet:cellItemsSet boundSetEntityDefinition:definition boundSetOwnsStoreObjects:ownsObjects];

	self = [self initWithDataStore:store];

    return self;
}


// overrides superclass
- (void)setBoundObject:(NSObject *)object
{
    [super setBoundObject:object];
    
    [self bindDataStoreToBoundObject];
}

// overrides superclass
- (void)setBoundObjectStore:(SCDataStore *)objectStore
{
    [super setBoundObjectStore:objectStore];
    
    [self bindDataStoreToBoundObject];
}

// overrides superclass
- (void)setBoundPropertyName:(NSString *)propertyName
{
    [super setBoundPropertyName:propertyName];
    
    [self bindDataStoreToBoundObject];
}

- (void)setDataStore:(SCDataStore *)store
{
    _dataStore = store;

    if (!_dataFetchOptions) {
        _dataFetchOptions = [store.defaultDataDefinition generateCompatibleDataFetchOptions];
    }

    [self bindDataStoreToBoundObject];
}

- (void)bindDataStoreToBoundObject
{
    if (self.boundObject && self.boundObjectStore && self.boundPropertyName && self.dataStore) {
        [self.dataStore bindStoreToPropertyName:self.boundPropertyName forObject:self.boundObject withDefinition:[self.boundObjectStore definitionForObject:self.boundObject]];
    }

}


//override superclass
- (void)layoutSubviews
{
    if (self.displayItemsCountInBadgeView) {
        if (self.dataStore.storeMode==SCStoreModeSynchronous) {
            self.badgeView.text = [NSString stringWithFormat:@"%i", (int)self.items.count];
        }

        else {
            self.badgeView.text = @"-";
            
            typeof(self) weak_self = self;
            [self.dataStore asynchronousFetchObjectsWithOptions:self.dataFetchOptions
                success:^(NSArray *results)
                {
                    weak_self.badgeView.text = [NSString stringWithFormat:@"%i", (int)results.count];
                }
                failure:nil noConnection:nil];
        }

    }

    [super layoutSubviews];
}

//overrides superclass
- (void)buildDetailModel:(SCTableViewModel *)detailModel
{
    [detailModel clear];

    if ([detailModel isKindOfClass:[SCArrayOfObjectsModel class]]) {
        SCArrayOfObjectsModel *objectsModel = (SCArrayOfObjectsModel *)detailModel;
        
        objectsModel.dataStore = self.dataStore;
        objectsModel.allowAddingItems = self.allowAddingItems;
        objectsModel.allowDeletingItems = self.allowDeletingItems;
        objectsModel.allowMovingItems = self.allowMovingItems;
        objectsModel.allowEditDetailView = self.allowEditDetailView;
        objectsModel.allowRowSelection = self.allowRowSelection;
        objectsModel.autoSelectNewItemCell = self.autoSelectNewItemCell;
        if ([detailModel.viewController isKindOfClass:[SCViewController class]]) {
            objectsModel.addButtonItem = [(SCViewController *)detailModel.viewController addButton];
        }

        else if ([detailModel.viewController isKindOfClass:[SCTableViewController class]]) {
            objectsModel.addButtonItem = [(SCTableViewController *)detailModel.viewController addButton];
        }

        [objectsModel setDetailViewControllerOptions:self.detailViewControllerOptions];
    }

    else {
        SCArrayOfObjectsSection *objectsSection = [SCArrayOfObjectsSection sectionWithHeaderTitle:nil dataStore:self.dataStore];
        
        objectsSection.boundObjectStore = self.boundObjectStore;
        objectsSection.allowAddingItems = self.allowAddingItems;
        objectsSection.allowDeletingItems = self.allowDeletingItems;
        objectsSection.allowMovingItems = self.allowMovingItems;
        objectsSection.allowEditDetailView = self.allowEditDetailView;
        objectsSection.allowRowSelection = self.allowRowSelection;
        objectsSection.autoSelectNewItemCell = self.autoSelectNewItemCell;
        if ([detailModel.viewController isKindOfClass:[SCViewController class]]) {
            objectsSection.addButtonItem = [(SCViewController *)detailModel.viewController addButton];
        }

        else if ([detailModel.viewController isKindOfClass:[SCTableViewController class]]) {
            objectsSection.addButtonItem = [(SCTableViewController *)detailModel.viewController addButton];
        }

        objectsSection.cellsImageViews = self.detailCellsImageViews;
        objectsSection.placeholderCell = self.placeholderCell;
        objectsSection.addNewItemCell = self.addNewItemCell;
        objectsSection.addNewItemCellExistsInNormalMode = self.addNewItemCellExistsInNormalMode;
        objectsSection.addNewItemCellExistsInEditingMode = self.addNewItemCellExistsInEditingMode;
        [objectsSection setDetailViewControllerOptions:self.detailViewControllerOptions];
        
        [objectsSection.sectionActions setActionsTo:self.detailSectionActions overrideExisting:YES];
        
        [detailModel addSection:objectsSection];
    }

}

- (void)commitDetailModelChanges:(SCTableViewModel *)detailModel
{
	[self cellValueChanged];
}

//overrides superclass
- (void)setEnabled:(BOOL)_enabled
{
    [super setEnabled:_enabled];
    
    [self determineAccessoryType];
}

//override superclass
- (void)willDisplay
{
	[super willDisplay];
	
    [self determineAccessoryType];
}

- (void)determineAccessoryType
{
    if (self.enabled && (self.dataStore.storeMode==SCStoreModeAsynchronous || self.items)) {
        self.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    else {
        self.accessoryType = UITableViewCellAccessoryNone;
    }

}

//override superclass
- (SCNavigationBarType)defaultDetailViewControllerNavigationBarType
{
    SCNavigationBarType navBarType;
    if (!self.allowAddingItems && !self.allowDeletingItems && !self.allowMovingItems) {
        navBarType = SCNavigationBarTypeNone;
    }

    else {
        if (self.allowAddingItems && !self.addNewItemCell) {
            navBarType = SCNavigationBarTypeAddEditRight;
        }

        else {
            navBarType = SCNavigationBarTypeEditRight;
        }

    }

    return navBarType;
}

//override superclass
- (void)didSelectCell
{
    [super didSelectCell];
    
	self.ownerTableViewModel.activeCell = self;
	
	// If table is in edit mode, just display the bound object's detail view
    if (self.editing) {
        [super didSelectCell];
        return;
    }

    NSIndexPath *indexPath = [self.ownerTableViewModel indexPathForCell:self];
    
    UIViewController *detailViewController = [self generatedDetailViewController:indexPath];
    
    
    [self presentDetailViewController:detailViewController forCell:self forRowAtIndexPath:indexPath withPresentationMode:self.detailViewControllerOptions.presentationMode];
}

// overrides superclass
- (UIViewController *)generatedDetailViewController:(NSIndexPath *)indexPath
{
    UIViewController *detailViewController;
    if ([self.ibDetailViewControllerIdentifier length]) {
        detailViewController = [SCUtilities instantiateViewControllerWithIdentifier:self.ibDetailViewControllerIdentifier usingStoryboard:self.ownerTableViewModel.viewController.storyboard];

        if (detailViewController) {
            [self configureDetailViewController:detailViewController];
            SCTableViewModel *detailModel = [self modelForViewController:detailViewController];
            [self.ownerTableViewModel configureDetailModel:detailModel];
            [self configureDetailModel:detailModel];
        }

        else {
            SCDebugLog(@"Warning: Could not instantiate view controller with id '%@' from Storyboard.", self.ibDetailViewControllerIdentifier);
        }

    }

    else {
        detailViewController = [self getDetailViewControllerForCell:self forRowAtIndexPath:indexPath allowUITableViewControllerSubclass:YES];
    }

    return detailViewController;
}


// overrides superclass
- (BOOL)generatesDetailView
{
    return YES;
}

//overrides superclass
- (void)setAttributesTo:(SCPropertyAttributes *)attributes
{
	[super setAttributesTo:attributes];

    if (![attributes isKindOfClass:[SCArrayOfObjectsAttributes class]]) {
        return;
    }

    SCArrayOfObjectsAttributes *objectsArrayAttributes = (SCArrayOfObjectsAttributes *)attributes;

    if (!self.dataStore) {
        self.dataStore = [objectsArrayAttributes.defaultObjectsDefinition generateCompatibleDataStore];
    }

    if (objectsArrayAttributes.objectsFetchOptions) {
        self.dataFetchOptions = objectsArrayAttributes.objectsFetchOptions;
    }

    self.allowAddingItems = objectsArrayAttributes.allowAddingItems;
	self.allowDeletingItems = objectsArrayAttributes.allowDeletingItems;
	self.allowMovingItems = objectsArrayAttributes.allowMovingItems;
	self.allowEditDetailView = objectsArrayAttributes.allowEditingItems;
    if ([objectsArrayAttributes.placeholderuiElement isKindOfClass:[SCTableViewCell class]]) {
        self.placeholderCell = (SCTableViewCell *)objectsArrayAttributes.placeholderuiElement;
    }

    if ([objectsArrayAttributes.addNewObjectuiElement isKindOfClass:[SCTableViewCell class]]) {
        self.addNewItemCell = (SCTableViewCell *)objectsArrayAttributes.addNewObjectuiElement;
    }

    if ([objectsArrayAttributes.ibPlaceholderText length] && !objectsArrayAttributes.placeholderuiElement) {
        self.placeholderCell = [SCTableViewCell cellWithText:objectsArrayAttributes.ibPlaceholderText textAlignment:objectsArrayAttributes.ibPlaceholderTextAlignment];
    }

    if ([objectsArrayAttributes.ibAddNewObjectText length] && !objectsArrayAttributes.addNewObjectuiElement) {
        self.addNewItemCell = [SCTableViewCell cellWithText:objectsArrayAttributes.ibAddNewObjectText textAlignment:NSTextAlignmentCenter];
    }

    self.addNewItemCellExistsInNormalMode = objectsArrayAttributes.addNewObjectuiElementExistsInNormalMode;
    self.addNewItemCellExistsInEditingMode = objectsArrayAttributes.addNewObjectuiElementExistsInEditingMode;
    
    [self.detailSectionActions setActionsTo:objectsArrayAttributes.sectionActions overrideExisting:NO];
}

- (NSArray *)items
{
    return [self.dataStore fetchObjectsWithOptions:self.dataFetchOptions];
}

@end

















