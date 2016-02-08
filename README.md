Your program will construct a type environment, and then perform unifications in that type environment, until either the program ends, or a unification fails. 


TYPE ::= TYPEVAR | PRIMITIVE_TYPE | FUNCTYPE | LISTTYPE;
PRIMITIVE_TYPE ::= �int� | �float� | �long� | �string�;
TYPEVAR ::= �`� VARNAME; 
VARNAME ::= [a-zA-Z][a-zA-Z0-9]*;
FUNCTYPE ::= �(� ARGLIST �)� -> TYPE | �(� �)� -> TYPE;
ARGLIST ::=   TYPE �,� ARGLIST | TYPE;
LISTTYPE ::= �[� TYPE �]�;
