# GraphicsFuzz tests

include_directories(
	..
	../util
	)

set(DEQP_VK_GRAPHICSFUZZ_SRCS
	vktGraphicsFuzzTests.cpp
	vktGraphicsFuzzTests.hpp
	)

set(DEQP_VK_GRAPHICSFUZZ_LIBS
	tcutil
	vkutil
	)

PCH(DEQP_VK_GRAPHICSFUZZ_SRCS ../pch.cpp)

add_library(deqp-vk-graphicsfuzz STATIC ${DEQP_VK_GRAPHICSFUZZ_SRCS})
target_link_libraries(deqp-vk-graphicsfuzz ${DEQP_VK_GRAPHICSFUZZ_LIBS})
