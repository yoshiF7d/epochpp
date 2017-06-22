#include "knowledge.h"
Form Form_create(char *string){
	Form form;
	if(!string){return NULL;}
	form = allocate(sizeof(Form_Sub));
	form->tag = FormS;
	form->content.string = string;
	form->height = 1;
	form->width = strlen(string);
	form->level = 0;
	form->next = NULL;
	return form;
}

Form Form_createForm(int tag, Form form2){
	Form form = allocate(sizeof(Form_Sub));
	
	form->tag = tag;
	form->content.head = form2;
	form->height = form2->height;
	form->width = form2->width;
	form->level = form2->level;
	form->next = NULL;
	return form;
}

void Form_delete(Form form){
	if(!form){return;}
	if(form->next){Form_delete(form->next);}
	switch(form->tag){
	  case FormS:
		deallocate(form->content.string);
		break;
	  case FormV:
	  case FormH:
		Form_delete(form->content.head);
		break;
	}
	deallocate(form);
}

void Form_lift(Form form, int level){
	Form f;
	if(!form){return;}
	switch(form->tag){
	  case FormS:
		form->level += level;
		break;
	  case FormH:
	  case FormV:
		form->level += level;
		for(f=form->content.head;f;f=f->next){
			Form_lift(f,level);
		}
	}
}

void Form_setLevel(Form form,int level,int base){
	Form f;
	if(!form){return;}
	switch(form->tag){
	  case FormS:
		form->level = level;
		break;
	  case FormH:
		for(f=form->content.head;f;f=f->next){
			Form_setLevel(f,level,base);
		}
	  case FormV:
		for(f=form->content.head;f;f=f->next){
			Form_lift(f,level-base);
		}
	}
}

Form Form_append(Form form, Form form2){
	Form f;
	if(!form){return form2;}
	for(f=form;f->next;f=f->next){}
	f->next = form2;
	return form;
}

Form Form_appendChild(Form form, Form form2){
	if(!form){return form2;}
	if(!form2){return form;}
	switch(form->tag){
	  case FormS:
		form = Form_appendChild(Form_createForm(FormH,form),form2);
		break;
	  case FormV:
		form->content.head = Form_append(form->content.head,form2);
		if(form2->width > form->width){
			form->width = form2->width;
		}
		form->height += form2->height;
		break;
	  case FormH:
		form->content.head = Form_append(form->content.head,form2);
		if(form2->height > form->height){
			form->height = form2->height;
		}
		form->width += form2->width+1;
		break;
	}
	return form;
}

Form Form_appendHorizontal(Form form, Form form2){
	int level,h,h2;
	if(!form){
		if(!form2){return NULL;}
		if(form2->tag != FormH){
			form2 = Form_createForm(FormH,form2);
		}
		return form2;
	}
	if(!form2){
		if(form->tag != FormH){
			form = Form_createForm(FormH,form);
		}
		return form;
	}
	
	switch(form->tag){
	  case FormV:
	  case FormS:
		form = Form_appendHorizontal(Form_createForm(FormH,form),form2);
		break;
	  case FormH:
		h = form->height;
		h2 = form2->height;
		
		if(form->level > form2->level){
			h2 += form->level-form2->level;
			Form_lift(form2,form->level-form2->level);
		}else{
			h += form2->level-form->level;
			Form_lift(form,form2->level-form->level);
		}
		form->content.head = Form_append(form->content.head,form2);
		//fprintf(stderr,"h : %d\n",h);
		//fprintf(stderr,"h2 : %d\n",h2);
		form->height = (h > h2) ? h : h2;
		form->width += form2->width+1;
		break;
	}
	return form;
}

Form Form_appendVertical(Form form, Form form2){
	if(!form){
		if(!form2){return NULL;}
		if(form2->tag != FormV){
			form2 = Form_createForm(FormV,form2);
		}
		return form2;
	}
	if(!form2){
		if(form->tag != FormV){
			form = Form_createForm(FormV,form);
		}
		return form;
	}
	
	switch(form->tag){
	  case FormH:
	  case FormS:
		form = Form_appendVertical(Form_createForm(FormV,form),form2);
		break;
	  case FormV:
		form->content.head = Form_append(form->content.head,form2);
		if(form2->width > form->width){
			form->width = form2->width;
		}
		Form_lift(form2,form->height);
		form->height += form2->height;
		break;
	}
	return form;
}

