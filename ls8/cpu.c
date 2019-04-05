#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_LEN 6

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char address)
{
  return cpu -> ram[address];
}

void cpu_ram_write(struct cpu *cpu, unsigned char address, unsigned char value)
{
  cpu -> ram[address] = value;
}
/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *path)
{
  // DAY 1:
  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };
  // //
  // int address = 0;
  //
  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }
  char *ext = &path[strlen(path) - 4];
  if (strcmp(ext, ".ls8") != 0)
  {
    fprintf(stderr, "Got an .ls8 file?\n");
    exit(1);
  }
  FILE *src;
  int lines = 0;
  src = fopen(path, "r");

  if (src == NULL)
  {
    fprintf(stderr, "Couldn't open file\n");
    exit(1);
  }

  for (char c = getc(src); c != EOF; c = getc(src))
  {
    if (c == '\n')
    {
      lines += 1;
    }
  }

  fseek(src, 0L, SEEK_SET);

  char data[lines + 1];
  char line[255];
  char *cut;
  int count = 0;

  while (fgets(line, sizeof(line), src) != NULL)
  {
    if (line[0] == '0' || line[0] == '1')
    {
      data[count] = strtol(line, &cut, 2);
      count += 1;
    }
    else
    {
      continue;
    }
  }

  fclose(src);

  for (int address = 0; address < count + 1; address++)
  {
    cpu -> ram[address] = data[address];
  }
  // TODO: Replace this with something less hard-coded
}


/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char registersA, unsigned char registersB)
{
  // (void)cpu;
  // (void)registersA;
  // (void)registersB;

  switch (op) {
    case ALU_MUL:
      // TODO
      cpu -> registers[registersA] *= cpu -> registers[registersB];
      break;

    // TODO: implement more ALU ops
    case ALU_ADD:
    cpu -> registers[registersA] += cpu -> registers[registersB];
    break;

    case ALU_CMP:
      if (cpu -> registers[registersA] == cpu -> registers[registersB])
      {
        cpu -> FL = 0x01;
      }
      else if (cpu -> registers[registersA] < cpu -> registers[registersB])
      {
        cpu -> FL = 0x04;
      }
      else
      {
        cpu -> FL = 0x02;
      }
      break;
  }
}


void (*handlers[256])(struct cpu *cpu, unsigned char opA, unsigned char opB) = {0};

void LDI_handler (struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  cpu -> registers[opA] = opB;
}

void HLT_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)cpu;
  (void)opA;
  (void)opB;
  exit(0);
}

void PRN_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opA;
  printf("%d\n", cpu -> registers[opA]);
}

void MUL_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  alu(cpu, ALU_MUL, opA, opB);
}

void POP_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  cpu -> registers[opA] = cpu_ram_read(cpu, cpu -> registers[7]);
  if (cpu -> registers[7] != 0xF4)
  {
    cpu -> registers[7] += 1;
  }
}

void PUSH_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  if (cpu -> registers[7] != 0x00)
  {
    cpu -> registers[7] -= 1;
    cpu_ram_write(cpu, cpu -> registers[7], cpu -> registers[opA]);
  }
  else
  {
    printf("Stack Overflow");
    exit(1);
  }
}

void CMP_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  alu(cpu, ALU_CMP, opA, opB);
}

void JMP_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  cpu -> PC = cpu -> registers[opA];
}

void JEQ_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  if (cpu -> FL == 1)
  {
    cpu -> PC = cpu -> registers[opA];
  }
  else
  {
    cpu -> PC += 2;
  }
}

void JNE_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  if(cpu -> FL != 1)
  {
    cpu -> PC = cpu -> registers[opA];
  }
  else
  {
    cpu -> PC += 2;
  }
}

void CALL_handler(struct cpu *cpu, unsigned char opA, unsigned char opB)
{
  (void)opB;
  cpu -> registers[7] -= 1;
  cpu_ram_write(cpu, cpu -> registers[7], cpu -> PC + 2);
  cpu -> PC = cpu -> registers[opA];
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char IR;
  unsigned char opA;
  unsigned char opB;

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    IR = cpu_ram_read(cpu, cpu -> PC);
    // 2. Figure out how many operands this next instruction requires
    // unsigned char operands = (IR >> 6) + 1;
    // 3. Get the appropriate value(s) of the operands following this instruction
    opA = cpu_ram_read(cpu, cpu -> PC + 1 & 0xff);
    opB = cpu_ram_read(cpu, cpu -> PC + 2 & 0xff);
    // 4. switch() over it to decide on a course of action.
    int ops = (IR & 0xC0) >> 6;
    // int next = (IR >> 6) + 1;
    // address = cpu -> PC;


    // printf("TRACE: %02X | %02X %02X %02X |", cpu->PC, IR, opA, opB);

    // switch(IR)
    // {
    //   // 5. Do whatever the instruction should do according to the spec.
    //   case LDI:
    //   cpu -> registers[opA] = opB;
    //   break;
    //
    //   case PRN:
    //   printf("%d\n", cpu -> registers[opA]);
    //   break;
    //
    //   case HLT:
    //   running = 0;
    //   break;
    //
    //   case MUL:
    //   alu(cpu, ALU_MUL, opA, opB);
    //   break;
    //
    //   case ADD:
    //   alu(cpu, ALU_ADD, opA, opB);
    //   break;
    //
    //   default:
    //   printf("I don't know this: 0x%02x at 0x%02x\n", IR, cpu -> PC);
    //   running = 0;
    //   break;
    // }

    handlers[LDI] = LDI_handler;
    handlers[HLT] = HLT_handler;
    handlers[PRN] = PRN_handler;
    handlers[MUL] = MUL_handler;
    handlers[HLT] = HLT_handler;
    handlers[POP] = POP_handler;
    handlers[PUSH] = PUSH_handler;
    handlers[CMP] = CMP_handler;
    handlers[JMP] = JMP_handler;
    handlers[JEQ] = JEQ_handler;
    handlers[JNE] = JNE_handler;

    if (handlers[IR])
    {
      handlers[IR](cpu, opA, opB);
    }
    else
    {
      printf("Sorry, I don't understand that");
      running = 0;
    }

    // 6. Move the PC to the next instruction.
    if ((IR & 0x10) != 0x10) { cpu->PC += (ops + 1); } 
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registersisters
  cpu -> PC = 0;
  cpu -> FL = 0;
  memset(cpu->registers, 0, sizeof(cpu->registers));
  memset(cpu->ram, 0, sizeof(cpu->ram));
  cpu -> registers[7] = 0xF4;
}
