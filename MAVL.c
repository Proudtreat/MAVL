/*
* Welcome to my language created by: @author:"proudtreat aka Mohammad Mousa Daher"
* @version: 0.1
* @name: MAVL (My Assembly Virtual Language)
*
* This is a for fun project made by me in order to make myself go through hell, learn C,
* and make something that matters not just another blunt calculator so guess what project
* i did for that? MAKING MY FREAKING OWN LANGUAGE.But seriously this project is nothing but
* a learning experience and a chance to create something that combines the best of C abstraction
* and flexibility while sticking to ASM simplicity explicitly for AVR MCU such as ATMEGA328P and
* others.Again the project is NOT serious or has the goal to substitute any existing language but
* a useful experience to toggle learning C through hell (it's working) and tackling some ASM like 
* syntax.
*/

//Libraries needed for the program (use as less libraries as you can)
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//Some definitions that can be altered for different architectures
#define RSIZE 32		//The registers size
#define RAM_SIZE 2048 	//The ram size in bytes right now equivalent to 2KB
#define C_FLAG 0x80		//The macro definition of the carry flag
#define H_FLAG 0x40		//The macro definition of the Half-carry flag
#define I_FLAG 0x20		//The macro definition of the Interrupt flag
#define T_FLAG 0x10		//The macro definition of the Transfer bit flag
#define N_FLAG 0x08		//The macro definition of the Negative flag
#define S_FLAG 0x04		//The macro definition of the Signed flag
#define V_FLAG 0x02		//The macro definition of the Overflow flag
#define Z_FLAG 0x01		//The macro definition of the Zero flag

//Custom values for the custom Special Use Status Register(SUSR)
#define CP_FLAG 0x80	//The macro definition of Compare Flag
#define RM_FLAG 0X40	//The macro definition of Remainder Flag
#define D0_FLAG 0X20	//The macro definition of Dividing by 0 flag
//TODO

/*
* the CPU struct for the vm that'll test our language.
* The contents of this struct:
* 	1- Registers: the variables that'll store our content from ascii text to
*		numbers (hex,bin,decimal).The size is predefined.
*	2-Ram: The stack at which certain data are going to be pushed and saved during
*		execution of the program.
*	3-sreg (Status registers): Are the flags of our system CARRY,HALF-CARRY,INTERRUPT,TRANSFER bit,
*		NEGATIVE,SIGNED,OVERFLOW,ZERO
*/
typedef struct{

	//Definition of the 8 bits (1 byte) variables
	uint8_t reg[RSIZE],ram[RAM_SIZE],sreg,susr;

	//Definition of the 16 bits (2 bytes) variables
	uint16_t stack_pointer,program_counter,prom[RAM_SIZE]; 

	//Special registers for special storing uses as they're not immutable by users but
	//set by the program.
	//They're accessible by users to fetch values BUT NOT edit.
	//S[0,1] => Stores multiplication results
	//S[2] => Stores division resultant remainder if there is a remainder
	uint8_t sureg[RSIZE];

}CPU;

//Functions reference to avoid future errors:
//This helps with referencing functions and not 
//care about their location in the code

//DATA:
	//Initializes CPU
	void init_cpu(CPU* cpu);
	//The fuction that deals PUSH into system RAM
	void store_into_ram8(CPU* cpu,uint8_t value, uint16_t address);
	//Same as the previous function but this takes 16bits as a value
	void store_into_ram16(CPU*cpu,uint16_t value, uint16_t address);
	//For automatic push to the RAM stack with 8 bits value 
	void push_into_ram8(CPU* cpu, uint8_t val);
	//For automatic push into RAM stack with 16 bits value
	void push_into_ram16(CPU* cpu, uint16_t val);
	//For automatic push into RAM stack with registers instead of direct value
	void push_reg_into_ram(CPU* cpu,uint8_t reg_num);
	//Gets a value of a specified ram value and store it in specified reg
	void grab_from_ram(CPU *cpu, uint16_t address,uint8_t reg_number);
	//Automatic pop from the ram stack
	void pop_from_ram(CPU* cpu,uint8_t reg_number);
	//Assigns the value to the register of a specified register number
	void assign_to_reg(CPU* cpu,uint8_t val, uint8_t reg_num);
	//Returns the lower 8 bits of the number
	uint8_t low(CPU* cpu, uint16_t imm);
	//Returns the higher 8 bits of the number
	uint8_t high(CPU* cpu, uint16_t imm);
	//Moves data REG1 = REG2 format => MOV REG1,REG2 where REG2 can be: normal register
	// or special use
	void mov(CPU *cpu, uint8_t reg1, uint8_t reg2,uint8_t isSU);
	//stores immediate value in register
	void li(CPU* cpu,uint8_t reg1, uint8_t imm);

