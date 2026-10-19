cmake_minimum_required(VERSION 3.16)
project(DominionGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt6
find_package(Qt6 COMPONENTS Core Network Gui Widgets REQUIRED)


# Sources shared between client and server
file(GLOB_RECURSE SHARED_SOURCES CONFIGURE_DEPENDS "src/shared/*.cpp")

# Sources for the server
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/server/*.cpp")

add_executable(DominionServer 
    ${SERVER_SOURCES}
    ${SHARED_SOURCES}
)

target_link_libraries(DominionServer PRIVATE 
    Qt6::Core 
    Qt6::Network
)

target_include_directories(DominionServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Sources for the client
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "src/client/*.cpp")

# Create the client executable for Windows (WIN32)
add_executable(DominionClient WIN32 
    ${CLIENT_SOURCES}
    ${SHARED_SOURCES}
)

target_include_directories(DominionClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(DominionClient PRIVATE 
    Qt6::Core 
    Qt6::Network 
    Qt6::Gui 
    Qt6::Widgets
)