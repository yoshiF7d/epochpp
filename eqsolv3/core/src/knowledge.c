#include "knowledge.h"
/*====== print func ======*/
char *Knowledge_Expr_toString(Expr expr){
	if(!expr){return String_copy("null");}
	switch(expr->symbol->id){
	  case id_Blank:
		return Blank_toString(expr);
		break;
	  case id_Blank2:
		return Blank2_toString(expr);
		break;
	  case id_Blank3:
		return Blank3_toString(expr);
		break;
	  case id_Integer:
		return Integer_toString(expr);
		break;
	  case id_Lambda:
		return Lambda_toString(expr);
		break;
	  case id_Pattern:
		return Pattern_toString(expr);
		break;
	  case id_Real:
		return Real_toString(expr);
		break;
	  case id_String:
		return String_toString(expr);
		break;
	  case id_Node:
		return Node_toString(expr);
		break;
	  case id_PointerHead:
		return PointerHead_toString(expr);
		break;
	  default:
		break;
	}
	return String_copy(expr->symbol->name);
}
/*====== symbol doc ======*/
/*=== global ===*/
void doc_global(int id){
	if(id==id_All){
		printf(
			"                          All is a setting used for certain options. In Part and related functions,\n"
			"                          All specifies all parts at a particular level.\n"
		);
	}
	if(id==id_ArcTan){
		printf(
			"                          ArcTan[z] gives the arc tangent tan^-1(z) of the complex number z.\n"
			"                          Arctan[x,y] gives the arc tangent of y/x, taking into account which quadrant the point (x,y) is in.\n"
		);
	}
	if(id==id_ArrayDepth){
		printf(
			"                          ArrayDepth[expr] gives the depth to which expr is a full array, with all the parts at a particular\n"
			"                          level being lists of the same length, or is a SparseArray object.\n"
		);
	}
	if(id==id_ArrayQ){
		printf(
			"                          ArrayQ[expr] gives True if expr is a full array or a SparseArray object, and gives False otherwise.\n"
		);
	}
	if(id==id_AtomQ){
		printf(
			"                          AtomQ[expr] yields True if expr is an expression which cannot be divided into subexpressions, and yields False otherwise.\n"
		);
	}
	if(id==id_Attributes){
		printf(
			"                          Attributes[symbol] prints the list of attributes for a symbol.\n"
		);
	}
	if(id==id_Binomial){
		printf(
			"                          Binomial[n,m] gives the binomial coefficient n_C_m.\n"
		);
	}
	if(id==id_Blank){
		printf(
			"                          _ or Blank[] is a pattern object that can stand for any expression.\n"
			"                          _h or Blank[h] can stand for any expression with head h.\n"
		);
	}
	if(id==id_Blank2){
		printf(
			"                          __(two _ characters) or Blank2[] is a pattern object that can stand for any squence of one or more expressions.\n"
			"                          __h or Blank2[h] can stand for any sequence of one or more expressions, all of which have head h.\n"
		);
	}
	if(id==id_Blank3){
		printf(
			"                          ___(three _ characters) or Blank3[] is a pattern object that can stand for any squence of zero or more expressions.\n"
			"                          ___h or Blank3[h] can stand for any sequence of expressions, all of which have head h.\n"
		);
	}
	if(id==id_Bottom){
		printf(
			"                          Bottom is a symbol that represents the bottom for purposes of alignment and positioning.\n"
		);
	}
	if(id==id_Center){
		printf(
			"                          Center is a symbol that represents the center for purposes of alignment and positioning. \n"
		);
	}
	if(id==id_Column){
		printf(
			"                          Column[{expr1,expr2,...}] is an object that formats with the expri arranged in a column, with expr1 above expr2, etc.\n"
			"                          Column[list,alignment] aligns each element horizontally in the specified way.\n"
			"                          Column[list,alignmanet,spacing] leaves the specified number of x-heights of spacing between successive elements.\n"
		);
	}
	if(id==id_ComplexInfinity){
		printf(
			"                          ComplexInfinity represents a quantity with infinite magnitude,\n"
			"                          but undetermined complex phase.\n"
		);
	}
	if(id==id_Constant){
		printf(
			"                          Constant is an attibute that indicates zero derivative of a symbol with respect to all parameters.\n"
		);
	}
	if(id==id_Cos){
		printf(
			"                          Cos[z] gives the cosine of z.\n"
		);
	}
	if(id==id_Cross){
		printf(
			"                          Cross[a,b] gives the vector cross product of a and b.\n"
		);
	}
	if(id==id_Depth){
		printf(
			"                          Depth[expr] gives the maximum number of indices needed to specify any part of expr, plus 1.\n"
		);
	}
	if(id==id_Dimensions){
		printf(
			"                          Dimensions[expr] gives a list of the dimensions of expr.\n"
			"                          Dimensions[expr,n] gives a list of the dimensions of expr down to level n.\n"
		);
	}
	if(id==id_Divide){
		printf(
			"                          x/y or Divide[x,y] is equivalent to xy^-1.\n"
		);
	}
	if(id==id_Dot){
		printf(
			"                          a.b.c or Dot[a,b,c] gives products of vectors, matrices, and tensors.\n"
		);
	}
	if(id==id_E){
		printf(
			"                          E is the exponential constant e (base of natural lograithms), with numerical value ~ 2.71828.\n"
		);
	}
	if(id==id_Equal){
		printf(
			"                          SameQ[expr] yields yields True if the expression lhs is identical to rhs, and yields False otherwise.\n"
		);
	}
	if(id==id_Evaluate){
		printf(
			"                          Evaluate[expr] evaluates expression expr.\n"
		);
	}
	if(id==id_Exp){
		printf(
			"                          Exp[z] gives the exponential of z.\n"
		);
	}
	if(id==id_False){
		printf(
			"                          False is the symbol for the Boolean value false.\n"
		);
	}
	if(id==id_First){
		printf(
			"                          Last[expr] gives the first element in expr.\n"
		);
	}
	if(id==id_Flat){
		printf(
			"                          Flat is an attribute that can be assigned to a symbol f to indicate that all expressions\n"
			"                          involving nested functions f should be flattened out. This property is accounted for \n"
			"                          in pattern matching.\n"
		);
	}
	if(id==id_Flatten){
		printf(
			"                          flattens out nested lists.\n"
		);
	}
	if(id==id_FullForm){
		printf(
			"                          FullForm[expr] prints as the full form of expr, with no special syntax.\n"
		);
	}
	if(id==id_Grid){
		printf(
			"                          Grid[{expr11,expr12,..},{expr21,expr22,..},..}]\n"
			"                          is an object that formats with the exprij arranged in a two-dimensional grid. \n"
		);
	}
	if(id==id_Help){
		printf(
			"                          Help shows informations about all symbols in eqsolv3.\n"
			"                          ?symbol or Help[symbol] shows information about a symbol.\n"
			"                          Help[\"Context`\"] shows informations in the specified context.\n"
		);
	}
	if(id==id_HoldAll){
		printf(
			"                          HoldAll is an attibute which specifies that all arguments to a function are to be maintained\n"
			"                          in an unevaluated form.\n"
		);
	}
	if(id==id_HoldAllComplete){
		printf(
			"                          HoldAllComplete is an attibute which specifies that all arguments to be a\n"
			"                          function are not to be modified or looked at in any way in the process of evaluation.\n"
		);
	}
	if(id==id_HoldFirst){
		printf(
			"                          HoldFirst os an attibute which specifies that the first argument to a funtion is to be\n"
			"                          maintained in unevaluated form.\n"
		);
	}
	if(id==id_HoldRest){
		printf(
			"                          HoldRest is an attibute which specifies that all but the first argument to a function\n"
			"                          are to be maintained in an unevaluated form.\n"
		);
	}
	if(id==id_Increment){
		printf(
			"                          x++ increses the value of x by 1, returning the old value of x.\n"
		);
	}
	if(id==id_Indeterminate){
		printf(
			"                          Indeterminate is a symbol that represents a numerical quantity whose magnitude cannot be determined.\n"
		);
	}
	if(id==id_Infinity){
		printf(
			"                          Infinity or ∞ is a symbol that represents positive infinite quantity.\n"
		);
	}
	if(id==id_Inner){
		printf(
			"                          Inner[f,list1,list2,g] is a generalization of Dot in which f plays the role of multiplication\n"
			"                          and g of addition.\n"
		);
	}
	if(id==id_Integer){
		printf(
			"                          Integer is the head used for integers.\n"
		);
	}
	if(id==id_Lambda){
		printf(
			"                          Lambda[body] or body& is a pure function. The formal parameters are # (or #1), #2, etc.\n"
			"                          Lambda[x,body] is a pure function with a single formal parameter x.\n"
			"                          Lambda[{Subscript[x, 1],Subscript[x, 2],\[Ellipsis]},body] is a pure function with a list of formal parameters.\n"
		);
	}
	if(id==id_Last){
		printf(
			"                          Last[expr] gives the last element in expr.\n"
		);
	}
	if(id==id_Left){
		printf(
			"                          Left is a symbol that represents the left-hand side for purposes of alignment and positioning. \n"
		);
	}
	if(id==id_Length){
		printf(
			"                          Length[expr] gives the number of elements in expr.\n"
		);
	}
	if(id==id_List){
		printf(
			"                          {e1,e2,...} is a list of elements.\n"
		);
	}
	if(id==id_Listable){
		printf(
			"                          Listable is an attibtes that can be assigned to a symbol f to indicate that the\n"
			"                          function f should automatically be threaded over lists that appear as its arguments.\n"
		);
	}
	if(id==id_Locked){
		printf(
			"                          Locked is an attribute that, once assigned, prevents modification of any attibutes of a symbol.\n"
		);
	}
	if(id==id_Log){
		printf(
			"                          Log[z] gives the natural logarithm of z (logarithm to base e). \n"
		);
	}
	if(id==id_Map){
		printf(
			"                          Map[f,expr] or f/@expr applies f to each element on the first level in expr.\n"
			"                          Map[f,expr,levelspec] applies f to parts of expr specified by levelspec.\n"
		);
	}
	if(id==id_MapAll){
		printf(
			"                          MapAll[f,expr] or f//@expr applies f to each element on the first level in expr.\n"
		);
	}
	if(id==id_MatchQ){
		printf(
			"                          MatchQ[expr,form] returns True If the pattern form matches expr, and returns False otherwise.\n"
		);
	}
	if(id==id_MatrixForm){
		printf(
			"                          MatrixForm[list] prints with the elements of list arranged in a regular array.\n"
		);
	}
	if(id==id_Minus){
		printf(
			"                          -x is arithmetic negation of x.\n"
		);
	}
	if(id==id_N){
		printf(
			"                          N[expr] gives the numerical value of expr.\n"
			"                          N[expr,n] attempts to give a result with n-digit precision.\n"
		);
	}
	if(id==id_None){
		printf(
			"                          None is a setting used for certain options.\n"
		);
	}
	if(id==id_Null){
		printf(
			"                          Null is a symbol used to indicate the absence of an expression or a result.\n"
			"                          It is not displayed in ordinary output. \n"
			"                          When Null appears as a complete output expression, no output is printed.\n"
		);
	}
	if(id==id_NumberQ){
		printf(
			"                          NumberQ[expr] gives True if expr is a number, and False otherwise.\n"
		);
	}
	if(id==id_NumericFunction){
		printf(
			"                          NumericFunction is an attibute that can be assigned to a symbol f to indicate that \n"
			"                          f[arg_1,arg_2,...] should be considered a numeric quantity whenever all the arg_i are\n"
			"                          numeric qunatities.\n"
		);
	}
	if(id==id_OneIdentity){
		printf(
			"                          OneIdentity is an attibute that can be assigned to a symbol f to indicate that\n"
			"                          f[x],f[f[x]], etc. are all equivalent to x for the purpose of pattern matching.\n"
		);
	}
	if(id==id_Operators){
		printf(
			"                          Operators shows registered operators in a tree form.\n"
		);
	}
	if(id==id_Order){
		printf(
			"                          Order[expr1,expr2] gives 1 if expr1 is before expr2 in canonical order,\n"
			"                          and -1 if expr1 is after expr2 in canonical order.\n"
			"                          it gives 0 if expr1 is identical to expr2.\n"
		);
	}
	if(id==id_Orderless){
		printf(
			"                          Orderless is an attribute that can be assigned to a symbol f to indicate that the\n"
			"                          elements e_i in expressions of the form f[e_1,e_2,...] should automatically be\n"
			"                          sorted into canonical order. This property is accounted for in pattern matching.\n"
		);
	}
	if(id==id_OutputForm){
		printf(
			"                          OutputForm[expr] prints as a two-dimensional representation of expr\n"
			"                          using only keyboard characters.\n"
		);
	}
	if(id==id_Part){
		printf(
			"                          expr[[i]] or Part[expr,i] gives the ith part of expr.\n"
			"                          expr[[-i]] counts from the end.\n"
			"                          expr[[i,j,...]] or Part[expr,i,j,...] is equivalent to expr[[i]][[j]].... \n"
			"                          expr[[{i1,i2,...}]] gives a list of the parts i1, i2, ... of expr. \n"
			"                          expr[[m;;n]] gives parts m through n.\n"
			"                          expr[[m;;n;;s]] gives parts m through n in steps of s.\n"
		);
	}
	if(id==id_Pattern){
		printf(
			"                          s:obj represents the pattern object obj, assigned the name s.\n"
		);
	}
	if(id==id_Pi){
		printf(
			"                          Pi is π, with numerical value ~ 3.14159.\n"
		);
	}
	if(id==id_Plus){
		printf(
			"                          x + y + z represents a sum of terms.\n"
		);
	}
	if(id==id_Postfix){
		printf(
			"                          Postfix[expr,f] or expr//f evaluates to f[expr].\n"
		);
	}
	if(id==id_Power){
		printf(
			"                          x^y gives x to the power y.\n"
		);
	}
	if(id==id_Precision){
		printf(
			"                          Precision[x] gives the effective number of digits of precision in the number x\n"
		);
	}
	if(id==id_Prefix){
		printf(
			"                          Prefix[expr,f] or f@expr evaluates to f[expr].\n"
		);
	}
	if(id==id_PreIncrement){
		printf(
			"                          ++x increses the value of x by 1, returning the new value of x.\n"
		);
	}
	if(id==id_Product){
		printf(
			"                          Product[f,{i,i_max}] evaluates the product.\n"
			"                          Product[f,{i,i_min,i_max}] starts with i = i_min.\n"
			"                          Product[f,[i,i_min,i_max,di}] uses steps di.\n"
			"                          Product[f,{i,{i1,i2,...} uses successive values i1,i2,... .\n"
			"                          Product[f,{i,i_min,i_max},{j,j_min,j_max},...] evaluates the multiple product.\n"
			"                          Product[f,i] gives the indefinite product.\n"
		);
	}
	if(id==id_Protected){
		printf(
			"                          Protected is an attibutes which prevents any values associated with a symbol from being modified.\n"
		);
	}
	if(id==id_Quit){
		printf(
			"                          Quit this program.\n"
		);
	}
	if(id==id_RandomInteger){
		printf(
			"                          RandomInteger[{imin,imax}] gives a pseudorandom integer in the range {imin,imax}.\n"
			"                          RandomInteger[imax] gives a pseudorandom integer in the range {0,...,imax}.\n"
			"                          RandomInteger[] pseudorandomly gives 0 or 1.\n"
			"                          RandomInteger[range,n] gives a list of n pseudorandom integers.\n"
			"                          RandomInteger[range,{n1,n2,...}] gives an n1*n2*... array of pseudorandom integers.\n"
		);
	}
	if(id==id_RandomReal){
		printf(
			"                          RandomReal[] gives a pseudorandom real number in the range 0 to 1.\n"
			"                          RandomReal[{xmin,xmax}] gives a psuedorandom real number in the range xmin to xmax.\n"
			"                          RandomReal[xmax] gives a pseudorandom real number in the range 0 to xmax.\n"
			"                          RandomReal[range,n] gives a list of n pseudorandom reals.\n"
			"                          RandomReal[range,{n1,n2,...}] gives an n1xn2x... array of pseudorandom reals.\n"
		);
	}
	if(id==id_ReadProtected){
		printf(
			"                          ReadProtected is an attibute that prevents values associated with a symbol from being seen.\n"
		);
	}
	if(id==id_Real){
		printf(
			"                          Real is the head used for reals.\n"
		);
	}
	if(id==id_Replace){
		printf(
			"                          Replace[expr,rules] applies a rule or list of rules in an attempt to transform the entire expression expr.\n"
			"                          Replace[expr,rules,levelspec] applies rules to parts of expr specified by levelspec.\n"
		);
	}
	if(id==id_ReplaceAll){
		printf(
			"                          expr /. rules applies a rule or list of rules in an attempt to transform each subpart of an expression expr.\n"
		);
	}
	if(id==id_Right){
		printf(
			"                          Right is a symbol that represents the right-hand side for purposes of alignment and positioning. \n"
		);
	}
	if(id==id_Row){
		printf(
			"                          Row[{expr1,expr2,...}] is an object that formats with the expri arranged in a row, potentially extending over several lines.\n"
			"                          Row[list,s] inserts s as a separator between successive elements.\n"
		);
	}
	if(id==id_Rule){
		printf(
			"                          lhs->rhs represents a rule that transforms lhs to rhs.\n"
		);
	}
	if(id==id_SameQ){
		printf(
			"                          SameQ[expr] yields yields True if the expression lhs is identical to rhs, and yields False otherwise.\n"
		);
	}
	if(id==id_ScientificForm){
		printf(
			"                          ScientificForm[expr] prints with all real numbers in expr given in scientific notation.\n"
			"                          ScientificForm[expr,n] prints with numbers given to n-digit precision. \n"
		);
	}
	if(id==id_SeedRandom){
		printf(
			"                          SeedRandom[n] resets the psudorandom generator, using n as a seed.\n"
			"                          SeedRandom[] resets the generator, using as a seed the time of day and certain attributes of the current Mathematica session.\n"
		);
	}
	if(id==id_Sequence){
		printf(
			"                          Sequence[expr1,expr2,...] represents a sequence of arguments to be spliced automatically into any function.\n"
		);
	}
	if(id==id_SequenceHold){
		printf(
			"                          SequenceHold is an attibute that specifies that Sequence objects appearing in the arguments of\n"
			"                          a function should not automatically be flattened out.\n"
		);
	}
	if(id==id_Set){
		printf(
			"                          lhs = rhs evaluates rhs and assigns the result to be the value of lhs.\n"
			"                          From then on, lhs is replaced by rhs whenever it appears.\n"
			"                          {l1,l2,...} = {r1,r2,...} evaluates the ri, and assigns the results to be the values of the corresponding li.\n"
		);
	}
	if(id==id_SetDelayed){
		printf(
			"                          From then on, lhs is replaced by rhs whenever it appears.\n"
			"                          {l1,l2,...} := {r1,r2,...} evaluates the ri, and assigns the results to be the values of the corresponding li.\n"
		);
	}
	if(id==id_Sign){
		printf(
			"                          Sign[expr] gives -1,0, or 1 depending on whether expr is negative, zero, or positive.\n"
		);
	}
	if(id==id_Sin){
		printf(
			"                          Sin[z] gives the sine of z.\n"
		);
	}
	if(id==id_Slot){
		printf(
			"                          # represents the first argument supplied to a pure function.\n"
			"                          # n represents the nth argument.\n"
		);
	}
	if(id==id_SlotSequence){
		printf(
			"                          ## represents the sequence of arguments supplied to a pure function. \n"
			"                          ## n represents the sequence of arguments supplied to a pure function,\n"
			"                          starting with the nth argument. \n"
		);
	}
	if(id==id_Sort){
		printf(
			"                          Sort[list] sorts the elements of list into canonical order.\n"
			"                          Sort[list,p] sorts using the ordering function p.\n"
		);
	}
	if(id==id_Span){
		printf(
			"                          i;;j represents a span of elements i through j.\n"
			"                          i;; represents a span from i to the end.\n"
			"                          ;;j represents a span from the beginning to j.\n"
			"                          ;; represents a span that includes all elements.\n"
			"                          i;;j;;k represents a span from i through j in steps of k.\n"
			"                          i;; ;;k represents a span from i to the end in steps of k.\n"
			"                          ;;j;;k represents a span from the beginning to j in steps of k.\n"
			"                          ;; ;;k represents a span from the beginning to the end in steps of k.\n"
		);
	}
	if(id==id_Sqrt){
		printf(
			"                          Sqrt[z] or ^/z gives square root of z.\n"
		);
	}
	if(id==id_String){
		printf(
			"                          String is the head of a character string \"text\" \n"
		);
	}
	if(id==id_Subtract){
		printf(
			"                          x - y is equivalent to x + (-1 * y).\n"
		);
	}
	if(id==id_Sum){
		printf(
			"                          Sum[f,{i,i_max}] evaluates the sum.\n"
			"                          Sum[f,{i,i_min,i_max}] starts with i = i_min.\n"
			"                          Sum[f,[i,i_min,i_max,di}] uses steps di.\n"
			"                          Sum[f,{i,{i1,i2,...} uses successive values i1,i2,... .\n"
			"                          Sum[f,{i,i_min,i_max},{j,j_min,j_max},...] evaluates the multiple sum.\n"
			"                          Sum[f,i] gives the indefinite sum.\n"
		);
	}
	if(id==id_Symbol){
		printf(
			"                          Symbol[\"name\"] refers to a symbol with the specified name. \n"
		);
	}
	if(id==id_Table){
		printf(
			"                          Table[expr,{i_max}] generates a list of i_max copies of expr.\n"
			"                          Table[expr,{i,i_max}] generates a list of the values of expr when i runs from 1 to i_max.\n"
			"                          Table[expr,{i,i_min,i_max}] starts with i=i_min.\n"
			"                          Table[expr,{i,i_min,i_max,di}] uses steps di.\n"
			"                          Table[expr,{i,{i_1, i_2, ...}}] uses the successive values i_1,i_2,...\n"
			"                          Table[expr,{i,i_min,i_max},{j,j_min,j_max}, ...] gives a nested list. The list associated with i is outermost.\n"
		);
	}
	if(id==id_Tan){
		printf(
			"                          Tan[z] gives the tangent of z.\n"
		);
	}
	if(id==id_Temporary){
		printf(
			"                          Temporary is an attribute assigned to symbols which are created as local variables by Module.\n"
		);
	}
	if(id==id_Thread){
		printf(
			"                          Thread[f[args]] threads f over any lists that appear in args.\n"
			"                          Thread[f[args],h] threads f over any objects with head h that appear in args.\n"
			"                          Thread[f[args],h,n] threads f over objects with head h that appear in the first n args.\n"
		);
	}
	if(id==id_Times){
		printf(
			"                          x*y*z or x y z represents a product of terms.\n"
		);
	}
	if(id==id_Top){
		printf(
			"                          Top is a symbol that represents the top for purposes of alignment and positioning.\n"
		);
	}
	if(id==id_TreeForm){
		printf(
			"                          TreeForm[expr] displays expr as a tree with different levels at different depths.\n"
		);
	}
	if(id==id_TreeHorizontalForm){
		printf(
			"                          TreeHorizontalForm[expr] displays expr as a horizontal tree with different levels at different depths.\n"
		);
	}
	if(id==id_True){
		printf(
			"                          True is the symbol for the Boolean value true.\n"
		);
	}
}

