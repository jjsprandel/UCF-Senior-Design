import React from "react";
import { Navbar, Nav } from "react-bootstrap";

function CustomNavbar() {
  return (
    <Navbar bg="dark" variant="dark" expand="lg">
      <Navbar.Brand href="/">SCAN</Navbar.Brand>
      <Nav className="ml-auto">
        <Nav.Link href="/">Analytics Dashboard</Nav.Link>
        <Nav.Link href="/profile">Activity Log</Nav.Link>
        <Nav.Link href="/logout">User Management</Nav.Link>
      </Nav>
    </Navbar>
  );
}

export default CustomNavbar;
