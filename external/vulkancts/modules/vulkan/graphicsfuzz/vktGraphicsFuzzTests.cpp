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
 * \brief GraphicsFuzz Tests
 *//*--------------------------------------------------------------------*/

#include "vktGraphicsFuzzTests.hpp"

#include "tcuTestLog.hpp"

#include "vktTestCase.hpp"
#include "vktTestCaseUtil.hpp"

#include "vkDefs.hpp"
#include "vkObjUtil.hpp"
#include "vkRefUtil.hpp"
#include "vkTypeUtil.hpp"

namespace vkt
{
namespace graphicsfuzz
{

namespace
{

using namespace vk;

tcu::TestStatus compileShaders (Context& context)
{
  const DeviceInterface&	vk					= context.getDeviceInterface();
  const VkDevice			vkDevice			= context.getDevice();
  const tcu::IVec2						renderSize				(256, 256);

  const Unique<VkRenderPass>				renderPass				(makeRenderPass(vk, vkDevice, VK_FORMAT_R8G8B8A8_UNORM));

  // Pipeline layout
  const VkPipelineLayoutCreateInfo		pipelineLayoutParams	=
      {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,			// sType
        DE_NULL,												// pNext
        (vk::VkPipelineLayoutCreateFlags)0,
        0u,														// setLayoutCount
        DE_NULL,												// pSetLayouts
        0u,														// pushConstantRangeCount
        DE_NULL,												// pPushConstantRanges
      };
  const Unique<VkPipelineLayout>			pipelineLayout			(createPipelineLayout(vk, vkDevice, &pipelineLayoutParams));


  // Shaders
  const Unique<VkShaderModule>			vertShaderModule		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("vert"), 0));
  const Unique<VkShaderModule>			fragShaderModule		(createShaderModule(vk, vkDevice, context.getBinaryCollection().get("frag"), 0));

  // Pipeline
  const std::vector<VkViewport>			viewports				(1, makeViewport(renderSize));
  const std::vector<VkRect2D>				scissors				(1, makeRect2D(renderSize));

  const Unique<VkPipeline>				pipeline				(makeGraphicsPipeline(vk,					// const DeviceInterface&            vk
                                                                                      vkDevice,				// const VkDevice                    device
                                                                                      *pipelineLayout,		// const VkPipelineLayout            pipelineLayout
                                                                                      *vertShaderModule,	// const VkShaderModule              vertexShaderModule
                                                                                      DE_NULL,				// const VkShaderModule              tessellationControlModule
                                                                                      DE_NULL,				// const VkShaderModule              tessellationEvalModule
                                                                                      DE_NULL,				// const VkShaderModule              geometryShaderModule
                                                                                      *fragShaderModule,	// const VkShaderModule              fragmentShaderModule
                                                                                      *renderPass,			// const VkRenderPass                renderPass
                                                                                      viewports,			// const std::vector<VkViewport>&    viewports
                                                                                      scissors));			// const std::vector<VkRect2D>&      scissors

  const ::std::string		result			= "GraphicsFuzz mock result";
  return tcu::TestStatus::pass(result);
}

#include "graphicsFuzzTests.inc"

} // anonymous

tcu::TestCaseGroup* createTests (tcu::TestContext& testCtx)
{
  	de::MovePtr<tcu::TestCaseGroup>	graphicsFuzzTests	(new tcu::TestCaseGroup(testCtx, "graphicsfuzz", "GraphicsFuzz Tests"));

#include "graphicsFuzzAddTests.inc"

    return graphicsFuzzTests.release();
}

} // api
} // vkt
