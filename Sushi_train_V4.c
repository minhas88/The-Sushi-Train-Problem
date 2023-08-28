#include <stdio.h>  //for input and output functions
#include <stdlib.h> // for memory allocation functions
#include <string.h> //for string manipulation functions 

#define MAX_LINE_LENGTH 50 
#define SHELF_LIFE_3PM 180  // 3 hours in minutes
#define SHELF_LIFE_4PM 90   // 1.5 hours in minutes
int current_shelf_life = SHELF_LIFE_3PM;

struct CumulativeData {
    int timestamp;
    int cumulative_in;
    int cumulative_out;
};

/*
Cum_Data: A pointer to a pointer to an array of CumulativeData structures. This will allow us to modify the pointer to the array if it needs to be reallocated.
numOfData: A pointer to an integer that holds the current number of elements in the Cum_Data array.
maxData: A pointer to an integer that holds the maximum capacity of the Cum_Data array.
timestamp: An integer representing the timestamp value to be inserted.
cumulative_in: An integer representing the cumulative scan in value to be inserted.
cumulative_out: An integer representing the cumulative scan out value to be inserted.
*/
void insertCum_Data(struct CumulativeData **Cum_Data, int *numOfData, int *maxData, int timestamp, int cumulative_in, int cumulative_out) {
    if (*numOfData == *maxData) { //checks if the current number of elements is equal to the maximum capacity
        *maxData = *maxData == 0 ? 1 : *maxData * 2; //doubles the value of *maxData using (? :). If *maxData is 0 it is set to 1; otherwise, it's doubled
        *Cum_Data = realloc(*Cum_Data, *maxData * sizeof(struct CumulativeData)); //resize the memory block pointed to by *Cum_Data to accommodate the new maximum capacity
    }

    (*Cum_Data)[*numOfData].timestamp = timestamp;
    (*Cum_Data)[*numOfData].cumulative_in = cumulative_in;
    (*Cum_Data)[*numOfData].cumulative_out = cumulative_out;
    (*numOfData)++;
}

/*
Cum_Data: A pointer to an array of CumulativeData structures.
numOfData: An integer representing the number of elements in the Cum_Data array.
target_time: An integer representing the timestamp we're looking for.
The use of struct CumulativeData* as the return type is correct because
you're returning a pointer to an element of CumulativeData structure.
*/
struct CumulativeData* getCum_Data(struct CumulativeData *Cum_Data, int numOfData, int target_time) {
    for (int i = 0; i < numOfData; i++) {
        if (Cum_Data[i].timestamp == target_time) {
            return &Cum_Data[i]; //return memory address of that element and matches the return type of the func
        }
    }
    return NULL;
}

struct sushiTrain {
    char *time_stamp;
    int scans_in;
    int scans_out;
    int hrs, mins, secs;
    int disp_trays, accum_scan_in, accum_scan_out;
};

char **parseLine(char *line, int *numTokens);
int parseTimeToken(char *timeToken, struct sushiTrain *trays);
void Adjustment1(struct sushiTrain *trays, int elapsed_minutes, int *last_adjustment_time, struct CumulativeData *Cum_Data, int numOfData);
void Adjustment2(struct sushiTrain *trays, int elapsed_minutes, struct CumulativeData *Cum_Data, int numOfData);

/*
trays: A pointer to a struct sushiTrain that contains information about the sushi train's current state.
elapsed_minutes: An integer representing the total number of minutes that have elapsed.
last_adjustment_time: A pointer to an integer that holds the timestamp of the last adjustment.
Cum_Data: A pointer to an array of struct CumulativeData containing cumulative data.
numOfData: An integer representing the number of data entries in Cum_Data.
*/
void Adjustment1(struct sushiTrain *trays, int elapsed_minutes, int *last_adjustment_time, struct CumulativeData *Cum_Data, int numOfData) {
    if (elapsed_minutes - *last_adjustment_time >= current_shelf_life) { //Checks if the time elapsed since the last adjustment is greater than or equal to the current shelf life
        int trays_in_3_hours_ago = 0;
        int index_3_hours_ago = numOfData - 1;
        /*
        a loop to calculate the cumulative scans in (cumulative_in) from 3 hours ago. 
        It starts by assuming there were no trays 3 hours ago (trays_in_3_hours_ago = 0). 
        Then, it iterates through the Cum_Data array from the most recent data entry (numOfData - 1) back in time.
        It continues iterating as long as both the index_3_hours_ago is greater than or equal to 0 and 
        the time difference between the current time and the timestamp at Cum_Data[index_3_hours_ago] 
        is within the current shelf life
        */
        while (index_3_hours_ago >= 0 && elapsed_minutes - Cum_Data[index_3_hours_ago].timestamp <= current_shelf_life) {
            trays_in_3_hours_ago = Cum_Data[index_3_hours_ago].cumulative_in;
            index_3_hours_ago--;
        }

        int trays_out_at_current_time = trays->accum_scan_out;
        if (trays_out_at_current_time < trays_in_3_hours_ago) {
            int missed_scans = trays_in_3_hours_ago - trays_out_at_current_time;
            trays->scans_out += missed_scans;
            //printf("Adjustment 1: Added %d trays to the current time point.\n", missed_scans);
        }

        *last_adjustment_time = elapsed_minutes;
    }
}