//ALU:
	//Does 2's complement of the value inside a register
	//Affected flags: N,Z,C
	void neg(CPU*cpu,uint8_t reg_num);
	//Does 1's complemetn of the value inside a register
	//Affected flags:
	void comp(CPU* cpu, uint8_t reg_num);
	//Subtracts 2 registers
	//Affected flags:Z,V,S,H,C,N
	void sub(CPU* cpu,uint8_t reg1,uint8_t reg2);
	//Adds 2 registers, this instruction triggers the following flags: Z,V,S,H,C,N
	void add(CPU* cpu, uint8_t reg1, uint8_t reg2,uint8_t isImm);
	//Clears the carry flag
	//Affected flags:Carry flag
	void clc(CPU *cpu);
	//Sets the carry flag
	//Affected flags: Carry Flag
	void sec(CPU* cpu);
	//Subs the register content with an immediate of 8 bits value
	//Affected flags:Z,V,S,H,C,N
	void subi(CPU* cpu, uint8_t reg1, uint8_t imm);
	//Divides the value existing in register 1 by register 2
	//Affected flags: Z,N, SUSR => RM_FLAG,D0_FLAG
	void div(CPU *cpu, uint8_t reg1, uint8_t reg2);
	//Divides the value existing in register 1 by immediate
	//Affected flags: Z,N, SUSR => RM_FLAG,D0_FLAG
	void divi(CPU *cpu, uint8_t reg1, uint8_t imm);
	//Multiplies the value existing in register 1 by register 2
	//Affected flags: Z,N,V,S
	void mul(CPU *cpu, uint8_t reg1, uint8_t reg2);
	//Multiplies the value existing in register 1 by immediate
	//Affected flags: Z,N,V,S
	void muli(CPU *cpu, uint8_t reg1, uint8_t imm);
	//Increments the contents of the register
	//Affected flags:V,N,S,H,Z,C
	void inc(CPU* cpu,uint8_t regn);
	//Decrements the contents of the register
	//Affected flags:V,N,S,H,Z,C
	void dec(CPU* cpu,uint8_t regn);
	//Swaps the instruction nibbles 
	//Example: 1010 0001 => 0001 1010
	void swap(CPU *cpu, uint8_t reg);

	//Comparison family
		//Compares if two numbers are equal
		//Affected flags: SUSR => CP_flag 
		void cp(CPU* cpu,uint8_t reg1,uint8_t reg2);
		//Compares if the value inside the register is equal to immediate
		//Affected flags: SUSR => CP_FLAG
		void cpi(CPU *cpu,uint8_t reg1,uint8_t imm);
		//Compares if the value inside the register1 is less than register2
		//Affected flags: SUSR => CP_flag
		void cpl(CPU *cpu, uint8_t reg1,uint8_t reg2);
		//Compares if the value inside the register1 is greater than register2
		//Affected flags: SUSR => CP_flag
		void cpg(CPU *cpu, uint8_t reg1,uint8_t reg2);
		//Compares if the value inside the register1 is >= register2
		//Affected flags: SUSR => CP_flag
		void cph(CPU *cpu, uint8_t reg1,uint8_t reg2);
		//Compares if the value inside the register1 is <= register2
		//Affected flags: SUSR => CP_flag
		void cps(CPU *cpu, uint8_t reg1,uint8_t reg2);

	//Logic shifting family

	//Rotates the number to the right maintaining LSB
	//by substituting it to the carry flag value
	//e.g: C_FLAG = 0, content of R0: 1010 1111 
	//ROR R0 => 0101 0111 and LSB becomes the new carry -> C_FLAG = 1
	//Affected flags: C_FLAG,N_flag
	//Notes: you might need to set or reset carry depending on what u 
	//want the logic be
	//These same rules apply for:ROL
	void ror(CPU* cpu, uint8_t reg);
	//Rotates the number to the left maintaining MSB
	//by substituting it to the carry flag value
	//e.g: C_FLAG = 0, content of R0: 1010 1111 
	//ROR R0 => 0101 0111 and 	MSB becomes the new carry -> C_FLAG = 0
	//Affected flags: C_FLAG,N_flag
	//Notes: you might need to set or reset carry depending on what u 
	//want the logic be
	//These same rules apply for:ROR
	void rol(CPU* cpu,uint8_t regNumber);
	//Logic Shift Left
	//it shifts the number to the left and then place the carry bit to the LSB
	//TODO: More detailed description
	void lol(CPU* cpu, uint8_t regNumber);
	//Logic Shift Right
	//It shifts the number to the right and then place the carry bit to the MSB
	//TODO: More detailed description
	void lor(CPU* cpu, uint8_t regNumber);


