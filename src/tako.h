#ifndef TAKO_H
#define TAKO_H

#include <stdint.h>
#include <stdbool.h>

#define TAKO_MAXIMUM_STACK_SIZE 1024
#define TAKO_MAXIMUM_PROGRAM_INSTRUCTIONS 1024

typedef enum TakoErr {
	TAKO_ERR_NONE = 0,
	TAKO_ERR_STACK_OVERFLOW,
	TAKO_ERR_STACK_UNDERFLOW,
	TAKO_ERR_ILLEGAL_INSTRUCTION,
	TAKO_ERR_ILLEGAL_OPERAND,
	TAKO_ERR_ILLEGAL_INSTRUCTION_ACCESS,
	TAKO_ERR_DIVISION_BY_ZERO,
} TakoErr;

const char* tako_err_as_cstr(TakoErr err);

typedef union TakoWord {
	int64_t ival;
	uint64_t uval;
	double fval;
	void* pval;
} TakoWord;

typedef enum TakoInstType {
	TAKO_INST_NONE = 0,
	TAKO_INST_HALT,
	TAKO_INST_JMP,
	TAKO_INST_JMPIF,
	TAKO_INST_COPY,
	
	TAKO_INST_PUSHI,
	TAKO_INST_ADDI,
	TAKO_INST_SUBI,
	TAKO_INST_MULI,
	TAKO_INST_DIVI,
	TAKO_INST_EQI,
	TAKO_INST_NEQI,
	TAKO_INST_GTI,
	TAKO_INST_GTEI,
	TAKO_INST_LTI,
	TAKO_INST_LTEI,
} TakoInstType;

const char* tako_inst_type_as_cstr(TakoInstType type);
bool tako_inst_type_has_operand(TakoInstType type);

typedef struct TakoInst {
	TakoInstType type;
	TakoWord operand;
} TakoInst;

TakoInst tako_inst_halt(void);
TakoInst tako_inst_jmp(uint64_t addr);
TakoInst tako_inst_jmpif(uint64_t addr);
TakoInst tako_inst_copy(uint64_t addr);
TakoInst tako_inst_pushi(int64_t val);
TakoInst tako_inst_addi(void);
TakoInst tako_inst_subi(void);
TakoInst tako_inst_muli(void);
TakoInst tako_inst_divi(void);
TakoInst tako_inst_eqi(void);
TakoInst tako_inst_neqi(void);
TakoInst tako_inst_lti(void);
TakoInst tako_inst_ltei(void);
TakoInst tako_inst_gti(void);
TakoInst tako_inst_gtei(void);

typedef struct TakoProgram {
	TakoInst insts[TAKO_MAXIMUM_PROGRAM_INSTRUCTIONS];
	uint64_t insts_count;
} TakoProgram;

void tako_program_push_inst(TakoProgram* program, TakoInst inst);
void tako_save_program_to_file(TakoProgram* program, const char* file_path);
void tako_load_program_from_file(TakoProgram* program, const char* file_path);

typedef struct TakoStack {
	TakoWord data[TAKO_MAXIMUM_STACK_SIZE];
	uint64_t size;
} TakoStack;

void tako_stack_dump(TakoStack* stack);

typedef struct TakoRuntime {
	TakoProgram program;
	TakoStack stack;
	bool halt;
	uint64_t program_cursor;
} TakoRuntime;

void tako_runtime_run(TakoRuntime* trt);

#endif

#if defined(TAKO_IMPLEMENTATION)

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