/*
trays: A pointer to a struct sushiTrain that contains information about the sushi train's current state.
elapsed_minutes: An integer representing the total number of minutes that have elapsed.
Cum_Data: A pointer to an array of struct CumulativeData containing cumulative data.
numOfData: An integer representing the number of data entries in Cum_Data.
*/
void Adjustment2(struct sushiTrain *trays, int elapsed_minutes, struct CumulativeData *Cum_Data, int numOfData) {
    int trays_difference = abs(trays->disp_trays);
    int _1_5_hours_earlier = elapsed_minutes - SHELF_LIFE_4PM; //Calculates the time 1.5 hours earlier than the current time
    // to retrieve the CumulativeData structure corresponding to the time 1.5 hours earlier (_1_5_hours_earlier) from the Cum_Data array.
    struct CumulativeData* timestamp_1_5 = getCum_Data(Cum_Data, numOfData, _1_5_hours_earlier); 
    if (timestamp_1_5 != NULL) {
        timestamp_1_5->cumulative_in += trays_difference;
        trays->accum_scan_in += trays_difference;
        //printf("Adjustment 2: Added %d trays 1.5 hours earlier.\n", timestamp_1_5->cumulative_in);
    }
}

/*
line: A string containing the line to be parsed.
numTokens: A pointer to an integer that will store the number of tokens (pieces) in the parsed line.
*/
char **parseLine(char *line, int *numTokens)
{
    char **tokens = NULL;
    char *token;
    *numTokens = 0;

    token = strtok(line, ",");
    while (token != NULL) {
        (*numTokens)++;
        // memory is allocated for the tokens array to store the pointers to the individual tokens.
        tokens = realloc(tokens, (*numTokens) * sizeof(char*));
        //assigns the current token (a pointer to a token) to the last position in the tokens array. 
        //strdup is used to create a duplicate of the token string and return a pointer to the duplicated string
        tokens[(*numTokens) - 1] = strdup(token);
        //The use of NULL as the first argument indicates that the function should 
        //continue where it left off in the previous call.
        token = strtok(NULL, ",");
    }

    return tokens;
}

/*
timeToken: A string containing a time token in the format "hh:mm:ss".
trays: A pointer to a struct sushiTrain where the parsed time components will be stored.
*/
int parseTimeToken(char *timeToken, struct sushiTrain *trays){
    char *sub_token = strtok(timeToken, ":");
    trays->hrs = atoi(sub_token);
    sub_token = strtok(NULL, ":");
    trays->mins = atoi(sub_token);
    sub_token = strtok(NULL, ":");
    trays->secs = atoi(sub_token);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        printf("Unable to open file: %s\n", argv[1]);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int numTokens;

    struct CumulativeData *Cum_Data = NULL;
    int numOfData = 0;
    int maxData = 0;
    int last_adjustment_time = -1;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        struct sushiTrain trays;
        char **tokens = parseLine(line, &numTokens);
        trays.time_stamp = tokens[1];
        parseTimeToken(trays.time_stamp, &trays);

        trays.scans_in = atoi(tokens[2]);
        trays.scans_out = atoi(tokens[3]);

        int elapsed_minutes = trays.hrs * 60 + trays.mins;
        if (elapsed_minutes >= 16 * 60) {
            current_shelf_life = SHELF_LIFE_4PM;
        }

        Adjustment1(&trays, elapsed_minutes, &last_adjustment_time, Cum_Data, numOfData);

        trays.accum_scan_in += trays.scans_in;
        trays.accum_scan_out += trays.scans_out;

        trays.disp_trays = trays.accum_scan_in - trays.accum_scan_out;
        if(trays.disp_trays < 0){
            Adjustment2(&trays, elapsed_minutes, Cum_Data, numOfData);
        }

        insertCum_Data(&Cum_Data, &numOfData, &maxData, elapsed_minutes, trays.accum_scan_in, trays.accum_scan_out);

        for (int i = 0; i < numTokens; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }

    //final output
    printf("Final output: %d\n", Cum_Data[numOfData - 1].cumulative_in - Cum_Data[numOfData - 1].cumulative_out);

    free(Cum_Data);

    fclose(file);

    return 0;
}
