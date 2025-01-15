import React, { useEffect, useState } from "react";
import { Container, Row, Col, Card } from "react-bootstrap";
import { ref, onValue } from "firebase/database";
import { database } from "../services/Firebase";

function Dashboard() {
  // Static data for now (no Firebase interaction)
  const [occupancy, setOccupancy] = useState(0);
  const [averageStay, setAverageStay] = useState("0 hours");

  useEffect(() => {
    const occupancyRef = ref(database, "occupancy");
    const averageStayRef = ref(database, "average_stay");

    const unsubscribeOccupancy = onValue(occupancyRef, (snapshot) => {
      const data = snapshot.val();
      setOccupancy(data);
    });

    const unsubscribeAverageStay = onValue(averageStayRef, (snapshot) => {
      const data = snapshot.val();
      setAverageStay(data);
    });

    // Cleanup subscriptions on unmount
    return () => {
      unsubscribeOccupancy();
      unsubscribeAverageStay();
    };
  }, []);

  return (
    <Container fluid className="d-flex flex-column flex-grow-1 overflow-auto">
      <Row className="flex-grow-1 mt-4">
        {/* Left Column */}
        <Col md={4} className="d-flex flex-column">
          <Card className="mb-3 flex-grow-1">
            <Card.Header>Occupancy</Card.Header>
            <Card.Body className="d-flex flex-column justify-content-center align-items-center">
              <Row className="w-100 d-flex justify-content-center align-items-center">
                <Col
                  md={6}
                  className="d-flex justify-content-center align-items-center"
                >
                  <img
                    src="/imgs/speed.png"
                    alt="Description"
                    className="img-fluid"
                  />
                </Col>
              </Row>
              <Row className="w-100 d-flex justify-content-center align-items-center">
                <Col className="d-flex justify-content-center align-items-center">
                  <p>{occupancy}</p>
                </Col>
              </Row>
            </Card.Body>
          </Card>
          <Card className="mb-3 flex-grow-1">
            <Card.Header>Average Stay Duration</Card.Header>
            <Card.Body className="d-flex flex-column justify-content-center align-items-center">
              <Row className="w-100 d-flex justify-content-center align-items-center">
                <Col
                  md={6}
                  className="d-flex justify-content-center align-items-center"
                >
                  <img
                    src="/imgs/clock.png"
                    alt="Description"
                    className="img-fluid"
                  />
                </Col>
              </Row>
              <Row className="w-100 d-flex justify-content-center align-items-center">
                <Col className="d-flex justify-content-center align-items-center">
                  <p>{averageStay} hours</p>
                </Col>
              </Row>
            </Card.Body>
          </Card>
        </Col>

        {/* Right Column */}
        <Col md={8} className="d-flex flex-column">
          <Card className="mb-3 flex-grow-1">
            <Card.Header>Occupancy Histogram</Card.Header>
            <Card.Body className="d-flex flex-column justify-content-center align-items-center">
              <p>Stuff here</p>
            </Card.Body>
          </Card>
        </Col>
      </Row>
    </Container>
  );
}

export default Dashboard;
