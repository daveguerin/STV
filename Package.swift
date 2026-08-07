// swift-tools-version:5.9

import PackageDescription

let package = Package(
    name: "STV",
    platforms: [
        .iOS(.v16)
    ],
    products: [
        .library(
            name: "STV",
            targets: ["STV"]),
    ],
    dependencies: [
    ],
    targets: [
        .target(
            name: "STV",
            dependencies: [],
            path: ".",
            exclude: ["AGENTS.md","README.md","CHANGELOG.md","Documentation","License","Scripts"],
            sources: ["Sources"],
            publicHeadersPath: "Sources",
            cSettings: [.headerSearchPath("Sources")]
        ),
        .testTarget(
            name: "STVTests",
            dependencies: ["STV"],
            path: "Tests/STVTests",
            resources: [.process("TestResources")]
        )
    ]
)
