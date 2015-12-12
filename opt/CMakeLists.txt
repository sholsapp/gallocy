macro(set_cache_variable VAR_NAME VAR_DESCRIPTION)
    set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL ${VAR_DESCRIPTION})
    message(STATUS "Set ${VAR_NAME} to ${${VAR_NAME}}.")
endmacro()


# Curl configuration

if(USE_SYSTEM_CURL)
    find_package(CURL)
endif()
if(NOT USE_SYSTEM_CURL OR NOT CURL_FOUND)
    message(STATUS "Not using system Curl, using built-in curl project instead.")
    option(BUILD_CURL_TESTS "Set to ON to build cURL tests." OFF)
    add_subdirectory(curl)
    set(CURL_FOUND TRUE)
    set(CURL_LIBRARIES libcurl)
    set(CURL_INCLUDE_DIRS
        ${CURL_SOURCE_DIR}/include
        ${CURL_BINARY_DIR}/include/curl)

    # Group under the "external" project folder in IDEs such as Visual Studio.
    set_property(TARGET curl PROPERTY FOLDER "external")
    set_property(TARGET libcurl PROPERTY FOLDER "external")
endif()

set_cache_variable(CURL_FOUND "Set if libcurl is found or built")
set_cache_variable(CURL_LIBRARIES "Location of libcurl")
set_cache_variable(CURL_INCLUDE_DIRS "Location of curl include files")


# GTest configuration
if(USE_SYSTEM_GTEST)
    find_package(GTest)
endif()
if(NOT USE_SYSTEM_GTEST OR NOT GTEST_FOUND)
    message(STATUS "Not using system gtest, using built-in googletest project instead.")
    if(MSVC)
        # By default, GTest compiles on Windows in CRT static linkage mode. We use this
        # variable to force it into using the CRT in dynamic linkage (DLL), just as CPR
        # does.
        set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use the shared c runtime")
    endif()
    add_subdirectory(googletest)
    set(GTEST_FOUND TRUE)
    set(GTEST_LIBRARIES gtest)
    set(GTEST_MAIN_LIBRARIES gtest_main)
    set(GTEST_BOTH_LIBRARIES gtest gtest_main)
    set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include)

    # Group under the "tests/gtest" project folder in IDEs such as Visual Studio.
set_property(TARGET gtest PROPERTY FOLDER "tests/gtest")
set_property(TARGET gtest_main PROPERTY FOLDER "tests/gtest")
endif()

set_cache_variable(GTEST_FOUND "Set if libgtest was found or built")
set_cache_variable(GTEST_LIBRARIES "Location of libgtest")
set_cache_variable(GTEST_MAIN_LIBRARIES "Location of libgtest-main")
set_cache_variable(GTEST_BOTH_LIBRARIES "Location of both gtest libraries")
set_cache_variable(GTEST_INCLUDE_DIRS "Location of gtest include files")
