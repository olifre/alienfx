ADD_LIBRARY(libalienfx alienfx_device.cpp alienfx_cmdparser.cpp)

INCLUDE_DIRECTORIES("include")
TARGET_LINK_LIBRARIES(libalienfx ${LIBUSB_1_LIBRARIES})
