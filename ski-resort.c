#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef DEBUG
#  define D(x) x
#else
#  define D(x) 
#endif

struct item {
    char name[40] ;
    float size ;
    int amount;
    char mu[10] ;
} ;

struct rentt {
    char name[40] ;
    int day, month, year, hours, minutes, seconds ;
    struct item items[256] ;
} ;

int main(int argc, char *argv[])
{
    FILE * inpf = fopen("input.txt", "r"), 
         * outf = fopen("output.txt", "w") ;

    // output buffer
    char buf[100000] = "\0" ;
    
    // the actual rent
    struct rentt rent[500] ;

    // input
    int cur = 0 ;
    while(fgets(rent[cur].name, 32, inpf) != NULL) {

        // get rid of \n at the end
        int sz = strlen(rent[cur].name) ;
        if(rent[cur].name[sz-1] == '\n')
            rent[cur].name[--sz] = '\0' ;

        // validate name
        if(sz < 2 || sz > 30)
            return fprintf(outf, "Invalid input!"), 0 ;

        for(int i = 0 ; i < sz ; ++i)
            if(!isalpha(rent[cur].name[i]) && rent[cur].name[i] != ' ')
                return fprintf(outf, "Invalid input!"), 0 ;

        // enter date and validate input
        char date[100] ;
        fgets(date, 100, inpf) ;
        if(!(isdigit(date[0]) , isdigit(date[1]) , date[2] == '/' 
                    && isdigit(date[3]) && isdigit(date[4]) && date[5] == '/' 
                    && isdigit(date[6]) && isdigit(date[7])
                    && isdigit(date[8]) && isdigit(date[9]) && date[10] == ' ' 
                    && isdigit(date[11]) && isdigit(date[12]) && date[13] == ':' 
                    && isdigit(date[14]) && isdigit(date[15]) && date[16] == ':' 
                    && isdigit(date[17]) && isdigit(date[18])))
            return fprintf(outf, "Invalid input!"), 0 ;
        sscanf(date, "%d/%d/%d %d:%d:%d", &rent[cur].day, &rent[cur].month, &rent[cur].year, &rent[cur].hours, &rent[cur].minutes, &rent[cur].seconds) ;

        // validate date
        if(rent[cur].month == 1 || rent[cur].month == 3 || rent[cur].month == 5
                || rent[cur].month == 7 || rent[cur].month == 8 
                || rent[cur].month == 10 || rent[cur].month == 12)
        {

            if(rent[cur].day > 31)
                return fprintf(outf, "Invalid input!"), 0 ;

        } else if( rent[cur].month == 2) {

            // leap year case
            if(rent[cur].year % 400 == 0) {
                if(rent[cur].day > 29)
                    return fprintf(outf, "Invalid input!"), 0 ;
            } else if(rent[cur].year % 4 == 0 && rent[cur].year % 100 != 0) {
                if( rent[cur].day > 29)
                    return fprintf(outf, "Invalid input!"), 0 ;
            } else {
                if(rent[cur].day > 28)
                    return fprintf(outf, "Invalid input!"), 0 ;
            }

        } else if ( rent[cur].month >= 1 && rent[cur].month <= 12) {
            if(rent[cur].day > 30)
                return fprintf(outf, "Invalid input!"), 0 ;
        } else
            return fprintf(outf, "Invalid input!"), 0 ;

        // validate time

        if(!(rent[cur].hours >= 0 && rent[cur].hours <= 23 
                && rent[cur].minutes >= 0 && rent[cur].minutes <= 59 
                && rent[cur].seconds >= 0 && rent[cur].seconds <= 59))
            return fprintf(outf, "Invalid input!"), 0 ;
        
        sprintf(buf + strlen(buf), "%s has rented ", rent[cur].name) ;

        int curi = 0 ;
        while(1) {
            char temp[256] ;
            char lol = fgets(temp, 256, inpf) ;
            
            // check if it is the last line
            if(lol == NULL || temp[0] == '\n') break ;

            // get rid of \n at the end
            sz = strlen(temp) ;
            if(temp[sz-1] == '\n')
                temp[--sz] = '\0' ;
            if(temp[sz] != '\0')
                temp[++sz] = '\0' ;
            

            // enter and validate the name of an item
            int curp = 0 ;
            for(int i = 0 ; i < sz ; ++i) {
                if(isalpha(temp[i]) || temp[i] == ' ') {
                    rent[cur].items[curi].name[curp++] = temp[i] ;
                }
                else {
                    if(curp == 0)
                        return fprintf(outf, "Invalid input!"), 0 ;

                    if(rent[cur].items[curi].name[curp-1] == ' ')
                        rent[cur].items[curi].name[--curp] = '\0' ;
                    else
                        return fprintf(outf, "Invalid input!"), 0 ;
                    break ;
                }
            }
            
            // copy only the size, the amount and the measurement unit of an item
            char vals[256] ;
            memcpy(vals, temp+curp, strlen(temp+curp)+1) ;
            // format them
            sscanf(vals, "%f%d%s", &rent[cur].items[curi].size, 
                    &rent[cur].items[curi].amount, 
                    rent[cur].items[curi].mu) ;
            
            D(printf("vals: %s\nsz: %f\nam: %d\nmu: %s\n", 
                        temp, rent[cur].items[curi].size, 
                        rent[cur].items[curi].amount, 
                        rent[cur].items[curi].mu)) ;
            // validation
            if(!(rent[cur].items[curi].size > 0.0 && rent[cur].items[curi].size <= 200.0 
                && rent[cur].items[curi].amount > 0 && rent[cur].items[curi].amount <= 30
                && (strcmp(rent[cur].items[curi].mu, "pair") == 0 
                    || strcmp(rent[cur].items[curi].mu, "pcs") == 0)))
                return fprintf(outf, "Invalid input!"), 0 ;
            
            // the case when there are several pairs
            if(strcmp(rent[cur].items[curi].mu, "pair") == 0 
                    && rent[cur].items[curi].amount > 1)
                sprintf(rent[cur].items[curi].mu, "pairs") ;
            
            curi ++ ;
        }

        // append items to the buffer
        if(curi == 1) {
            sprintf(buf + strlen(buf), "%d %s of %s of size %g", 
            rent[cur].items[0].amount, rent[cur].items[0].mu, 
            rent[cur].items[0].name, rent[cur].items[0].size) ;
        } else for(int i = 0 ; i < curi ; ++i)
        {
            if(i == curi-1)
                sprintf(buf + strlen(buf), " and %d %s of %s of size %g", 
                        rent[cur].items[i].amount, rent[cur].items[i].mu, 
                        rent[cur].items[i].name, rent[cur].items[i].size) ;
            else if(i == 0)
                sprintf(buf + strlen(buf), "%d %s of %s of size %g", 
                        rent[cur].items[i].amount, rent[cur].items[i].mu, 
                        rent[cur].items[i].name, rent[cur].items[i].size) ;
            else
                sprintf(buf + strlen(buf), ", %d %s of %s of size %g", 
                        rent[cur].items[i].amount, rent[cur].items[i].mu, 
                        rent[cur].items[i].name, rent[cur].items[i].size) ;
        }

        // if there is no any item
        if(curi <= 0)
            return fprintf(outf, "Invalid input!"), 0 ;
        
        // append the date/time to the buffer
        sprintf(buf + strlen(buf), " on %d%d/%d%d/%d%d%d%d at %d%d:%d%d:%d%d.\n", 
                rent[cur].day/10, rent[cur].day%10, 
                rent[cur].month/10, rent[cur].month%10, 
                rent[cur].year/1000, rent[cur].year/100%10, 
                rent[cur].year/10%10, rent[cur].year%10, 
                rent[cur].hours/10, rent[cur].hours%10, 
                rent[cur].minutes/10, rent[cur].minutes%10, 
                rent[cur].seconds/10, rent[cur].seconds%10) ;

        cur++ ;
    }

    // print out the buffer
    fprintf(outf, "%s", buf) ;
}