Form Form_appendVerticalCenter(Form form, Form form2){
	if(form && form2){
		if(form2->width > form->width){
			form = Form_appendHorizontal(Form_create(repeatedString(' ',(form2->width-form->width)/2-1)),form);
		}else if(form2->width < form->width){
			form2 = Form_appendHorizontal(Form_create(repeatedString(' ',(form->width-form2->width)/2-1)),form2);
		}
	}
	return Form_appendVertical(form,form2);
}

Form Form_appendVerticalRight(Form form, Form form2){
	if(form && form2){
		if(form2->width > form->width){
			form = Form_appendHorizontal(Form_create(repeatedString(' ',(form2->width-form->width)-1)),form);
		}else if(form2->width < form->width){
			form2 = Form_appendHorizontal(Form_create(repeatedString(' ',(form->width-form2->width)-1)),form2);
		}
	}
	return Form_appendVertical(form,form2);
}


Form Form_appendHorizontalCenter(Form form, Form form2){
	return Form_appendHorizontal(form,form2);
}

Form strip(Form form){
	Form f,fs,fe;
	if(form->tag==FormH){
		fs = form->content.head;
		if(fs->tag==FormS){
			if(!strcmp(fs->content.string,"(")){
				for(f=fs;f->next;f=f->next){fe = f;}
				f = fe;
				fe = fe->next;
				if(fe->tag==FormS){
					if(!strcmp(fe->content.string,")")){
						form->width -= 4;
						form->content.head = fs->next;
						f->next = NULL;
						deallocate(fs->content.string);
						deallocate(fe->content.string);
						deallocate(fs);
						deallocate(fe);
					}
				}
			}
		}
	}
	return form;
}

Form Form_super(Form b, Form s){
	s = Form_appendHorizontal(Form_create(repeatedString(' ',b->width)),strip(s));
	b = Form_appendVertical(s,b);
	b->level = s->level+1;
	return b;
}

Form Form_sub(Form b, Form s){
	s = Form_appendHorizontal(Form_create(repeatedString(' ',b->width)),strip(s));
	b = Form_appendVertical(b,s);
	b->level = s->level-1;
	return b;
}

void Form_post(Form form, char *str){
	Form f,g;
	char *buf;
	int len = strlen(str);
	
	for(f=form;;){
		switch(f->tag){
		  case FormS:
			f->width+=len;
			buf = String_join(f->content.string,str);
			deallocate(f->content.string);
			f->content.string = buf;
			return;
		  case FormH:
			if(f->next){
				f=f->next;
			}else{
				f->width+=len;
				f=f->content.head;
			}
			break;
		  case FormV:
			for(g=f;g;g=g->next){f->width+=len;}
			f=f->content.head;
			break;
		}
	}
}

void Form_pre(Form form, char *str){
	Form f,g;
	char *buf;
	int len = strlen(str);

	for(f=form;;){
		switch(f->tag){
		  case FormS:
			f->width+=len;
			buf = String_join(str,f->content.string);
			deallocate(f->content.string);
			f->content.string = buf;
			return;
		  case FormH:
			f->width+=len;
			f=f->content.head;
			break;
		  case FormV:
			for(g=f;g;g=g->next){f->width+=len;}
			f=f->content.head;
			break;
		}
	}
}
/*
int isminus(Expr expr){
	if(!expr){return 0;}
	if(expr->symbol->id == id_Times){
		if(expr->child->symbol->id==id_Integer){
			if(expr->child->prec <= I32_LIMIT){
				if(expr->child->data.integer32 == -1){
					return 1;
				}
			}
		}
	}
	return 0;
}
*/
int stripminus(Form form){
	Form f,fs;
	if(form->tag==FormH){
		fs = form->content.head;
		if(fs->tag==FormS){
			if(!strcmp(fs->content.string,"-")){
				if(fs->next){
					form->content.head = fs->next;
					form->width -= 2;
					deallocate(fs->content.string);
					deallocate(fs);
				}else{
					deallocate(fs->content.string);
					fs->content.string=String_copy("1");
				}
				return 1;
			}
		}
	}
	return 0;
}

Form Form_makeTerm(Expr expr, int flag);
Form Form_makePower(Expr expr, int flag);

