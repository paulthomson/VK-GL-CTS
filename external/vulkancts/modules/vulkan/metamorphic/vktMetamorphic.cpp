/*-------------------------------------------------------------------------
 * Vulkan Conformance Tests
 * ------------------------
 *
 * Copyright (c) 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *//*!
 * \file
 * \brief Metamorphic Tests
 *//*--------------------------------------------------------------------*/

#include "vktMetamorphic.hpp"

#include "vkBuilderUtil.hpp"
#include "vkCmdUtil.hpp"
#include "vkDefs.hpp"
#include "vkImageUtil.hpp"
#include "vkMemUtil.hpp"
#include "vkObjUtil.hpp"
#include "vkQueryUtil.hpp"
#include "vkRefUtil.hpp"
#include "vkTypeUtil.hpp"

#include "tcuImageCompare.hpp"
#include "tcuTestLog.hpp"

#include "deUniquePtr.hpp"

namespace vkt
{
namespace metamorphic
{

using namespace vk;
using de::UniquePtr;

typedef de::SharedPtr<vk::Unique<vk::VkBuffer> > 		VkBufferSp;
typedef de::SharedPtr<vk::Allocation>					AllocationSp;

namespace {

void recordCommandBufferForRenderShaderPair (
	const DeviceInterface&					vk,
	const deUint32							queueFamilyIndex,
	const Unique<VkImage>&					image,
	const Unique<VkRenderPass>&				renderPass,
	const tcu::IVec2&						renderSize,
	const Unique<VkFramebuffer>&			framebuffer,
	const tcu::Vec4&						clearColor,
	const Unique<VkPipeline>&				pipeline,
	const Unique<VkBuffer>&					vertexBuffer,
	const Unique<VkBuffer>&					readImageBufferReference,
	const VkDeviceSize						imageSizeBytes,
	const Unique<VkCommandBuffer>&			cmdBuf,
	const Unique<VkPipelineLayout>&			pipelineLayout,
	const Unique<VkDescriptorSet>&			descriptorSet
)
{
	// Record commands
	beginCommandBuffer(vk, *cmdBuf);

	{
		const VkMemoryBarrier		vertFlushBarrier	=
			{
				VK_STRUCTURE_TYPE_MEMORY_BARRIER,			// sType
				DE_NULL,									// pNext
				VK_ACCESS_HOST_WRITE_BIT,					// srcAccessMask
				VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,		// dstAccessMask
			};
		const VkImageMemoryBarrier	colorAttBarrier		=
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,		// sType
				DE_NULL,									// pNext
				0u,											// srcAccessMask
				(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT|
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT),		// dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,					// oldLayout
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,	// newLayout
				queueFamilyIndex,							// srcQueueFamilyIndex
				queueFamilyIndex,							// dstQueueFamilyIndex
				*image,										// image
				{
					VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
					0u,											// baseMipLevel
					1u,											// levelCount
					0u,											// baseArrayLayer
					1u,											// layerCount
				}											// subresourceRange
			};
		vk.cmdPipelineBarrier(*cmdBuf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, (VkDependencyFlags)0, 1, &vertFlushBarrier, 0, (const VkBufferMemoryBarrier*)DE_NULL, 1, &colorAttBarrier);
	}

	beginRenderPass(vk, *cmdBuf, *renderPass, *framebuffer, makeRect2D(0, 0, renderSize.x(), renderSize.y()), clearColor);