//Initializes the cpu and resets each value to 0
void init_cpu(CPU* cpu){

	//Sets all register values down to 0
	for(size_t i = 0; i < RSIZE; i ++){
		
		cpu -> reg[i] = 0x00;
	
	}
	
	//sets all ram locations to the value 0
	for(size_t i = 0; i < RAM_SIZE; i ++){

		cpu -> ram[i] = 0;

		cpu -> prom[i] = 0;
	}

	//Setting the program counter value to 0
	cpu -> program_counter = 0;

	//Setting the stack pointer value to 0
	cpu -> stack_pointer = RAM_SIZE - 1;

	//Setting status registers value to 0
	//The order is: C H I T N S V Z 
	//C: carry
	//H: half carry
	//I: interrupt
	//T: Transfer bit
	//N: Negative
	//S: signed (V XOR N)
	//V: Overflow
	//Z: zero
	cpu -> sreg = 0;

}

//The fuction that deals PUSH into system RAM
void store_into_ram8(CPU* cpu,uint8_t value, uint16_t address){

	//Pushes the 8 bits value (1 byte) to the specified memory location
	cpu -> ram[address] = value;

}

//Same as the previous function but this takes 16bits as a value
void store_into_ram16(CPU*cpu,uint16_t value, uint16_t address){

	//Pushes the higher byte to the lower address
	//value >> 8 is getting the higher byte(8 bits)
	cpu -> ram[address] = (value >> 8) & 0xff;

	//Pushes the lower byte to the higher address
	cpu -> ram[address + 1] = value & 0xff;

}

//For automatic push to the RAM stack with 8 bits value 
void push_into_ram8(CPU* cpu, uint8_t val){

	//Pushes the value into the stack and then decrements the stack pointer
	//in order to show the next free ram location in the stack
	cpu -> ram[cpu -> stack_pointer--] = val & 0xff;

}

//For automatic push into RAM stack with 16 bits value
void push_into_ram16(CPU* cpu, uint16_t val){

	//Pushes the lower byte into a higher address in the stack
	cpu -> ram[cpu -> stack_pointer--] = val & 0xff;

	//Pushes the higher byte into a lower address in the stack 
	cpu -> ram[cpu -> stack_pointer--] = (val >> 8) & 0xff;

}

//For automatic push into RAM stack with registers instead of direct value
void push_reg_into_ram(CPU* cpu,uint8_t reg_num){

	//Pushes the value of the register to the free space of the ram
	cpu -> ram[ cpu -> stack_pointer --] = cpu -> reg[reg_num];

}

//Gets a value of a specified ram value and store it in specified reg
void grab_from_ram(CPU *cpu, uint16_t address,uint8_t reg_number){

	//gets the value stored in the ram address and stores it
	//in the specified register with the called number
	cpu -> reg[reg_number] = cpu -> ram[address];

}

