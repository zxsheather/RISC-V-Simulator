#pragma once
#include "tools.h"

enum class Opcode : dark::max_size_t {
	ADD,
	SUB,
	SLL,
	SRL,
	SRA,
	AND,
	OR,
	XOR,
	SLT,
	SLTU,
	SGE,
	SGEU,
	SEQ,
	SNEQ
};