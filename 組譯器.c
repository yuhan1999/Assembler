#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define OP 20  //�] opTable �� 20 �� 
#define primeTable 11
typedef struct OpTable{		//build Table
	char name[10];
	char infro[10];
	char format[5];
	char code[5];
	struct OpTable* next;
}opTable;
typedef struct Locctr* Use;
typedef struct Locctr{
	int counter;	
	int address;	
	char name[10];	//block name
	int num;		//bloc number
	Use next;
}Locctr;
typedef struct Node* list;
typedef struct Node{		//���� �`�I
	char name[10];
	char extend;	// + ,' '
	char opcode[10];
	char mark;		// =,#,@
	char oper1[10];	//�B�⤸ 1 
	char oper; 		//���j��, �B�� + �� - 
	char oper2[10];	//�B�⤸ 2 
	int address;
	char target[20];
	Use block;
	list next;
};
typedef struct Reg{			//build register Table
	char name[5];
	int num;
}reg;

list symTab[primeTable];
list litTab[primeTable];
char Fname[20];
int use_num = 1;
list head = NULL;
list lit_head = NULL;
Use useHead = NULL;
int base, pc;

void onepass(char*);
list newnode(void);
list setnode(char*);
void printPool(void);
int Hash(char*);
void buildLitTab(int,list,Use);
void buildSymTab(int,list); 
void printLitTab(void);
void printSymTab(void);
void clearList(Use);
list searchSymTab(list,char*);
Use newBlock(void);
Use buildBlock(char*);
Use searchBlock(char*);
int searchOpTab(char*);
list searchLitTab(list,char*);
void twopass(void);
int searchReg(char*);
opTable optab[] = {	 		//�� opTab 
	{"STL","m","3/4","14"},
	{"LDB","m","3/4","68"},
	{"JSUB","m","3/4","48"},
	{"LDA","m","3/4","00"},
	{"COMP","m","3/4","28"},
	{"JEQ","m","3/4","30"},
	{"J","m","3/4","3C"},
	{"STA","m","3/4","0C"},
	{"CLEAR","r1","2","B4"},
	{"LDT","m","3/4","74"},
	{"TD","m","3/4","E0"},
	{"RD","m","3/4","D8"},
	{"COMPR","r1,r2","2","A0"},
	{"STCH","m","3/4","54"},
	{"TIXR","r1","2","B8"},
	{"LDCH","m","3/4","50"},
	{"WD","m","3/4","DC"},
	{"JLT","m","3/4","38"},
	{"STX","m","3/4","10"},
	{"RSUB","NULL","3/4","4C"},
};
reg regtab[] = {
	{"A",0},
	{"X",1},
	{"L",2},
	{"PC",8},
	{"SW",9},
	{"B",3},
	{"S",4},
	{"T",5},
	{"F",6},
};

