#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
const int PASS_LEN=50; // Maximum any password will be


// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    char plain[PASS_LEN];
    char hash[35];
};

// TODO
// Read in the dictionary file and return an array of entry structs.
// Each entry should contain both the hash and the dictionary
// word.
struct entry *read_dictionary(char *filename, int *size)
{
    char line[30];
    int initialSize = 100;
    int counter = 0;
    char * hashcode[33];
    FILE *d = fopen(filename, "r");
    //make sure file exists
    if (!d)
    {
        fprintf(stderr, "Can't open %s for reading: ", filename);
        perror(NULL);
        exit(1);
    }
    //allocate memory for an array of struct entrys 
    struct entry * password_and_hashes = malloc(initialSize *sizeof( struct entry));
    //read each word from the dictionary file 
    while(fgets(line, 30, d) !=NULL)
    {
        //add more memory if the array needs to be bigger
        if(counter == initialSize)
        {
            password_and_hashes = realloc(password_and_hashes, (initialSize+=1) *sizeof(struct entry));
        }
        //replace the newline with null at the end of each line
        char *nl = strchr(line, '\n');
        if(nl)*nl = '\0';
        //hash each word and have a pointer point to the hashed code
        *hashcode = md5(line, strlen(line));
        
        //printf("%s\n", line);
        //add the word into the entry array and add the corresponding hash code into the fields of the sruct entries
        sscanf(line,"%s\n", password_and_hashes[counter].plain);
        sscanf(*hashcode, "%s\n", password_and_hashes[counter].hash);

        //print output to make sure you're getting the right things
       // printf("%s\n", password_and_hashes[counter].plain);
       // printf("%s\n", password_and_hashes[counter].hash);
        
        counter++;
    }

    *size = counter;
    fclose(d);
    //return the pointer to the array of struct enties
    return password_and_hashes;  
}
//compare two hashes for sorting
int cHash( const void *h1, const void *h2)
{ 
    struct entry *hash1 = (struct entry *)h1;
    struct entry *hash2 = (struct entry *)h2;

    return strcmp(hash1->hash, hash2->hash);
}
//compare a hash to the hash of the struct entry hash and see if they match
int matchHash (const void *h1, const void *tar)
{
    char * target = (char *)h1;
    struct entry *hash1 = (struct entry *)tar;

    return strcmp(target, hash1->hash);
}
int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }
    int size= 0;
    // TODO: Read the dictionary file into an array of entry structures
    struct entry *dict = read_dictionary(argv[2], &size);
    
    //Sort the hashed dictionary using qsort.
    // sorts the array by hash value.
    qsort(dict, size, sizeof(struct entry), cHash);

    // Open the hash file for reading.
    FILE *hf = fopen(argv[1], "r");
    if (!hf)
    {
        fprintf(stderr, "Can't open %s for reading: ", argv[1]);
        perror(NULL);
        exit(1);
    }
    char hash[40];
    int count=0;
    while(fgets(hash, 40, hf)!=NULL)
    {
        char *nl = strchr(hash, '\n');
        if(nl)*nl = '\0';
            // For each hash, search for it in the dictionary using
           // binary search.
        struct entry *located = bsearch(hash, dict, size, sizeof(struct entry), matchHash);
        
        if (located)
        {
            // Print out both the hash and word.
            printf("FOUND IT: %s\nHASH: %s\n\n", dict[count].plain, dict[count].hash);
        }

        count++;
    } 
 fclose(hf);   
}
