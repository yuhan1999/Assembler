#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define OP 20  //設 opTable 有 20 個 
#define primeTable 11

typedef struct OpTable{		//build Table
	char name[10];
	char infro[10];
	char format[5];
	char code[5];
	struct OpTable* next;
}opTable;
typedef struct Node* list;
typedef struct Node{		//產生 節點
	char name[10];
	char extend;	// + ,' '
	char opcode[10];
	char mark;		// =,#,@
	char oper1[10];	//運算元 1 
	char oper; 		//分隔號, 運算 + 或 - 
	char oper2[10];	//運算元 2 
	char location[20];
	char target[20];
	list next;
};
typedef struct Reg{			//build register Table
	char name[5];
	int num;
}reg;
list symTab[primeTable];
list litTab[primeTable];
char fname[20];
list head = NULL;
void buildSymTab(char*);
list newnode(void);
list setnode(char*);
void Print(void);
int Hash(char*);
void buildLitTab(int,list);
void iprintLitTab(void);
opTable optab[] = {	 		//建 opTab 
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

list newnode(){				//新增一個 node 
	list node = (list)malloc(sizeof(struct Node));
	node -> next = NULL;
	return node;
}

void Print(void){
	list ptr = head;
	while(ptr != NULL){
		printf("%s   %c%s   %c%s   %c%s\n",ptr->name,ptr->extend,ptr->opcode,ptr->mark,ptr->oper1,ptr->oper,ptr->oper2);
		ptr = ptr->next; 
	}
}

list setnode(char* str){		//把node串起來 
	list node = newnode();
	int i = 0, j = 0, flag = 0, temp;
	char tmp[10];
	memset(tmp,'\0',10);
	if(str[0] == '\t'){		//若開頭為空格 ， 助記碼 
		i = 1;
	}else{					//名稱 	
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
	while(str[i] != '\t' && str[i] != '\0'){	//助記碼				
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
	//運算碼 oper
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
	//串起來 
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
	int sum = 0;
	sum = strlen(str) % primeTable;
	return sum;
}

void printLitTab(void){
	
	int i;
	for(i=0;i<11;i++){
		if(litTab[i]!=NULL){
			list ptr = litTab[i];
			while(ptr!=NULL){
				printf("%c%s\n",ptr->extend,ptr->opcode);
			}
		}
	}
}
void buildLitTab(int index,list ptr){
	list node = newnode();
	
	strcpy(node->name,tmp);
	printf("%s",ptr->name);
	strcpy(node->extend,ptr->mark);
	strcpy(node->opcode,ptr->oper1);
	strcpy(node->mark,'\0');
	strcpy(node->oper1,'\0');
	strcpy(node->oper,'\0');
	strcpy(node->oper2,'\0');
	printf("%s   %c%s   %c%s   %c%s\n",node->name,node->extend,node->opcode,node->mark,node->oper1,node->oper,node->oper2);
	if(litTab[index] == NULL){
		litTab[index] = newnode();	
		litTab[index] = ptr;
			
		list tmp = head;
		while(tmp -> next != NULL){
			tmp = tmp -> next;
		}
		tmp -> next = node;
	}else{
		list tmp = litTab[index];
		while(tmp->next != NULL){
			if(strcmp(litTab[index],ptr->oper1) == 0){
				break;
			}else{
				tmp = tmp -> next;
			}
		}
		tmp -> next = node;
	}
}
void buildSymTab(char* fname){	//建 symTab 	
	char c;
	char str[100];
	int flag, index=0, tabIndex=0;
	
	FILE *fp = fopen(fname,"r");	//讀取檔案
	while(1){
		flag = fscanf(fp,"%c",&c);
		if(c != '\0' && c != '\n' && flag != EOF){
			str[index] = c;
			index++;
		}else{
			str[index] = '\0';
			index = 0;
			
			if(str[0] == '.') continue; 		//註解跳過 
			
			list node = setnode(&str[0]);	//把str的內容分類 ，並串起來 
			if(node->mark == '='){			//literal
				tabIndex = Hash(node->oper1);
				buildLitTab(tabIndex, node);
				printLitTab();
			}
			if(flag==EOF) break;
		}
	}
	 
}
int main(){	
	//建 SymTab
	buildSymTab("srcpro2.9.txt");
	//symTab("srcpro2.11.txt");
	//Print();
	//建 串列
	//印出 pool
	//印出 OpTab
	//印出 SymTab
	//印出 RegTab 
}