//Automatic pop from the ram stack
void pop_from_ram(CPU* cpu,uint8_t reg_number){

	//Increments the stack pointer in order to point at the top element
	cpu -> stack_pointer++;

	//Puts the value of the top element in the stack and puts it inside
	//register with the specified register number
	cpu -> reg[reg_number] = cpu -> ram[cpu -> stack_pointer];

	//Cleans the location that was previously popped and makes it free
	cpu -> ram[cpu -> stack_pointer] = 0;

}

//Assigns the value to the register of a specified register number
void assign_to_reg(CPU* cpu,uint8_t val, uint8_t reg_num){

	//Register number (reg_num) = an 8bits max numeric value
	cpu -> reg[reg_num] = val;

}

//Does 2's complement of the value inside a register
//Affected flags: N,Z,C
void neg(CPU*cpu,uint8_t reg_num){

	uint16_t res =(uint16_t)~cpu -> reg[reg_num] + 1;

	cpu -> reg[reg_num] = (uint8_t) res;

	//Checks if the number is equal to zero thus triggering or not triggering the Z flag
	cpu -> reg[reg_num] == 0 ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;
 
	//Checks if the number is negative thus triggering or not triggering the N flag
	(cpu -> reg[reg_num] >>  7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

	//Checks if there's a carry and thus triggering or not triggering the C flag
	res > 0x0ff ? cpu -> sreg | C_FLAG: cpu -> sreg & ~C_FLAG;

}

//Does 1's complemetn of the value inside a register
//Affected flags:
void comp(CPU* cpu, uint8_t reg_num){

	cpu -> reg[reg_num] = ~cpu -> reg[reg_num];

}

//Subtracts 2 registers
//Affected flags:Z,V,S,H,C,N
void sub(CPU* cpu,uint8_t reg1,uint8_t reg2){

	//Then adds the values found in these registers and toggles the flags
	//corresponding to the resullt.
	add(cpu,reg1,(~reg2 + 1),0);
}

//Adds 2 registers, this instruction triggers the following flags: Z,V,S,H,C,N
void add(CPU* cpu, uint8_t reg1, uint8_t reg2,uint8_t isImm){

	//isImm indicates if reg2 is the index of a register or is an immediate value 
	if(isImm == 0){
		//Adds the 2 registers and stores the value inside the first register
		uint16_t result = (uint16_t)cpu -> reg[reg1] + (uint16_t)cpu -> reg[reg2];

		//Special case which is checking half carry before the main operation happens:
		//masks the value of register 1 and register 2 to the first 4 bits then checks if
		//their sum is bigger than 0x0f which means a half carry is found triggering H flag.
		cpu -> sreg = (((cpu -> reg[reg1] & 0x0f) + (cpu -> reg[reg2] & 0x0f)) > 0x0f) ? cpu -> sreg | H_FLAG: cpu -> sreg & ~H_FLAG;

		//Checks if both value found in registers are of the same sign
		uint8_t same_sign = (cpu -> reg[reg1] >> 7) == (cpu -> reg[reg2] >> 7);
		
		//Stores the result from 16 bits to 8 bits by bit masking getting rid of additional values preserving the
		//correct mathematical value of the operation.
		cpu -> reg[reg1] = result & 0xff;

		//if the number stored inside the reg is equal to zero then Z flag is trigerred
		cpu -> sreg = (cpu -> reg[reg1] == 0) ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

		//If the result is bigger than 0xff than carry happened and therefor C flag is trigerred
		cpu -> sreg = result > 0xff ? cpu -> sreg | C_FLAG: cpu -> sreg & ~C_FLAG;

		//If both numbers are same sign AND the result had a different sign thus an overflow triggering V flag
		cpu -> sreg = (same_sign) && ((cpu -> reg[reg2] >> 7) != (cpu -> reg[reg1] >> 7)) ? cpu -> sreg | V_FLAG : cpu -> sreg & ~V_FLAG;

		//Checks if the number is negative so it triggers the N flag
		cpu -> sreg = (cpu -> reg[reg1] >> 7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

		//Xor the N flag and V flag in order to trigger the S flag (signed)
		cpu -> sreg = ((cpu -> sreg & V_FLAG) ^ ((cpu -> sreg & N_FLAG) >> 2)) ? cpu -> sreg | S_FLAG: cpu -> sreg & ~S_FLAG;
	}

	//Adds the register content with an 8 bit immediate value
	else{

		//Adds the 2 registers and stores the value inside the first register
		uint16_t result = (uint16_t)cpu -> reg[reg1] + (uint16_t)reg2;

		//Special case which is checking half carry before the main operation happens:
		//masks the value of register 1 and register 2 to the first 4 bits then checks if
		//their sum is bigger than 0x0f which means a half carry is found triggering H flag.
		cpu -> sreg = (((cpu -> reg[reg1] & 0x0f) + (reg2 & 0x0f)) > 0x0f) ? cpu -> sreg | H_FLAG: cpu -> sreg & ~H_FLAG;

		//Checks if both value found in registers are of the same sign
		uint8_t same_sign = (cpu -> reg[reg1] >> 7) == (reg2 >> 7);
		
		//Stores the result from 16 bits to 8 bits by bit masking getting rid of additional values preserving the
		//correct mathematical value of the operation.
		cpu -> reg[reg1] = result & 0xff;

		//if the number stored inside the reg is equal to zero then Z flag is trigerred
		cpu -> sreg = (cpu -> reg[reg1] == 0) ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

		//If the result is bigger than 0xff than carry happened and therefor C flag is trigerred
		cpu -> sreg = result > 0xff ? cpu -> sreg | C_FLAG: cpu -> sreg & ~C_FLAG;

		//If both numbers are same sign AND the result had a different sign thus an overflow triggering V flag
		cpu -> sreg = (same_sign) && ((reg2 >> 7) != (cpu -> reg[reg1] >> 7)) ? cpu -> sreg | V_FLAG : cpu -> sreg & ~V_FLAG;

		//Checks if the number is negative so it triggers the N flag
		cpu -> sreg = (cpu -> reg[reg1] >> 7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

		//Xor the N flag and V flag in order to trigger the S flag (signed)
		//We shift by 2 bits for the N flag since it's located at 0x08 (0000 1000) and V flag is at 0x02(0000 0010)
		//Meaning for proper comparison between the 2 flags we need them to be in the same location => shifting by 2 bits
		cpu -> sreg = ((cpu -> sreg & V_FLAG) ^ ((cpu -> sreg & N_FLAG) >> 2)) ? cpu -> sreg | S_FLAG: cpu -> sreg & ~S_FLAG;
		

	}
}

//Clears the carry flag
//Affected flags:Carry flag
void clc(CPU* cpu){

	cpu -> sreg &= ~C_FLAG;

}

//Sets the carry flag
//Affected flags: Carry flag
void sec(CPU *cpu){

	cpu -> sreg |= C_FLAG;

}

//Subs the register of an 8 bits immediate value 
//Affected flags:H,Z,C,V,S,N
void subi(CPU* cpu,uint8_t reg1,uint8_t imm){

	//Then adds the values found in these registers and toggles the flags
	//corresponding to the resullt.
	add(cpu,reg1,(~imm + 1),1);
}

//Compares if two numbers are equal
//Affected flags: SUSR => CP_flag 
void cp(CPU* cpu,uint8_t reg1,uint8_t reg2){

	//if reg[reg1] = reg[reg2] => set cp flag
	cpu -> susr = (cpu-> reg[reg1] == cpu -> reg[reg2]) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register is equal to immediate
//Affected flags: SUSR => CP_flag 
void cpi(CPU *cpu,uint8_t reg1,uint8_t imm){

	cpu -> susr = (cpu -> reg[reg1] == imm) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is less than register2
//Affected flags: SUSR => CP_flag
void cpl(CPU *cpu, uint8_t reg1,uint8_t reg2){

	cpu -> susr = (cpu -> reg[reg1] < cpu -> reg[reg2]) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is greater than register2
//Affected flags: SUSR => CP_flag
void cpg(CPU *cpu, uint8_t reg1,uint8_t reg2){

	cpu -> susr = (cpu -> reg[reg1] > cpu -> reg[reg2]) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is >= register2
//Affected flags: SUSR => CP_flag
void cph(CPU *cpu, uint8_t reg1,uint8_t reg2){

	cpu -> susr = (cpu -> reg[reg1] >= cpu -> reg[reg2]) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is <= register2
//Affected flags: SUSR => CP_flag
void cps(CPU *cpu, uint8_t reg1,uint8_t reg2){

	cpu -> susr = (cpu -> reg[reg1] <= cpu -> reg[reg2]) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is less than immediate
//Affected flags: SUSR => CP_flag
void cpli(CPU *cpu, uint8_t reg1,uint8_t imm){

	cpu -> susr = (cpu -> reg[reg1] < imm) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is greater than immediate
//Affected flags: SUSR => CP_flag
void cpgi(CPU *cpu, uint8_t reg1,uint8_t imm){

	cpu -> susr = (cpu -> reg[reg1] > imm) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is >= immediate
//Affected flags: SUSR => CP_flag
void cphi(CPU *cpu, uint8_t reg1,uint8_t imm){

	cpu -> susr = (cpu -> reg[reg1] >= imm) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Compares if the value inside the register1 is less than immediate
//Affected flags: SUSR => CP_flag
void cpsi(CPU *cpu, uint8_t reg1,uint8_t imm){

	cpu -> susr = (cpu -> reg[reg1] <= imm) ? cpu -> susr | CP_FLAG : cpu -> susr & ~CP_FLAG;

}

//Returns the lower 8 bits of the number
uint8_t low(CPU* cpu, uint16_t imm){

	return imm & 0xff;

}
//Returns the higher 8 bits of the number
uint8_t high(CPU* cpu, uint16_t imm){

	return (imm >> 8) & 0xff;

}

//Divides the value existing in register 1 by register 2
//Affected flags: Z,N,SUSR => RM_FLAG, D0_FLAG
void div(CPU *cpu, uint8_t reg1, uint8_t reg2){

	if(cpu -> reg[reg2] != 0){

		//Divides both numbers and store the value in reg[reg1]
		cpu -> reg[reg1] = (uint8_t)(cpu -> reg[reg1] / cpu -> reg[reg2]);

		//Stores the remainder value in S[2]
		cpu -> sureg[2] = cpu -> reg[reg1] % cpu -> reg[reg2];

		//Checks if the value of division is 0 thus triggering or not triggering Z flag
		cpu -> sreg = cpu -> reg[reg1] == 0 ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

		//Checks if the number is negative thus triggering or not triggering N flag
		cpu -> sreg = (cpu -> reg[reg1] >> 7)  == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

		//Checks if there's a remainder thus triggering or not triggering RM_FLAG
		cpu -> susr = cpu -> sureg[2] == 0 ? cpu -> susr & ~RM_FLAG: cpu -> susr | RM_FLAG;

	}else{

		cpu -> susr = cpu -> susr | D0_FLAG;
		//todo:Practial error message or else...? to be decided

	}
	
}

//Divides the value existing in register 1 by imm
//Affected flags: Z,N,SUSR => RM_FLAG, D0_FLAG
void divi(CPU *cpu, uint8_t reg1, uint8_t imm){

	if(imm != 0){

		//Divides both numbers and store the value in reg[reg1]
		cpu -> reg[reg1] = (uint8_t)(cpu -> reg[reg1] / imm);

		//Stores the remainder value in S[2]
		cpu -> sureg[2] = cpu -> reg[reg1] % imm;

		//Checks if the value of division is 0 thus triggering or not triggering Z flag
		cpu -> sreg = cpu -> reg[reg1] == 0 ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

		//Checks if the number is negative thus triggering or not triggering N flag
		cpu -> sreg = (cpu -> reg[reg1] >> 7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

		//Checks if there's a remainder thus triggering or not triggering RM_FLAG
		cpu -> susr = cpu -> sureg[2] == 0 ? cpu -> susr & ~RM_FLAG: cpu -> susr | RM_FLAG;

	}else{

		cpu -> susr = cpu -> susr | D0_FLAG;
		//todo:Practial error message or else...? to be decided

	}
	
}

//Multiplies the value existing in register 1 by register 2
//Affected flags: Z,N,V,S
void mul(CPU *cpu, uint8_t reg1, uint8_t reg2){

	//Stores the result in a 16 bit variable at first
	uint16_t result = (uint16_t) cpu -> reg[reg1] * (uint16_t) cpu -> reg[reg2];

	//Stores the low nibble in S[0]
	cpu -> sureg[0] = low(cpu, result);

	//Stores high nibble in S[1]
	cpu -> sureg[1] = high(cpu,result);

	//Checks if the value of multiplication is 0 thus triggering or not triggering Z flag
	cpu -> sreg = result == 0 ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

	//Checks if the number is negative thus triggering or not triggering N flag
	cpu -> sreg = (result >> 15) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

	//Checks if both value found in registers are of the same sign
	uint8_t same_sign = (cpu -> reg[reg1] >> 7) == (cpu -> reg[reg2] >> 7);

	//If both numbers are same sign AND the result had a different sign thus an overflow triggering V flag
	cpu -> sreg = (same_sign) && ((cpu -> reg[reg2] >> 7) != (result >> 15)) ? cpu -> sreg | V_FLAG : cpu -> sreg & ~V_FLAG;
	
	//Xor the N flag and V flag in order to trigger the S flag (signed)
	cpu -> sreg = ((cpu -> sreg & V_FLAG) ^ ((cpu -> sreg & N_FLAG) >> 2)) ? cpu -> sreg | S_FLAG: cpu -> sreg & ~S_FLAG;
}

//Multiplies the value existing in register 1 by register 2
//Affected flags: Z,N,V,S
void muli(CPU *cpu, uint8_t reg1, uint8_t imm){

	//Stores the result in a 16 bit variable at first
	uint16_t result = (uint16_t) cpu -> reg[reg1] * (uint16_t) imm;

	//Stores the low nibble in S[0]
	cpu -> sureg[0] = low(cpu, result);

	//Stores high nibble in S[1]
	cpu -> sureg[1] = high(cpu,result);

	//Checks if the value of multiplication is 0 thus triggering or not triggering Z flag
	cpu -> sreg = result == 0 ? cpu -> sreg | Z_FLAG: cpu -> sreg & ~Z_FLAG;

	//Checks if the number is negative thus triggering or not triggering N flag
	cpu -> sreg = (result >> 15) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & ~N_FLAG;

	//Checks if both value found in registers are of the same sign
	uint8_t same_sign = (cpu -> reg[reg1] >> 7) == (imm >> 7);

	//If both numbers are same sign AND the result had a different sign thus an overflow triggering V flag
	cpu -> sreg = (same_sign) && ((imm >> 7) != (result >> 15)) ? cpu -> sreg | V_FLAG : cpu -> sreg & ~V_FLAG;
	
	//Xor the N flag and V flag in order to trigger the S flag (signed)
	cpu -> sreg = ((cpu -> sreg & V_FLAG) ^ ((cpu -> sreg & N_FLAG) >> 2)) ? cpu -> sreg | S_FLAG: cpu -> sreg & ~S_FLAG;
}

//Moves data REG1 = REG2 format => MOV REG1,REG2 where REG2 can be: normal register
// or special use
void mov(CPU *cpu, uint8_t reg1, uint8_t reg2,uint8_t isSU){

	//isSU indicates if the register is a special use register S
	if(isSU){

		cpu -> reg[reg1] = cpu -> sureg[reg2];

	}
	//Else it's a normal register so fetch data from it 
	else cpu -> reg[reg1] = cpu -> reg[reg2];
}

//stores immediate value in register
void li(CPU* cpu,uint8_t reg1, uint8_t imm){

	cpu -> reg[reg1] = imm;

}

//Increments the contents of the register
//Affected flags:V,N,S,H,Z,C
void inc(CPU* cpu,uint8_t regn){

	add(cpu,regn,1,1);

}

//Decrements the contents of the register
//Affected flags:V,N,S,H,Z,C
void dec(CPU* cpu,uint8_t regn){

	add(cpu,regn,-1,1);

}

//Swaps the instruction nibbles 
//Example: 1010 0001 => 0001 1010
void swap(CPU *cpu, uint8_t reg){

	cpu -> reg[reg] = (cpu -> reg[reg] >> 4) + (cpu -> reg[reg] << 4);

}

//Rotates the number to the right maintaining LSB
//by substituting it to the carry flag value
//e.g: C_FLAG = 0, content of R0: 1010 1111 
//ROR R0 => 0101 0111 and LSB becomes the new carry -> C_FLAG = 1
//Affected flags: C_FLAG,N_flag
//Notes: you might need to set or reset carry depending on what u 
//want the logic be
//These same rules apply for:ROL
void ror(CPU* cpu, uint8_t reg){

	//Stores the LSB for adjusting the C_Flag
	//Since carry flag is the MSB we shift the LSB by 7 locations
	//making it located at the MSB whether it was 0 or 1
	//P.S:excuse the naming of the variable as 'LSB'
	uint8_t LSB = cpu -> reg[reg] << 7;

	//Mask the status register at the carry flag to only extract the carry value
	//which is located at the MSB then we shift the content of R[reg] by 1 bit 
	//then we add them and update the value of R[reg] to the new shifted value
	cpu -> reg[reg] = (cpu -> sreg & C_FLAG) + (cpu -> reg[reg] >> 1);

	//OR the status register with the value of 'LSB' to change the C_FLAG
	//depending on the value of old R[reg] LSB value.
	cpu -> sreg |= LSB;

	//Checks if the new R[reg] MSB is 1 to toggle or not toggle the N_FLAG
	cpu -> sreg = (cpu -> reg[reg] >> 7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & N_FLAG;
}

//Rotates the number to the left maintaining MSB
//by substituting it to the carry flag value
//e.g: C_FLAG = 0, content of R0: 1010 1111 
//ROR R0 => 0101 0111 and 	MSB becomes the new carry -> C_FLAG = 0
//Affected flags: C_FLAG,N_flag
//Notes: you might need to set or reset carry depending on what u 
//want the logic be
//These same rules apply for:ROR
void rol(CPU* cpu,uint8_t regNumber){

	//Stores the MSB for adjusting the C_Flag
	//Since carry flag is the LSB we shift the MSB by 7 locations
	//making it located at the MSB whether it was 0 or 1
	//P.S:excuse the naming of the variable as 'MSB'
	uint8_t MSB = cpu -> reg[regNumber] >> 7;

	//Mask the status register at the carry flag to only extract the carry value
	//which is located at the LSB then we shift the content of R[reg] by 7 bits 
	//then we add them and update the value of R[reg] to the new shifted value
	cpu -> reg[regNumber] = ((cpu -> sreg & C_FLAG) >> 7) + (cpu -> reg[regNumber] << 1);

	//OR the status register with the value of 'MSB' to change the C_FLAG
	//depending on the value of old R[reg] MSB value.
	cpu -> sreg |= (MSB << 7);

	//Checks if the new R[reg] MSB is 1 to toggle or not toggle the N_FLAG
	cpu -> sreg = (cpu -> reg[regNumber] >> 7) == 1 ? cpu -> sreg | N_FLAG: cpu -> sreg & N_FLAG;

	//TODO:Check if implementation misses something
}

//Logic Shift Left
//it shifts the number to the left and then place the carry bit to the LSB
//TODO: More detailed description
void lol(CPU* cpu, uint8_t regNumber){

	//Mask the status register at the carry flag to only extract carry value
	cpu -> reg[regNumber] = ((cpu -> sreg & C_FLAG) >> 7) + (cpu -> reg[regNumber] << 1);

	//TODO:Check for further deeper implementation and Case study

}

//Logic Shift Right
//It shifts the number to the right and then place the carry bit to the MSB
//TODO: More detailed description
void lor(CPU* cpu, uint8_t regNumber){

	//Masks the status register at the carry flag to only extract carry
	cpu -> reg[regNumber] = (cpu -> sreg & C_FLAG) + (cpu -> reg[regNumber] >> 1);

	//TODO:Check for further deeper implementation and Case study

}

//main function
int main(){

	//The input instruction line
	char instruction_line[100];
	
	CPU cpu;

	init_cpu(&cpu);

	//The infinite loop of the trial machine
	while(strcmp(instruction_line,"nop\n")){

		fgets(instruction_line,sizeof(instruction_line),stdin);
		
	}

return 0;
}
