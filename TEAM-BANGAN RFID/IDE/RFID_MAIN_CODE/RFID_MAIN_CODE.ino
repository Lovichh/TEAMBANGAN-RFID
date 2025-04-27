#include <SPI.h> // Library for SPI communication
#include <MFRC522.h> // Library for RFID module

#define SS_PIN 10 // RFID module SS pin connected to D10
#define RST_PIN 9 // RFID module RST pin connected to D9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

byte card_ID[4]; // Array to hold the UID of scanned card
byte Name1[4] = {0x46, 0x70, 0xDE, 0x00}; // Registered card UID for Student 1
byte Name2[4] = {0xF9, 0x09, 0x04, 0x01}; // Registered card UID for Student 2
byte Name3[4] = {0xD4, 0x87, 0x1D, 0x2A}; // Registered card UID for Student 3
byte Name4[4] = {0x39, 0x39, 0xE5, 0xB9}; // Registered card UID for Student 4

int NumbCard[4] = {0, 0, 0, 0}; // Track if the student has timed in (0) or timed out (1)
int j = -1; // Variable to know which student was scanned

int const RedLed = 6; // Red LED connected to D6
int const GreenLed = 5; // Green LED connected to D5
int const Buzzer = 8; // Buzzer connected to D8

String Name; // Student Name
String UID; // Student UID as string
String GradeSection; // Student Grade and Section
String ContactNumber; // Student Contact Number
String TimeIn[4] = {"", "", "", ""}; // Array to store Time In for each student
String TimeOut[4] = {"", "", "", ""}; // Array to store Time Out for each student

void setup() {
    Serial.begin(9600); // Start serial communication at 9600 baud
    SPI.begin(); // Start SPI communication
    mfrc522.PCD_Init(); // Initialize the RFID reader

    Serial.println("CLEARSHEET"); // Clear previous sheet (for logging)
    Serial.println("LABEL,Date,Time,Name,UID,Grade & Section,Contact Number,Time In,Time Out"); // Labels for spreadsheet

    pinMode(RedLed, OUTPUT); // Set Red LED pin as output
    pinMode(GreenLed, OUTPUT); // Set Green LED pin as output
    pinMode(Buzzer, OUTPUT); // Set Buzzer pin as output
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) { // Check if a new card is present
        return; // If no card, exit loop
    }
    if (!mfrc522.PICC_ReadCardSerial()) { // Check if the card data can be read
        return; // If not readable, exit loop
    }

    UID = ""; // Clear previous UID
    for (byte i = 0; i < mfrc522.uid.size; i++) { // Loop through UID bytes
        card_ID[i] = mfrc522.uid.uidByte[i]; // Save UID bytes
        UID += String(card_ID[i], HEX); // Convert UID bytes to hex string
    }

    if (memcmp(card_ID, Name1, 4) == 0) { // Compare UID with Name1
        Name = "Ean Gingoyon"; // Assign Name
        UID = "12345678"; // Assign custom UID
        GradeSection = "Grade 10 - Happiness"; // Assign Grade & Section
        ContactNumber = "9876543210"; // Assign Contact Number
        j = 0; // Index for this student
    } else if (memcmp(card_ID, Name2, 4) == 0) { // Compare UID with Name2
        Name = "Jane Smith";
        UID = "87654321";
        GradeSection = "Grade 12 - B";
        ContactNumber = "9123456789";
        j = 1;
    } else if (memcmp(card_ID, Name3, 4) == 0) { // Compare UID with Name3
        Name = "Lybrone Neri";
        UID = "11223344";
        GradeSection = "Grade 11 - A";
        ContactNumber = "9000000001";
        j = 2;
    } else if (memcmp(card_ID, Name4, 4) == 0) { // Compare UID with Name4
        Name = "Rean Alvarez";
        UID = "99887766";
        GradeSection = "Grade 9 - C";
        ContactNumber = "9000000002";
        j = 3;
    } else { // If the card UID doesn't match any known cards
        digitalWrite(GreenLed, LOW); // Turn off Green LED
        digitalWrite(RedLed, HIGH); // Turn on Red LED
        for (int i = 0; i < 3; i++) { // Buzz three times
            digitalWrite(Buzzer, HIGH);
            delay(200);
            digitalWrite(Buzzer, LOW);
            delay(200);
        }
        delay(500); // Small delay
        digitalWrite(RedLed, LOW); // Turn off Red LED
        return; // Exit the loop (unknown card)
    }

    String currentTime = "TIME"; // Placeholder for time (replace with real time later)

    if (NumbCard[j] == 0) { // If student is timing in
        TimeIn[j] = currentTime; // Record Time In
        TimeOut[j] = ""; // Clear previous Time Out
        NumbCard[j] = 1; // Update status to "timed in"

        Serial.print("DATA,DATE," + currentTime + "," + Name + "," + UID + "," + GradeSection + "," + ContactNumber + "," + TimeIn[j] + ","); // Print Time In data
        Serial.println();
        Serial.println("SAVEWORKBOOKAS,Attendance/AttendanceList"); // Save to file
    } else if (NumbCard[j] == 1) { // If student is timing out
        TimeOut[j] = currentTime; // Record Time Out

        Serial.print("DATA,DATE," + currentTime + "," + Name + "," + UID + "," + GradeSection + "," + ContactNumber + ",," + TimeOut[j]); // Print Time Out data
        Serial.println();

        NumbCard[j] = 0; // Reset status to allow next Time In
    }

    digitalWrite(GreenLed, HIGH); // Turn on Green LED (success)
    digitalWrite(RedLed, LOW); // Turn off Red LED
    for (int i = 0; i < 2; i++) { // Buzzer beep twice
        digitalWrite(Buzzer, HIGH);
        delay(100);
        digitalWrite(Buzzer, LOW);
        delay(100);
    }
    delay(500); // Wait half a second
    digitalWrite(GreenLed, LOW); // Turn off Green LED
    digitalWrite(RedLed, LOW); // Ensure Red LED is off
}
