#import "SCArrayStore.h"
#import "SCClassDefinition.h"
#import "SCCoreDataFetchOptions.h"
#import "SCCoreDataStore.h"
#import "SCDateDefinition.h"
#import "SCDictionaryDefinition.h"
#import "SCNumberDefinition.h"
#import "SCStringDefinition.h"
#import "SCUserDefaultsDefinition.h"
#import "SCUserDefaultsStore.h"

static void consumeDefinition(SCDataDefinition *_Nonnull definition) {
}
static void consumeProperty(SCPropertyDefinition *_Nonnull property) {
}
static void consumeString(NSString *_Nonnull string) {
}
static void consumeStore(SCDataStore *_Nonnull store) {
}
static void contracts(SCDataDefinition *_Nonnull definition, SCPropertyDefinition *_Nonnull property, SCCoreDataStore *_Nonnull coreStore) {
    [definition addPropertyDefinition:nil];                                     // expected-warning {{null passed to a callee that requires a non-null argument}}
    [SCClassDefinition definitionWithClass:NSObject.class propertyNames:@[@1]]; // expected-warning {{object of type 'NSNumber *' is not compatible with array element type 'NSString *'}}
    property.objectBindings = @{@"1": @1};                                      // expected-warning {{object of type 'NSNumber *' is not compatible with dictionary value type 'NSString *'}}
    [coreStore commitDataWithError:NULL];
    coreStore.commitFailure = nil;
    definition.titlePropertyName = nil;
    property.attributes = nil;
    property.title = nil;
    [SCArrayStore storeWithObjectsArray:nil defaultDefiniton:nil];
    [SCDataFetchOptions optionsWithSortKey:nil sortAscending:YES filterPredicate:nil];
    [SCNumericTextFieldAttributes attributesWithMinimumValue:nil maximumValue:nil allowFloatValue:YES placeholder:nil];
    [SCDateAttributes attributesWithDateFormatter:nil];
    SCDataStoreFailure_Block failure = ^(NSError *_Nullable error) {};
    failure(nil);
    consumeDefinition([coreStore definitionForObject:@"unknown"]); // expected-warning {{implicit conversion from nullable pointer}}
    consumeDefinition([SCDataDefinition new]);
    consumeProperty([definition propertyDefinitionWithName:@"missing"]); // expected-warning {{implicit conversion from nullable pointer}}
    consumeProperty([definition propertyDefinitionAtIndex:0]);
    consumeString(definition.dataStructureName);                    // expected-warning {{implicit conversion from nullable pointer}}
    consumeStore([definition generateCompatibleDataStore]);         // expected-warning {{implicit conversion from nullable pointer}}
    [SCSegmentedAttributes attributesWithSegmentTitlesArray:@[@1]]; // expected-warning {{object of type 'NSNumber *' is not compatible with array element type 'NSString *'}}
    NSArray<NSSortDescriptor *> *descriptors = [SCDataFetchOptions options].sortDescriptors;
    consumeString(descriptors[0].key); // expected-warning {{implicit conversion from nullable pointer}}
    consumeString([[SCPropertyGroup new] propertyNameAtIndex:0]);
}
