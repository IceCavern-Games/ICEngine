vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Ice-Cavern-Games/ICEngine
    REF "${VERSION}"
    SHA512 0
    HEAD_REF vcpkg-port
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        ic-renderer-vulkan IC_RENDERER_VULKAN
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME "icengine"
    CONFIG_PATH "lib/cmake/glfw3"
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# file(
#     INSTALL "${SOURCE_PATH}/LICENSE.md"
#     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
#     RENAME copyright
# )
# configure_file(
#     "${CMAKE_CURRENT_LIST_DIR}/usage"
#     "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
#     COPYONLY
# )
