#include "cachelab.h"
#include<getopt.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>

#define HIT 0
#define MISS 1
#define EVICTION 2

typedef struct{
    bool valid;
    unsigned long tag;
    int last_time;
}cache_line;
int verbose=0;
int s,E,b;
char t[20];
int result[3]={0};
char result_string[3][15]={"hit","miss","eviction"};
int the_time=0;
int if_occupied[1024]={0};

cache_line** get_opt(int argc,char** argv); //获取参数、设置sEb,并且分配cache空间、同时初始化
void read_cache(cache_line** cache);    //从文件中读取操作
void cache_calculate(cache_line** cache,int set,unsigned long tag); //根据操作判断是否命中/不命中/驱逐
void update(cache_line* a_group,int situation,int index,unsigned long tag); //更新cache
int LRU_eviction(cache_line* a_group);  //找到evication的替换行
void destory(cache_line** cache);

int main(int argc,char** argv)
{
    cache_line** cache=get_opt(argc,argv);
    read_cache(cache);
    destory(cache);
    printSummary(result[0], result[1], result[2]);
    return 0;
}


cache_line** get_opt(int argc,char** argv){
    int opt;
    while((opt=getopt(argc,argv,"vs:E:b:t:"))!=-1){
        switch(opt){
            case 'v':
                verbose=1;
                break;
            case 's':
                s=atoi(optarg);
                break;
            case 'E':
                E=atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(t,optarg);
                break;
            default:
                break;
        }
    }
    int S=pow(2,s);
    cache_line** cache=(cache_line**)malloc(S*sizeof(cache_line*));
    for(int i=0;i<S;i++){
        cache[i]=(cache_line*)malloc(E*sizeof(cache_line));
    }
    for(int i=0;i<S;i++){
        for(int j=0;j<E;j++){
            cache[i][j].valid=0;
            cache[i][j].last_time=0;
        }
    }

    return cache;
}

void read_cache(cache_line** cache){
    FILE* file;
    file=fopen(t,"r");

    int set;
    unsigned long tag;

    char operation;
    unsigned long address;
    int size;
    while(fscanf(file," %c %lx,%d",&operation,&address,&size)==3){
        set=(address>>b)&((unsigned long)(1<<s)-1);
        tag=(address>>(s+b));

        switch(operation){
            case 'L':
            case 'S':
                if(verbose){
                    printf("%c %lx,%d ",operation,address,size);
                }
                cache_calculate(cache,set,tag);
                printf("\n");
                break;
            case 'M':
                if(verbose){
                    printf("%c %lx,%d ",operation,address,size);
                }
                cache_calculate(cache,set,tag);
                cache_calculate(cache,set,tag);
                printf("\n");
                break;
        }
    }
}


void cache_calculate(cache_line** cache,int set,unsigned long tag){
    bool if_hit=false;

    for(int i=0;i<E;i++){
        if((cache[set][i].valid==1)&&(cache[set][i].tag==tag)){
            update(cache[set],HIT,i,tag);
            if_hit=true;
            break;
        }
    }

    if(!if_hit){
        if(if_occupied[set]<E){
            if_occupied[set]++;
            for(int j=0;j<E;j++){
                if(cache[set][j].valid==0){
                    update(cache[set],MISS,j,tag);
                    break;
                }
            }
        }
        else{
            int index=LRU_eviction(cache[set]);
            update(cache[set],MISS,index,tag);
            update(cache[set],EVICTION,index,tag);
        }
    }
}

void update(cache_line* a_group,int situation,int index,unsigned long tag){
    result[situation]++;
    if(verbose) {printf("%s ",result_string[situation]);}
    a_group[index].tag=tag;
    a_group[index].valid=1;
    a_group[index].last_time=the_time;
    the_time++;
}

int LRU_eviction(cache_line* a_group){
    int min=a_group[0].last_time;
    int index=0;
    for(int i=1;i<E;i++){
        if(a_group[i].last_time<min){
            index=i;
            min=a_group[i].last_time;
        }
    }
    return index;
}

void destory(cache_line** cache)
{
    int S = pow(2, s);
    for(int i=0; i<S; i++)
    {
        free(cache[i]);
    }
    free(cache);
}