	vk.cmdBindPipeline(*cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
	vk.cmdBindDescriptorSets(*cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, 1, &descriptorSet.get(), 0, DE_NULL);
	{
	const VkDeviceSize bindingOffset = 0;
	vk.cmdBindVertexBuffers(*cmdBuf, 0u, 1u, &vertexBuffer.get(), &bindingOffset);
	}
	vk.cmdDraw(*cmdBuf, 6u, 1u, 0u, 0u);
	endRenderPass(vk, *cmdBuf);

	{
		const VkImageMemoryBarrier	renderFinishBarrier	=
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,		// sType
				DE_NULL,									// pNext
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,		// outputMask
				VK_ACCESS_TRANSFER_READ_BIT,				// inputMask
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,	// oldLayout
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,		// newLayout
				queueFamilyIndex,							// srcQueueFamilyIndex
				queueFamilyIndex,							// dstQueueFamilyIndex
				*image,										// image
				{
					VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
					0u,											// baseMipLevel
					1u,											// mipLevels
					0u,											// baseArraySlice
					1u,											// arraySize
				}											// subresourceRange
			};
		vk.cmdPipelineBarrier(*cmdBuf, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, (VkDependencyFlags)0, 0, (const VkMemoryBarrier*)DE_NULL, 0, (const VkBufferMemoryBarrier*)DE_NULL, 1, &renderFinishBarrier);
	}

	{
		const VkBufferImageCopy	copyParams	=
			{
				(VkDeviceSize)0u,						// bufferOffset
				(deUint32)renderSize.x(),				// bufferRowLength
				(deUint32)renderSize.y(),				// bufferImageHeight
				{
					VK_IMAGE_ASPECT_COLOR_BIT,				// aspectMask
					0u,										// mipLevel
					0u,										// baseArrayLayer
					1u,										// layerCount
				},										// imageSubresource
				{ 0u, 0u, 0u },							// imageOffset
				{
					(deUint32)renderSize.x(),
					(deUint32)renderSize.y(),
					1u
				}										// imageExtent
			};
		vk.cmdCopyImageToBuffer(*cmdBuf, *image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *readImageBufferReference, 1u, &copyParams);
	}

	{
		const VkBufferMemoryBarrier	copyFinishBarrier	=
			{
				VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,	// sType
				DE_NULL,									// pNext
				VK_ACCESS_TRANSFER_WRITE_BIT,				// srcAccessMask
				VK_ACCESS_HOST_READ_BIT,					// dstAccessMask
				queueFamilyIndex,							// srcQueueFamilyIndex
				queueFamilyIndex,							// dstQueueFamilyIndex
				*readImageBufferReference,					// buffer
				0u,											// offset
				imageSizeBytes								// size
			};
		vk.cmdPipelineBarrier(*cmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, (VkDependencyFlags)0, 0, (const VkMemoryBarrier*)DE_NULL, 1, &copyFinishBarrier, 0, (const VkImageMemoryBarrier*)DE_NULL);
	}

