#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_tls.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"
#include "cJSON.h"
#include "json_parser.h"

// Define a struct to store a field name and its value
typedef struct {
    const char *name;   // Name of the field (e.g., "firstName")
    char value[64];     // Field value (e.g., name of the user)
} StringField;

typedef struct {
    const char *name;   // Name of the field (e.g., "averageStayDuration")
    float value;        // Field value (e.g., duration in float)
} FloatField; 

typedef struct {
    const char *name;   // Name of the field (e.g., "checkInStatus")
    bool value;         // Field value (true or false)
} BoolField;

// Define a User struct that contains multiple field structs
typedef struct {
    StringField firstName;
    StringField lastName;
    StringField averageStayDuration;
    BoolField checkInStatus;
    FloatField totalOccupancyTime;
} User;


// Generic function to parse JSON into a struct
bool parse_firebase_response(const char *json_response, const char *user_id, void *user_data) {
    // Parse the JSON response into a cJSON object
    cJSON *root = cJSON_Parse(json_response);
    if (!root) {
        printf("Failed to parse JSON\n");
        return false;
    }

    // Get the user data object for the specific user ID
    cJSON *user_obj = cJSON_GetObjectItem(root, user_id);
    if (user_obj == NULL) {
        printf("User ID %s not found in JSON\n", user_id);
        cJSON_Delete(root);
        return false;
    }

    // Loop through the struct fields and parse them dynamically
    cJSON *json_field;
    size_t offset = 0;
    size_t struct_size = sizeof(User);
    for (offset = 0; offset < struct_size; offset++) {
        // Get the field as we go through the struct
        void *field_ptr = (char *)user_data + offset;

        // Use reflection-like behavior to get the field name (e.g., "firstName", "lastName", etc.)
        StringField *field = (StringField *)field_ptr;
        
        // Get the corresponding JSON field
        json_field = cJSON_GetObjectItem(user_obj, field->name);
        if (json_field) {
            // Parse the value based on the type
            if (cJSON_IsString(json_field)) {
                strncpy(field->value, json_field->valuestring, sizeof(field->value) - 1);
            } else if (cJSON_IsNumber(json_field)) {
                if (strcmp(field->name, "averageStayDuration") == 0 || strcmp(field->name, "totalOccupancyTime") == 0) {
                    ((FloatField *)field)->value = (float)cJSON_GetNumberValue(json_field);
                }
            } else if (cJSON_IsBool(json_field)) {
                ((BoolField *)field)->value = cJSON_IsTrue(json_field);
            }
        }
    }

    // Clean up and free the cJSON object
    cJSON_Delete(root);
    return true;
}