/*=== internal ===*/
void doc_internal(int id){
	if(id==id_DebugForm){
		printf(
			"                          DebugForm[expr] prints as the debug form of expr.\n"
		);
	}
	if(id==id_EvaluateBuiltinRules){
		printf(
			"                          EvaluateBuiltinRules[expr] evaluates expression expr without using builtin rules in order to evaluates builtin rules itself.\n"
		);
	}
	if(id==id_Node){
		printf(
			"                          Node is a internal symbol which is used as a temporary node in parsing phase.\n"
		);
	}
	if(id==id_PatternB){
		printf(
			"                          PatternB is a internal symbol used to convert symb_expr to Pattern[symb,Blank[expr]],\n"
			"                          and symb__ to Pattern[symb,Blank] in parsing phase.\n"
		);
	}
	if(id==id_PatternB2){
		printf(
			"                          PatternB2 is a internal symbol used to convert symb__expr to Pattern[symb,Blank2[expr]],\n"
			"                          and symb__ to Pattern[symb,Blank2] in parsing phase.\n"
		);
	}
	if(id==id_PatternB3){
		printf(
			"                          PatternB3 is a internal symbol used to convert symb___expr to Pattern[symb,Blank3[expr]],\n"
			"                          and symb___ to Pattern[symb,Blank3]in parsing phase.\n"
		);
	}
	if(id==id_Pointer){
		printf(
			"                          Pointer is a internal symbol used in matching function to store submatch expression.\n"
		);
	}
	if(id==id_PointerHead){
		printf(
			"                          PointerHead is a internal symbol used to represent head of expression consists of multiple symbols.\n"
		);
	}
	if(id==id_PreSymbolMark){
		printf(
			"                          PreSymbolMark is a internal symbol which is used to represent special symbol.\n"
		);
	}
	if(id==id_SymbolMark){
		printf(
			"                          PreSymbolMark is a internal symbol which is used to represent special symbol.\n"
		);
	}
}

