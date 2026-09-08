// Compile-only consumer: this file intentionally exercises Swift's imported types.
import STV
import UIKit

func verifySTVSwiftContracts() {
    let bindings: [String: String] = SCUtilities.bindingsDictionary(forBindingsString: nil)
    let encoded: String? = SCUtilities.bindingsString(forBindingsDictionary: bindings)
    let cell = SCTableViewCell()
    cell.boundObject = nil
    cell.boundPropertyName = nil
    cell.cellActions.calculatedValue = { _, _ in nil }
    cell.cellActions.leadingSwipeActions = { _, _ -> [UIContextualAction]? in nil }
    let images: [UIImageView]? = cell.detailCellsImageViews
    let custom = SCCustomCell()
    let controls: [UIView] = custom.inputControlsSortedByTag
    let model = SCTableViewModel()
    let titles: [String]? = model.sectionIndexTitles
    let controller = SCViewController()
    controller.delegate = nil
    controller.containedViewController = nil
    controller.popoverController = nil
    let definition = SCDataDefinition()
    let missing: SCPropertyDefinition? = definition.propertyDefinition(withName: "missing")
    let sortDescriptors: [NSSortDescriptor] = SCDataFetchOptions().sortDescriptors()
    let failure: SCDataStoreFailure_Block = { error in
        let optionalError: Error? = error
        _ = optionalError
    }
    let store = SCCoreDataStore()
    do {
        try store.commitDataWithError()
    } catch {
        let lastError: Error? = store.lastCommitError
        _ = lastError
    }
    _ = (encoded, images, controls, titles, missing, sortDescriptors, failure)
}
