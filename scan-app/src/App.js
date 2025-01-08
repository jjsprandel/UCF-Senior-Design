// src/App.js
import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Layout from "./components/Layout";
import Dashboard from "./pages/Dashboard";
import CheckIns from "./components/CheckIns";

function App() {
  return (
    <Router>
      <Routes>
        {/* Define the layout route */}
        <Route path="/" element={<Layout />}>
          {/* Define routes inside Layout */}
          <Route index element={<Dashboard />} />
          <Route path="checkins" element={<CheckIns />} />
        </Route>
      </Routes>
    </Router>
  );
}

export default App;
