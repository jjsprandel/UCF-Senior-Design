#include "firebase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_sntp.h"
#include "time.h"

#define FIREBASE_URL "https://scan-9ee0b-default-rtdb.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyD5eL4GArXQk5qpqRAdMtt5dyZtN10db10"

static const char *TAG = "firebase";

const char *firebase_root_cert = "-----BEGIN CERTIFICATE-----\n"
                                 "MIIFYDCCBEigAwIBAgIQXZ19BURwpKYO6SJLAkCxfDANBgkqhkiG9w0BAQsFADA7\n"
                                 "MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQww\n"
                                 "CgYDVQQDEwNXUjEwHhcNMjUwMTEyMjM1MTI2WhcNMjUwNDEyMjM1MTI1WjAtMSsw\n"
                                 "KQYDVQQDDCIqLnVzLWNlbnRyYWwxLmZpcmViYXNlZGF0YWJhc2UuYXBwMIIBIjAN\n"
                                 "BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA6stPlUpg+0eUICaAtzKu9Ft5aYRj\n"
                                 "+T5sKZllIdhrRI+SXsD63I8qbpfdIH86LULMM6XfhmzIjE0rngtHWvTfRPYCRmgt\n"
                                 "bjy4ldUS8BfS1dtenSw36qfg4FF0Dp+qj8rjuaVi5eCdARTGTme+UPRSfx6u7KG4\n"
                                 "BrclqHbREzEA1VsyMw1j3dOqifEVWm69F6621ZsdIt+4WmD7GlSNpFrzDCHhm7Vu\n"
                                 "YuXxLHmDkxoJJSS+SrIFUXlFv+uu0/4o84aZAQBm9jUIuwhQXsN+nyn4AhLkfFJj\n"
                                 "teE1OLvwnMIFwHDWD86Uut8t/9TTlDZ8GOp3mR2nJo3LLINdUXDQ8BUsDQIDAQAB\n"
                                 "o4ICbDCCAmgwDgYDVR0PAQH/BAQDAgWgMBMGA1UdJQQMMAoGCCsGAQUFBwMBMAwG\n"
                                 "A1UdEwEB/wQCMAAwHQYDVR0OBBYEFC5BeEyk/7yYMfHmTa+qiG7daJcKMB8GA1Ud\n"
                                 "IwQYMBaAFGZpSdTeKpyRA8+JDiS4DjADboguMF4GCCsGAQUFBwEBBFIwUDAnBggr\n"
                                 "BgEFBQcwAYYbaHR0cDovL28ucGtpLmdvb2cvcy93cjEvWFowMCUGCCsGAQUFBzAC\n"
                                 "hhlodHRwOi8vaS5wa2kuZ29vZy93cjEuY3J0MD8GA1UdEQQ4MDaCIioudXMtY2Vu\n"
                                 "dHJhbDEuZmlyZWJhc2VkYXRhYmFzZS5hcHCCECouZmlyZWJhc2Vpby5jb20wEwYD\n"
                                 "VR0gBAwwCjAIBgZngQwBAgEwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2MucGtp\n"
                                 "Lmdvb2cvd3IxL2FGN1pOZllwTFhRLmNybDCCAQMGCisGAQQB1nkCBAIEgfQEgfEA\n"
                                 "7wB2AE51oydcmhDDOFts1N8/Uusd8OCOG41pwLH6ZLFimjnfAAABlF0k5AMAAAQD\n"
                                 "AEcwRQIhANxxGgB3+DRV2D8pAsC8Nuj9h1jl13i9h1KhndValtE1AiAW4hg1VOtC\n"
                                 "0yscuWTttKoPgU9kIA0YC+4dBVCD6qlgqwB1AH1ZHhLheCp7HGFnfF79+NCHXBSg\n"
                                 "TpWeuQMv2Q6MLnm4AAABlF0k5CwAAAQDAEYwRAIgOsSDwgYu7SyG8JY98q5Ta5Vp\n"
                                 "nCpC4otFgJJPPneFwr4CIHF1V0YWRPHLDhK2K38Fam9n/l4YpqIFu3ugZHc07pYA\n"
                                 "MA0GCSqGSIb3DQEBCwUAA4IBAQBCO/pf+ZlL2AklpmsNrVTU51OkQ17etJBFUeEp\n"
                                 "Zr+JhemW1FgrmPZp5xlJXKf/XHydbQdjRWbcvCQ0Ju5KmJH83mvYVEHlbDMLyUWS\n"
                                 "aHy7sjhweCrpnQKIkOKAyaUcoKbx0Y24YESPaPZeHgSwA0lSBhchoflQ4poPqZ8A\n"
                                 "PLV5/peFqMMzX3ePmu/3wja5jrF71diehZVlmZDHYDjRorbmPE96oA9vkFGpXVTg\n"
                                 "De6VTk4EeUaubBF5KB7rycL8bqQA4SpR+x+Mm8Czu4llKuiRIgwezvFp3f4q89fm\n"
                                 "baDw5s0Y9HyqtX8w99TK8sQFLo2GGY8b5O8MudSaAYojYmvw\n"
                                 "-----END CERTIFICATE-----\n";

void initialize_sntp(void) {
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_stop(); // Stop SNTP client if it is running
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
}

void obtain_time(void) {
    initialize_sntp();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

void write_to_firebase(const char *path, const char *json_data) {
    char url[256];
    snprintf(url, sizeof(url), "%s/%s.json?auth=%s", FIREBASE_URL, path, FIREBASE_API_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .cert_pem = NULL,                     // Skip certificate verification
        .skip_cert_common_name_check = true, // Skip hostname verification (optional)
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_data, strlen(json_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void read_from_firebase(const char *path) {
    obtain_time(); // Ensure time is synchronized before making HTTPS requests

    char url[256];
    snprintf(url, sizeof(url), "%s/%s.json?auth=%s", FIREBASE_URL, path, FIREBASE_API_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,                     // Skip certificate verification
        .skip_cert_common_name_check = true, // Skip hostname verification (optional)
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        char buffer[1024];
        int content_length = esp_http_client_get_content_length(client);
        esp_http_client_read(client, buffer, content_length);
        buffer[content_length] = '\0';

        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld, Data: %s",
                 esp_http_client_get_status_code(client),
                 (long long int)content_length, buffer);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}