ADD_LIBRARY(libalienfx SHARED alienfx_device.cpp alienfx_cmdparser.cpp)

INCLUDE_DIRECTORIES("include")
TARGET_LINK_LIBRARIES(libalienfx ${LIBUSB_1_LIBRARIES})

INSTALL(TARGETS libalienfx LIBRARY DESTINATION lib)
