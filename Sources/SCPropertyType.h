/*
 *  SCPropertyType.h
 *  Sensible TableView
 *
 *  Copyright © 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <Foundation/Foundation.h>

/// The UI-generation choices for an `SCPropertyDefinition`.
///
/// STV resolves these choices using the definition's data type, read-only
/// state, visibility, attributes, and any custom cell configuration.
/// Object, array, and selection content can instead become inline sections
/// when the relevant attributes request expansion.
typedef NS_ENUM(NSInteger, SCPropertyType) {
    /// No normal-mode UI type. For `editingModeType`, inherits the normal type.
    SCPropertyTypeUndefined = 0,
    /// Infers a supported UI type from the property data type and read-only state.
    /// Unsupported data types do not generate a cell.
    SCPropertyTypeAutoDetect = 10,
    /// Requests an SCLabelCell interface element.
    SCPropertyTypeLabel = 20,
    /// Requests an SCTextViewCell interface element.
    SCPropertyTypeTextView = 30,
    /// Requests an SCTextFieldCell interface element.
    SCPropertyTypeTextField = 40,
    /// Requests an SCNumericTextFieldCell interface element.
    SCPropertyTypeNumericTextField = 50,
    /// Requests an SCSliderCell interface element.
    SCPropertyTypeSlider = 60,
    /// Requests an SCSegmentedCell interface element.
    SCPropertyTypeSegmented = 70,
    /// Requests an SCSwitchCell interface element.
    SCPropertyTypeSwitch = 80,
    /// Requests an SCDateCell interface element.
    SCPropertyTypeDate = 90,
    /*
     SCPropertyTypeImagePicker=100;
     Intentionally left unused while SCImagePickerCell is excluded from STV 7.
	*/
    /// Requests an SCSelectionCell interface element.
    SCPropertyTypeSelection = 110,
    /// Requests an SCObjectSelectionCell interface element.
    SCPropertyTypeObjectSelection = 120,
    /// Requests an SCObjectCell interface element.
    SCPropertyTypeObject = 130,
    /// Requests an SCArrayOfObjectsCell interface element.
    SCPropertyTypeArrayOfObjects = 140,
    /// Requests a custom cell, falling back to `SCCustomCell`.
    SCPropertyTypeCustom = 150,
    /// Suppresses generation of a cell for the property.
    SCPropertyTypeNone = 160
};
