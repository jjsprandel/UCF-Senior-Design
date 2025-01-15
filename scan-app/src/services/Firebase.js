import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyD5eL4GArXQk5qpqRAdMtt5dyZtN10db10",
  authDomain: "scan-9ee0b.firebaseapp.com",
  databaseURL: "https://scan-9ee0b-default-rtdb.firebaseio.com",
  projectId: "scan-9ee0b",
  storageBucket: "scan-9ee0b.firebasestorage.app",
  messagingSenderId: "10238793701",
  appId: "1:10238793701:web:13514dcaaefcef2692c10c",
};

const app = initializeApp(firebaseConfig);
export const database = getDatabase(app);
