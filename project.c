#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#define ARROW_UP    256 + 72
#define ARROW_DOWN  256 + 80
#define ARROW_LEFT  256 + 75
#define ARROW_RIGHT 256 + 77
#define MAX_STRING_LENGTH 100
#define MAX_OPERATOR_COUNT 100
#define MAX_OPERATOR_NAME 15


typedef struct{ //Op
    char name[MAX_OPERATOR_NAME];
    int speed;
    bool post;
    bool removed;
}Op;

int endofmenu(int y,int n){
    if(y<n)
        return y;
    return n-1;
}

void spacing(){
    printf("\n  ");
}

void header(){
    system("cls");
    printf("\e[H\n  %-15s %-10s %-10s %-10s\n\n","Name","Speed","Health","Post");
}

char * booltopost(int z){
    if(z==1)
        return "attacker";
    return "defender";
}

int getcharrow(){
    int ch = getch();
    if ( ch == 0 || ch == 224 )
        ch = 256 + getch();
    return ch;
}

//Returns index based on position in visible list
int ytoi(Op ops[],int y){
    int n=y;
    for(int i=0;y>=0;i++){
        if(ops[i].removed==true)
            n++;
        else
            y--;
    }
    return n;
}

int countallvisible(Op ops[], int n){
    int visible=0;
    for(int i=0;i<n;i++){
        if(ops[i].removed==false)
            visible++;
    }
    return visible;
}

void swapop(Op * op1,Op * op2){
    Op p;
    p=*op1;
    *op1=*op2;
    *op2=p;
}

bool comparestrings(char s1[],char s2[]){
    for(int i=0;s1[i]!='\0',s2[i]!='\0';i++){
        if(toupper(s1[i])>toupper(s2[i]))
            return 0;
        if(toupper(s1[i])<toupper(s2[i]))
            return 1;
    }
    return 0;
}

void shiftchoices(int choices[],int cc,int y){
    for(int i=0;i<cc;i++){
        if(choices[i]>choices[y])
            choices[i]--;
    }
}

int startswith(Op op, char startingpattern[],int ch){
    switch(ch){
        case 0:
            for(int i=0;startingpattern[i]!='\0';i++){
                if(toupper(op.name[i])!=toupper(startingpattern[i]))
                    return 1;
            }
            break;
        case 1:
            for(int i=0;startingpattern[i]!='\0';i++){
                if(op.speed!=startingpattern[i]-48)
                    return 1;
            }
            break;
        case 2:
            for(int i=0;startingpattern[i]!='\0';i++){
                if(4-op.speed!=startingpattern[i]-48)
                    return 1;
            }
            break;
        case 3:
            for(int i=0;startingpattern[i]!='\0';i++){
                if(toupper(booltopost(op.post)[i])!=toupper(startingpattern[i]))
                    return 1;
            }
            break;
    }
    return 0;
}

void rmwhitespaces(char in[], char out[]){
    int i=0;
    for(i;in[i]!='\0',in[i]!=' ';i++)
        out[i]=in[i];
    out[i]='\0';
}

int menu(char text[][MAX_STRING_LENGTH],int cc,bool vertical,int y,int uly){
    int x=0,ch;
    while(true){
        if(vertical==true){printf("\n\e[%d;0H",uly);}
        else printf("\e[%d;3H",y+uly);
        printf("\e[?25l");
        for(int i=0;i<cc;i++){
            if((i==y&&vertical==true)||(vertical==false&&i==x)){
                if(vertical==true)printf("  \e[7m%s\e[m\n",text[i]);
                else printf("\e[7m%s \e[m",text[i]);
            }
            else{
                if(vertical==true)printf("  %s\n",text[i]);
                else printf("%s ",text[i]);
            }
        }
        ch = getcharrow();
        switch(ch){
            case ARROW_UP:if(y-1>=0&&vertical==true)y--;break;
            case ARROW_DOWN:if(y+1<cc&&vertical==true)y++;break;
            case ARROW_LEFT:if(x-1>=0&&vertical==false)x--;break;
            case ARROW_RIGHT:if(x+1<cc&&vertical==false)x++;break;
            case 13:if(vertical==true)return y;return x;
            case 27:return -2;
            default:break;
        }
    }
}

