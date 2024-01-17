function(increment_version minor_or_major)

    file(READ "version.txt" PROJECT_VERSION_FILE)
    string(STRIP "${PROJECT_VERSION_FILE}" PROJECT_VERSION_FILE) # Remove leading/trailing whitespace

    # Extract major, minor, and patch numbers
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\1" PROJECT_VERSION_MAJOR ${PROJECT_VERSION_FILE})
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\2" PROJECT_VERSION_MINOR ${PROJECT_VERSION_FILE})
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\3" PROJECT_VERSION_PATCH ${PROJECT_VERSION_FILE})

    # Increment patch number (you can adapt this logic for minor/major version increments)
    math(EXPR PROJECT_VERSION_PATCH "${PROJECT_VERSION_PATCH} + 1")

    # Write back the updated version to version.txt
    file(WRITE "version.txt" "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

endfunction()
