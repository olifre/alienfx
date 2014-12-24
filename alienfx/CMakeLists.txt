ADD_EXECUTABLE(alienfx alienfx.cpp)
TARGET_LINK_LIBRARIES(alienfx libalienfx)

INSTALL(TARGETS alienfx DESTINATION bin)
