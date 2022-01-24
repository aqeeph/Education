;DATA SEGMENT IS DEFINED 
MY_DATA SEGMENT PARA 'DATA'
    NUM1 DB -3
    NUM2 DW 0AC0FH
MY_DATA ENDS

MY_CODE SEGMENT PARA 'CODE'
MY_PROG PROC FAR

        MOV AX,MY_DATA
        MOV DS,AX

        ;ADD/ADC examples
        MOV CX,0AAAAH
        ADD CX,5556H
        ADC DL,0

        MOV CX,0AAAAH
        ADD CX,5555H
        INC CX
        ADC DL,0

        MOV CL,-30
        MOV CH,-128
        ADC CH,CL

        ;SUB/SBB examples
        MOV CX,0A0AH
        MOV BX,0707H
        SUB BX,CX
        MOV DX,BX

        MOV CX,0A0AH
        MOV BX,0707H
        SUB BL,CL
        SBB BH,CH

        ;Unsigned Multiplication example
        MOV AL, 155H
        MOV BL, 3
        MUL BL

        ;Unsigned Division example
        MOV AX, 157
        MOV BL, 6
        DIV BL

        ;Signed Multiplication example
        MOV AX,NUM2
        IMUL NUM1

        ;Signed Division example
        MOV NUM2,-5
        IDIV NUM2

        MOV AH, 04CH
        INT 21H
        
        RET
MY_PROG ENDP
MY_CODE ENDS
        END MY_PROG
