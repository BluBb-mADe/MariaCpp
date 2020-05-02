file(GLOB HEADERS *.hpp)
INSTALL(FILES ${HEADERS} DESTINATION include/mariacpp)
INSTALL(FILES bits/bind.hpp DESTINATION include/mariacpp/bits)