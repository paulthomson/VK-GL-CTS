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

#include "vktTestGroupUtil.hpp"

#include "tcuDefs.hpp"
#include "tcuTestCase.hpp"
#include "tcuTestLog.hpp"
#include "tcuFunctionLibrary.hpp"
#include "tcuPlatform.hpp"

#include "vkDefs.hpp"
#include "vkPlatform.hpp"

#include "vktTestCase.hpp"

#include "vkRefUtil.hpp"
#include "vkDeviceUtil.hpp"

namespace vkt
{
namespace graphicsfuzz
{

namespace
{

class GraphicsFuzzTestInstance : public TestInstance
{
public:
								GraphicsFuzzTestInstance	(Context&	ctx)
									: TestInstance	(ctx)
	{}
	virtual tcu::TestStatus		iterate						(void)
	{
		tcu::TestLog&			log				= m_context.getTestContext().getLog();
		log << tcu::TestLog::Message << "GraphicsFuzz mock test" << tcu::TestLog::EndMessage;
		const ::std::string		result			= "GraphicsFuzz mock result";
		return tcu::TestStatus::pass(result);
    }

private:
};

class GraphicsFuzzTestCase : public TestCase
{
public:
							GraphicsFuzzTestCase	(tcu::TestContext&		testCtx)
								: TestCase	(testCtx, "graphicsfuzz_mock", "Run some shaders.")
	{}

	virtual					~GraphicsFuzzTestCase	(void)
	{}
	virtual TestInstance*	createInstance			(Context&				ctx) const
	{
		return new GraphicsFuzzTestInstance(ctx);
	}

private:
};

void createGraphicsFuzzTests (tcu::TestCaseGroup* graphicsFuzzTests)
{
	tcu::TestContext&	testCtx	= graphicsFuzzTests->getTestContext();

	graphicsFuzzTests->addChild(new GraphicsFuzzTestCase	(testCtx));
}

} // anonymous

tcu::TestCaseGroup* createTests (tcu::TestContext& testCtx)
{
	return createTestGroup(testCtx, "graphicsfuzz", "GraphicsFuzz Tests", createGraphicsFuzzTests);
}

} // api
} // vkt
