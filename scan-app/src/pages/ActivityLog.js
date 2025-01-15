import React from "react";
import { Container, Row, Col, Form, Table } from "react-bootstrap";

function ActivityLog() {
  // Sample data for the table
  const data = [
    {
      userId: "1",
      firstName: "John",
      lastName: "Doe",
      action: "Check-In",
      time: "10:30 AM",
      location: "Entrance",
    },
    {
      userId: "2",
      firstName: "Jane",
      lastName: "Smith",
      action: "Check-Out",
      time: "11:00 AM",
      location: "Exit",
    },
    {
      userId: "3",
      firstName: "Mark",
      lastName: "Lee",
      action: "Check-In",
      time: "12:00 PM",
      location: "Entrance",
    },
  ];

  return (
    <Container fluid className="d-flex flex-column flex-grow-1 overflow-auto">
      {/* Row for the search bar */}
      <Row className="mt-4">
        <Col md={3}>
          <Form>
            <Form.Group controlId="search">
              <Form.Control type="text" placeholder="Search..." />
            </Form.Group>
          </Form>
        </Col>
      </Row>

      {/* Row for the table */}
      <Row className="flex-grow-1 mt-4">
        <Col>
          <Table striped bordered hover responsive>
            <thead>
              <tr>
                <th>User ID</th>
                <th>First Name</th>
                <th>Last Name</th>
                <th>Action</th>
                <th>Time</th>
                <th>Location</th>
              </tr>
            </thead>
            <tbody>
              {data.map((item, index) => (
                <tr key={index}>
                  <td>{item.userId}</td>
                  <td>{item.firstName}</td>
                  <td>{item.lastName}</td>
                  <td>{item.action}</td>
                  <td>{item.time}</td>
                  <td>{item.location}</td>
                </tr>
              ))}
            </tbody>
          </Table>
        </Col>
      </Row>
    </Container>
  );
}

export default ActivityLog;
