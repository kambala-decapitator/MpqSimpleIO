import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        consoleApplication: true
        files: "main.cpp"

        Properties {
            condition: qbs.targetOS.contains("macos")
            cpp.frameworks: "StormLib"
            cpp.frameworkPaths: "."
        }

        Group { // Properties for the produced executable
            fileTagsFilter: "application"
        }
    }
}
