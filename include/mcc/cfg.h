#ifndef MCC_CFG_H
#define MCC_CFG_H

#include "mcc/ir.h"

//---------------------------------------------------------------------------------------- Data structure: CFG

struct mcc_basic_block {
	struct mcc_ir_row *leader;
	struct mcc_basic_block *child_left;
	struct mcc_basic_block *child_right;
};

// To simplify visiting all blocks only once
struct mcc_basic_block_chain {
	struct mcc_basic_block *head;
	struct mcc_basic_block_chain *next;
};

//---------------------------------------------------------------------------------------- Functions: CFG

struct mcc_basic_block *mcc_cfg_generate(struct mcc_ir_row *ir);

struct mcc_basic_block_chain *mcc_cfg_generate_block_chain(struct mcc_ir_row *ir);

//---------------------------------------------------------------------------------------- Functions: Set up datastructs

struct mcc_basic_block *mcc_cfg_new_basic_block(struct mcc_ir_row *leader,
                                                struct mcc_basic_block *child_left,
                                                struct mcc_basic_block *child_right);

// Delete only CFG datastructure and not contained IR
void mcc_delete_cfg(struct mcc_basic_block *head);

// Delete CFG and contained IR
void mcc_delete_cfg_and_ir(struct mcc_basic_block *head);

#endif // MCC_CFG_H