Use newBlock(){
	Use node = (Use)malloc(sizeof(Locctr));
	node -> next = NULL;
	node -> counter = 0;
	return node;
}
list newnode(){				//�s�W�@�� node 
	list node = (list)malloc(sizeof(struct Node));
	node -> next = NULL;
	return node;
}
void printPool(void){
	int i = 1;
	FILE* fp = fopen("OBJFILE.txt","w");
	fprintf(fp,"Filename: %s\n",Fname);
	fprintf(fp,"------------------------------�iLiteral Pool�j------------------------------\n");
	fprintf(fp,"%3s%8s%6s\t%s\t\t\t\t\t %s\n\n","Row","Address","Block","Code","Target");
	list ptr = head;
	while(ptr != NULL){
		fprintf(fp,"%-4d %04X%6d\t\t%-6s%5c%-6s%5c%-6s%5c%-s\t %s\n",i,ptr->address,ptr->block->num,ptr->name,ptr->extend,ptr->opcode,ptr->mark,ptr->oper1,ptr->oper,ptr->oper2,ptr->target);
		ptr = ptr->next;
		i++;
	}
}
list setnode(char* str){		//��node�������æ�_�� head
	list node = newnode();
	int i = 0, j = 0, flag = 0, temp;
	char tmp[10];
	memset(tmp,'\0',10);
	if(str[0] == '\t'){		//�Y�}�Y���Ů� �A �U�O�X 
		i = 1;
	}else{					//name
		while(str[i] != '\t' && str[i] != '\0'){
			tmp[j] = str[i];
			if(str[i+1] != '\t' && str[i+1] != '\0'){	
				j++;
			}
			i++;
		}	
		j = 0;
		i++;
	}
	strcpy(node->name, tmp);
//	printf("%s\n",node->name);
	memset(tmp,'\0',10);
	while(str[i] != '\t' && str[i] != '\0'){	//�U�O�X				
		if(str[i] == '+'){
			node->extend = str[i];
			flag = 1;
		}else{
			tmp[j] = str[i];
			if(str[i+1] != '\t' && str[i+1] != '\0'){
				j++;
			}
		}
		i++;
	}
	if(flag == 0){
		node -> extend = '\0'; 
	}
	strcpy(node->opcode, tmp);
//	printf("\t%c%s\t",node->extend, node->opcode);
	j = 0;
	flag = 0;
	//�B��X oper
	i++;
	temp = i;
	memset(tmp,'\0',10);
	if(str[temp]=='#' || str[temp]=='@' || str[temp]=='='){
		node -> mark = str[temp];
		i++;
	}else{
		node -> mark = '\0';
	}
	while(str[i] != '\t' && str[i] != '\0'){
		if(str[i]=='-' || str[i]=='+' || str[i]==','){
			strcpy(node->oper1, tmp);
			node->oper = str[i];
			j = 0;
			i++;
			memset(tmp,'\0',10);
			while(str[i]!='\t' && str[i]!='\0'){
				tmp[j] = str[i];
				if(str[i+1] != '\t' && str[i+1] != '\0'){	
					j++;
				}
				i++;
			}
			strcpy(node->oper2, tmp);
			flag = 1;
			break;
		}else{
			tmp[j] = str[i];
			if(str[i+1] != '\t' && str[i+1] != '\0'){	
				j++;
			}
			i++;
		}
	}
	if(flag == 0){
		strcpy(node->oper1, tmp);
		memset(tmp,'\0',10);
		node -> oper = '\0';
		strcpy(node->oper2, tmp);
	}
//	printf("%c%s\t%c%s\n",node->mark,node->oper1,node->oper, node->oper2);
	j = 0;
	flag = 0;
	memset(tmp,'\0',10);
	//��_�� 
	if(head == NULL){
		head = node;
	}else{
		list ptr = head;
		while(ptr->next!=NULL){
			ptr = ptr -> next;
		}
		ptr -> next = node;
	}
	return node;
}
int Hash(char* str){
	int sum = 0 ,i;
	int len = strlen(str);
	for(i=0; i<len; i++){
		sum += str[i];
	}
	sum %= primeTable;
	return sum;
}
void printLitTab(void){			//�٭n��+address  
	int i, j=1;
	printf("Filename: %s\n",Fname);
	printf("---------------�iLITTAB�j---------------\n");
	printf("%4s%10s\t%s     \n","Row","LitName","Address");
	for(i=0; i<primeTable; i++){
		if(litTab[i] != NULL){
			list ptr = litTab[i];
			while(ptr != NULL){
				printf("%4d%10s\t%04d\n", j, ptr->opcode, ptr->address);
				ptr = ptr -> next;
				j++;
			}
		}
	}
}
void buildLitTab(int index, list ptr, Use use){
	int flag = 0;
	list node = newnode();			//�s��LitTab 
	node -> extend = '=';
	strcpy(node->opcode, ptr->oper1);
	node -> block = use;
	
	list temp = newnode();			//�s��Lit_head 
	strcpy(temp->name, "*");
	temp -> extend = '=';
	strcpy(temp->opcode, node->opcode);
	temp -> mark = '\0';
	strcpy(temp->oper1, "\0");
	temp -> oper = '\0';
	strcpy(temp->oper2, "\0");
	temp -> block = use;
	
	if(litTab[index] == NULL){	
		litTab[index] = newnode();
		litTab[index] = node;
	}else{
		list tmp = litTab[index];
		while(tmp -> next != NULL){
			if(strcmp(tmp->opcode, node->opcode) == 0){	//������ 
				flag = 1;
				break;
			}else{
				tmp = tmp -> next;
			}
		}
		if(strcmp(tmp->opcode, node->opcode) != 0){
			tmp -> next = node;
		}else{
			flag = 1;
		}
	}
	
	if(flag == 0){
		if(lit_head == NULL){
			lit_head = temp;
		}else{
			list t = lit_head;
			while(t -> next != NULL){
				t = t -> next;
			}
			t -> next = temp;
		}
	}	
}
void printSymTab(void){
	int i, j=1;
	printf("\nFilename: %s\n",Fname);
	printf("---------------�iSYMTAB�j---------------\n");
	printf("%4s%10s\t%s\n","Row","SymName","Address");
	for(i=0; i<primeTable; i++){
		if(symTab[i] != NULL){
			list ptr = symTab[i];
			while(ptr != NULL){
				printf("%4d%10s\t%04d\n", j, ptr->name, ptr->address);
				ptr = ptr -> next;
				j++;
			}
		}
	}
}
void buildSymTab(int index,list node){
	list ptr = newnode();
	ptr -> address = node -> address;
	ptr -> block = node -> block;
	strcpy(ptr->name, node->name);
	
	if(symTab[index] == NULL){
		symTab[index] = newnode();
		symTab[index] = ptr;
	}else{
		list tmp = symTab[index];
		while(tmp->next != NULL){
			tmp = tmp -> next;
		}
		tmp -> next = ptr;
	}
}
list searchLitTab(list ptr, char* str){
	while(ptr != NULL){
		if(!strcmp(ptr->opcode, str)){
			return ptr;
		}else{
			ptr = ptr -> next;
		}
	}
	return NULL;
}
void clearList(Use use){
	int index;
	list ptr;
	while(lit_head != NULL){
		index = Hash(lit_head->opcode);
		ptr = searchLitTab(litTab[index], lit_head->opcode);
		lit_head -> address = use -> counter;
		ptr -> address = use -> counter;
		ptr -> block = use;
		lit_head -> block = use;
		if(lit_head -> opcode[0] == 'C'){
			use -> counter += strlen(lit_head -> opcode) - 3;
		}else if(lit_head -> opcode[0] == 'X'){
			use -> counter += (strlen(lit_head -> opcode) - 3) / 2;
		}
		lit_head = lit_head -> next;
	}
	lit_head = NULL;
}
list searchSymTab(list ptr, char* str){
	while(ptr != NULL){
		if(!strcmp(ptr->name, str)){		//find 
			return ptr;
		}else{
			ptr = ptr -> next;
		}
	}
	return NULL;
}
Use buildBlock(char* str){
	Use u = newBlock();
	strcpy(u->name, str);
	u -> num = use_num++;
	if(useHead == NULL){
		useHead = u;
	}else{
		Use tmp = useHead;
		while(tmp->next != NULL){
			tmp = tmp -> next;
		}
		tmp -> next = u;
	}
	return u;
}
Use searchBlock(char* str){
	Use tmp = useHead;
	while(tmp != NULL){
		if(!strcmp(tmp->name, str)){
			return tmp;
		}else{
			tmp = tmp -> next;
		}
	}
	return NULL;
}
int searchOpTab(char* str){
	int i;
	for(i=0; i<20; i++){
		if(!strcmp(optab[i].name, str)){
			return i;
		}
	}
	return -1;
}
int searchReg(char* str){
	int i;
	for(i=0; i<9; i++){
		if(!strcmp(regtab[i].name, str)){
			return regtab[i].num;
		}
	}
	return -1;
}
void onepass(char* fname){	//�� symTab�BlitTab�Baddress
	sprintf(Fname,"%s",fname);
	char c;
	char str[100];
	int flag, index=0, tabIndex=0;
	memset(str,'\0',100);
	useHead = newBlock();
	useHead -> num = 0;
	strcpy(useHead->name, "DEFAULT");
	Use use = useHead;
	
	FILE *fp = fopen(fname,"r");	//Ū���ɮ�
	while(1){
		flag = fscanf(fp,"%c",&c);
		if(c != '\0' && c != '\n' && flag != EOF){
			str[index] = c;
			index++;
		}else{
			str[index] = '\0';
			index = 0;
			
			if(str[0] == '.') continue; 		//���Ѹ��L 
			list node = setnode(str);	//��str�����e���� �A�æ�_�� 
			if(!strcmp(node->opcode, "USE")){	//USE �� BLOCK 
				if(!strcmp(node->oper1, "\0")){	//���Ĥ@��
					 use = useHead;
				}else{
					use =  searchBlock(node->oper1);
					if(use == NULL){	//�S���w�qblock���N�Ф@�� 
						use = buildBlock(node->oper1);
					}
				}
			}
			node -> block = use;
			node -> address = use -> counter;
			strcpy(node->target, "\0");
			
			if(node->mark == '='){			//literal
				tabIndex = Hash(node->oper1);
				buildLitTab(tabIndex, node, use);
			}
			if(!strcmp(node->opcode, "LTORG")){ //LTORG �`�ƭn�[�b�U���@�� 
				node -> next = lit_head;
				clearList(use);
			}
			if(!strcmp(node->opcode, "END")){	//END �`�ƭn�[�b�U���@�� 
				node -> next = lit_head;
				clearList(use);
			}
			if(!strcmp(node->opcode, "EQU")){	//EQU �n���B�� + - * /���~�������B�� 
				tabIndex = Hash(node->oper1); 
				list op1 = searchSymTab(symTab[tabIndex], node->oper1);
				tabIndex = Hash(node->oper2);
				list op2 = searchSymTab(symTab[tabIndex], node->oper2);
				if(node->oper == '+'){
					node->address = op1->address + op2->address;
				}else if(node->oper == '-'){
					node->address = op1->address - op2->address;
				}else if(node->oper == '*'){
					node->address = op1->address * op2->address;
				}else if(node->oper == '/'){
					node->address = op1->address / op2->address;
				}
			}
			//�ҥ�2-11 
			if(!strcmp(node->opcode, "START")){
				node -> address = atoi(node->oper1);
				use -> address = atoi(node->oper1);
				use -> counter = atoi(node->oper1);
			}else{
				int len = 0;
				int num = searchOpTab(node->opcode);
				if(num != -1){
					if(optab[num].format[0] == '2'){
						len = 2;
					}else if(node->extend == '+'){
						len = 4;
					}else{
						len = 3;
					}
				}else if(!strcmp(node->opcode, "WORD")){
					len = 3;
				}else if(!strcmp(node->opcode, "RESW")){
					len = 3 * atoi(node->oper1);
				}else if(!strcmp(node->opcode, "RESB")){
					len = atoi(node->oper1);
				}else if(!strcmp(node->opcode, "BYTE")){
					len = strlen(node->oper1) - 3;
					if(node->oper1[0] == 'X') len /= 2;
				}
				if(node->name[0] != '\0'){	//symTab
					tabIndex = Hash(node->name);
					buildSymTab(tabIndex, node);
				}
				use->counter += len;
				len = 0;
			}
			if(flag == EOF) break;
		}
	}
	int cnt = useHead -> address;
	use = useHead;
	while(use != NULL){
		use -> address = cnt;
		cnt += use -> counter;
		use = use -> next;
	}
}
void twopass(){
	list ptr = head;
	int index, opni, num, xbpe, disp, i;
	list tmp;
	char str[10],dispStr[10];
	while(ptr != NULL){
		if(!strcmp(ptr->opcode, "BASE")){
			index = Hash(ptr->oper1);
			tmp = searchSymTab(symTab[index], ptr->oper1);
			base = tmp -> address;
			strcpy(ptr->target,"\0");
		}else{
			index = searchOpTab(ptr->opcode);
			if(index != -1){
				opni = (int)strtol(optab[index].code, NULL, 16);
				if(ptr->mark == '#'){
					opni += 1;
				}else if(ptr->mark == '@'){
					opni += 2;
				}else{
					opni += 3;
				}
				
				int Index;
				Index = Hash(ptr->oper1);
				if(ptr->mark == '='){		//oper1 is literal
					tmp = searchLitTab(litTab[Index], ptr->oper1);
				}else{		//oper1 is symbol
					tmp = searchSymTab(symTab[Index], ptr->oper1);
				}
				
				if(ptr->extend == '+'){		//set pc
					pc = ptr->address + 4;
				}else if(optab[Index].format[0] == '3'){
					pc = ptr->address + 3;
				}else{
					pc = ptr->address + 2;
				}
				
				xbpe = 0;
				if(ptr->extend == '+'){		//�榡�|  x = 1 : xx1xxxxx 
					if(tmp != NULL){
						sprintf(str, "%02X1%05X", opni, tmp->address);
					}else{
						sprintf(str, "%02X1%05X", opni, atoi(ptr->oper1));
					}
					strcpy(ptr->target, str);
				}else if(optab[index].format[0] == '3'){	//�榡�T 
					if(tmp == NULL){	//not literal & symbol
						num = atoi(ptr->oper1);
						sprintf(dispStr, "%03X", num);
					}else{
						disp = tmp->address + tmp->block->address - pc;
						if(-2048 > disp || disp > 2047){	//base
							xbpe += 4;
							disp = tmp->address - base; //TA - base
						}else{	//program counter
							xbpe += 2;	//TA - pc
						}
						sprintf(dispStr, "%03X", disp);
						if(disp < 0){
							sprintf(dispStr, "%s", dispStr+5);
						}
						if(!strcmp(ptr->oper2, "X")){
							xbpe += 8;
						}
					}
					sprintf(str, "%02X%X%s",opni, xbpe, dispStr);
					strcpy(ptr->target, str);
				}else{	//�榡�G 
					opni -= 3;
					if(!strcmp(ptr->oper2, "\0")){
						sprintf(str, "%X%X0", opni, searchReg(ptr->oper1));
					}else{
						sprintf(str, "%X%X%X", opni, searchReg(ptr->oper1), searchReg(ptr->oper2));
					}
					strcpy(ptr->target, str);
				}
			}else if(ptr->extend == '='){
				int cnt = 0; //��str�h�s 
				memset(str, '\0', 10);
				for(i=2; i<strlen(ptr->opcode)-1; i++){
					if(ptr->opcode[0] == 'C'){
						str[cnt++] = ptr->opcode[i] / 16 + '0';
						if(ptr->opcode[i]%16 >= 10){
							str[cnt++] = ptr->opcode[i] % 16 + '7';
						}else{
							str[cnt++] = ptr->opcode[i] % 16 + '0';
						}
					}else{
						str[cnt++] = ptr->opcode[i];
					}
				}
				str[cnt] == '\0';
				strcpy(ptr->target, str);
			}else if(!strcmp(ptr->opcode, "BYTE")){
				int cnt = 0;
				for(i=2; i<strlen(ptr->oper1)-1; i++){
					if(ptr->oper1[0] == 'C'){
						ptr->target[cnt++] = ptr->oper1[i] / 16 + '0';
						if(ptr->opcode[i]%16 >= 10){
							ptr->target[cnt++] = ptr->oper1[i] % 16 + '7';
						}else{
							ptr->target[cnt++] = ptr->oper1[i] % 16 + '0';
						}
					}else{
						ptr->target[cnt++] = ptr->oper1[i];
					}
				}
				ptr->target[cnt] == '\0';
			}else if(!strcmp(ptr->opcode, "WORD")){
				sprintf(ptr->target, "%X", atoi(ptr->oper1));
			}
		}
		ptr = ptr -> next;	
	}
}
int main(){	
	//�� SymTab
//	onepass("srcpro2.9.txt");
	onepass("srcpro2.11.txt");
	twopass();	
	printLitTab();
	printSymTab();
	printPool();
	//�L�X OpTab
	//�L�X RegTab 
}
