/* Solution to comp20005 Assignment 2, 2021 semester 1.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Lichuan Li, 1074497
   Dated:     11/05/2021

*/

/*****************************************************************************/

#include<stdio.h>
#include<math.h>

#define MAXMALL 50
#define MIN_SQR_RNG -25.0
#define MAX_SQR_RNG 25.0
#define S2_INTVL 0.1
#define MIDPT 0.5
#define MIN_RA 1.0
#define N_SHOPN 1
#define TTL_GR 250000
#define PCTG 100
#define STG3 3
#define STG4 4
#define S3N4_XINTVL 1.0
#define S3N4_YINTVL 2.0
#define S3N4_YMAX 24.0
#define S3N4_YMIN -24.0
#define EDGE_RSDT_XMIN -24.5
#define EDGE_RSDT_XMAX 24.5
#define TRUE 1
#define FALSE 0

typedef struct {
    char name;
    double xloc, yloc, glam, carp;
} malls_t;

typedef struct {
    double xloc,yloc;
} loc_t;

typedef struct {
    int num;
} count_t;

typedef struct {
    double area, pctg;
} area_pctg_t;

int read_data(malls_t *malls);
int edge_chk (loc_t mp_resident);
int ra_calc(loc_t *mp_resident, malls_t *malls, int n_malls);

double distance(malls_t *mall, loc_t *mp_resident);

char blank_chk(loc_t cntrl_resident, malls_t *malls, int n_mall, int n_malls);

void ta_daa(void);
void hdr(int stage);
void remov_hdr(void);
void stage_1(malls_t *malls, int n_malls);
void stage_2(malls_t *malls, int n_malls);
void stage_3n4(malls_t *malls, int n_malls, int stage);
void op_stage2(malls_t *malls, count_t *num_pref_malls, int n_malls);
void op_stage3n4(malls_t *malls, loc_t mp_resident, int nth_mall, int n_malls, 
                  int stage);

area_pctg_t area_calc(count_t *num_pref_malls, int n_th);
loc_t midpoint(double xloc, double yloc, double x_intvl, double y_intvl);


/*****************************************************************************/
// Main function
// Variable: n_malls, represents the number of malls
int
main(int argc, char *argv[]) {
    int n_malls = 0;
    malls_t malls[MAXMALL];
    
    n_malls = read_data(malls);

    stage_1(malls, n_malls);
    stage_2(malls, n_malls);
    stage_3n4(malls, n_malls, STG3);
    stage_3n4(malls, n_malls, STG4);
    ta_daa();
  
    /* Programming is fun! */
    return 0;
}


/*****************************************************************************/
/* Defining function read_data(), it reads the data into array 'mall'. 

   'i' represents the number of lines that is read, which is equivalent to 
   the number of malls.

   Output: -- the number of malls.
*/
int
read_data(malls_t *malls) {
    int i = 0;

    remov_hdr(); 

    while ((scanf(" %c%lf%lf%lf%lf", 
        &(*(malls + i)).name, 
        &(*(malls + i)).xloc, 
        &(*(malls + i)).yloc, 
        &(*(malls + i)).glam, 
        &(*(malls + i)).carp)) == 5) {

        i++;
    }

    (*(malls + i)).name = ' ';

    return i;
}


/*****************************************************************************/
// Defining function remov_hdr(), removing the headers in the .tsv file
void
remov_hdr(void) {
    char c;

    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            return;
        }
    }
}


/*****************************************************************************/
/* Defining function stage_1(), the required stage_1 outputs:
       -- How many malls are read
       -- The name of the mall
       -- The location in kilometres of the mall
       -- The glamour score of the mall
       -- The difficulty of getting a carpark of the mall
*/
void 
stage_1(malls_t *malls, int n_malls) {

    printf("S1, %d malls described in input file\n", n_malls);

    for (int i =0; i < n_malls; i++){
        printf("S1, mall %d = %c, xloc = %5.1f, yloc = %5.1f, "
                "glam = %4.1f, carp = %4.1f\n", i, 
                (*(malls + i)).name, 
                (*(malls + i)).xloc, 
                (*(malls + i)).yloc, 
                (*(malls + i)).glam, 
                (*(malls + i)).carp);
    }

    printf("\n");
}