Form Form_frac(Form num, Form den, int flag){
	Form bar;
	int len;
	if(!den){return num;}
	if(!num){num = Form_create(String_copy("1"));}
	
	num = strip(num);
	den = strip(den);
	
	//mflag =	stripminus(num) ^ stripminus(den);
	
	if(flag & fm_sub || flag & fm_oneline){
		num = Form_appendHorizontal(num,Form_create(String_copy("/")));
		num = Form_appendHorizontal(num,den);
	}else{
		if(num->width > den->width){
			len = num->width;
			den = Form_appendHorizontal(Form_create(repeatedString(' ',(num->width - den->width)/2-1)),den);
		}else if(num->width < den->width){
			len = den->width;
			num = Form_appendHorizontal(Form_create(repeatedString(' ',(den->width - num->width)/2-1)),num);
		}else{
			len = num->width;
		}
		num = Form_appendVertical(NULL,num);
		num = Form_appendVertical(num,bar=Form_create(repeatedString('-',len)));
		num = Form_appendVertical(num,den);
		num->level = bar->level;
	}
	//if(mflag){num = Form_appendHorizontal(Form_create(String_copy("-")),num);}
	return num;
}



Form Form_make(Expr expr, int flag){
	Expr e;
	Form f=NULL,term=NULL;
	char *head=NULL,*lp=NULL,*rp=NULL,*delim=NULL;
	int parp;
	
	if(!expr){printf("No Form\n"); return NULL;}
	if(expr->parent){
		parp=expr->parent->symbol->precedence;
		if(parp > expr->symbol->precedence && parp < 670){flag |= fm_par;}
	}
	if(!(flag & fm_nosign) && Sign_mod(expr)<0){
		flag |= fm_nosign;
		f = Form_create(String_copy("-"));
	}
	/*=== thin veneer code ===*/
	if(expr->symbol->id==id_Lambda){
		if(flag & fm_nosign){f = Form_appendHorizontal(f,Form_make(expr->data.lambda.body,flag));}
		else{f = Form_make(expr->data.lambda.body,flag);}
		f = Form_appendHorizontal(f,Form_create(String_copy("&")));
		if(expr->child){lp = "["; rp = "]"; delim = ",";}
		goto main;
	}
	if(expr->symbol->id==id_PointerHead){
		if(flag & fm_nosign){
			f = Form_appendHorizontal(f,Form_create(String_copy("(")));
			f = Form_appendHorizontal(f,Form_make(expr->data.expr,flag));
		}else{
			f = Form_appendHorizontal(Form_create(String_copy("(")),Form_make(expr->data.expr,flag));
		}
		f = Form_appendHorizontal(f,Form_create(String_copy(")")));
		if(expr->child){lp = "["; rp = "]"; delim = ",";}
		goto main;
	}
	/*=== thin veneer code ===*/
	if(expr->child){
		if(expr->symbol->id==id_Times){
			if(flag & fm_nosign){return Form_appendHorizontal(f,Form_makeTerm(expr->child,flag));}
			else{return Form_makeTerm(expr->child,flag);}
		}else if(expr->symbol->id==id_Power){
			if(flag & fm_nosign){return Form_appendHorizontal(f,Form_makePower(expr,flag));}
			else{return Form_makePower(expr,flag);}
		}else if(expr->symbol->id==id_Divide){
			if(flag & fm_nosign){
				return Form_appendHorizontal(f,
					Form_frac(
						Form_make(expr->child,flag & ~ fm_par),
						Form_make(expr->child->next,flag & ~ fm_par),
						flag & ~ fm_par)
					);
			}else{
				return Form_frac(
					Form_make(expr->child,flag & ~ fm_par),
					Form_make(expr->child->next,flag & ~ fm_par),
					flag & ~ fm_par);
			}
		}else{
			delim = expr->symbol->sign;
			if(expr->symbol->id==id_List){
				lp = "{";
				rp = "}";
				delim = ",";
			}else if(!delim || (expr->flag & ef_func)){
				head = Expr_toString(expr);
				lp = "[";
				rp = "]";
				delim = ",";
			}else if(flag & fm_par){
				lp = "(";
				rp = ")";
			}
		}
	}else{
		/*head only*/
		if(expr->symbol->id==id_Slot){
			head = String_copy(expr->symbol->sign);
			goto main;
		}
		head = Expr_toString(expr);
		if((flag & fm_nosign) && *head=='-'){
			delim=String_clip(head+1,String_tail(head));
			deallocate(head);
			head=delim;
			delim=NULL;
		}
	}
  main:
	f = Form_appendHorizontal(f,Form_create(head));
	if(expr->child){
		f = Form_appendHorizontal(f,Form_create(String_copy(lp)));
		for(e=expr->child;e;e=e->next){
			f = Form_appendHorizontal(f,Form_make(e,flag & ~ fm_par));
			flag &= ~ fm_nosign;
			if(e->next){
				if(expr->symbol->id==id_Plus){
					if(Sign_mod(e->next)<0){continue;}
				}
				f = Form_appendHorizontal(f,Form_create(String_copy(delim)));
			}
		}
		f = Form_appendHorizontal(f,Form_create(String_copy(rp)));
		/*delim is copied inside of Form_pre, Form_post*/
		if(expr->symbol->associativity==un_pre && !(expr->flag & ef_func)){Form_pre(f,delim);}
		if(expr->symbol->associativity==un_post && !(expr->flag & ef_func)){Form_post(f,delim);}
	}else if(!head){
		if(lp){f = Form_appendHorizontal(f,Form_create(String_copy(lp)));}
		if(rp){f = Form_appendHorizontal(f,Form_create(String_copy(rp)));}
		if(delim && !lp && !rp){f = Form_appendHorizontal(f,Form_create(String_copy(delim)));}
	}
	return f;
}

