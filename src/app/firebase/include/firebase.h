#ifndef FIREBASE_H
#define FIREBASE_H

void write_to_firebase(const char *path, const char *json_data);
void read_from_firebase(const char *path);
void initialize_sntp(void);
void obtain_time(void);

#endif // FIREBASE_H