;DATA SEGMENT IS DEFINED 
MY_DATA SEGMENT PARA 'DATA'
    LIST1 DB 1,2,3,4,5 
    LIST2 DB 5 DUP(7)
    LIST3 DW 111h, 222h, 333h, 444h, 555h
MY_DATA ENDS

MY_CODE SEGMENT PARA 'CODE'
MY_PROG PROC FAR

        MOV AX,MY_DATA
        MOV DS,AX
        MOV ES,AX

        ;MOVSB example
        CLD
        LEA SI, LIST1
        LEA DI, LIST2
        MOV CX, 5
        REP MOVSB

        ;LODSW example
        LEA SI, LIST3
        MOV CX, 5
        REP LODSW

        ;STOSB example
        LEA DI, LIST2
        MOV AL, 12H
        MOV CX, 5
        REP STOSB

        ;XCHG example
        MOV AH, 24H
        XCHG AL, AH

        ;XLATB example
        LEA BX, LIST1 
        MOV AL, 2
        XLATB


        MOV AH, 04CH
        INT 21H
        
        RET
MY_PROG ENDP
MY_CODE ENDS
        END MY_PROG