/*=== physics ===*/
void doc_physics(int id){
	if(id==id_P){
		P_doc(id);
	}
	if(id==id_Wce){
		printf(
			"                          Electron cycroton angular frequency.\n"
			"                          usage : Wce/.B->(value of magnetic field strength [T])\n"
		);
	}
	if(id==id_Wpe){
		printf(
			"                          Electron plasma angular frequency.\n"
			"                          usage : Wpe/.n->(value of electron density [/cc])\n"
			"                          usage2 : Wpe[(value of electron density [/cc])]\n"
		);
	}
}

/*=== user ===*/
void doc_user(int id){
	if(id==id_Cow){
		printf(
			"                          Cow[] displays cow.\n"
		);
	}
	if(id==id_Fish){
		printf(
			"                          Fish[] displays fish.\n"
		);
	}
	if(id==id_Hello){
		printf(
			"                          Hello[] displays greeting message.\n"
		);
	}
	if(id==id_Hi){
		printf(
			"                          Hi[] displays greeting message.\n"
		);
	}
	if(id==id_Matrix){
		printf(
			"                          Matrix[delay] shows the activity of the virtual reality environment of the Matrix.\n"
			"                          Press 'q' to quit.\n"
		);
	}
	if(id==id_Neko){
		printf(
			"                          Neko[] displays neko.\n"
		);
	}
	if(id==id_Pokemon){
		printf(
			"                          Pokemon[name] shows Pokemon.\n"
		);
	}
	if(id==id_Train){
		printf(
			"                          Train[delay,options] shows a steam locomotive. delay time determines the speed of the train.\n"
			"                          options are \"Logo\",\"Accident\",\"Fly\" and \"C51\".\n"
			"                          option \"Logo\" shows long sl.\n"
			"                          option \"Accident\" shows sl with some accident.\n"
			"                          option \"Fly\" shows flying sl.\n"
			"                          option \"C51\" shows C51 type sl. \n"
		);
	}
	if(id==id_Yojyo){
		printf(
			"                          Yojyo[] displays yojyo.\n"
		);
	}
}

