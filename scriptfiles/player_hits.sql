CREATE TABLE IF NOT EXISTS player_hits (
    u_id int(11) NOT NULL,
    amount int(11) NOT NULL DEFAULT '0',
    reason varchar(32) NOT NULL DEFAULT '',
    placed_by varchar(24) NOT NULL DEFAULT '',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        players(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);
