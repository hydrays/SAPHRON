project(SAPHRON)
cmake_minimum_required(VERSION 2.5.5)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# SAPHRON version number.
set(SAPHRON_VERSION_MAJOR "0")
set(SAPHRON_VERSION_MINOR "0")
set(SAPHRON_VERSION_TINY "8")  

# Default type is release
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

# Enable/disable energy evaluation parallelization. 
# Also sets the minimum value of intermolecular and 
# intramolecular neighbors for parallelization. 
set(PARALLEL_INTER true)
set(PARALLEL_INTRA true)
set(MIN_INTER_NEIGH 10)
set(MIN_INTRA_NEIGH 10)

# Default is openmp.
set(SAPHRON_OPENMP false CACHE BOOL "Enable OpenMP parallelization")

# Default is MPI.
set(SAPHRON_MPI false CACHE BOOL "Enable MPI parallelization")

set(SAPH_USE_LINAL false)
SET(ARMA_PATHS "" CACHE STRING "Search directory for Armadillo")

# BEGIN - Identify linear algebra dependencies.
include(FindArma)
#include(FindQuadMath)
#include(FindgFortran)
include(FindBLAS)
include(FindLAPACK)
include(FindMKL)

# If boost and MPI are found, turn on multi-walker
find_package(Boost
	1.55
	COMPONENTS mpi
)

find_package(MPI)

message(STATUS "")
message(STATUS "      ARMA_FOUND = ${ARMA_FOUND}"  )
#message(STATUS "  QUADMATH_FOUND = ${QUADMATH_FOUND}"  )
#message(STATUS "  GFORTRAN_FOUND = ${GFORTRAN_FOUND}"  )
message(STATUS "      BLAS_FOUND = ${BLAS_FOUND}"    )
message(STATUS "    LAPACK_FOUND = ${LAPACK_FOUND}"  )
message(STATUS "       MKL_FOUND = ${MKL_FOUND}"     )

if(MKL_FOUND)
	set(SAPH_LIBS ${SAPH_LIBS} ${MKL_LIBRARIES})
elseif(LAPACK_FOUND)
	set(SAPH_LIBS ${SAPH_LIBS} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${GFORTRAN_LIBRARIES} ${QUADMATH_LIBRARIES})
endif()

message(STATUS "")
message(STATUS "*** SAPHRON will use the following libraries:")
message(STATUS "*** SAPH_LIBS = ${SAPH_LIBS}")
message(STATUS "")
# END - Identify linear algebra dependencies.

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${ARMA_INCLUDES}")

set(SOURCE
	src/Constraints/Constraint.cpp
	src/DensityOfStates/DOSOrderParameter.cpp
	src/ForceFields/ForceField.cpp
	src/ForceFields/ForceFieldManager.cpp
	src/JSON/jsoncpp.cpp
	src/JSON/schema.cpp
	src/Moves/Move.cpp
	src/Observers/DLMFileObserver.cpp
	src/Observers/JSONObserver.cpp
	src/Observers/XYZObserver.cpp
	src/Particles/Particle.cpp
	src/Simulation/DOSSimulation.cpp
	src/Simulation/SimObservable.cpp
	src/Simulation/SimObserver.cpp
	src/Simulation/Simulation.cpp
	src/Simulation/LangevinSimulation.cpp
	src/Simulation/MonteCarloSimulation.cpp	
	src/Utils/Histogram.cpp
	src/Worlds/World.cpp
	src/Validator/RequirementLoader.cpp
)


# Add schema builder
add_custom_target(
    schema ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/__bogus.h
    )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/__bogus.h
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                   COMMAND python schema/buildschema.py
                   COMMENT "Generating schema source files"
                   )

add_library(libsaphron ${SOURCE})
set_target_properties(libsaphron PROPERTIES PREFIX "")
target_link_libraries(libsaphron ${SAPH_LIBS})
add_dependencies(libsaphron schema)

add_executable(saphron 
	src/main.cpp 
	src/Simulation/SimBuilder.cpp
)
target_link_libraries(saphron ${SAPH_LIBS} libsaphron)

# Require C++11
set_property(TARGET libsaphron PROPERTY CXX_STANDARD 11)
set_property(TARGET libsaphron PROPERTY CXX_STANDARD_REQUIRED ON)

# Configure flags 
# - Globals
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -pthread -std=c++11")
set_target_properties(libsaphron saphron PROPERTIES COMPILE_FLAGS "-Wall -Wextra")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed")

if(SAPHRON_OPENMP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fopenmp")
	message(STATUS "*** OpenMP Parallelization enabled!")
endif()

set(MULTI_WALKER OFF)
if(Boost_FOUND AND MPI_CXX_FOUND AND SAPHRON_MPI)
	include_directories(${MPI_CXX_INCLUDE_PATH})
	include_directories(${Boost_INCLUDE_DIRS})
	add_definitions(${MPI_CXX_COMPILE_FLAGS})
	link_directories(${MPI_CXX_LIBRARIES})
	target_link_libraries(libsaphron ${MPI_CXX_LIBRARIES} ${Boost_LIBRARIES})
	message(STATUS "*** MPI multi-walker DOS enabled!")
	message(STATUS "")
	set(MULTI_WALKER ON)
endif()

# Build config header
configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

message(STATUS "")

install(TARGETS saphron libsaphron 
	RUNTIME DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ 
	ARCHIVE DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

message(STATUS "CMAKE_CXX_FLAGS           = ${CMAKE_CXX_FLAGS}"          )
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "CMAKE_REQUIRED_INCLUDES   = ${CMAKE_REQUIRED_INCLUDES}"  )
