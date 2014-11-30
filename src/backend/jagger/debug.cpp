//===- debug.cpp -----------------------------------------------*- C++ --*-===//
// Copyright 2014  Google
//
// Licensed under the Apache License", "Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing", "software
// distributed under the License is distributed on an "AS IS" BASIS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND", "either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//

#include "til/TIL.h"
#include "til/Global.h"
#include "til/VisitCFG.h"
#include "types.h"
#include <stdio.h>

namespace Jagger {

bool validateTIL(ohmu::til::BasicBlock* block) {
  if (!block) {
    printf("block is null.\n");
    return false;
  }
  if (block->parent()->blockID() >= block->blockID()) {
    printf("parent block has id greater than current block.\n");
    return false;
  }
  if (block->arguments().size() != 0) {
    if (block->predecessors().size() == 0) {
      printf("block has arguments but no predecessors.\n");
      return false;
    }
    for (auto& pred : block->predecessors()) {
      // todo: validate that pred has been validated
      if (pred->terminator()->opcode() != ohmu::til::COP_Goto) {
        printf("block with arguments has predecessor"
               " that doesn't terminate with a goto.\n");
        return false;
      }
    }
    if (!block->terminator()) {
      printf("block doesn't have a terminator.\n");
      return false;
    }
  }
  return true;
}

bool validateTIL(ohmu::til::SCFG* cfg) {
  if (!cfg) {
    printf("cfg is null.\n");
    return false;
  }
  if (cfg->blocks().size() == 0) {
    printf("cfg contains no blocks.\n");
    return false;
  }
  for (auto& block : cfg->blocks()) {
    if (!validateTIL(block.get()))
      return false;
    if (block->cfg() != cfg) {
      printf("block doesn't point back to cfg.\n");
      return false;
    }
  }
  return true;
}

bool validateTIL(ohmu::til::Global* global) {
  if (!global) {
    printf("module is null.\n");
    return false;
  }
  ohmu::til::VisitCFG visitCFG;
  visitCFG.traverseAll(global->global());
  auto& cfgs = visitCFG.cfgs();
  if (cfgs.empty()) {
    printf("module has no cfgs.");
    return false;
  }
  for (auto cfg : cfgs)
    if (!validateTIL(cfg))
      return false;
  return true;
}

const char* opcodeNames[] = {
    "NOP",                "CASE_HEADER",        "JOIN_HEADER",
    "JOIN_COPY",          "USE",                "LAST_USE",
    "ONLY_USE",           "VALUE_KEY",          "PHI0",
    "PHI1",               "PHI2",               "PHI3",
    "PHI4",               "PHI5",               "PHI6",
    "PHI7",               "DESTRUCTIVE_VALUE0", "DESTRUCTIVE_VALUE1",
    "DESTRUCTIVE_VALUE2", "DESTRUCTIVE_VALUE3", "DESTRUCTIVE_VALUE4",
    "DESTRUCTIVE_VALUE5", "DESTRUCTIVE_VALUE6", "DESTRUCTIVE_VALUE7",
    "VALUE0",             "VALUE1",             "VALUE2",
    "VALUE3",             "VALUE4",             "VALUE5",
    "VALUE6",             "VALUE7",             "ISA_OP",
    "CLOBBER_LIST",       "REGISTER_HINT",      "IMMEDIATE_BYTES",
    "BYTES_HEADER",       "ALIGNED_BYTES",      "BYTES",
    "CALL",               "RET",                "JUMP",
    "BRANCH",             "BRANCH_TARGET",
    "COMPARE",            "COMPARE_ZERO",       "NOT",
    "LOGIC",              "LOGIC3",             "BITFIELD_EXTRACT",
    "BITFIELD_INSERT",    "BITFIELD_CLEAR",     "COUNT_ZEROS",
    "POPCNT",             "BIT_TEST",           "MIN",
    "MAX",                "ADD",                "SUB",
    "NEG",                "ADDR",               "MUL",
    "DIV",                "IMULHI",             "IDIV",
    "IMOD",               "ABS",                "RCP",
    "SQRT",               "RSQRT",              "EXP2",
    "CONVERT",            "FIXUP",              "SHUFFLE",
    "IGNORE_LANES",       "BLEND",              "BLEND_ZERO",
    "PREFETCH",           "LOAD",               "EXPAND",
    "GATHER",             "INSERT",             "BROADCAST",
    "STORE",              "COMPRESS",           "SCATTER",
    "EXTRACT",            "MEMSET",             "MEMCPY"};

void printDebug(EventBuilder builder, size_t numEvents) {
  size_t offset = (numEvents + 2) / 3;
  for (size_t i = offset, e = numEvents + offset; i != e; ++i) {
    if (builder.data(i) >= offset && builder.data(i) < e)
      printf("%3d : %8d > %s\n", i, builder.data(i),
             opcodeNames[builder.code(i)]);
    else
      printf("%3d : %08x > %s\n", i, builder.data(i),
             opcodeNames[builder.code(i)]);
  }
}

}  // namespace Jagger {