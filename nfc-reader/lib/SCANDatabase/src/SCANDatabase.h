#ifndef SCANDatabase_h
#define SCANDatabase_h

#include <Firebase_ESP_Client.h>

struct UserInfo {
    String passkey;
    bool checkInStatus;
    bool validUserInfo = 0;
};

class SCANDatabase {
public:
    SCANDatabase(FirebaseData &fbdo, const String &wifiSSID, const String &wifiPassword, const String &apiKey, const String &databaseUrl);
    void checkIn(const String &userUcfId, bool debugMessages = 0);
    void checkOut(const String &userUcfId, bool debugMessages = 0);
    UserInfo getUserInfo(const String &userUcfId, bool debugMessages = 0);
    void begin();
private:
    FirebaseData *fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    bool signupOK;
    String wifiSSID;
    String wifiPassword;
    String apiKey;
    String databaseUrl;
};

#endif