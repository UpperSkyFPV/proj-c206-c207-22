CREATE TABLE IF NOT EXISTS User (
    id   INTEGER PRIMARY KEY,
    name VARCHAR(255) NOT NULL
);

INSERT INTO User(name) VALUES ('Test 1');

/* SELECT * FROM User; */
