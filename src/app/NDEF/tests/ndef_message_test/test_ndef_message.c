#include <limits.h>
#include <unity.h>
#include <PN532.h>
#include <ndef_message.h>
#include <ndef_record.h>
#include "esp_heap_caps.h"

void assertNoLeak(void (*callback)(void))
{
    size_t start = heap_caps_get_free_size();
    callback();
    size_t end = heap_caps_get_free_size();
    TEST_ASSERT_EQUAL(0, (int)(start - end));
}

void assertBytesEqual(const uint8_t *expected, const uint8_t *actual, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(expected[i], actual[i]);
    }
}

// Unit Tests
void test_messageDelete()
{
    size_t start = esp_get_free_heap_size();
    ndefMessage_t *m1 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

    create_ndef_message_empty(m1);

    addTextRecord(&m1, "Foo");

    free(m1);

    size_t end = esp_get_free_heap_size();
    TEST_ASSERT_EQUAL(0, (int)(start - end));
}

void test_assign()
{
    size_t start = esp_get_free_heap_size();

    if (true)
    {
        ndefMessage_t *m1 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m1);
        addTextRecord(&m1, "Hello ESP32!");

        ndefMessage_t *m2 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m2);
        *m2 = *m1;

        ndefRecord_t r1 = *m1->records[0];
        ndefRecord_t r2 = *m2->records[0];

        TEST_ASSERT_EQUAL(r1.tnf, r2.tnf);
        TEST_ASSERT_EQUAL(r1.type_length, r2.type_length);
        TEST_ASSERT_EQUAL(r1.payload_length, r2.payload_length);
        TEST_ASSERT_EQUAL(r1.id_length, r2.id_length);

        uint8_t p1[r1.payload_length];
        uint8_t p2[r2.payload_length];

        memcpy(p1, r1.payload, r1.payload_length);
        memcpy(p2, r2.payload, r2.payload_length);

        int size = r1.payload_length;
        assertBytesEqual(p1, p2, size);

        free(m1);
        free(m2);
    }

    size_t end = esp_get_free_heap_size();
    TEST_ASSERT_EQUAL(0, (int)(start - end));
}

void test_assign_2()
{
    size_t start = esp_get_free_heap_size();

    if (true)
    {
        ndefMessage_t *m1 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m1);
        addTextRecord(&m1, "We the People of the United States, in Order to form a more perfect Union... ");

        ndefMessage_t *m2 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m2);
        *m2 = *m1;

        ndefRecord_t r1 = *m1->records[0];
        ndefRecord_t r2 = *m2->records[0];

        TEST_ASSERT_EQUAL(r1.tnf, r2.tnf);
        TEST_ASSERT_EQUAL(r1.type_length, r2.type_length);
        TEST_ASSERT_EQUAL(r1.payload_length, r2.payload_length);
        TEST_ASSERT_EQUAL(r1.id_length, r2.id_length);

        uint8_t p1[r1.payload_length];
        uint8_t p2[r2.payload_length];

        memcpy(p1, r1.payload, r1.payload_length);
        memcpy(p2, r2.payload, r2.payload_length);

        int size = r1.payload_length;
        assertBytesEqual(p1, p2, size);
    }

    size_t end = esp_get_free_heap_size();
    TEST_ASSERT_EQUAL(0, (int)(start - end));
}

void test_assign_3()
{
    size_t start = esp_get_free_heap_size();

    if (true)
    {
        ndefMessage_t *m1 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m1);
        addTextRecord(&m1, "We the People of the United States, in Order to form a more perfect Union... ");

        ndefMessage_t *m2 = (ndefMessage_t *)malloc(sizeof(ndefMessage_t));

        create_ndef_message_empty(m2);
        *m2 = *m1;

        free(m1);

        ndefRecord_t r = *m2->records[0];

        TEST_ASSERT_EQUAL(TNF_WELL_KNOWN, r2.tnf);
        TEST_ASSERT_EQUAL(1, r2.type_length);
        TEST_ASSERT_EQUAL(79, r2.payload_length);
        TEST_ASSERT_EQUAL(0, r2.id_length);

        uint8_t p1[r1.payload_length];
        uint8_t p2[r2.payload_length];

        memcpy(p1, r1.payload, r1.payload_length);
        memcpy(p2, r2.payload, r2.payload_length);

        int size = r1.payload_length;
        assertBytesEqual(p1, p2, size);
    }

    size_t end = esp_get_free_heap_size();
    TEST_ASSERT_EQUAL(0, (int)(start - end));
}