Form Form_makeTerm(Expr expr, int flag){
	Expr e;
	Form num=NULL, den=NULL;
	long n;
	
	for(e=expr;e;e=e->next){
		if(e->symbol->id==id_Integer){
			if(Integer_toInt(e)==-1 && (flag & fm_nosign)){continue;}
		}
		if(e->symbol->id==id_Power){
			if(e->child){
				if(e->child->next){
					if(e->child->next->symbol->id==id_Integer){
						if(Integer_toInt(e->child->next)==-1){
							den = Form_appendHorizontal(den,Form_make(e->child,flag));
							continue;
						}
						if(e->child->symbol->id == id_Power){
							if(flag & fm_oneline){
								num = Form_appendHorizontal(num,Form_makePower(e->child,flag | fm_par));
								num = Form_appendHorizontal(num,Form_create(String_copy("^")));
								num = Form_appendHorizontal(num,Form_make(e->child->next,flag | fm_sub));
							}else{
								num = Form_appendHorizontal(num,
								Form_super(
									Form_makePower(e->child,flag | fm_par),
									Form_make(e->child->next,flag | fm_sub)
									)
								);
							}
						}else{
							if(flag & fm_oneline){
								num = Form_appendHorizontal(num,Form_make(e->child,flag | fm_sub));
								num = Form_appendHorizontal(num,Form_create(String_copy("^")));
								num = Form_appendHorizontal(num,Form_make(e->child->next,flag | fm_sub));
							}else{
								num = Form_appendHorizontal(num,
								Form_super(
									Form_make(e->child,flag | fm_sub),
									Form_make(e->child->next,flag | fm_sub)
									)
								);
							}
						}
						continue;
					}
				}
			}
		}
		num = Form_appendHorizontal(num,Form_make(e,flag & ~ fm_par));
	}
	num = Form_frac(num,den,flag & ~ fm_par);
	if(flag & fm_par){
		den = Form_appendHorizontal(NULL,Form_create(String_copy("(")));
		den = Form_appendHorizontal(den,num);
		num = Form_appendHorizontal(den,Form_create(String_copy(")")));
	}
	return num;
}

Form Form_makePower(Expr expr,int flag){
	Form num=NULL, den=NULL;
	if(expr->child){
		if(expr->child->next){
			if(expr->child->next->symbol->id==id_Integer){
				if(Integer_toInt(expr->child->next)==-1){
					den = Form_appendHorizontal(den,Form_make(expr->child,flag & ~ fm_par));
					return Form_frac(num,den,flag & ~ fm_par);
				}
			}
			if(expr->child->symbol->id == id_Power){
				if(flag & fm_oneline){
					num = Form_appendHorizontal(Form_makePower(expr->child,(flag | fm_sub) | fm_par),Form_create(String_copy("^")));
					num = Form_appendHorizontal(num,Form_make(expr->child->next,(flag | fm_sub) & ~ fm_par));
				}else{
					num = Form_super(Form_makePower(expr->child,flag | fm_par),Form_make(expr->child->next,(flag | fm_sub) & ~ fm_par));
				}
			}else{
				if(flag & fm_oneline){
					num = Form_appendHorizontal(Form_make(expr->child,(flag | fm_sub) & ~ fm_par),Form_create(String_copy("^")));
					num = Form_appendHorizontal(num,Form_make(expr->child->next,(flag | fm_sub) & ~ fm_par));
				}else{
					num = Form_super(Form_make(expr->child,flag & ~ fm_par),Form_make(expr->child->next,(flag | fm_sub) & ~ fm_par));
				}
			}
		}
	}
	num = Form_frac(num,den,flag & ~ fm_par);
	if(flag & fm_par){
		den = Form_appendHorizontal(NULL,Form_create(String_copy("(")));
		den = Form_appendHorizontal(den,num);
		num = Form_appendHorizontal(den,Form_create(String_copy(")")));
	}
	return num;
}

