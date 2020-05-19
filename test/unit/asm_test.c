#include <CuTest.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/asm.h"
#include "mcc/ast.h"
#include "mcc/ir.h"
#include "mcc/semantic_checks.h"
#include "mcc/symbol_table.h"

void test1(CuTest *tc)
{
	// Define test input and create IR
	const char input[] = "int main(){return 42;}";
	struct mcc_parser_result parser_result;
	parser_result = mcc_parse_string(input, MCC_PARSER_ENTRY_POINT_PROGRAM);
	CuAssertIntEquals(tc, parser_result.status, MCC_PARSER_STATUS_OK);
	struct mcc_symbol_table *table = mcc_symbol_table_create((&parser_result)->program);
	struct mcc_semantic_check *checks = mcc_semantic_check_run_all((&parser_result)->program, table);
	CuAssertIntEquals(tc, checks->status, MCC_SEMANTIC_CHECK_OK);
	struct mcc_ir_row *ir = mcc_ir_generate((&parser_result)->program, table);
	CuAssertPtrNotNull(tc, ir);

	struct mcc_asm *code = mcc_asm_generate(ir);
	// asm
	CuAssertPtrNotNull(tc, code);

	// sections
	CuAssertPtrEquals(tc, NULL, code->data_section);
	CuAssertPtrNotNull(tc, code->text_section);

	// "main"
	CuAssertPtrNotNull(tc, code->text_section->function);
	CuAssertPtrNotNull(tc, code->text_section->function->label);
	CuAssertStrEquals(tc, code->text_section->function->label, "main");
	CuAssertPtrEquals(tc, NULL, code->text_section->function->next);

	// First line
	CuAssertPtrNotNull(tc, code->text_section->function->head);
	CuAssertPtrNotNull(tc, code->text_section->function->head->first);
	CuAssertPtrEquals(tc, NULL, code->text_section->function->head->second);
	CuAssertIntEquals(tc, code->text_section->function->head->opcode, MCC_ASM_PUSHL);
	CuAssertIntEquals(tc, MCC_ASM_OPERAND_REGISTER, code->text_section->function->head->first->type);
	CuAssertIntEquals(tc, MCC_ASM_EBP, code->text_section->function->head->first->reg);

	// Second line
	CuAssertIntEquals(tc, code->text_section->function->head->next->opcode, MCC_ASM_MOVL);
	CuAssertPtrNotNull(tc, code->text_section->function->head->next->first);
	CuAssertPtrNotNull(tc, code->text_section->function->head->next->second);
	CuAssertIntEquals(tc, code->text_section->function->head->next->opcode, MCC_ASM_MOVL);
	CuAssertIntEquals(tc, MCC_ASM_OPERAND_REGISTER, code->text_section->function->head->next->first->type);
	CuAssertIntEquals(tc, MCC_ASM_ESP, code->text_section->function->head->next->first->reg);
	CuAssertIntEquals(tc, MCC_ASM_OPERAND_REGISTER, code->text_section->function->head->next->second->type);
	CuAssertIntEquals(tc, MCC_ASM_EBP, code->text_section->function->head->next->second->reg);
}

void stack_frame_size(CuTest *tc)
{
	// Define test input and create IR
	const char input[] =
	    "int main(){int a; a = 1; int b; b = 1; while (a < 10) { a = a +1;  b = b -1;}  return b;}";
	struct mcc_parser_result parser_result;
	parser_result = mcc_parse_string(input, MCC_PARSER_ENTRY_POINT_PROGRAM);
	CuAssertIntEquals(tc, parser_result.status, MCC_PARSER_STATUS_OK);
	struct mcc_symbol_table *table = mcc_symbol_table_create((&parser_result)->program);
	struct mcc_semantic_check *checks = mcc_semantic_check_run_all((&parser_result)->program, table);
	CuAssertIntEquals(tc, checks->status, MCC_SEMANTIC_CHECK_OK);
	struct mcc_ir_row *ir = mcc_ir_generate((&parser_result)->program, table);
	CuAssertPtrNotNull(tc, ir);

	struct mcc_asm *code = mcc_asm_generate(ir);
	CuAssertPtrNotNull(tc, code);
	CuAssertIntEquals(tc, MCC_ASM_SUBL, code->text_section->function->head->next->next->opcode);
	CuAssertIntEquals(tc, MCC_ASM_OPERAND_LITERAL, code->text_section->function->head->next->next->first->type);
	CuAssertIntEquals(tc, MCC_ASM_OPERAND_REGISTER, code->text_section->function->head->next->next->second->type);
	CuAssertIntEquals(tc, MCC_ASM_ESP, code->text_section->function->head->next->next->second->reg);
	CuAssertIntEquals(tc, 8, code->text_section->function->head->next->next->first->literal);
}

// clang-format off

#define TESTS \
	TEST(test1) \
	TEST(stack_frame_size)

// clang-format on

#include "main_stub.inc"
#undef TESTS
