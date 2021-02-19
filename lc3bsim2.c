/*
  Name 1: Rohan Mital
  UTEID 1: rm53327

/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the finstr_regst partner 
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the finstr_regst partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word addest_regess A
   MEMORY[A][1] stores the most significant byte of word at word addest_regess A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int addest_regess; /* this is a byte addest_regess */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (addest_regess = (start >> 1); addest_regess <= (stop >> 1); addest_regess++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", addest_regess << 1, addest_regess << 1, MEMORY[addest_regess][1], MEMORY[addest_regess][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (addest_regess = (start >> 1); addest_regess <= (stop >> 1); addest_regess++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", addest_regess << 1, addest_regess << 1, MEMORY[addest_regess][1], MEMORY[addest_regess][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/



int signextend(int number, int f, int t){
   if (number>>(f-1) == 1) {
      int bshift = 1<<f;
      int num = t - f;
      while (num-- > 0) {
         number += bshift;
         bshift = bshift << 1;
      }
   }
   return number;
}


void setcc(int reg) {
   int b = reg>>15;
   if (reg == 0) {
      NEXT_LATCHES.P = 0;
      NEXT_LATCHES.Z = 1;
      NEXT_LATCHES.N = 0;
   } else if (b == 1) {
      NEXT_LATCHES.P = 0;
      NEXT_LATCHES.Z = 0;
      NEXT_LATCHES.N = 1;
   } else {
      NEXT_LATCHES.P = 1;
      NEXT_LATCHES.Z = 0;
      NEXT_LATCHES.N = 0;
   }
}


void process_instruction(){
  /*  function: process_instruction
   *
   *    Process one instruction at a time
   *       -Fetch one instruction
   *       -Decode
   *       -Execute
   *       -Update NEXT_LATCHES
   */

   
   int instr_reg = (MEMORY[CURRENT_LATCHES.PC/2][1] << 8) + MEMORY[CURRENT_LATCHES.PC/2][0]; 
   NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
   int opcode = instr_reg >> 12;

   switch (opcode) {

      case 0 : /* BR */
      {
         int n = (instr_reg>>11) - (instr_reg>>11<<1);
         int z = (instr_reg>>10) - (instr_reg>>10<<1);
         int p = (instr_reg>>9) - (instr_reg>>9<<1);
         if (n & CURRENT_LATCHES.N || z & CURRENT_LATCHES.Z || p & CURRENT_LATCHES.P) {
            int pcoffset9 = instr_reg - (instr_reg>>9<<9);
            pcoffset9 = Low16bits(signextend(pcoffset9, 9, 16) << 1);
            NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + pcoffset9);
         }
         break;
      }

      case 1 : /* ADD */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int source_reg1 = (instr_reg>>6) - (instr_reg>>9<<3);
         int A = (instr_reg>>5) - (instr_reg>>6<<1);
         if (A == 0) {
            int source_reg2 = instr_reg - (instr_reg>>3<<3);
            NEXT_LATCHES.REGS[dest_reg] = Low16bits(CURRENT_LATCHES.REGS[source_reg1] + CURRENT_LATCHES.REGS[source_reg2]);
         } else if (A == 1) {
            int imm5 = instr_reg - (instr_reg>>5<<5);
            imm5 = signextend(imm5,5,16);
            NEXT_LATCHES.REGS[dest_reg] = Low16bits(CURRENT_LATCHES.REGS[source_reg1] + imm5);
         }
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      case 2 : /* LDB */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int base_reg = (instr_reg>>6) - (instr_reg>>9<<3);
         int boffset6 = instr_reg - (instr_reg>>6<<6);
         int addest_regess = Low16bits(signextend(boffset6,6,16) + CURRENT_LATCHES.REGS[base_reg]);
         int byte = addest_regess%2;
         addest_regess = addest_regess/2;

         NEXT_LATCHES.REGS[dest_reg] = signextend(MEMORY[addest_regess][byte],8,16);
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      case 3 : /* STB */
      {
         int sr = (instr_reg >> 9) - (instr_reg>>12<<3);
         int base_reg = (instr_reg>>6) - (instr_reg>>9<<3);
         int boffset6 = instr_reg - (instr_reg>>6<<6);
         int addest_regess = Low16bits(signextend(boffset6,6,16) + CURRENT_LATCHES.REGS[base_reg]);
         int byte = addest_regess % 2;
         addest_regess = addest_regess/2;
         MEMORY[addest_regess][byte] = CURRENT_LATCHES.REGS[sr] & 0x00FF;
         break;
      }

      case 4 : /* JSR, JSRR */
      {
         int temp = NEXT_LATCHES.PC;
         int bit11 = (instr_reg>>11) - (instr_reg>>12<<1);
         if (bit11 == 1) {
            int pcoffset11 = instr_reg - (instr_reg>>11<<11);
            pcoffset11 = Low16bits(signextend(pcoffset11,11,16) << 1);
            NEXT_LATCHES.PC = Low16bits(temp + pcoffset11);
         } else {
            int base_reg = (instr_reg >> 6) - (instr_reg>>9<<3);
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[base_reg];
         }
         NEXT_LATCHES.REGS[7] = temp;
         break;
      }




      case 5 : /* AND */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int source_reg1 = (instr_reg>>6) - (instr_reg>>9<<3);
         int A = (instr_reg>>5) - (instr_reg>>6<<1);
         if (A == 0) {
            int source_reg2 = instr_reg - (instr_reg>>3<<3);
            NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] & CURRENT_LATCHES.REGS[source_reg2];
         } else if (A == 1) {
            int imm5 = instr_reg - (instr_reg>>5<<5);
            imm5 = signextend(imm5,5,16);
            NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] & imm5;
         }
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      case 6 : /* LDW */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int base_reg = (instr_reg>>6) - (instr_reg>>9<<3);
         int offset6 = instr_reg - (instr_reg>>6<<6);
         offset6 = Low16bits(signextend(offset6,6,16)<<1);
         int addest_regess = Low16bits(CURRENT_LATCHES.REGS[base_reg] + offset6);
         addest_regess /= 2;
         NEXT_LATCHES.REGS[dest_reg] = (MEMORY[addest_regess][1]<<8) + MEMORY[addest_regess][0];
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      case 7 : /* STW */
      {
         int sr = (instr_reg >> 9) - (instr_reg>>12<<3);
         int base_reg = (instr_reg>>6) - (instr_reg>>9<<3);
         int offset6 = instr_reg - (instr_reg>>6<<6);
         offset6 = Low16bits(signextend(offset6,6,16)<<1);
         int addest_regess = Low16bits(CURRENT_LATCHES.REGS[base_reg] + offset6);
         addest_regess /= 2;
         MEMORY[addest_regess][1] = CURRENT_LATCHES.REGS[sr] >> 8;
         MEMORY[addest_regess][0] = CURRENT_LATCHES.REGS[sr] & 0x00FF;
         break;
      }



      case 9 : /* XOR, NOT */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int source_reg1 = (instr_reg>>6) - (instr_reg>>9<<3);
         int A = (instr_reg>>5) - (instr_reg>>6<<1);
         if (A == 0) {
            int source_reg2 = instr_reg - (instr_reg>>3<<3);
            NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] ^ CURRENT_LATCHES.REGS[source_reg2];
         } else if (A == 1) {
            int imm5 = instr_reg - (instr_reg>>5<<5);
            imm5 = signextend(imm5,5,16);
            NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] ^ imm5;
         }
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      
            case 12 : /* JMP, RET */
      { 
         int base_reg = (instr_reg>>6) - (instr_reg>>9<<3);
         NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[base_reg];
         break;
      }

      case 13 : /* SHF */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int source_reg1 = (instr_reg>>6) - (instr_reg>>9<<3);
         int bit5 = (instr_reg>>5) - (instr_reg>>6<<1);
         int bit4 = (instr_reg>>4) - (instr_reg>>5<<1);
         int amount4 = instr_reg - (instr_reg>>4<<4);
         if (bit4 == 0) {
            NEXT_LATCHES.REGS[dest_reg] = Low16bits(CURRENT_LATCHES.REGS[source_reg1] << amount4);
         } else if (bit5 == 1) { /* arithmetic */
            if (CURRENT_LATCHES.REGS[source_reg1]>>15 == 0) {
               NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] >> amount4;
            } else {
               int temp = CURRENT_LATCHES.REGS[source_reg1];
               while (amount4-->0) {
                  temp = (temp >> 1) + 0x8000;
               }
               NEXT_LATCHES.REGS[dest_reg] = temp;
            }
         } else if (bit5 == 0) { /* logic */
            NEXT_LATCHES.REGS[dest_reg] = CURRENT_LATCHES.REGS[source_reg1] >> amount4;
         }
         setcc(NEXT_LATCHES.REGS[dest_reg]);
         break;
      }

      case 14 : /* LEA */
      {
         int dest_reg = (instr_reg >> 9) - (instr_reg>>12<<3);
         int pcoffset9 = instr_reg - (instr_reg>>9<<9);
         pcoffset9 = Low16bits(signextend(pcoffset9, 9, 16) << 1);
         /* assumes pc+ + offset is always within memory location */
         NEXT_LATCHES.REGS[dest_reg] = Low16bits(NEXT_LATCHES.PC + pcoffset9);
         break;
      }


      case 15 : /* TRAP */
      {
         NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
         int trapvect8 = instr_reg - (instr_reg>>8<<8);
         NEXT_LATCHES.PC =  (MEMORY[trapvect8][1] << 8) + MEMORY[trapvect8][0];
         break;
      }

      
      
      
      

      
   }
  

}