int mainMenu(bool isadmin){
    int cofchcs[2]={5,8};
    char chcs[2][10][MAX_STRING_LENGTH]={ {{"Sign as administrator"},{"Print whole DB"},{"Search in DB"},{"Sort DB"},{"End"}},
                            {{"Change password"},{"Print whole DB"},{"Search in DB"},{"Sort DB"},{"Add element"},{"Edit element"},{"Remove element"},{"Switch to user"}}};
    system("cls");
    return (menu(chcs[isadmin],cofchcs[isadmin],true,0,2)+1)%cofchcs[isadmin];
}

int sequencemenu(char text[][MAX_STRING_LENGTH],int cc,int chcs[]){
    int ch,y=0,j=0;
    sprintf(text[cc-1],"DONE");
    while(true){
        printf("\e[H");
        printf("\n");
        for(int i=0;i<cc-1;i++){
            if(i==y){
                printf("  \e[7m%-10s[%d]\e[m\n",text[i],chcs[i]);
            }
            else{
                printf("  %-10s[%d]\n",text[i],chcs[i]);
            }
        }
        printf("\n");
        if(y==cc-1)
            printf("  \e[7m%-10s\e[m\n",text[cc-1]);
        else
            printf("  %-10s\n",text[cc-1]);
        ch = getcharrow();
        switch(ch){
            case ARROW_UP:if(y-1>=0)y--;break;
            case ARROW_DOWN:if(y+1<cc)y++;break;
            case 13:if(y==cc-1)return j;if(chcs[y]==0){j++;chcs[y]=j;}else{j--;shiftchoices(chcs,cc-1,y);chcs[y]=0;}break;
            case 27:return -2;
            default:break;
        }
    }
}

void scanPasswd(char passwd[]){
    fflush(stdin);
    printf("\e[30m");
    scanf("%s",passwd);
    printf("\e[m");
}

void createpassword(){
    char passwd[100];
    FILE * out = fopen("passwd.txt","w+");
    scanPasswd(passwd);
    fprintf(out,"%s",passwd);
    fclose(out);
}

bool signin(char inserpasswd[]){
    char passwd[100];
    char cpasswd[100];
    FILE * in = fopen("passwd.txt","r");
    system("cls");
    spacing();
    if(fscanf(in,"%s",cpasswd)!=1){
        fclose(in);
        printf("Create password\n");
        printf("  Insert password:");
        createpassword();
        return true;
    }
    else{
        fclose(in);
        printf("%s",inserpasswd);
        scanPasswd(passwd);
        if(strcmp(passwd,cpasswd)==0)
            return true;
        printf("  The password is incorrect\n");
        spacing();
        system("pause");
        return false;
    }
}

void changepasswd(){

    if(signin("Insert old password:")==true){
        spacing();
        printf("Type in new password:");
        createpassword();
        spacing();
        printf("Password successfully changed");
        spacing();
        system("pause");
    }
}

