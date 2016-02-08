Your program will construct a type environment, and then perform unifications in that type environment, until either the program ends, or a unification fails. 


TYPE ::= TYPEVAR | PRIMITIVE_TYPE | FUNCTYPE | LISTTYPE;
PRIMITIVE_TYPE ::= ÔintÕ | ÔfloatÕ | ÔlongÕ | ÔstringÕ;
TYPEVAR ::= Ô`Õ VARNAME; 
VARNAME ::= [a-zA-Z][a-zA-Z0-9]*;
FUNCTYPE ::= Ô(Ô ARGLIST Ô)Õ -> TYPE | Ô(Ô Ô)Õ -> TYPE;
ARGLIST ::=   TYPE Ô,Õ ARGLIST | TYPE;
LISTTYPE ::= Ô[Ô TYPE Ô]Õ;
