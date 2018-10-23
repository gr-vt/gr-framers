INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_FRAMERS framers)

FIND_PATH(
    FRAMERS_INCLUDE_DIRS
    NAMES framers/api.h
    HINTS $ENV{FRAMERS_DIR}/include
        ${PC_FRAMERS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FRAMERS_LIBRARIES
    NAMES gnuradio-framers
    HINTS $ENV{FRAMERS_DIR}/lib
        ${PC_FRAMERS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FRAMERS DEFAULT_MSG FRAMERS_LIBRARIES FRAMERS_INCLUDE_DIRS)
MARK_AS_ADVANCED(FRAMERS_LIBRARIES FRAMERS_INCLUDE_DIRS)