void printall(Op ops[], int n,char search[],int ch,int yoffset){
    header();
    printf("\e[%d;1H",yoffset);
    for(int i=0;i<n;i++){
        if(ops[i].removed==false&&(startswith(ops[i],search,ch)==0))
            printf("  %-15s %-10d %-10d %-10s\n",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
    }
    spacing();
}

void opsort(Op ops[],int n,int chce){   
    for(int i=0;i<n-1;i++){
        for(int y=n-1;y>i;y--){
                if((!comparestrings(ops[y-1].name,ops[y].name)&&chce==0)||(ops[y-1].speed>ops[y].speed&&chce==1)||(ops[y-1].speed<ops[y].speed&&chce==2)||(ops[y-1].post>ops[y].post&&chce==3)){
                    swapop(&ops[y],&ops[y-1]);
            }
        }
    }
}

void opsortmenu(Op ops[],int n){
    int cc=4,chcs[++cc],j;
    char text[10][MAX_STRING_LENGTH]={{"Name"},{"Speed"},{"Health"},{"Post"}};
    system("cls");
    j=sequencemenu(text,cc,chcs);
    if(j!=-2){
        for(int d=cc;d>0;d--){
            for(int i=0;i<cc;i++){
                if(chcs[i]==d)
                    opsort(ops,n,i);
            }
        }
    }
}

void opsearch(Op ops[],int n){
    int x,input,i=0,brk=0;
    char text[10][MAX_STRING_LENGTH]={{"Name           "},{"Speed     "},{"Health    "},{"Post      "}},searched[MAX_STRING_LENGTH]={""},sf[MAX_STRING_LENGTH];
    system("cls");
    printall(ops,n,searched,0,6);
    x=menu(text,4,false,0,2);
    while(x!=-2){
        while(brk!=1){
            printall(ops,n,searched,x,6);
            rmwhitespaces(text[x],sf);
            printf("\e[4;3H\e[?25hSearching for %s: %s",sf,searched);
            input=getch();
            switch(input){
                case 8:if(i-1>=0)i--;searched[i]='\0';break;
                case 27:brk=1;printf("\e[?25l");break;
                default:if(i+1<MAX_STRING_LENGTH)i++;searched[i-1]=input;searched[i]='\0';break;
            }
        }
        brk=0;
        x=menu(text,4,false,0,2);
    }
}

int OpstoString(Op ops[],int n,char opstring[][MAX_STRING_LENGTH]){
    int y=0;
    for(int i=0;i<n;i++){
        if(ops[i].removed==false){
            sprintf(opstring[y], "%-15s %-10d %-10d %-10s",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
            y++;
        }
    }
    return y;
}

void OptoStrings(Op op,char opstring[][MAX_STRING_LENGTH]){
    sprintf(opstring[0],"%-15s",op.name);
    sprintf(opstring[1],"%-10d",op.speed);
    sprintf(opstring[2],"%-10d",4-op.speed);
    sprintf(opstring[3],"%-10s",booltopost(op.post));
}

int changeopname(char string[],int index,Op ops[],int n){
    int no=0;
    for(int i=0;i<n;i++){
        if(strcmp(ops[i].name,string)==0){
            no=1;
            break;
        }
    }
    if(no==0){
        strcpy(ops[index].name,string);
        return 0;
    }
    return 1;
}

int changeopspeed(int num, Op * op, bool speed){
    if(num>=1&&num<=3){
        if(speed==1)
            (*op).speed=num;
        else
            (*op).speed=4-num;
        return 0;
    }
    return -1;
}

int changeoppost(char string[],Op * op){
    if(strcmp("attacker",string)==0||strcmp("1",string)==0){
        (*op).post=1;
        return 0;
    }
    else if(strcmp("defender",string)==0||strcmp("0",string)==0){
        (*op).post=0;
        return 0;
    }
    return 1;
}

int editElement(int x,int y,int trueindex, Op ops[],int n,bool highlight){
    char tmp[MAX_STRING_LENGTH];
    int tmpi,r=0;
    if(highlight==true)
        printf("\e[7m");
    switch(x){
        case 0:printf("\e[%d;%dH               \e[%d;%dH",y,3,y,3);scanf("%15s",tmp);if(!changeopname(tmp,trueindex,ops,n))r=0;else r=-1;break;
        case 1:printf("\e[%d;%dH          \e[%d;%dH",y,16+3,y,16+3);scanf("%d",&tmpi);if(!changeopspeed(tmpi,&ops[trueindex],1))r=0;else r=-2;break;
        case 2:printf("\e[%d;%dH          \e[%d;%dH",y,27+3,y,27+3);scanf("%d",&tmpi);if(!changeopspeed(tmpi,&ops[trueindex],0))r=0;else r=-3;break;
        case 3:printf("\e[%d;%dH          \e[%d;%dH",y,38+3,y,38+3);scanf("%10s",tmp);if(!changeoppost(tmp,&ops[trueindex]))r=0;else r=-4;break;
        default:r=-5;
    }
    printf("\e[m");
    fflush(stdin);
    return r;
}

void edite(Op ops[],int n){
    char opsstring[n][MAX_STRING_LENGTH],opstring[4][MAX_STRING_LENGTH];
    int x=0,y=0,uly=4;
    header();
    OpstoString(ops,n,opsstring);
    y = menu(opsstring,countallvisible(ops,n),true,y,uly);
    while(y!=-2){
        OptoStrings(ops[ytoi(ops,y)],opstring);
        x = menu(opstring,4,false,y,uly);
        if(x>=0&&x<=3){
            editElement(x,y+uly,ytoi(ops,y),ops,n,true);
            OpstoString(ops,n,opsstring);
        }
        y = menu(opsstring,countallvisible(ops,n),true,y,uly);
    }
}

int adde(Op ops[],int n){
    int y=2;
    header();
    printf("\e[?25h");
    for(int x=0;x<4;x++){
        if(editElement(x,y,n,ops,n,false)!=0)
            x--;
    }
    ops[n].removed=0;
    printf("\e[?25l");
    return 0;
}

void rme(Op ops[], int n){
    char opsstring[n][MAX_STRING_LENGTH];
    int y=0,c=countallvisible(ops,n);
    while(y!=-2){
        header();
        c=OpstoString(ops,n,opsstring);
        //system("cls");
        y=menu(opsstring,c,true,endofmenu(y,c),4);
        if(y>=0&&y<c){
            ops[ytoi(ops,y)].removed=true;
        }
    }
}

int reade(FILE * in, Op ops[],int n){
    int i=0,tmp;
    while(i<n&&fscanf(in,"%s %d %d\n",ops[i].name,&ops[i].speed,&tmp)==3){
        ops[i].post=tmp;
        ops[i].removed=false;
        i++;
    }
    rewind(in);
    return i;
}

void commit(char out[], Op ops[], int n){
    FILE * outf = fopen(out,"w");
    for(int i=0;i<n;i++){
        if(ops[i].removed==false){
            fprintf(outf,"%s %d %d\n",ops[i].name,ops[i].speed,ops[i].post);
        }
    }
}

int main(){
    int n=MAX_OPERATOR_COUNT;
    Op ops[n];
    char out[]="opsout.txt";
    /*TEMNO*/
    DWORD mode = 0;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_PROCESSED_INPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, mode);
    /*SVITANI*/
    FILE * io = fopen(out,"a+");
    FILE * passwd = fopen("passwd.txt","a");
    if(io==NULL||passwd==NULL)return -1;
    fclose(passwd);
    int choice;
    n=reade(io,ops,n);
    bool admin=true;
    while(true){
        choice = mainMenu(admin);
        switch(choice){
            case 1:if(admin==false){if((admin = signin("Insert password:"))==true){spacing();printf("Login successfull");spacing();system("pause");};}else{changepasswd();}break;
            case 2:printall(ops,n,"",0,4);system("pause");break;
            case 3:opsearch(ops,n);break;
            case 4:opsortmenu(ops,n);break;
            case 5:if(adde(ops,n)==0)n++;break;
            case 6:edite(ops,n);break;
            case 7:rme(ops,n);break;
            case 0:if(admin==false){fclose(io);commit(out,ops,n);return 0;}admin=false;break;
            default:break;
        }
        commit(out,ops,n);
    }
}