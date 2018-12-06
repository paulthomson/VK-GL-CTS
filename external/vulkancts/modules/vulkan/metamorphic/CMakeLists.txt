# Metamorphic tests

include_directories(
	..
	../util
	)

set(DEQP_VK_METAMORPHIC_SRCS
	vktMetamorphic.cpp
	vktMetamorphic.hpp
	vktMetamorphicTests.cpp
	vktMetamorphicTests.hpp
	)

set(DEQP_VK_METAMORPHIC_LIBS
	tcutil
	vkutil
	)

PCH(DEQP_VK_METAMORPHIC_SRCS ../pch.cpp)

add_library(deqp-vk-metamorphic STATIC ${DEQP_VK_METAMORPHIC_SRCS})
target_link_libraries(deqp-vk-metamorphic ${DEQP_VK_METAMORPHIC_LIBS})
