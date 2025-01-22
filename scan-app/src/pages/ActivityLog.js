import React, { useState, useEffect } from "react";
import { Container, Row, Col, Form, Table } from "react-bootstrap";
import { ref, onValue } from "firebase/database";
import { database } from "../services/Firebase"; // Adjust the import path as necessary

function ActivityLog() {
  const [activityLog, setActivityLog] = useState([]);
  const [users, setUsers] = useState({});

  useEffect(() => {
    const activityLogRef = ref(database, "activityLog");
    const usersRef = ref(database, "users");

    const unsubscribeActivityLog = onValue(activityLogRef, (snapshot) => {
      const data = snapshot.val();
      const activityLogArray = data
        ? Object.keys(data).map((key) => ({
            id: key,
            ...data[key],
          }))
        : [];
      setActivityLog(activityLogArray);
    });

    const unsubscribeUsers = onValue(usersRef, (snapshot) => {
      const data = snapshot.val();
      setUsers(data || {});
    });

    // Cleanup subscriptions on unmount
    return () => {
      unsubscribeActivityLog();
      unsubscribeUsers();
    };
  }, []);

  const formatTimestamp = (timestamp) => {
    const year = timestamp.substring(0, 4);
    const month = timestamp.substring(4, 6);
    const day = timestamp.substring(6, 8);
    const hour = timestamp.substring(9, 11);
    const minute = timestamp.substring(11, 13);
    const second = timestamp.substring(13, 15);

    const date = new Date(
      `${year}-${month}-${day}T${hour}:${minute}:${second}`
    );
    return date.toLocaleString("en-US", {
      month: "long",
      day: "numeric",
      year: "numeric",
      hour: "numeric",
      minute: "numeric",
      second: "numeric",
      hour12: true,
    });
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
              {activityLog.map((item, index) => {
                const user = users[item.userId] || {};
                return (
                  <tr key={index}>
                    <td>{item.userId}</td>
                    <td>{user.firstName || "N/A"}</td>
                    <td>{user.lastName || "N/A"}</td>
                    <td>{item.action}</td>
                    <td>{formatTimestamp(item.timestamp)}</td>
                    <td>{item.location}</td>
                  </tr>
                );
              })}
            </tbody>
          </Table>
        </Col>
      </Row>
    </Container>
  );
}

export default ActivityLog;
