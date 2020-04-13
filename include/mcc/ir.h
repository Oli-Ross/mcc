#ifndef MCC_IR_H
#define MCC_IR_H

#include "mcc/ast.h"
#include "mcc/symbol_table.h"

enum mcc_ir_instruction {
	MCC_IR_INSTR_JUMP,
	MCC_IR_INSTR_JUMPFALSE,
};

enum mcc_ir_arg_type {
	MCC_IR_TYPE_ROW,
	MCC_IR_TYPE_VAR,
};

struct mcc_ir_arg {
	enum mcc_ir_arg_type type;

	union {
		char *var;
		struct mcc_ir_row *row;
	};
};

struct mcc_ir_row {
	enum mcc_ir_instruction instr;

	struct mcc_ir_arg *arg1;
	struct mcc_ir_arg *arg2;

	unsigned row_no;

	struct mcc_ir_row *prev_row;
	struct mcc_ir_row *next_row;
};

struct mcc_ir_row *mcc_ir_generate(struct mcc_ast_program *ast, struct mcc_symbol_table *table);

#endif