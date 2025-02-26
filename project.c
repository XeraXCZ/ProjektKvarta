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


typedef struct{ //Op
    char name[100];
    int speed;
    bool post;
    bool removed;
}Op;

int yback(int y,int n){
    if(y<n)
        return y;
    return n-1;
}

void spacing(){
    printf("\n  ");
}

/*void clearfromto(int ulx,int uly,int brx,int bry){
    printf("\e[%d;%dH",ulx,uly);
    for(int i=0;i<uly-bry;i++){
        for(int k=0;k<ulx-brx;k++)
            printf(" ");
        printf("\e[%d;%dH",ulx,uly+i);
    }
}*/

void header(char string[]){
    system(string);
    spacing();
    printf("%-15s %-10s %-10s %-10s\n\n","Name","Speed","Health","Post");
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

int ytoreal(Op ops[],int y){
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
    int m=0;
    for(int i=0;i<n;i++){
        if(ops[i].removed==false)
            m++;
    }
    return m;
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
}

int menu(char text[][100],int cc,bool vertical,int y){
    int x=0,ch;
    while(true){
        if(vertical==true){ system("cls");printf("\n");}
        else printf("\e[%d;3H",y+2);
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

int mainMenu(bool a){
    int cofchcs[2]={5,8};
    char chcs[2][10][100]={ {{"Sign as administrator"},{"Print whole DB"},{"Search in DB"},{"Sort DB"},{"End"}},
                            {{"Change password"},{"Print whole DB"},{"Search in DB"},{"Sort DB"},{"Add element"},{"Edit element"},{"Remove element"},{"Switch to user"}}};
    return (menu(chcs[a],cofchcs[a],true,0)+1)%cofchcs[a];
}

int sequencemenu(char text[][100],int cc,int chcs[]){
    int ch,y=0,j=0;
    sprintf(text[cc-1],"DONE");
    while(true){
        system("cls");
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
            case 13:if(y==cc-1)return 0;if(chcs[y]==0){chcs[y]=j;j++;}else{chcs[y]=0;j--;}break;
            case 27:return -2;
            default:break;
        }
    }
}

void scanPasswd(char passwd[]){
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

void printall(Op ops[], int n){
    header("cls");
    for(int i=0;i<n;i++){
        if(ops[i].removed==false)
        printf("  %-15s %-10d %-10d %-10s\n",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
    }
    spacing();
    system("pause");
}

void opsort(Op ops[],int n,int chce){       
    for(int i=0;i<n-2;i++){
        for(int y=n-1;y>i;y--){
                if((!comparestrings(ops[y-1].name,ops[y].name)&&chce==0)||(ops[y-1].speed>ops[y].speed&&chce==1)||(ops[y-1].post>ops[y].post&&chce==2)){
                    swapop(&ops[y],&ops[y-1]);
            }
        }
    }
}

void opsortmenu(Op ops[],int n){
    int chce=0,cc=4,chcs[cc++];
    char text[10][100]={{"Name"},{"Speed"},{"Health"},{"Post"}};
    sequencemenu(text,cc,chcs);
    //for(int i=0;i<cc;i++)
    opsort(ops,n,chce);
}

int OpstoString(Op ops[],int n,char opstring[][100]){
    int y=0;
    for(int i=0;i<n;i++){
        if(ops[i].removed==false){
            sprintf(opstring[y], "%-15s %-10d %-10d %-10s",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
            y++;
        }
    }
    return y;
}

void OptoStrings(Op op,char opstring[][100]){
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
    char tmp[100];
    int tmpi,r=0;
    if(highlight==true)
        printf("\e[7m");
    switch(x){
        case 0:printf("\e[%d;%dH               \e[%d;%dH",y+2,3,y+2,3);scanf("%15s",tmp);if(!changeopname(tmp,trueindex,ops,n))r=0;else r=-1;break;
        case 1:printf("\e[%d;%dH          \e[%d;%dH",y+2,16+3,y+2,16+3);scanf("%d",&tmpi);if(!changeopspeed(tmpi,&ops[trueindex],1))r=0;else r=-2;break;
        case 2:printf("\e[%d;%dH          \e[%d;%dH",y+2,27+3,y+2,27+3);/*if(y==trueindex){*/scanf("%d",&tmpi);if(!changeopspeed(tmpi,&ops[trueindex],0))r=0;else r=-3;/*}else printf("%d",4-ops[trueindex].speed); r=0;*/break;
        case 3:printf("\e[%d;%dH          \e[%d;%dH",y+2,38+3,y+2,38+3);scanf("%10s",tmp);if(!changeoppost(tmp,&ops[trueindex]))r=0;else r=-4;break;
        default:r=-5;
    }
    printf("\e[m");
    fflush(stdin);
    return r;
}

void edite(Op ops[],int n){
    char opsstring[n][100],opstring[4][100];
    int x=0,y=0;
    OpstoString(ops,n,opsstring);
    y = menu(opsstring,countallvisible(ops,n),true,y);
    while(y!=-2){
        OptoStrings(ops[y],opstring);
        x = menu(opstring,4,false,y);
        if(x>=0&&x<=3){
            editElement(x,y,ytoreal(ops,y),ops,n,true);
            OpstoString(ops,n,opsstring);
        }
        y = menu(opsstring,n,true,y);
    }
}

int adde(Op ops[],int n){
    int y=2;
    header("cls");
    printf("\e[?25h");
    for(int x=0;x<4;x++){
        if(editElement(x,y,n,ops,n,false)!=0)
            x--;
        //editElement(x,y,n,ops,n,false);
    }
    ops[n].removed=0;
    printf("\e[?25l");
    return 0;
}

void rme(Op ops[], int n){
    char opsstring[n][100];
    int y=0,c=countallvisible(ops,n);
    while(y!=-2){
        c=OpstoString(ops,n,opsstring);
        y=menu(opsstring,c,true,yback(y,c));
        if(y>=0&&y<c){
            ops[ytoreal(ops,y)].removed=true;
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
    int n=100;
    Op ops[n];
    char out[]="opsout.txt",in[]="ops.txt";
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
            case 2:printall(ops,n);break;
            case 3:break;
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