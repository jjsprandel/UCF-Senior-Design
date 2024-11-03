#ifndef SCANDatabase_h
#define SCANDatabase_h

#include <Firebase_ESP_Client.h>

class SCANDatabase {
public:
    SCANDatabase(FirebaseData &fbdo);
    void checkIn(const String &userUcfId);
    void checkOut(const String &userUcfId);
    void getUserInfo();
    void begin(const String &wifiSSID, const String &wifiPassword, const String &apiKey, const String &databaseUrl);
private:
    FirebaseData *fbdo;
    String lastCheckInId;
    FirebaseAuth auth;
    FirebaseConfig config;
    bool signupOK;
};

#endif