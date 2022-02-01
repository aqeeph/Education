MY_DATA SEGMENT PARA 'DATA'
    LIST1 DW 11, 12H, 1314H
    LIST2 DW 3 DUP(?)
    LIST3 DB 9, 10, 11
    LIST4 DB 3 DUP(?)
MY_DATA ENDS

MY_STACK SEGMENT
    DW  128  DUP(0)
MY_STACK ENDS

MY_CODE SEGMENT PARA 'CODE'
MY_PROG PROC FAR
    
    MOV AX,MY_DATA
    MOV DS,AX
    
    ;Copying LIST1 to LIST2 in reverse order by using the stack
    ;PUSH OPERATIONS
    PUSH LIST1
        
    MOV DI,2
    PUSH LIST1[DI]
    
    MOV AX, LIST1[DI+2]
    PUSH AX
    
    ;POP OPERATIONS
    POP LIST2
    
    MOV DI,2
    POP LIST2[DI]
    
    POP AX
    MOV LIST2[DI+2],AX
    
    
    ;Write the code that copies LIST1 to LIST2 in reverse order by using the stack and loops
    
    MOV DI, 0H
    MOV CX, 03H
    DONGU1: PUSH LIST1[DI]
            ADD DI, 02H
    LOOP DONGU1
    
    MOV CX, 03H
    MOV DI, 0H
    DONGU2: POP LIST2[DI]
    ADD DI, 02H
    LOOP DONGU2 
       
    ;Write the code that copies LIST3 to LIST4 in reverse order by using the stack and loops.
    ;Please note that LIST3 and LIST4 contain byte-sized elements but the stack operations are word-sized.
    
    
    MOV DI, 0H
    MOV CX, 03H
    MOV AH,0H 
    DONGU4: MOV AL,LIST3[DI]
            PUSH AX
            ADD DI, 01H
    LOOP DONGU4
    
    MOV DI, 0H
    MOV CX, 03H
    DONGU5: POP AX
            MOV LIST4[DI],AL
            ADD DI, 01H
    LOOP DONGU5 
    
    ;Write a program that finds the total of the numbers stored as LIST1's elements 
    ;a) by using loops only
    
    MOV DI, 0H
    MOV CX, 03H
    
    ;AX FOR TOTAL OF THE NUMBERS
    MOV AX, 0
    DONGU6:ADD AX, LIST1[DI]
           ADD DI, 02H 
    LOOP DONGU6    
     
    ;b) by using loops and the stack
    
    ;AX FOR TOTAL OF THE NUMBERS
    MOV AX, 0
    
    MOV DI, 0H
    MOV CX, 03H
    DONGU7: PUSH LIST1[DI]
            ADD DI, 02H
    LOOP DONGU7
    
    MOV DI, 0H
    MOV CX, 03H
    DONGU8: POP LIST2[DI]
    ADD AX, LIST2[DI]
    ADD DI, 02H
    LOOP DONGU8  
        
    MOV AX,4c00h
    INT 21h  

MY_PROG ENDP
MY_CODE ENDS
        END MY_PROG
