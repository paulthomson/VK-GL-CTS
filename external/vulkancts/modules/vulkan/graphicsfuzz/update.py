#!/usr/bin/env python3

#-------------------------------------------------------------------------
# Update GraphicsFuzz tests
# --------------------------------------
#
# Copyright (c) 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------

import argparse
import json
import os

def GLFuncToType(func):
    if func == 'glUniform1f':
        return 'float'
    elif func == 'glUniform2f':
        return 'tcu::Vec2'
    else:
        raise Exception('Invalid or unsupported glFunc: {}'.format(func))

def uniformArgsToInit(uType, uArgs):
    if uType == 'float':
        assert(len(uArgs) == 1)
        return '{}f'.format(uArgs[0])
    elif uType == 'tcu::Vec2':
        assert(len(uArgs) == 2)
        return 'tcu::Vec2({}f, {}f)'.format(uArgs[0], uArgs[1])
    else:
        raise Exception('Invalid or unsupported type: {}'.format(uType))

def findUniformWithBinding(uniforms, binding):
    for name, entry in uniforms.items():
        if entry['binding'] == binding:
            return name

def uniformsJSONToCDecl(jsonFilename):
    res = ''
    with open(jsonFilename, 'r') as f:
        uniforms = json.load(f)
    for i in range(len(uniforms)):
        name = findUniformWithBinding(uniforms, i)
        res += '// {}\n'.format(name)
        uType = GLFuncToType(uniforms[name]['func'])
        uInit = uniformArgsToInit(uType, uniforms[name]['args'])
        res += '{} u{} = {};\n'.format(uType, i, uInit)
        res += 'uniformEntry.size = sizeof({});\n'.format(uType)
        res += 'uniformEntry.value = (void *)(&u{});\n'.format(i)
        res += 'uniformEntries.push_back(uniformEntry);\n\n'

    return res

def asmToCString(asmFilename):
    cstring = ''
    with open(asmFilename, 'r') as f:
        for line in f.readlines():
            # remove trailing newline and escape double quotes
            line = line.rstrip().replace('"', '\\"')
            # Surround by double quotes and add '\n' inside the C string
            line = '"{}\\n"'.format(line)
            # Add final newline
            line += '\n'
            cstring += line
    return cstring

def createProgsFuncName(testName):
    return 'create{}Progs'.format(testName)

def runFuncName(testName):
    return 'run{}'.format(testName)

def testDeclaration(testName, asmFilename, jsonFilename):

    res = ''

    res += 'void create{}Progs (SourceCollections& dst)\n'.format(testName)
    res += '{\n'
    res += 'dst.spirvAsmSources.add("frag1") <<\n'
    res += asmToCString(asmFilename)
    res += ';\n'
    res += '}\n\n'

    res += 'tcu::TestStatus run{} (Context& context) {\n'
    res += 'std::vector<UniformEntry> uniformEntries;\n'
    res += 'UniformEntry uniformEntry;\n\n'
    res += uniformsJSONToCDecl(jsonFilename)
    res += 'return renderShaderPair(context, uniformEntries);\n'
    res += '}\n'

    return res

def parseArgs ():
    parser = argparse.ArgumentParser(description = "Update GraphicsFuzz tests")
    parser.add_argument("reference", help="Reference SPIR-V fragment shader in textual assembly")
    parser.add_argument("variant", help="Variant SPIR-V fragment shader in textual assembly")
    parser.add_argument("uniforms", help="Uniforms declaration in JSON file")
    return parser.parse_args()

if __name__ == "__main__":
    args = parseArgs()
    assert(os.path.exists(args.reference))
    assert(os.path.exists(args.variant))
    print(testDeclaration('Hugues', args.reference, args.uniforms))
