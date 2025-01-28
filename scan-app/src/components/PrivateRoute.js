// src/components/PrivateRoute.js
import React, { useContext } from "react";
import { Navigate } from "react-router-dom";
import { AuthContext } from "../contexts/AuthContext";

const PrivateRoute = ({ children }) => {
  const { isAuthenticated } = useContext(AuthContext);
  console.log("PrivateRoute: isAuthenticated =", isAuthenticated);
  return isAuthenticated ? children : <Navigate to="/auth" />;
};

export default PrivateRoute;
