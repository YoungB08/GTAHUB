CREATE TABLE IF NOT EXISTS player_phones (
    u_id INT NOT NULL,
    phone_number INT NOT NULL UNIQUE,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (u_id),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS player_phone_contacts (
    u_id INT NOT NULL,
    contact_name VARCHAR(24) NOT NULL,
    phone_number INT NOT NULL,
    PRIMARY KEY (u_id, phone_number),
    FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE ON UPDATE CASCADE
);
