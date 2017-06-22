#include "knowledge.h"
#include "data.h"
/*====== data initialization function ======*/
void data_init(union un_data *data, int id){
	switch(id){
	  case id_Blank:
		Blank_data_init(data);
		break;
	  case id_Blank2:
		Blank2_data_init(data);
		break;
	  case id_Blank3:
		Blank3_data_init(data);
		break;
	  case id_Integer:
		Integer_data_init(data);
		break;
	  case id_Lambda:
		Lambda_data_init(data);
		break;
	  case id_Pattern:
		Pattern_data_init(data);
		break;
	  case id_Real:
		Real_data_init(data);
		break;
	  case id_String:
		String_data_init(data);
		break;
	  case id_Node:
		Node_data_init(data);
		break;
	  case id_Pointer:
		Pointer_data_init(data);
		break;
	  case id_PointerHead:
		PointerHead_data_init(data);
		break;
	  default:
		memset(data,0,sizeof(union un_data));
	}
}
/*====== data finish function ======*/
void data_finish(union un_data *data,int id){
	switch(id){
	  case id_Integer:
		Integer_data_finish(data);
		break;
	  case id_Lambda:
		Lambda_data_finish(data);
		break;
	  case id_Pattern:
		Pattern_data_finish(data);
		break;
	  case id_Real:
		Real_data_finish(data);
		break;
	  case id_String:
		String_data_finish(data);
		break;
	  case id_Pointer:
		Pointer_data_finish(data);
		break;
	  case id_PointerHead:
		PointerHead_data_finish(data);
		break;
	}
}
/*====== data copy function ======*/
void data_copy(union un_data *datas,union un_data *datad,int id){
	switch(id){
	  case id_Blank:
		Blank_data_copy(datas,datad);
		break;
	  case id_Blank2:
		Blank2_data_copy(datas,datad);
		break;
	  case id_Blank3:
		Blank3_data_copy(datas,datad);
		break;
	  case id_Integer:
		Integer_data_copy(datas,datad);
		break;
	  case id_Lambda:
		Lambda_data_copy(datas,datad);
		break;
	  case id_Pattern:
		Pattern_data_copy(datas,datad);
		break;
	  case id_Real:
		Real_data_copy(datas,datad);
		break;
	  case id_String:
		String_data_copy(datas,datad);
		break;
	  case id_Node:
		Node_data_copy(datas,datad);
		break;
	  case id_Pointer:
		Pointer_data_copy(datas,datad);
		break;
	  case id_PointerHead:
		PointerHead_data_copy(datas,datad);
		break;
	  default:
		memcpy(datad,datas,sizeof(union un_data));
		break;
	}
}
/*====== data equals function ======*/
int data_equals(union un_data *d1,union un_data *d2,int id){
	switch(id){
	  case id_Integer:
		return Integer_data_equals(d1,d2);
	  case id_Real:
		return Real_data_equals(d1,d2);
	  case id_String:
		return String_data_equals(d1,d2);
	}
	return 1;
}
