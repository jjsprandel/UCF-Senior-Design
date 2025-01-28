// src/App.js
import React from "react";
import {
  BrowserRouter as Router,
  Routes,
  Route,
  Navigate,
} from "react-router-dom";
import Layout from "./components/Layout";
import Dashboard from "./pages/Dashboard";
import CheckIns from "./components/CheckIns";
import ActivityLog from "./pages/ActivityLog";
import UserManagement from "./pages/UserManagement";
import Auth from "./pages/Auth";
import { AuthProvider } from "./contexts/AuthContext";
import PrivateRoute from "./components/PrivateRoute";

function App() {
  return (
    <AuthProvider>
      <Router>
        <Routes>
          {/* Define the auth route */}
          <Route path="/auth" element={<Auth />} />
          {/* Define the layout route */}
          <Route
            path="/app"
            element={
              <PrivateRoute>
                <Layout />
              </PrivateRoute>
            }
          >
            {/* Define routes inside Layout */}
            <Route index element={<Dashboard />} />
            <Route path="checkins" element={<CheckIns />} />
            <Route path="activity-log" element={<ActivityLog />} />
            <Route path="user-management" element={<UserManagement />} />
          </Route>
          {/* Redirect all other routes to the auth page */}
          <Route path="*" element={<Navigate to="/auth" />} />
        </Routes>
      </Router>
    </AuthProvider>
  );
}

export default App;