TakoErr _tako_runtime_run(TakoRuntime* trt)
{
	if(trt->program_cursor >= trt->program.insts_count)
		return TAKO_ERR_ILLEGAL_INSTRUCTION_ACCESS;
	TakoInst inst = trt->program.insts[trt->program_cursor];
	switch (inst.type)
	{
		case TAKO_INST_NONE: {
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_HALT: {
			trt->halt = true;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_JMP: {
			trt->program_cursor = inst.operand.uval;
		} break;
		case TAKO_INST_JMPIF: {
			if(trt->stack.size < 1)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->program_cursor += 1;
			if(trt->stack.data[trt->stack.size - 1].ival != 0) {
				trt->stack.size -= 1;
				trt->program_cursor = inst.operand.uval;
			}
		} break;
		case TAKO_INST_COPY: {
			if(trt->stack.size < 1)
				return TAKO_ERR_STACK_UNDERFLOW;
			if(trt->stack.size - 1 - inst.operand.uval >= trt->stack.size)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size] = trt->stack.data[trt->stack.size - 1 - inst.operand.uval];
			trt->stack.size += 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_PUSHI: {
			if(trt->stack.size >= TAKO_MAXIMUM_STACK_SIZE)
				return TAKO_ERR_STACK_OVERFLOW;
			trt->stack.data[trt->stack.size++].ival = inst.operand.ival;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_ADDI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival += trt->stack.data[trt->stack.size - 1].ival;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_SUBI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival -= trt->stack.data[trt->stack.size - 1].ival;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_MULI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival *= trt->stack.data[trt->stack.size - 1].ival;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_DIVI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			if(trt->stack.data[trt->stack.size - 1].ival == 0)
				return TAKO_ERR_DIVISION_BY_ZERO;
			trt->stack.data[trt->stack.size - 2].ival /= trt->stack.data[trt->stack.size - 1].ival;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_EQI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival == trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_NEQI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival != trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_GTI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival > trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_GTEI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival >= trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_LTI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival < trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		case TAKO_INST_LTEI: {
			if(trt->stack.size < 2)
				return TAKO_ERR_STACK_UNDERFLOW;
			trt->stack.data[trt->stack.size - 2].ival = trt->stack.data[trt->stack.size - 2].ival <= trt->stack.data[trt->stack.size - 1].ival ? 1 : 0;
			trt->stack.size -= 1;
			trt->program_cursor += 1;
		} break;
		default: {
			return TAKO_ERR_ILLEGAL_INSTRUCTION;
		} break;
	}
	return TAKO_ERR_NONE;
}

void tako_runtime_run(TakoRuntime* trt)
{
	trt->halt = false;
	trt->program_cursor = 0;
	while(!trt->halt) {
		TakoErr err = _tako_runtime_run(trt);
		if(err != TAKO_ERR_NONE) {
			printf("ERROR: %s\n", tako_err_as_cstr(err));
			exit(EXIT_FAILURE);
		}
		if(trt->program_cursor >= trt->program.insts_count) {
			trt->halt = true;
		}
	}
}

void tako_program_push_inst(TakoProgram* program, TakoInst inst)
{
	program->insts[program->insts_count++] = inst;
}

void tako_save_program_to_file(TakoProgram* program, const char* file_path)
{
	FILE* f = fopen(file_path, "wb");
	if(f == NULL) {
		fprintf(stderr, "ERROR: couldn't open file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}
	fwrite(program->insts, sizeof(TakoInst), program->insts_count, f);
	if(ferror(f)) {
		fprintf(stderr, "ERROR: couldn't write to file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

void tako_load_program_from_file(TakoProgram* program, const char* file_path)
{
	FILE* f = fopen(file_path, "rb");
	if(f == NULL) {
		fprintf(stderr, "ERROR: couldn't open file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}

	if(fseek(f, 0, SEEK_END) < 0) {
		fprintf(stderr, "ERROR: couldn't read to file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);        
	}

	long m = ftell(f);
	if(m < 0) {
		fprintf(stderr, "ERROR: couldn't read to file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}

	assert(m % sizeof(TakoInst) == 0);
	assert((size_t) m <= TAKO_MAXIMUM_PROGRAM_INSTRUCTIONS * sizeof(TakoInst));

	if(fseek(f, 0, SEEK_SET) < 0) {
		fprintf(stderr, "ERROR: couldn't read to file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}

	program->insts_count = fread(program->insts, sizeof(TakoInst), m / sizeof(TakoInst), f);
	if(ferror(f)) {
		fprintf(stderr, "ERROR: failed to read to file \"%s\"\n", file_path);
		exit(EXIT_FAILURE);
	}

	fclose(f);
}

void tako_stack_dump(TakoStack* stack)
{
	printf("STACK(%zu):\n", stack->size);
	for(uint64_t i = 0; i < stack->size; ++i) {
		printf("[%zu]  %ld\n", i, stack->data[i].ival);
	}
}

const char* tako_err_as_cstr(TakoErr err)
{
	switch (err)
	{
		case TAKO_ERR_NONE: return "None";
		case TAKO_ERR_STACK_OVERFLOW: return "Stack overflow";
		case TAKO_ERR_STACK_UNDERFLOW: return "Stack underflow";
		case TAKO_ERR_ILLEGAL_INSTRUCTION: return "Illegal instruction";
		case TAKO_ERR_ILLEGAL_OPERAND: return "Illegal operand";
		case TAKO_ERR_ILLEGAL_INSTRUCTION_ACCESS: return "Illegal instruction access";
		case TAKO_ERR_DIVISION_BY_ZERO: return "Division by zero";
		default: assert(0 && "Invalid error type");
	}
	assert(0 && "Invalid error type");
	return NULL;
}

const char* tako_inst_type_as_cstr(TakoInstType type)
{
	switch (type)
	{
		case TAKO_INST_NONE: return "none";
		case TAKO_INST_HALT: return "halt";
		case TAKO_INST_JMP: return "jmp";
		case TAKO_INST_JMPIF: return "jmpif";
		case TAKO_INST_COPY: return "copy";
		case TAKO_INST_PUSHI: return "pushi";
		case TAKO_INST_ADDI: return "addi";
		case TAKO_INST_SUBI: return "subi";
		case TAKO_INST_MULI: return "muli";
		case TAKO_INST_DIVI: return "divi";
		case TAKO_INST_EQI: return "eqi";
		case TAKO_INST_NEQI: return "neqi";
		case TAKO_INST_GTI: return "gti";
		case TAKO_INST_GTEI: return "gtei";
		case TAKO_INST_LTI: return "lti";
		case TAKO_INST_LTEI: return "ltei";
		default: assert(0 && "Invalid instruction type");
	}
	assert(0 && "Invalid instruction type");
	return NULL;
}

bool tako_inst_type_has_operand(TakoInstType type)
{
	switch (type)
	{
		case TAKO_INST_NONE: return false;
		case TAKO_INST_HALT: return false;
		case TAKO_INST_JMP: return true;
		case TAKO_INST_JMPIF: return true;
		case TAKO_INST_COPY: return true;
		case TAKO_INST_PUSHI: return true;
		case TAKO_INST_ADDI: return false;
		case TAKO_INST_SUBI: return false;
		case TAKO_INST_MULI: return false;
		case TAKO_INST_DIVI: return false;
		case TAKO_INST_EQI: return false;
		case TAKO_INST_NEQI: return false;
		case TAKO_INST_GTI: return false;
		case TAKO_INST_GTEI: return false;
		case TAKO_INST_LTI: return false;
		case TAKO_INST_LTEI: return false;
		default: assert(0 && "Invalid instruction type");
	}
	assert(0 && "Invalid instruction type");
	return NULL;
}

TakoInst tako_inst_halt(void) { return (TakoInst) { .type = TAKO_INST_HALT, }; }
TakoInst tako_inst_jmp(uint64_t addr) { return (TakoInst) { .type = TAKO_INST_JMP, .operand = { .uval = addr } }; }
TakoInst tako_inst_jmpif(uint64_t addr) { return (TakoInst) { .type = TAKO_INST_JMPIF, .operand = { .uval = addr } }; }
TakoInst tako_inst_copy(uint64_t addr) { return (TakoInst) { .type = TAKO_INST_COPY, .operand = { .uval = addr } }; }
TakoInst tako_inst_pushi(int64_t val) { return (TakoInst) { .type = TAKO_INST_PUSHI, .operand = { .ival = val } }; }
TakoInst tako_inst_addi(void) { return (TakoInst) { .type = TAKO_INST_ADDI, }; }
TakoInst tako_inst_subi(void) { return (TakoInst) { .type = TAKO_INST_SUBI, }; }
TakoInst tako_inst_muli(void) { return (TakoInst) { .type = TAKO_INST_MULI, }; }
TakoInst tako_inst_divi(void) { return (TakoInst) { .type = TAKO_INST_DIVI, }; }
TakoInst tako_inst_eqi(void) { return (TakoInst) { .type = TAKO_INST_EQI, }; }
TakoInst tako_inst_neqi(void) { return (TakoInst) { .type = TAKO_INST_NEQI, }; }
TakoInst tako_inst_lti(void) { return (TakoInst) { .type = TAKO_INST_LTI, }; }
TakoInst tako_inst_ltei(void) { return (TakoInst) { .type = TAKO_INST_LTEI, }; }
TakoInst tako_inst_gti(void) { return (TakoInst) { .type = TAKO_INST_GTI, }; }
TakoInst tako_inst_gtei(void) { return (TakoInst) { .type = TAKO_INST_GTEI, }; }

#endif
