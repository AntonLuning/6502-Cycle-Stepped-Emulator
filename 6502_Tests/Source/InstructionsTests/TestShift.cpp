#include "../ComputerTest.h"

class ShiftTest : public ComputerTest {};

TEST_F(ShiftTest, ASLAccumulator)
{
	BYTE program[] = {
		0x0A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x66);	// 0110 0110
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ASLAccumulatorWithZero)
{
	BYTE program[] = {
		0x0A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ASLAccumulatorWithNegative)
{
	BYTE program[] = {
		0x0A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xF3;	// 1111 0011

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xE6);	// 1110 0110
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ASLZeroPage)
{
	BYTE program[] = {
		0x06, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);	// 0100 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x20), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ShiftTest, ASLZeroPageX)
{
	BYTE program[] = {
		0x16, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x22, 0x42);	// 0100 0010
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x22), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ASLZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0x16, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1F, 0x42);	// 0100 0010
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1F), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ASLAbsolute)
{
	BYTE program[] = {
		0x0E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A20, 0x42);	// 0100 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A20), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ASLAbsoluteX)
{
	BYTE program[] = {
		0x1E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A22, 0x42);	// 0100 0010
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A22), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}

TEST_F(ShiftTest, ASLAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x1E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1B1F, 0x42);	// 0100 0010
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1B1F), 0x84);	// 1000 0100
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}


TEST_F(ShiftTest, LSRAccumulator)
{
	BYTE program[] = {
		0x4A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x34;	// 0011 0100

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x1A);	// 0001 1010
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, LSRAccumulatorWithZero)
{
	BYTE program[] = {
		0x4A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, LSRAccumulatorWithNegative)
{
	BYTE program[] = {
		0x4A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xF3;	// 1111 0011

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x79);	// 0111 1001
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, LSRZeroPage)
{
	BYTE program[] = {
		0x46, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x41);	// 0100 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x20), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ShiftTest, LSRZeroPageX)
{
	BYTE program[] = {
		0x56, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x22, 0x41);	// 0100 0001
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x22), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, LSRZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0x56, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1F, 0x41);	// 0100 0001
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1F), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, LSRAbsolute)
{
	BYTE program[] = {
		0x4E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A20, 0x41);	// 0100 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A20), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, LSRAbsoluteX)
{
	BYTE program[] = {
		0x5E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A22, 0x41);	// 0100 0001
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A22), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 7);
}

TEST_F(ShiftTest, LSRAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x5E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1B1F, 0x41);	// 0100 0001
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1B1F), 0x20);	// 0010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 7);
}


TEST_F(ShiftTest, ROLAccumulator)
{
	BYTE program[] = {
		0x2A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x66);	// 0110 0110
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ROLAccumulatorWithZero)
{
	BYTE program[] = {
		0x2A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ROLAccumulatorWithNegative)
{
	BYTE program[] = {
		0x2A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xF3;	// 1111 0011
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xE6);	// 1110 0110
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ROLAccumulatorWithCarry)
{
	BYTE program[] = {
		0x2A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x67);	// 0110 0111
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, ROLZeroPage)
{
	BYTE program[] = {
		0x26, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);	// 0100 0010
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x20), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ShiftTest, ROLZeroPageX)
{
	BYTE program[] = {
		0x36, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x22, 0x42);	// 0100 0010
	MCU->CPU.X = 0x02;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x22), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ROLZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0x36, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1F, 0x42);	// 0100 0010
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1F), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ROLAbsolute)
{
	BYTE program[] = {
		0x2E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A20, 0x42);	// 0100 0010
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A20), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, ROLAbsoluteX)
{
	BYTE program[] = {
		0x3E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A22, 0x42);	// 0100 0010
	MCU->CPU.X = 0x02;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A22), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}

TEST_F(ShiftTest, ROLAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x3E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1B1F, 0x42);	// 0100 0010
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1B1F), 0x85);	// 1000 0101
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}


TEST_F(ShiftTest, RORAccumulator)
{
	BYTE program[] = {
		0x6A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x34;	// 0011 0100
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x1A);	// 0001 1010
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, RORAccumulatorWithZero)
{
	BYTE program[] = {
		0x6A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, RORAccumulatorWithNegative)
{
	BYTE program[] = {
		0x6A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xF3;	// 1111 0011
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x79);	// 0111 1001
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, RORAccumulatorWithCarry)
{
	BYTE program[] = {
		0x6A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x99);	// 1001 1001
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ShiftTest, RORZeroPage)
{
	BYTE program[] = {
		0x66, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x41);	// 0100 0001
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x20), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ShiftTest, RORZeroPageX)
{
	BYTE program[] = {
		0x76, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x22, 0x41);	// 0100 0001
	MCU->CPU.X = 0x02;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x22), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, RORZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0x76, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1F, 0x41);	// 0100 0001
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1F), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, RORAbsolute)
{
	BYTE program[] = {
		0x6E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A20, 0x41);	// 0100 0001
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A20), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ShiftTest, RORAbsoluteX)
{
	BYTE program[] = {
		0x7E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A22, 0x41);	// 0100 0001
	MCU->CPU.X = 0x02;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1A22), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}

TEST_F(ShiftTest, RORAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x7E, 0x20, 0x1A
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1B1F, 0x41);	// 0100 0001
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x1B1F), 0xA0);	// 1010 0000
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 7);
}