/*****************************************************************************/
/* Defining function stage_2(), the required stage_2 outpus:
       -- Number of grid cells processed
       -- Size of each grid cell (km^2)
       -- Attraction area of each mall (km^2)
       -- Percentage attraction of each mall
*/
void 
stage_2(malls_t *malls, int n_malls) {
    int i, nth_mall;
    double x, y;
    loc_t mp_resident;
    count_t num_pref_malls[n_malls + N_SHOPN]; 

    // Initializing 'num_pref_malls'
    for (i = 0; i < n_malls + N_SHOPN; i++) {
        num_pref_malls[i].num = 0;
    }

    // Choosing the most attractive mall of each cell
    for (x = MAX_SQR_RNG; x > MIN_SQR_RNG; x-=S2_INTVL) {  
        for (y = MAX_SQR_RNG; y > MIN_SQR_RNG ; y-=S2_INTVL){  
            mp_resident = midpoint(x, y, -S2_INTVL, -S2_INTVL);
            nth_mall = ra_calc(&mp_resident, malls, n_malls);
            num_pref_malls[nth_mall].num++;
        }
    }

    // Printing out the outpus
    op_stage2(malls, num_pref_malls, n_malls);
}

/*****************************************************************************/
// Defining function midpoint(), calculating the midpoint 
// Outpus: -- Midpoint of the given grid cell
loc_t
midpoint(double xloc, double yloc, double x_intvl, double y_intvl) {
    loc_t mp; 

    mp.xloc = (xloc + (xloc + x_intvl)) * MIDPT;
    mp.yloc = (yloc + (yloc + y_intvl)) * MIDPT;
    return mp;
}


/*****************************************************************************/
// Defining function ra_calc(), calculating RA value at a given point
// Output: -- returning the nth mall with the largest RA value
//         -- returning 'n_malls' if RA value is below 1.0
int
ra_calc(loc_t *mp_resident, malls_t *malls, int n_malls) {
    int i, nth_mall;
    double dist, ra, tmp_ra;
    
    ra = MIN_RA;
    nth_mall = n_malls;

    for (i = 0; i < n_malls; i++) {
        dist = distance(malls + i, mp_resident);
        tmp_ra = (*(malls + i)).glam / (dist + (*(malls + i)).carp);

        if (tmp_ra >= ra) {
            ra = tmp_ra;
            nth_mall = i;
        }
    }

    return nth_mall;
}


/*****************************************************************************/
// Defining function distance(), calculating the distance between two points
// Output: -- distance between two points (km)
double
distance(malls_t *mall, loc_t *mp_resident) {
    double dist_x, dist_y, dist;
    
    dist_x = (*mall).xloc - (*mp_resident).xloc;
    dist_y = (*mall).yloc - (*mp_resident).yloc;
    dist = sqrt(pow(dist_x,2) + pow(dist_y,2));
    
    return dist;
}


/*****************************************************************************/
// Defining function op_stage2(), printing the required outputs of stage 2
void
op_stage2(malls_t *malls, count_t *num_pref_malls, int n_malls) {
    int i;
    area_pctg_t area_n_pctg;

    printf("S2, %d grid cells processed, each %4.3f km^2\n", 
                TTL_GR,                       pow(S2_INTVL, 2) );

    for (i = 0; i < n_malls + N_SHOPN; i++) {
        area_n_pctg = area_calc(num_pref_malls, i);

        if (i < n_malls) {
            printf("S2, mall %c attracts %6.1f km^2 = %5.1f%% of city\n", 
                    (*(malls + i)).name, area_n_pctg.area, area_n_pctg.pctg);
        }
        else {
            printf("S2, no shopping for %6.1f km^2 = %5.1f%% of city\n", 
                    area_n_pctg.area, area_n_pctg.pctg);
        }
    }

    printf("\n");
}


/*****************************************************************************/
// Defining function area_calc(), giving an output in the type of area_pctg_t
// Output: -- a structure 'area_n_pctg'
area_pctg_t 
area_calc(count_t *num_pref_malls, int n_th) {
    area_pctg_t area_n_pctg;

    area_n_pctg.area = (*(num_pref_malls + n_th)).num * pow(S2_INTVL, 2);
    area_n_pctg.pctg = (*(num_pref_malls + n_th)).num / (double)TTL_GR * PCTG;

    return area_n_pctg;
}


