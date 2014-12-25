ADD_LIBRARY(alienfxdev SHARED alienfx_device.cpp alienfx_utils.cpp)

INCLUDE_DIRECTORIES("include")
TARGET_LINK_LIBRARIES(alienfxdev ${LIBUSB_1_LIBRARIES})

INSTALL(TARGETS alienfxdev LIBRARY DESTINATION lib)