void Form_print_mod(Form form, char **box, int ws){
	Form f;
	int j;
	char *p;
	
	switch(form->tag){
	  case FormS:
		for(j=ws,p=form->content.string;*p;p++,j++){box[form->level][j] = *p;}
		break;
	  case FormV:
		for(f=form->content.head;f;f=f->next){
			Form_print_mod(f,box,ws);
		}
		break;
	  case FormH:
		for(f=form->content.head;f;f=f->next){
			Form_print_mod(f,box,ws);
			ws += f->width+1;
		}
		break;
	}
}

void Form_print_mod_oneline(Form form, char *box, int ws){
	Form f;
	int j;
	char *p;
	
	switch(form->tag){
	  case FormS:
		for(j=ws,p=form->content.string;*p;p++,j++){box[j] = *p;}
		break;
	  case FormV:
		for(f=form->content.head;f;f=f->next){
			Form_print_mod_oneline(f,box,ws);
		}
		break;
	  case FormH:
		for(f=form->content.head;f;f=f->next){
			Form_print_mod_oneline(f,box,ws);
			ws += f->width+1;
		}
		break;
	}
}

void Form_print(Form form){
	char **box;
	int i,j;
	if(!form){return;}
	box = allocate(form->height*sizeof(char*));
	for(i=0;i<form->height;i++){
		box[i] = allocate(form->width);
		for(j=0;j<form->width;j++){box[i][j]=' ';}
	}
	Form_print_mod(form,box,0);
	for(i=0;i<form->height;i++){
		for(j=0;j<form->width;j++){
			putchar(box[i][j]);
		}
		putchar('\n');
	}
	for(i=0;i<form->height;i++){deallocate(box[i]);}
	deallocate(box);
}

#define INDENT for(i=0;i<indent;i++){putchar(' ');}
void Form_print2(Form form, int indent){
	Form f;
	int i;
	switch(form->tag){
	  case FormS:
		INDENT printf(KCYN "string " KNRM);
		printf("%s\n",form->content.string);
		INDENT printf("%d,",form->width);
		printf("%d,",form->height);
		printf("%d\n",form->level);
		break;
	  case FormV:
		INDENT printf(KBLU "vertical\n" KNRM);
		INDENT printf("%d,",form->width);
		printf("%d,",form->height);
		printf("%d\n",form->level);
		for(f=form->content.head;f;f=f->next){
			Form_print2(f,indent+1);
		}
		break;
	  case FormH:
		INDENT printf(KRED "horizontal\n" KNRM);
		INDENT printf("%d,",form->width);
		printf("%d,",form->height);
		printf("%d\n",form->level);
		for(f=form->content.head;f;f=f->next){
			Form_print2(f,indent+1);
		}
		break;
	}
	putchar('\n');
}

char *OutputForm_mod_str(Expr expr){
	Form form;
	char *buf;
	int i,j;
	form = Form_make(expr,fm_oneline);
	if(!form){return NULL;}
	if(form->height != 1){fprintf(stderr,"something unexpected occurs\n");}
	buf = allocate(form->width+1);
	for(j=0;j<form->width;j++){buf[j]=' ';}
	Form_print_mod_oneline(form,buf,0);
	Form_delete(form);
	buf[form->width] = '\0';
	return buf;
}

Expr OutputForm_mod(Expr expr){
	Form f;
	if(!expr){printf("Null\n"); return NULL;}
	f = Form_make(expr,0);
/*	
#ifdef LEAKDETECT
	Form_print2(f,1);
#endif	
*/	
	Form_print(f);
	Form_delete(f);
	return expr;
}

Expr OutputForm(Expr expr){
	if(!expr->child){return expr;}
	OutputForm_mod(expr->child);
	output_flag = 0;
	return expr;
}
