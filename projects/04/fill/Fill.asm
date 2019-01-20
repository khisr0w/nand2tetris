// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

(START)
    @16384
    D=A
    @pixel
    M=D
    // Key Stroke
    @24576
    D=M
    @UNFILL
    D;JEQ
    @FILL
    D;JGT
(FILL)
    
    @24576
    D=M
    @START
    D;JEQ
    @pixel
    D=M
    @24576
    D=D-A
    @FEND
    D;JEQ
    @pixel
    A=M
    M=-1
    @pixel
    M=M+1
    @FILL
    0;JMP
(FEND)
    @24576
    D=M
    @START
    D;JGT
    @FEND
    0;JMP

(UNFILL)
    
    @24576
    D=M
    @START
    D;JGT
    @pixel
    D=M
    @24576
    D=D-A
    @UEND
    D;JEQ
    @pixel
    A=M
    M=0
    @pixel
    M=M+1
    @UNFILL
    0;JMP
(UEND)
    @24576
    D=M
    @START
    D;JGT
    @UEND
    0;JMP