/*****************************************************************************/
// Defining function stage_3n4(), stage 3 and 4 required outputs:
/*     -- Stage 3: Drawing a grid 
       -- Stage 4: Drawing a grid with edge only
*/
void
stage_3n4(malls_t *malls, int n_malls, int stage) {
    int nth_mall;
    double x, y;
    loc_t mp_resident;

    hdr(stage);
    for (y = MAX_SQR_RNG; y > MIN_SQR_RNG; y-= S3N4_YINTVL) {   
        for (x = MIN_SQR_RNG; x < MAX_SQR_RNG; x+= S3N4_XINTVL) {

            mp_resident = midpoint(x, y, S3N4_XINTVL, -S3N4_YINTVL);
            nth_mall = ra_calc(&mp_resident, malls, n_malls);
            
            if (x == MIN_SQR_RNG) {
                printf("S%d, %5.1fkm |", stage, mp_resident.yloc);
            }

            op_stage3n4(malls, mp_resident, nth_mall, n_malls, stage);        
        }
        printf("|\n");
    }

    hdr(stage);
    printf("\n");
}


/*****************************************************************************/
// Defining function hdr(), printing the headerline for the drawing grid
void
hdr(int stage) {
    int i;
    
    printf("S%d,         +", stage);
    for (i = 0; i < (-MIN_SQR_RNG) + MAX_SQR_RNG; i++) {
        printf("-");
    }
    printf("+\n");
}


/*****************************************************************************/
// Defining function op_stage3n4(), printing out the drawing of stage 3 and 4
void 
op_stage3n4(malls_t *malls, loc_t mp_resident, int nth_mall, 
             int n_malls, int stage) {
                
    char pref_mall;

    if (stage == STG3 || edge_chk(mp_resident)) {
        pref_mall = malls[nth_mall].name;
    }
    else{
        pref_mall = blank_chk(mp_resident, malls, nth_mall, n_malls);
    }

    printf("%c",  pref_mall);  
}


/*****************************************************************************/
// Defining function edge_chk()
// Outpus: --returning 1 if the resident is at the edge of the grid
//         --returning 0 if the resident is not at the edge of the grid
int 
edge_chk (loc_t mp_resident) {
    int output = 0;
    
    if (mp_resident.xloc == EDGE_RSDT_XMAX 
        || mp_resident.xloc == EDGE_RSDT_XMIN) {
        output = 1;
    }

    if (mp_resident.yloc == S3N4_YMAX || mp_resident.yloc == S3N4_YMIN) {
        output = 1;
    }

    return output;
}

/*****************************************************************************/
/* Defining function blank_chk(), chekcing if the preferred malls of 
   surroundings are same to the preferred mall of chosen point

   Output: -- returning the name of the mall if the preferred mall of the point 
              is not same to the surroudings'

           -- returning ' ' if the preferred mall of the point is same to the 
              surroudings'
*/
char 
blank_chk(loc_t cntrl_resident, malls_t *malls, int nth_mall, int n_malls) {
    int adj_nth_mall;
    double x_diff, y_diff;
    char pref_mall;
    loc_t adj_resident;

    pref_mall = ' ';
    for (x_diff = -S3N4_XINTVL; x_diff <= S3N4_XINTVL; x_diff+=S3N4_XINTVL) {
        for (y_diff = -S3N4_YINTVL; y_diff <= S3N4_YINTVL;y_diff+=S3N4_YINTVL){
            adj_resident.xloc = cntrl_resident.xloc + x_diff;
            adj_resident.yloc = cntrl_resident.yloc + y_diff;
            
            adj_nth_mall = ra_calc(&adj_resident, malls, n_malls);

            if (adj_nth_mall != nth_mall) {
                pref_mall = (*(malls + nth_mall)).name;
                break;
            }
        }
    }
    
    return pref_mall;
}


/*****************************************************************************/
// Defining function ta_daa(), printing 'ta daa!'
void
ta_daa(void) {
    printf("ta daa!\n");
}