/*static field create function*/
void *static_create(int id){
	switch(id){
	  case id_P:
		return P_static_create();
	  default:
		break;
	}
	return NULL;
}

/*static field delete function*/
void static_delete(void *data, int id){
	switch(id){
	  case id_P:
		P_static_delete(data);
		break;
	  default:
		break;
	}
}

#define SYMBOL_APPEND(name,func,attributes,precedence,associativity,sign,builtinrules,alias) \
table = HashTable_append(table,#name,symbol=Symbol_create(#name,id_##name,func,attributes,associativity,precedence,sign,builtinrules,alias));\
idtable[id_##name]=symbol;\
nametable[id_##name]=#name;

Symbol symbol;
SymbolTable SymbolTable_global(Symbol idtable[], char *nametable[]){
	SymbolTable table = HashTable_create2(512,Symbol_vtoString);
	SYMBOL_GLOBAL(SYMBOL_APPEND)
	return table;
}
Symbol symbol;
SymbolTable SymbolTable_internal(Symbol idtable[], char *nametable[]){
	SymbolTable table = HashTable_create2(512,Symbol_vtoString);
	SYMBOL_INTERNAL(SYMBOL_APPEND)
	return table;
}
Symbol symbol;
SymbolTable SymbolTable_physics(Symbol idtable[], char *nametable[]){
	SymbolTable table = HashTable_create2(512,Symbol_vtoString);
	SYMBOL_PHYSICS(SYMBOL_APPEND)
	return table;
}
Symbol symbol;
SymbolTable SymbolTable_user(Symbol idtable[], char *nametable[]){
	SymbolTable table = HashTable_create2(512,Symbol_vtoString);
	SYMBOL_USER(SYMBOL_APPEND)
	return table;
}
