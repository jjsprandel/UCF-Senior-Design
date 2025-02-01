/**
 * Import function triggers from their respective submodules:
 *
 * const {onCall} = require("firebase-functions/v2/https");
 * const {onDocumentWritten} = require("firebase-functions/v2/firestore");
 *
 * See a full list of supported triggers at https://firebase.google.com/docs/functions
 */

// const { onRequest } = require("firebase-functions/v2/https");
// const logger = require("firebase-functions/logger");

const { onValueWritten } = require("firebase-functions/v2/database");
const admin = require("firebase-admin");
admin.initializeApp();

// Fix: Use onValueWritten instead of functions.database.ref()
exports.updateOccupancy = onValueWritten(
  "/activityLog/{logId}",
  async (event) => {
    const beforeData = event.data.before?.val(); // Data before the write
    const afterData = event.data.after?.val(); // Data after the write

    // If the event is deleted, do nothing
    if (!afterData) {
      return null;
    }

    const activityType = afterData.action; // "Check-In" or "Check-Out"
    const occupancyRef = admin.database().ref("/stats/occupancy");

    // Get the current occupancy value
    const occupancySnapshot = await occupancyRef.once("value");
    let occupancy = occupancySnapshot.val() || 0;

    // Update the occupancy value based on the activity type
    if (activityType === "Check-In") {
      occupancy += 1;
    } else if (activityType === "Check-Out") {
      occupancy -= 1;
    }

    // Update the occupancy value in the database
    return occupancyRef.set(occupancy);
  }
);

// Create and deploy your first functions
// https://firebase.google.com/docs/functions/get-started

// exports.helloWorld = onRequest((request, response) => {
//   logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });
