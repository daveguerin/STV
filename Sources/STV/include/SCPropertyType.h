/*
 *  SCPropertyType.h
 *  Sensible TableView
 *
 *  Copyright 2011-2015 Sensible Cocoa. All rights reserved.
 *
 *
 */

#import <Foundation/Foundation.h>

/** @enum The types of an SCPropertyDefinition */
typedef NS_ENUM(NSInteger, SCPropertyType)
{
    /**	Undefined property type */
    SCPropertyTypeUndefined=0,
	/** The object bound to the property will detect the best user interface element to generate. */
	SCPropertyTypeAutoDetect=10,
	/**	The object bound to the property will generate an SCLabelCell interface element */
	SCPropertyTypeLabel=20,
	/**	The object bound to the property will generate an SCTextViewCell interface element */
	SCPropertyTypeTextView=30,
	/**	The object bound to the property will generate an SCTextFieldCell interface element */
	SCPropertyTypeTextField=40,
	/**	The object bound to the property will generate an SCNumericTextFieldCell interface element */
	SCPropertyTypeNumericTextField=50,
	/**	The object bound to the property will generate an SCSliderCell interface element */
	SCPropertyTypeSlider=60,
	/**	The object bound to the property will generate an SCSegmentedCell interface element */
	SCPropertyTypeSegmented=70,
	/**	The object bound to the property will generate an SCSwitchCell interface element */
	SCPropertyTypeSwitch=80,
	/**	The object bound to the property will generate an SCDateCell interface element */
	SCPropertyTypeDate=90,
	/**	The object bound to the property will generate an SCImagePickerCell interface element */
	SCPropertyTypeImagePicker=100,
	/**	The object bound to the property will generate an SCSelectionCell interface element */
	SCPropertyTypeSelection=110,
	/**	The object bound to the property will generate an SCObjectSelectionCell interface element */
	SCPropertyTypeObjectSelection=120,
	/**	The object bound to the property will generate an SCObjectCell interface element */
	SCPropertyTypeObject=130,
	/**	The object bound to the property will generate an SCArrayOfObjectsCell interface element */
	SCPropertyTypeArrayOfObjects=140,
	/**	The object bound to the property will generate a custom interface element */
    SCPropertyTypeCustom=150,
    /**	The object bound to the property will not generate an interface element */
    SCPropertyTypeNone=160
};




