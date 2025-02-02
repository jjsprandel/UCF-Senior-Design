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

// Update occupancy count, user check-in status, and timestamps based on activity log
exports.updateOccupancy = onValueWritten(
  "/activityLog/{logId}",
  async (event) => {
    const beforeData = event.data.before?.val(); // Data before the write
    const afterData = event.data.after?.val(); // Data after the write

    // Validate that required fields exist
    if (
      !afterData ||
      !afterData.action ||
      !afterData.userId ||
      !afterData.location ||
      !afterData.timestamp
    ) {
      return null;
    }

    const activityType = afterData.action; // "Check-In" or "Check-Out"
    const userId = afterData.userId;
    const location = afterData.location;
    const timestamp = afterData.timestamp; // Timestamp from activity log

    const occupancyRef = admin.database().ref(`/stats/occupancy/${location}`);
    const userCheckInStatusRef = admin
      .database()
      .ref(`/users/${userId}/CheckInStatus`);
    const userLastCheckInRef = admin
      .database()
      .ref(`/users/${userId}/lastCheckIn`);
    const userLastCheckOutRef = admin
      .database()
      .ref(`/users/${userId}/lastCheckOut`);

    // Get the current occupancy value for this location
    const occupancySnapshot = await occupancyRef.once("value");
    let occupancy = occupancySnapshot.val() || 0;

    // Prepare updates
    let updates = {};

    if (activityType === "Check-In") {
      occupancy += 1;
      updates[`/users/${userId}/CheckInStatus`] = true;
      updates[`/users/${userId}/lastCheckIn`] = timestamp;
    } else if (activityType === "Check-Out") {
      occupancy -= 1;
      updates[`/users/${userId}/CheckInStatus`] = false;
      updates[`/users/${userId}/lastCheckOut`] = timestamp;
    }

    // Update occupancy count
    updates[`/stats/occupancy/${location}`] = occupancy;

    // Perform batch update
    return admin.database().ref().update(updates);
  }
);

// Create and deploy your first functions
// https://firebase.google.com/docs/functions/get-started

// exports.helloWorld = onRequest((request, response) => {
//   logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });
