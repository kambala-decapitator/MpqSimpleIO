import qbs

Project {
    minimumQbsVersion: "1.7.1"

    CppApplication {
        consoleApplication: true
        files: "main.cpp"

        cpp.cxxLanguageVersion: "c++11"
        cpp.includePaths: "."
        cpp.libraryPaths: "StormLib"
        cpp.staticLibraries: "StormLib"

        Properties {
            condition: qbs.targetOS.contains("macos")
            cpp.dynamicLibraries: ["bz2", "z"]
        }
        Properties {
            condition: qbs.targetOS.contains("windows")
            cpp.cxxFlags: {
                var flags = ["/UUNICODE", "/U_UNICODE"] // ANSI build
                flags.push(qbs.buildVariant == "debug" ? "/MTd" : "/MT") // fix LNK4098, StormLib uses MT for static libs
                return flags
            }
            cpp.defines: "STORMLIB_NO_AUTO_LINK"
            cpp.staticLibraries: qbs.buildVariant == "debug" ? "StormLibDAS" : "StormLibRAS"
        }

        Group { // Properties for the produced executable
            fileTagsFilter: "application"
        }
    }
}
