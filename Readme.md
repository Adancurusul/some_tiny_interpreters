# Some tiny interpreters
## Brief introduction 
This project offers two very portable language interpreters :one is basic ,the other is brainfuck.    

The interpreters can be poted to any platform and  take up very little memory ,__only need to support "printf"function and c language compiler.__  

Here list some platforms that has already support : <u>  

gd32vf103(riscv) ,stm32(arm) ,stc89c52(8051) ,k210(riscv) ,prv332(riscv) and all platforms with ,linux mac win system</u>  

By the way ,that brainfuck interpreter was just  for fun,I don't think anyone would be glad to write it(really not made for people)

## How to port it to your device

>Firstly you platform need to support printf and c language compiler.Then you need to change your code into array(any way is ok we just want an array of code).You can refer to the ___use.c___ .It's necessay to init interpreter by using your code array as a parameter of ___interpreter_init(code_array)___ .At last ,just call ___do_interpretation();___ ,the ___interpreter_finished()___ function returns 1 when the interpreter  end.
## The syntax of this programming language


To be updated

## Math Libraries

For 0.0.4 we can use _pow,powf,sqrt,exp,log,ln,sin,cos,tan,arctan,artsin,arccos,sinh,consh,tanh_ 

## Updates

>v0.0.1 :Only integers are supported, and only 26 English letters are used as variables  
v0.0.2 :Only support integers, but it can be named in any C language variable name(a lot of bugs  )  
v0.0.3 :Supports double-precision floating-point arithmetic (a lot of bugs  )  
v0.0.4 :Supports pow,powf,sqrt,exp,log,ln,sin,cos,tan,arctan,artsin,arccos,sinh,consh,tanh math library operation



## Write at the end

This project is still under development so there are many bugs , if you have some advice email me: chen.yuheng@nexuslink.cn
