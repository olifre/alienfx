ADD_EXECUTABLE(alienfx alienfx.cpp)
TARGET_LINK_LIBRARIES(alienfx alienfxdev)

INSTALL(TARGETS alienfx DESTINATION bin)
