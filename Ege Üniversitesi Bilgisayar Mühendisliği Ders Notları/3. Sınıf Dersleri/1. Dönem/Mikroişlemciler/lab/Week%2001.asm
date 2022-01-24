;DATA SEGMENT IS DEFINED 
MY_DATA SEGMENT PARA 'DATA'
    SAYI EQU 5
    NUM1 db ?
    NUM2 dw ?
    LIST DB  1,2,3,4,5,6,7,8,9,10
MY_DATA ENDS

MY_CODE SEGMENT PARA 'CODE'
MY_PROG PROC FAR
        ;db 0 20 


;AX=   ,DS=

        MOV AX,MY_DATA

;AX=   ,DS=

        MOV DS,AX

;AX=    ,DS=
;db 0 20 
;
 
       
;IMMEDIATE ADDRESSING
        MOV NUM1,23H
;AX=
;CX=  , CL=
;[0000]=

         MOV AX,0FFFFH       
        MOV CL,5
        MOV NUM1,100

;AX=     ,CX=  ,CL=  ,NUM1=
;REGISTER ADDRESSING
;BX=  ,CX=
  
        MOV BX,CX          

;BX=   ,CX=   

;DIRECT ADDRESSING

;AX=     ,CX=   
;db 0 5
;

        MOV NUM2,AX
        MOV CL,LIST 

;AX=     , CX=
;db 0 5 
;
;REGISTER INDIRECT ADDRESSING
;BX=   ,AX=
;db 0 5
;

        MOV BX, OFFSET LIST
        MOV AX,[BX]        

;BX=    ,AX=
;db 0 5
;
;BASE INDEXED ADDRESSING
;DI=  ,BX=  ,CX=   ,CL=   ,DX=
;db 0 10
;
        MOV DI,2
        MOV BX, OFFSET LIST
        MOV CL, [BX][DI]+2
        MOV BX,4
        MOV DL, LIST[BX][DI]
        MOV LIST[BX+DI],3 
          
;DI=   ,BX=   ,CX=   ,DX=
;db 0 10
;
;DIRECT INDEXED ADRESSING
;DI=   ,AX=     ,AL=
;db 03 05
;

	  MOV DI,2
	  MOV AL,LIST[DI]			

;DI=   ,AX=
;db 03 05
;
;BASE RELATIVE ADDRESSING
;BX=  ,AX=
;db 0 10
;	    

	  MOV BX,OFFSET LIST
       MOV AX,[BX]+4
;BX=   ,AX=
       MOV AX,0
       MOV AX,[BX+4] 

;BX=   ,AX=
		        
        MOV AH, 04CH
       INT 21H
        
        RET
MY_PROG ENDP
MY_CODE ENDS
        END MY_PROG
