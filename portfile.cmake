vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL git@github.com/Ice-Cavern-Games/ICEngine.git
    # REF 7482de6071d21db77a7236155da44c172a7f6c9e # commit hash (pointing to a version tag)
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        ic-renderer-vulkan IC_RENDERER_VULKAN
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

# vcpkg_cmake_config_fixup(
#     PACKAGE_NAME "glfw3"
#     CONFIG_PATH "lib/cmake/glfw3"
# )

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# file(
#     INSTALL "${SOURCE_PATH}/LICENSE.md"
#     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
#     RENAME copyright
# )