	endCommandBuffer(vk, *cmdBuf);
}

} // anonymous

tcu::TestStatus renderShaderPair (Context& context, std::vector<UniformEntry> uniformEntries)
{
	const VkDevice							vkDevice				= context.getDevice();
	const DeviceInterface&					vk						= context.getDeviceInterface();
	const VkQueue							queue					= context.getUniversalQueue();
	const deUint32							queueFamilyIndex		= context.getUniversalQueueFamilyIndex();
	SimpleAllocator							memAlloc				(vk, vkDevice, getPhysicalDeviceMemoryProperties(context.getInstanceInterface(), context.getPhysicalDevice()));
	const tcu::IVec2						renderSize				(256, 256);
	const VkFormat							colorFormat				= VK_FORMAT_R8G8B8A8_UNORM;
	const tcu::Vec4							clearColor				(0.0f, 0.0f, 0.0f, 0.0f);

	const tcu::Vec4							vertices[]				=
			{
					tcu::Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
					tcu::Vec4(-1.0f, +1.0f, 0.0f, 1.0f),
					tcu::Vec4(+1.0f, +1.0f, 0.0f, 1.0f),

					tcu::Vec4(+1.0f, +1.0f, 0.0f, 1.0f),
					tcu::Vec4(+1.0f, -1.0f, 0.0f, 1.0f),
					tcu::Vec4(-1.0f, -1.0f, 0.0f, 1.0f)
			};

	const VkBufferCreateInfo				vertexBufferParams		=
			{
					VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
					DE_NULL,								// pNext
					0u,										// flags
					(VkDeviceSize)sizeof(vertices),			// size
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,		// usage
					VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
					1u,										// queueFamilyIndexCount
					&queueFamilyIndex,						// pQueueFamilyIndices
			};
	const Unique<VkBuffer>					vertexBuffer			(createBuffer(vk, vkDevice, &vertexBufferParams));
	const UniquePtr<Allocation>				vertexBufferMemory		(memAlloc.allocate(getBufferMemoryRequirements(vk, vkDevice, *vertexBuffer), MemoryRequirement::HostVisible));

	VK_CHECK(vk.bindBufferMemory(vkDevice, *vertexBuffer, vertexBufferMemory->getMemory(), vertexBufferMemory->getOffset()));

	const VkDeviceSize						imageSizeBytes			= (VkDeviceSize)(sizeof(deUint32)*renderSize.x()*renderSize.y());
	const VkBufferCreateInfo				readImageBufferParams	=
			{
					VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,		// sType
					DE_NULL,									// pNext
					(VkBufferCreateFlags)0u,					// flags
					imageSizeBytes,								// size
					VK_BUFFER_USAGE_TRANSFER_DST_BIT,			// usage
					VK_SHARING_MODE_EXCLUSIVE,					// sharingMode
					1u,											// queueFamilyIndexCount
					&queueFamilyIndex,							// pQueueFamilyIndices
			};
	const Unique<VkBuffer>					readImageBufferReference1			(createBuffer(vk, vkDevice, &readImageBufferParams));
	const UniquePtr<Allocation>				readImageBufferReferenceMemory1		(memAlloc.allocate(getBufferMemoryRequirements(vk, vkDevice, *readImageBufferReference1), MemoryRequirement::HostVisible));
    const Unique<VkBuffer>					readImageBufferReference2			(createBuffer(vk, vkDevice, &readImageBufferParams));
    const UniquePtr<Allocation>				readImageBufferReferenceMemory2		(memAlloc.allocate(getBufferMemoryRequirements(vk, vkDevice, *readImageBufferReference2), MemoryRequirement::HostVisible));

	VK_CHECK(vk.bindBufferMemory(vkDevice, *readImageBufferReference1, readImageBufferReferenceMemory1->getMemory(), readImageBufferReferenceMemory1->getOffset()));
    VK_CHECK(vk.bindBufferMemory(vkDevice, *readImageBufferReference2, readImageBufferReferenceMemory2->getMemory(), readImageBufferReferenceMemory2->getOffset()));

	const VkImageCreateInfo					imageParams				=
			{
					VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,									// sType
					DE_NULL,																// pNext
					0u,																		// flags
					VK_IMAGE_TYPE_2D,														// imageType
					VK_FORMAT_R8G8B8A8_UNORM,												// format
					{ (deUint32)renderSize.x(), (deUint32)renderSize.y(), 1 },				// extent
					1u,																		// mipLevels
					1u,																		// arraySize
					VK_SAMPLE_COUNT_1_BIT,													// samples
					VK_IMAGE_TILING_OPTIMAL,												// tiling
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,	// usage
					VK_SHARING_MODE_EXCLUSIVE,												// sharingMode
					1u,																		// queueFamilyIndexCount
					&queueFamilyIndex,														// pQueueFamilyIndices
					VK_IMAGE_LAYOUT_UNDEFINED,												// initialLayout
			};

	const Unique<VkImage>					image					(createImage(vk, vkDevice, &imageParams));
	const UniquePtr<Allocation>				imageMemory				(memAlloc.allocate(getImageMemoryRequirements(vk, vkDevice, *image), MemoryRequirement::Any));

	VK_CHECK(vk.bindImageMemory(vkDevice, *image, imageMemory->getMemory(), imageMemory->getOffset()));

	DescriptorSetLayoutBuilder descriptorSetLayoutBuilder;

	for (deUint32 i = 0; i < uniformEntries.size(); i++) {
		descriptorSetLayoutBuilder.addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL, DE_NULL);
	}
	const Unique<VkDescriptorSetLayout> descriptorSetLayout (descriptorSetLayoutBuilder.build(vk, vkDevice));

