# FoxXML
## Intro
An XML esolang written in cpp.

Hi its really late and right before the deadline so sorry if this doesnt make sense, please go to the examples folder for actual examples.

## Tags

| tag     	| use                                                                                                                                                                         	|
|---------	|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------	|
| PROGRAM 	| a variable scope, with a name.<br>uses name attribute.<br>uses logging attribute.<br>can be jumped to like a function with call.<br>adds the scope to the program stack.    	|
| CALL    	| jumps instruction pointer to the program and pops the program stack.<br>uses name attribute.<br>prioritizes name variable.                                                  	|
| QUIT    	| exits all execution.                                                                                                                                                        	|
| ARG     	| feeds in from argv from c. <br>uses index attribute.<br>parent can see value.                                                                                               	|
| SET     	| set a variable in the current scope.<br>uses name attribute.<br>uses value attribute.<br>uses type attribute.<br>prioritizes value from children.<br>can be used by parent. 	|
| VAR     	| parent can see a variables value.<br>uses name attribute.                                                                                                                   	|
| PRINTLN 	| prints child value and a new line                                                                                                                                           	|
| ADDINT  	| only int operation.<br>uses val1 attribute.<br>uses val2 attribute.<br>adds the ints.                                                                                       	|

## Styling

Its reccomended that you add CSS.

```xml
<style>
    PROGRAM {
        background-color: black;
        color: green;
    }
</style>
```
