// src/pages/Auth.js
import React, { useState, useContext } from "react";
import { Container, Row, Col, Form, Button } from "react-bootstrap";
import { useNavigate } from "react-router-dom";
import { AuthContext } from "../contexts/AuthContext";

function Auth() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [isSignUp, setIsSignUp] = useState(true);
  const { login } = useContext(AuthContext);
  const navigate = useNavigate();

  const handleAuth = (e) => {
    e.preventDefault();
    // Placeholder for authentication logic
    console.log(isSignUp ? "Sign Up" : "Login", email, password);
    login();
    console.log("Auth: User logged in");
    navigate("/app");
  };

  return (
    <Container
      className="d-flex flex-column justify-content-center align-items-center"
      style={{ height: "100vh" }}
    >
      <Row className="w-100">
        <Col md={{ span: 6, offset: 3 }}>
          <h2>{isSignUp ? "Sign Up" : "Login"}</h2>
          <Form onSubmit={handleAuth}>
            <Form.Group controlId="formBasicEmail">
              <Form.Label>Email address</Form.Label>
              <Form.Control
                type="email"
                placeholder="Enter email"
                value={email}
                onChange={(e) => setEmail(e.target.value)}
              />
            </Form.Group>

            <Form.Group controlId="formBasicPassword">
              <Form.Label>Password</Form.Label>
              <Form.Control
                type="password"
                placeholder="Password"
                value={password}
                onChange={(e) => setPassword(e.target.value)}
              />
            </Form.Group>

            <Button variant="primary" type="submit" className="mt-3">
              {isSignUp ? "Sign Up" : "Login"}
            </Button>
          </Form>
          <Button
            variant="link"
            onClick={() => setIsSignUp(!isSignUp)}
            className="mt-3"
          >
            {isSignUp
              ? "Already have an account? Login"
              : "Don't have an account? Sign Up"}
          </Button>
        </Col>
      </Row>
    </Container>
  );
}

export default Auth;