	DescriptorPoolBuilder descriptorPoolBuilder;
	descriptorPoolBuilder.addType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (deUint32) uniformEntries.size());
	Move<VkDescriptorPool> descriptorPool = descriptorPoolBuilder.build(vk, vkDevice, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1);

	// make descriptor set
	const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
		  {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,		// VkStructureType				sType;
			DE_NULL,											// const void*					pNext;
			descriptorPool.get(),								// VkDescriptorPool				descriptorPool;
			1u,													// deUint32						descriptorSetCount;
			&descriptorSetLayout.get(),							// const VkDescriptorSetLayout*	pSetLayouts;
	 	 };

	Unique<VkDescriptorSet> descriptorSet (allocateDescriptorSet(vk, vkDevice, &descriptorSetAllocateInfo));

	// create uniformEntries buffers
	std::vector<de::SharedPtr<Unique<VkBuffer> > > uniformBuffers;
	std::vector<de::SharedPtr<Allocation> > uniformBufferMemories;


	for (deUint32 i = 0; i < uniformEntries.size(); i++) {
			const vk::VkBufferCreateInfo	createInfo =
				{
						vk::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						DE_NULL,
						0u,								// flags
						(vk::VkDeviceSize)uniformEntries[i].size,	// size
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,	// usage
						vk::VK_SHARING_MODE_EXCLUSIVE,	// sharingMode
						0u,								// queueFamilyCount
						DE_NULL,						// pQueueFamilyIndices
				};


		VkBufferSp buffer (new Unique<VkBuffer>(vk::createBuffer(vk, vkDevice, &createInfo)));
		VkMemoryRequirements requirements = getBufferMemoryRequirements(vk, vkDevice, **buffer);
		AllocationSp allocation (memAlloc.allocate(requirements, MemoryRequirement::HostVisible).release());

		void *data = mapMemory(vk, vkDevice, allocation->getMemory(), 0, requirements.size, 0);
		memcpy(data, uniformEntries[i].value, uniformEntries[i].size);
		vk.unmapMemory(vkDevice, allocation->getMemory());

		VK_CHECK(vk.bindBufferMemory(vkDevice, **buffer, allocation->getMemory(), allocation->getOffset()));

		uniformBuffers.push_back(buffer);
		uniformBufferMemories.push_back(allocation);
	}

	DescriptorSetUpdateBuilder  descriptorSetUpdateBuilder;
	for (deUint32 i = 0; i < uniformEntries.size(); i++)
	{
		VkDescriptorBufferInfo descriptorBufferInfo = makeDescriptorBufferInfo(**uniformBuffers[i], 0, uniformEntries[i].size);
		descriptorSetUpdateBuilder.writeSingle(*descriptorSet, DescriptorSetUpdateBuilder::Location::binding(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &descriptorBufferInfo);
	}
	descriptorSetUpdateBuilder.update(vk, vkDevice);

	const Unique<VkRenderPass>				renderPass				(makeRenderPass(vk, vkDevice, VK_FORMAT_R8G8B8A8_UNORM));

	const VkImageViewCreateInfo				colorAttViewParams		=
			{
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,		// sType
					DE_NULL,										// pNext
					0u,												// flags
					*image,											// image
					VK_IMAGE_VIEW_TYPE_2D,							// viewType
					VK_FORMAT_R8G8B8A8_UNORM,						// format
					{
							VK_COMPONENT_SWIZZLE_R,
							VK_COMPONENT_SWIZZLE_G,
							VK_COMPONENT_SWIZZLE_B,
							VK_COMPONENT_SWIZZLE_A
					},												// components
					{
							VK_IMAGE_ASPECT_COLOR_BIT,						// aspectMask
							0u,												// baseMipLevel
							1u,												// levelCount
							0u,												// baseArrayLayer
							1u,												// layerCount
					},												// subresourceRange
			};
	const Unique<VkImageView>				colorAttView			(createImageView(vk, vkDevice, &colorAttViewParams));

	// Pipeline layout
	const VkPipelineLayoutCreateInfo		pipelineLayoutParams	=
			{
					VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,			// sType
					DE_NULL,												// pNext
					(vk::VkPipelineLayoutCreateFlags)0,
					1u,														// setLayoutCount
					&descriptorSetLayout.get(),								// pSetLayouts
					0u,														// pushConstantRangeCount
					DE_NULL,												// pPushConstantRanges
			};
	const Unique<VkPipelineLayout>			pipelineLayout			(createPipelineLayout(vk, vkDevice, &pipelineLayoutParams));

	// Shaders
	const Unique<VkShaderModule>			vertShaderModule1		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("vert1"), 0));
	const Unique<VkShaderModule>			fragShaderModule1		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("frag1"), 0));
	const Unique<VkShaderModule>			vertShaderModule2		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("vert2"), 0));
	const Unique<VkShaderModule>			fragShaderModule2		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("frag2"), 0));

	// Pipeline
	const std::vector<VkViewport>			viewports				(1, makeViewport(renderSize));
	const std::vector<VkRect2D>				scissors				(1, makeRect2D(renderSize));

	const Unique<VkPipeline>				pipeline1				(makeGraphicsPipeline(vk,					// const DeviceInterface&            vk
																						  vkDevice,				// const VkDevice                    device
																						  *pipelineLayout,		// const VkPipelineLayout            pipelineLayout
																						  *vertShaderModule1,	// const VkShaderModule              vertexShaderModule
																						  DE_NULL,				// const VkShaderModule              tessellationControlModule
																						  DE_NULL,				// const VkShaderModule              tessellationEvalModule
																						  DE_NULL,				// const VkShaderModule              geometryShaderModule
																						  *fragShaderModule1,	// const VkShaderModule              fragmentShaderModule
																						  *renderPass,			// const VkRenderPass                renderPass
																						  viewports,			// const std::vector<VkViewport>&    viewports
																						  scissors));			// const std::vector<VkRect2D>&      scissors

	const Unique<VkPipeline>				pipeline2				(makeGraphicsPipeline(vk,					// const DeviceInterface&            vk
																						  vkDevice,				// const VkDevice                    device
																						  *pipelineLayout,		// const VkPipelineLayout            pipelineLayout
																						  *vertShaderModule2,	// const VkShaderModule              vertexShaderModule
																						  DE_NULL,				// const VkShaderModule              tessellationControlModule
																						  DE_NULL,				// const VkShaderModule              tessellationEvalModule
																						  DE_NULL,				// const VkShaderModule              geometryShaderModule
																						  *fragShaderModule2,	// const VkShaderModule              fragmentShaderModule
																						  *renderPass,			// const VkRenderPass                renderPass
																						  viewports,			// const std::vector<VkViewport>&    viewports
																						  scissors));			// const std::vector<VkRect2D>&      scissors

	// Framebuffer
	const VkFramebufferCreateInfo			framebufferParams		=
			{
					VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,				// sType
					DE_NULL,												// pNext
					0u,														// flags
					*renderPass,											// renderPass
					1u,														// attachmentCount
					&*colorAttView,											// pAttachments
					(deUint32)renderSize.x(),								// width
					(deUint32)renderSize.y(),								// height
					1u,														// layers
			};
	const Unique<VkFramebuffer>				framebuffer				(createFramebuffer(vk, vkDevice, &framebufferParams));

	const VkCommandPoolCreateInfo			cmdPoolParams			=
			{
					VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,					// sType
					DE_NULL,													// pNext
					VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,			// flags
					queueFamilyIndex,											// queueFamilyIndex
			};
	const Unique<VkCommandPool>				cmdPool					(createCommandPool(vk, vkDevice, &cmdPoolParams));

	// Command buffer
	const VkCommandBufferAllocateInfo		cmdBufParams			=
			{
					VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,			// sType
					DE_NULL,												// pNext
					*cmdPool,												// pool
					VK_COMMAND_BUFFER_LEVEL_PRIMARY,						// level
					1u,														// bufferCount
			};
	const Unique<VkCommandBuffer>			cmdBuf1					(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));
	const Unique<VkCommandBuffer>			cmdBuf2					(allocateCommandBuffer(vk, vkDevice, &cmdBufParams));

	recordCommandBufferForRenderShaderPair(vk, queueFamilyIndex, image, renderPass, renderSize, framebuffer, clearColor, pipeline1, vertexBuffer, readImageBufferReference1, imageSizeBytes, cmdBuf1, pipelineLayout, descriptorSet);
	recordCommandBufferForRenderShaderPair(vk, queueFamilyIndex, image, renderPass, renderSize, framebuffer, clearColor, pipeline2, vertexBuffer, readImageBufferReference2, imageSizeBytes, cmdBuf2, pipelineLayout, descriptorSet);

	// Upload vertex data
	{
		const VkMappedMemoryRange	range			=
				{
						VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,	// sType
						DE_NULL,								// pNext
						vertexBufferMemory->getMemory(),		// memory
						0,										// offset
						(VkDeviceSize)sizeof(vertices),			// size
				};
		void*						vertexBufPtr	= vertexBufferMemory->getHostPtr();

		deMemcpy(vertexBufPtr, &vertices[0], sizeof(vertices));
		VK_CHECK(vk.flushMappedMemoryRanges(vkDevice, 1u, &range));
	}

	// Submit & wait for completion
	submitCommandsAndWait(vk, vkDevice, queue, cmdBuf1.get());
    submitCommandsAndWait(vk, vkDevice, queue, cmdBuf2.get());

	// Compare results
	const tcu::TextureFormat			tcuFormat		= vk::mapVkFormat(colorFormat);

	const VkMappedMemoryRange			range1			=
			{
					VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,	// sType
					DE_NULL,								// pNext
					readImageBufferReferenceMemory1->getMemory(),		// memory
					0,										// offset
					imageSizeBytes,							// size
			};
	const tcu::ConstPixelBufferAccess	resultAccess1	(tcuFormat, renderSize.x(), renderSize.y(), 1, readImageBufferReferenceMemory1->getHostPtr());
	VK_CHECK(vk.invalidateMappedMemoryRanges(vkDevice, 1u, &range1));

	const VkMappedMemoryRange			range2			=
		{
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,	// sType
				DE_NULL,								// pNext
				readImageBufferReferenceMemory2->getMemory(),		// memory
				0,										// offset
				imageSizeBytes,							// size
		};
	const tcu::ConstPixelBufferAccess	resultAccess2	(tcuFormat, renderSize.x(), renderSize.y(), 1, readImageBufferReferenceMemory2->getHostPtr());
	VK_CHECK(vk.invalidateMappedMemoryRanges(vkDevice, 1u, &range2));


	const tcu::UVec4	threshold		(0u);
	const tcu::IVec3	posDeviation	(1,1,0);

	if (tcu::intThresholdPositionDeviationCompare(context.getTestContext().getLog(),
												  "ComparisonResult",
												  "Image comparison result",
												  resultAccess1,
												  resultAccess2,
												  threshold,
												  posDeviation,
												  true,
												  tcu::COMPARE_LOG_RESULT))
		return tcu::TestStatus::pass("Rendering succeeded");
	else
		return tcu::TestStatus::fail("Image comparison failed");

	// Unreachable
	return tcu::TestStatus::pass("Rendering succeeded");
}

} // metamorphic
} // vkt
