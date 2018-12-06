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

#include "vktMetamorphicTests.hpp"
#include "vktMetamorphic.hpp"

#include "vktTestCase.hpp"
#include "vktTestCaseUtil.hpp"

namespace vkt
{
namespace metamorphic
{

namespace {

using namespace vk;

void createFooProgs (SourceCollections& dst)
{

  dst.spirvAsmSources.add("vert1") <<
      "               OpCapability Shader\n"
      "          %1 = OpExtInstImport \"GLSL.std.450\"\n"
      "               OpMemoryModel Logical GLSL450\n"
      "               OpEntryPoint Vertex %main \"main\" %_ %pos %inColor\n"
      "               OpSource GLSL 400\n"
      "               OpSourceExtension \"GL_ARB_separate_shader_objects\"\n"
      "               OpSourceExtension \"GL_ARB_shading_language_420pack\"\n"
      "               OpName %main \"main\"\n"
      "               OpName %gl_PerVertex \"gl_PerVertex\"\n"
      "               OpMemberName %gl_PerVertex 0 \"gl_Position\"\n"
      "               OpMemberName %gl_PerVertex 1 \"gl_PointSize\"\n"
      "               OpMemberName %gl_PerVertex 2 \"gl_ClipDistance\"\n"
      "               OpName %_ \"\"\n"
      "               OpName %pos \"pos\"\n"
      "               OpName %bufferVals \"bufferVals\"\n"
      "               OpMemberName %bufferVals 0 \"f\"\n"
      "               OpName %myBufferVals \"myBufferVals\"\n"
      "               OpName %inColor \"inColor\"\n"
      "               OpMemberDecorate %gl_PerVertex 0 BuiltIn Position\n"
      "               OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize\n"
      "               OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance\n"
      "               OpDecorate %gl_PerVertex Block\n"
      "               OpDecorate %pos Location 0\n"
      "               OpMemberDecorate %bufferVals 0 Offset 0\n"
      "               OpDecorate %bufferVals Block\n"
      "               OpDecorate %myBufferVals DescriptorSet 0\n"
      "               OpDecorate %myBufferVals Binding 0\n"
      "               OpDecorate %inColor Location 1\n"
      "       %void = OpTypeVoid\n"
      "          %3 = OpTypeFunction %void\n"
      "      %float = OpTypeFloat 32\n"
      "    %v4float = OpTypeVector %float 4\n"
      "       %uint = OpTypeInt 32 0\n"
      "     %uint_1 = OpConstant %uint 1\n"
      "%_arr_float_uint_1 = OpTypeArray %float %uint_1\n"
      "%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1\n"
      "%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex\n"
      "          %_ = OpVariable %_ptr_Output_gl_PerVertex Output\n"
      "        %int = OpTypeInt 32 1\n"
      "      %int_0 = OpConstant %int 0\n"
      "%_ptr_Input_v4float = OpTypePointer Input %v4float\n"
      "        %pos = OpVariable %_ptr_Input_v4float Input\n"
      "%_ptr_Output_v4float = OpTypePointer Output %v4float\n"
      " %bufferVals = OpTypeStruct %float\n"
      "%_ptr_Uniform_bufferVals = OpTypePointer Uniform %bufferVals\n"
      "%myBufferVals = OpVariable %_ptr_Uniform_bufferVals Uniform\n"
      "    %inColor = OpVariable %_ptr_Input_v4float Input\n"
      "       %main = OpFunction %void None %3\n"
      "          %5 = OpLabel\n"
      "         %18 = OpLoad %v4float %pos\n"
      "         %20 = OpAccessChain %_ptr_Output_v4float %_ %int_0\n"
      "               OpStore %20 %18\n"
      "               OpReturn\n"
      "               OpFunctionEnd\n";

  dst.spirvAsmSources.add("frag1") <<
      "; SPIR-V\n"
      "; Version: 1.0\n"
      "; Generator: Khronos Glslang Reference Front End; 7\n"
      "; Bound: 27\n"
      "; Schema: 0\n"
      "               OpCapability Shader\n"
      "          %1 = OpExtInstImport \"GLSL.std.450\"\n"
      "               OpMemoryModel Logical GLSL450\n"
      "               OpEntryPoint Fragment %main \"main\" %outColor %gl_FragCoord\n"
      "               OpExecutionMode %main OriginUpperLeft\n"
      "               OpSource GLSL 400\n"
      "               OpName %main \"main\"\n"
      "               OpName %outColor \"outColor\"\n"
      "               OpName %gl_FragCoord \"gl_FragCoord\"\n"
      "               OpDecorate %outColor Location 0\n"
      "               OpDecorate %gl_FragCoord BuiltIn FragCoord\n"
      "       %void = OpTypeVoid\n"
      "          %3 = OpTypeFunction %void\n"
      "      %float = OpTypeFloat 32\n"
      "    %v4float = OpTypeVector %float 4\n"
      "%_ptr_Output_v4float = OpTypePointer Output %v4float\n"
      "   %outColor = OpVariable %_ptr_Output_v4float Output\n"
      "    %float_1 = OpConstant %float 1\n"
      "    %float_0 = OpConstant %float 0\n"
      "         %12 = OpConstantComposite %v4float %float_1 %float_1 %float_0 %float_1\n"
      "%_ptr_Input_v4float = OpTypePointer Input %v4float\n"
      "%gl_FragCoord = OpVariable %_ptr_Input_v4float Input\n"
      "       %uint = OpTypeInt 32 0\n"
      "     %uint_0 = OpConstant %uint 0\n"
      "%_ptr_Input_float = OpTypePointer Input %float\n"
      "   %float_42 = OpConstant %float 125\n"
      "       %bool = OpTypeBool\n"
      "%_ptr_Output_float = OpTypePointer Output %float\n"
      "       %main = OpFunction %void None %3\n"
      "          %5 = OpLabel\n"
      "               OpStore %outColor %12\n"
      "         %18 = OpAccessChain %_ptr_Input_float %gl_FragCoord %uint_0\n"
      "         %19 = OpLoad %float %18\n"
      "         %22 = OpFOrdLessThan %bool %19 %float_42\n"
      "               OpSelectionMerge %24 None\n"
      "               OpBranchConditional %22 %23 %24\n"
      "         %23 = OpLabel\n"
      "         %26 = OpAccessChain %_ptr_Output_float %outColor %uint_0\n"
      "               OpStore %26 %float_0\n"
      "               OpBranch %24\n"
      "         %24 = OpLabel\n"
      "               OpReturn\n"
      "               OpFunctionEnd\n";

  dst.spirvAsmSources.add("vert2") <<
      "               OpCapability Shader\n"
      "          %1 = OpExtInstImport \"GLSL.std.450\"\n"
      "               OpMemoryModel Logical GLSL450\n"
      "               OpEntryPoint Vertex %main \"main\" %_ %pos %inColor\n"
      "               OpSource GLSL 400\n"
      "               OpSourceExtension \"GL_ARB_separate_shader_objects\"\n"
      "               OpSourceExtension \"GL_ARB_shading_language_420pack\"\n"
      "               OpName %main \"main\"\n"
      "               OpName %gl_PerVertex \"gl_PerVertex\"\n"
      "               OpMemberName %gl_PerVertex 0 \"gl_Position\"\n"
      "               OpMemberName %gl_PerVertex 1 \"gl_PointSize\"\n"
      "               OpMemberName %gl_PerVertex 2 \"gl_ClipDistance\"\n"
      "               OpName %_ \"\"\n"
      "               OpName %pos \"pos\"\n"
      "               OpName %bufferVals \"bufferVals\"\n"
      "               OpMemberName %bufferVals 0 \"f\"\n"
      "               OpName %myBufferVals \"myBufferVals\"\n"
      "               OpName %inColor \"inColor\"\n"
      "               OpMemberDecorate %gl_PerVertex 0 BuiltIn Position\n"
      "               OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize\n"
      "               OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance\n"
      "               OpDecorate %gl_PerVertex Block\n"
      "               OpDecorate %pos Location 0\n"
      "               OpMemberDecorate %bufferVals 0 Offset 0\n"
      "               OpDecorate %bufferVals Block\n"
      "               OpDecorate %myBufferVals DescriptorSet 0\n"
      "               OpDecorate %myBufferVals Binding 0\n"
      "               OpDecorate %inColor Location 1\n"
      "       %void = OpTypeVoid\n"
      "          %3 = OpTypeFunction %void\n"
      "      %float = OpTypeFloat 32\n"
      "    %v4float = OpTypeVector %float 4\n"
      "       %uint = OpTypeInt 32 0\n"
      "     %uint_1 = OpConstant %uint 1\n"
      "%_arr_float_uint_1 = OpTypeArray %float %uint_1\n"
      "%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1\n"
      "%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex\n"
      "          %_ = OpVariable %_ptr_Output_gl_PerVertex Output\n"
      "        %int = OpTypeInt 32 1\n"
      "      %int_0 = OpConstant %int 0\n"
      "%_ptr_Input_v4float = OpTypePointer Input %v4float\n"
      "        %pos = OpVariable %_ptr_Input_v4float Input\n"
      "%_ptr_Output_v4float = OpTypePointer Output %v4float\n"
      " %bufferVals = OpTypeStruct %float\n"
      "%_ptr_Uniform_bufferVals = OpTypePointer Uniform %bufferVals\n"
      "%myBufferVals = OpVariable %_ptr_Uniform_bufferVals Uniform\n"
      "    %inColor = OpVariable %_ptr_Input_v4float Input\n"
      "       %main = OpFunction %void None %3\n"
      "          %5 = OpLabel\n"
      "         %18 = OpLoad %v4float %pos\n"
      "         %20 = OpAccessChain %_ptr_Output_v4float %_ %int_0\n"
      "               OpStore %20 %18\n"
      "               OpReturn\n"
      "               OpFunctionEnd\n";

  dst.spirvAsmSources.add("frag2") <<
"; SPIR-V\n"
"; Version: 1.0\n"
"; Generator: Khronos Glslang Reference Front End; 7\n"
"; Bound: 47\n"
"; Schema: 0\n"
"               OpCapability Shader\n"
"          %1 = OpExtInstImport \"GLSL.std.450\"\n"
"               OpMemoryModel Logical GLSL450\n"
"               OpEntryPoint Fragment %main \"main\" %outColor %gl_FragCoord\n"
"               OpExecutionMode %main OriginUpperLeft\n"
"               OpSource ESSL 310\n"
"               OpName %main \"main\"\n"
"               OpName %outColor \"outColor\"\n"
"               OpName %gl_FragCoord \"gl_FragCoord\"\n"
"               OpName %buf1 \"buf1\"\n"
"               OpMemberName %buf1 0 \"foo\"\n"
"               OpName %_ \"\"\n"
"               OpName %buf2 \"buf2\"\n"
"               OpMemberName %buf2 0 \"bar\"\n"
"               OpName %__0 \"\"\n"
"               OpDecorate %outColor Location 0\n"
"               OpDecorate %gl_FragCoord BuiltIn FragCoord\n"
"               OpMemberDecorate %buf1 0 Offset 0\n"
"               OpDecorate %buf1 Block\n"
"               OpDecorate %_ DescriptorSet 0\n"
"               OpDecorate %_ Binding 0\n"
"               OpMemberDecorate %buf2 0 Offset 0\n"
"               OpDecorate %buf2 Block\n"
"               OpDecorate %__0 DescriptorSet 0\n"
"               OpDecorate %__0 Binding 1\n"
"       %void = OpTypeVoid\n"
"          %3 = OpTypeFunction %void\n"
"      %float = OpTypeFloat 32\n"
"    %v4float = OpTypeVector %float 4\n"
"%_ptr_Output_v4float = OpTypePointer Output %v4float\n"
"   %outColor = OpVariable %_ptr_Output_v4float Output\n"
"    %float_1 = OpConstant %float 1\n"
"    %float_0 = OpConstant %float 0\n"
"         %12 = OpConstantComposite %v4float %float_1 %float_1 %float_0 %float_1\n"
"%_ptr_Input_v4float = OpTypePointer Input %v4float\n"
"%gl_FragCoord = OpVariable %_ptr_Input_v4float Input\n"
"       %uint = OpTypeInt 32 0\n"
"     %uint_0 = OpConstant %uint 0\n"
"%_ptr_Input_float = OpTypePointer Input %float\n"
"       %buf1 = OpTypeStruct %float\n"
"%_ptr_Uniform_buf1 = OpTypePointer Uniform %buf1\n"
"          %_ = OpVariable %_ptr_Uniform_buf1 Uniform\n"
"        %int = OpTypeInt 32 1\n"
"      %int_0 = OpConstant %int 0\n"
"%_ptr_Uniform_float = OpTypePointer Uniform %float\n"
"       %bool = OpTypeBool\n"
"%_ptr_Output_float = OpTypePointer Output %float\n"
"       %buf2 = OpTypeStruct %float\n"
"%_ptr_Uniform_buf2 = OpTypePointer Uniform %buf2\n"
"        %__0 = OpVariable %_ptr_Uniform_buf2 Uniform\n"
"  %float_0_5 = OpConstant %float 0.5\n"
"     %uint_2 = OpConstant %uint 2\n"
"       %main = OpFunction %void None %3\n"
"          %5 = OpLabel\n"
"               OpStore %outColor %12\n"
"         %18 = OpAccessChain %_ptr_Input_float %gl_FragCoord %uint_0\n"
"         %19 = OpLoad %float %18\n"
"         %26 = OpAccessChain %_ptr_Uniform_float %_ %int_0\n"
"         %27 = OpLoad %float %26\n"
"         %29 = OpFOrdLessThan %bool %19 %27\n"
"               OpSelectionMerge %31 None\n"
"               OpBranchConditional %29 %30 %31\n"
"         %30 = OpLabel\n"
"         %33 = OpAccessChain %_ptr_Output_float %outColor %uint_0\n"
"               OpStore %33 %float_0\n"
"               OpBranch %31\n"
"         %31 = OpLabel\n"
"         %34 = OpAccessChain %_ptr_Input_float %gl_FragCoord %uint_0\n"
"         %35 = OpLoad %float %34\n"
"         %39 = OpAccessChain %_ptr_Uniform_float %__0 %int_0\n"
"         %40 = OpLoad %float %39\n"
"         %41 = OpFOrdGreaterThan %bool %35 %40\n"
"               OpSelectionMerge %43 None\n"
"               OpBranchConditional %41 %42 %43\n"
"         %42 = OpLabel\n"
"         %46 = OpAccessChain %_ptr_Output_float %outColor %uint_2\n"
"               OpStore %46 %float_0_5\n"
"               OpBranch %43\n"
"         %43 = OpLabel\n"
"               OpReturn\n"
"               OpFunctionEnd\n";

}

tcu::TestStatus runFoo (Context& context) {
    std::vector<UniformEntry> uniformEntries;
    UniformEntry uniformEntry;

    float u1 = 125.0f;
    uniformEntry.size = sizeof(float);
    uniformEntry.value = (void *)(&u1);
    uniformEntries.push_back(uniformEntry);

    float u2 = 300.0f;
    uniformEntry.size = sizeof(float);
    uniformEntry.value = (void *)(&u2);
    uniformEntries.push_back(uniformEntry);

    return renderShaderPair(context, uniformEntries);
}

} // anonymous

tcu::TestCaseGroup* createTests (tcu::TestContext& testCtx)
{
  	de::MovePtr<tcu::TestCaseGroup>	metamorphicTests	(new tcu::TestCaseGroup(testCtx, "metamorphic", "Metamorphic Tests"));

    addFunctionCaseWithPrograms	(metamorphicTests.get(), "foo",	"", createFooProgs, runFoo);

    return metamorphicTests.release();
}

} // metamorphic
} // vkt
