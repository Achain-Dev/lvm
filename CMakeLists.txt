AUX_SOURCE_DIRECTORY(libraries/api API_SRC)
AUX_SOURCE_DIRECTORY(libraries/cli CLI_SRC)
AUX_SOURCE_DIRECTORY(libraries/client CLIENT_SRC)
AUX_SOURCE_DIRECTORY(libraries/db DB_SRC)
AUX_SOURCE_DIRECTORY(libraries/net NET_SRC)
AUX_SOURCE_DIRECTORY(libraries/rpc RPC_SRC)
AUX_SOURCE_DIRECTORY(libraries/rpc_stubs RPC_STUBS_SRC)
AUX_SOURCE_DIRECTORY(libraries/utilities UTILITIES_SRC)
AUX_SOURCE_DIRECTORY(libraries/wallet WALLET_SRC)
AUX_SOURCE_DIRECTORY(libraries/blockchain BLOCKCHAIN_SRC)
AUX_SOURCE_DIRECTORY(libraries/glua GLUA_SRC)

LIST(REMOVE_ITEM BLOCKCHAIN_SRC "libraries/blockchain/applink.c")
LIST(REMOVE_ITEM GLUA_SRC "libraries/glua/main.cpp" "libraries/glua/cdemo1.cpp" "libraries/glua/thinkyoung_lua_api.demo.cpp")

cmake_minimum_required(VERSION 2.8)
SET (CMAKE_CXX_COMPILER     "/usr/bin/g++")

# Boost settings
set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_MULTITHREADED   ON)
set(Boost_USE_STATIC_RUNTIME  ON)

# Specify Boost libraries that will use.
FIND_PACKAGE( Boost 1.54.0 REQUIRED COMPONENTS system program_options chrono context coroutine date_time exception filesystem iostreams locale thread )

set(CMAKE_CXX_FLAGS "-std=c++11 -DALP_GLOBAL_API_LOG=1" )
set(Boost_INCLUDE_DIR　"/usr/include/boost")
set(Boost_LIBRARY_DIR  "/usr/lib/x86_64-linux-gnu")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} 
        "libraries/include"
        "../fc-20160602formal/include"
        "../leveldb/include"
        "../miniupnp"
        "../openssl-1.0.2k/include"
)

LINK_DIRECTORIES( ${Boost_LIBRARY_DIR}
        "../fc-20160602formal"
        "../leveldb"
        "../miniupnp/miniupnpc"
        "../openssl-1.0.2k"
        "../fc-20160602formal/vendor/secp256k1-zkp/src/project_secp256k1-build/.libs"
        "."
        "libraries/api"
        "libraries/blockchain"
        "libraries/cli"
        "libraries/client"
        "libraries/db"
        "libraries/glua"
        "libraries/net"
        "libraries/rpc"
        "libraries/rpc_stubs"
        "libraries/utilities"
        "libraries/wallet"
)

set(SOURCE_FILES
	Main.cpp
)

add_library(glua ${GLUA_SRC})
add_library(all ${API_SRC} ${BLOCKCHAIN_SRC} ${CLI_SRC} ${CLIENT_SRC} ${DB_SRC} ${NET_SRC} ${RPC_SRC} ${RPC_STUBS_SRC} ${UTILITIES_SRC} ${WALLET_SRC})

add_executable(thinkyoung ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(thinkyoung liball.a libglua.a libfc.a libsecp256k1.a libleveldb.a libminiupnpc.a ${Boost_LIBRARIES} libcrypto.a dl)
