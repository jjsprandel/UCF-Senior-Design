import React from "react";
import { Container, Row, Col, Card } from "react-bootstrap";

function Dashboard() {
  // Static data for now (no Firebase interaction)
  const data = [
    { name: "John Doe", time: "10:30 AM" },
    { name: "Jane Smith", time: "11:00 AM" },
    { name: "Mark Lee", time: "12:00 PM" },
  ];

  return (
    <Container fluid className="d-flex flex-column">
      <Row className="flex-grow-1 mt-4">
        {/* Left Column */}
        <Col md={4} className="d-flex flex-column">
          <Row className="flex-grow-1">
            <Col md={12}>
              <Card className="mb-3 h-100">
                <Card.Header>Check-In Activity</Card.Header>
                <Card.Body>
                  <ul>
                    {data.map((item, index) => (
                      <li key={index}>
                        {item.name} checked in at {item.time}
                      </li>
                    ))}
                  </ul>
                </Card.Body>
              </Card>
            </Col>
          </Row>
          <Row className="flex-grow-1">
            <Col md={12}>
              <Card className="mb-3 h-100">
                <Card.Header>Check-In Activity</Card.Header>
                <Card.Body>{/* Additional content can go here */}</Card.Body>
              </Card>
            </Col>
          </Row>
        </Col>

        {/* Right Column */}
        <Col md={8} className="d-flex flex-column">
          <Card className="mb-3 flex-grow-1">
            <Card.Header>Important Information</Card.Header>
            <Card.Body>
              <p>Details about important notices or alerts will go here.</p>
            </Card.Body>
          </Card>
        </Col>
      </Row>
    </Container>
  );
}

export default Dashboard;
