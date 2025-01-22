import React, { useState, useEffect } from "react";
import { Container, Row, Col, Form, Table, Card } from "react-bootstrap";
import { ref, onValue } from "firebase/database";
import { database } from "../services/Firebase"; // Adjust the import path as necessary

function UserManagement() {
  const [users, setUsers] = useState([]);
  const [profileData, setProfileData] = useState(null);

  useEffect(() => {
    const usersRef = ref(database, "users");

    const unsubscribeUsers = onValue(usersRef, (snapshot) => {
      const data = snapshot.val();
      const usersArray = data
        ? Object.keys(data).map((key) => ({
            userId: key, // Ensure userId is set correctly
            ...data[key],
          }))
        : [];
      setUsers(usersArray);
      if (usersArray.length > 0) {
        setProfileData(usersArray[0]);
      }
    });

    // Cleanup subscription on unmount
    return () => {
      unsubscribeUsers();
    };
  }, []);

  const handleRowClick = (user) => {
    setProfileData(user);
  };

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

      {/* Row for the table and profile card */}
      <Row className="flex-grow-1 mt-4">
        {/* Data table */}
        <Col md={6}>
          <Table striped bordered hover responsive>
            <thead>
              <tr>
                <th>User ID</th>
                <th>First Name</th>
                <th>Last Name</th>
                <th>Check-In Status</th>
              </tr>
            </thead>
            <tbody>
              {users.map((item, index) => (
                <tr
                  key={index}
                  onClick={() => handleRowClick(item)}
                  style={{ cursor: "pointer" }}
                >
                  <td>{item.userId}</td>
                  <td>{item.firstName || "N/A"}</td>
                  <td>{item.lastName || "N/A"}</td>
                  <td>{item.checkInStatus ? "Checked In" : "Checked Out"}</td>
                </tr>
              ))}
            </tbody>
          </Table>
        </Col>
        {/* Profile Card */}
        <Col md={6}>
          <Card className="mb-3 flex-grow-1">
            <Card.Header>Profile</Card.Header>
            <Card.Body className="d-flex flex-column justify-content-center align-items-center">
              {profileData && (
                <>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={2}
                      className="d-flex justify-content-center align-items-center"
                    >
                      <img
                        src="/imgs/user.png"
                        alt="Description"
                        className="img-fluid"
                      />
                    </Col>
                  </Row>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>First Name:</p>
                    </Col>
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>{profileData.firstName || "N/A"}</p>
                    </Col>
                  </Row>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>Last Name:</p>
                    </Col>
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>{profileData.lastName || "N/A"}</p>
                    </Col>
                  </Row>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>Location:</p>
                    </Col>
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>{profileData.location || "N/A"}</p>
                    </Col>
                  </Row>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>Total Occupancy Time:</p>
                    </Col>
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>{profileData.totalOccupancyTime || "N/A"}</p>
                    </Col>
                  </Row>
                  <Row className="w-100 d-flex justify-content-center align-items-center">
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>Average Stay Duration:</p>
                    </Col>
                    <Col
                      md={6}
                      className="d-flex justify-content-left align-items-left"
                    >
                      <p>{profileData.averageStayDuration || "N/A"}</p>
                    </Col>
                  </Row>
                </>
              )}
            </Card.Body>
          </Card>
        </Col>
      </Row>
    </Container>
  );
}

export default UserManagement;
