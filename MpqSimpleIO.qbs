import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        consoleApplication: true
        files: "main.cpp"

        Properties {
            condition: qbs.targetOS.contains("macos")
            cpp.dynamicLibraries: ["bz2", "z"]
            cpp.includePaths: "."
            cpp.libraryPaths: "StormLib"
            cpp.staticLibraries: "StormLib"
        }

        Group { // Properties for the produced executable
            fileTagsFilter: "application"
        }
    }
}
