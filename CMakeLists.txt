cmake_minimum_required(VERSION 3.16)
project(DominionGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client/ui"
)

# Qt6
find_package(Qt6 COMPONENTS Core Network Gui Widgets REQUIRED)

file(GLOB_RECURSE SHARED_HEADERS CONFIGURE_DEPENDS "include/shared/*.hpp")
file(GLOB_RECURSE SERVER_HEADERS CONFIGURE_DEPENDS "include/server/*.hpp")
file(GLOB_RECURSE CLIENT_HEADERS CONFIGURE_DEPENDS "include/client/*.hpp")

file(GLOB_RECURSE CLIENT_IMAGES CONFIGURE_DEPENDS "images/*.*")

file(GLOB_RECURSE SHARED_SOURCES CONFIGURE_DEPENDS "src/shared/*.cpp")

# server
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/server/*.cpp")

add_executable(DominionServer 
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
    ${SHARED_SOURCES}
    ${SHARED_HEADERS}
)

target_include_directories(DominionServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(DominionServer PRIVATE Qt6::Core Qt6::Network)

# client
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS 
    "src/client/*.cpp" 
    "src/client/*.ui"
)

add_executable(DominionClient WIN32 
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
    ${SHARED_SOURCES}
    ${SHARED_HEADERS}
)

target_include_directories(DominionClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(DominionClient PRIVATE -fsanitize=address)
target_link_libraries(DominionClient PRIVATE 
    Qt6::Core 
    Qt6::Network 
    Qt6::Gui 
    Qt6::Widgets
)
target_link_options(DominionClient PRIVATE -fsanitize=address)

qt_add_resources(DominionClient card_images
    PREFIX "/"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CLIENT_IMAGES}
)